/*
 *	
 */

#include "config.h"
#include <string.h>
//from standard X11 headers
#include <X11/Xlocale.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
//from IMdkit
#include <IMdkit.h>
#include <Xi18n.h>
//from GLib
#include <glib.h>
//others
#include "simplexim.h"


Window g_server_window = None;
char   g_xim_name [128];
char   g_locale_ctype [128];

// XIM support for styles, here we implement a -on-the-spot IM
XIMStyles g_ims_styles;
XIMEncodings g_ims_encodings;

//private data used to construct XIMStyles, XIMEncodings
XIMStyle _ims_styles [] = {
	XIMPreeditPosition  | XIMStatusNothing,
        XIMPreeditCallbacks | XIMStatusNothing,
        XIMPreeditNothing   | XIMStatusNothing,
        XIMPreeditPosition  | XIMStatusCallbacks,
        XIMPreeditCallbacks | XIMStatusCallbacks,
        XIMPreeditNothing   | XIMStatusCallbacks,
        0
};
XIMEncoding _ims_encodings[] = {
        "COMPOUND_TEXT",
        0
};

//dispatchers
static int ims_protocol_handler (XIMS xims, IMProtocol *call_data);

static int xim_open_handler (XIMS xims, IMOpenStruct *call_data);
static int xim_close_handler(XIMS ims, IMCloseStruct *call_data);
static int xim_disconnect_ic_handler (XIMS xims, IMDisConnectStruct *call_data);
static int xim_create_ic_handler(XIMS ims, IMChangeICStruct *call_data, int *icid);
static int xim_destroy_ic_handler(XIMS ims, IMDestroyICStruct *call_data, int *icid);
static int xim_set_ic_values_handler(XIMS ims, IMChangeICStruct *call_data, int *icid);
static int xim_get_ic_values_handler(XIMS ims, IMChangeICStruct *call_data, int *icid);
static int xim_forward_event_handler(XIMS ims, IMForwardEventStruct *call_data, int *icid);
static int xim_set_ic_focus_handler(XIMS ims, IMChangeFocusStruct *call_data, int *icid);
static int xim_unset_ic_focus_handler(XIMS ims, IMChangeFocusStruct *call_data, int *icid);
//static int xim_trigger_handler(XIMS ims, IMTriggerNotifyStruct *call_data, int *icid);
//static int xim_sync_reply_handler(XIMS ims, IMSyncXlibStruct *call_data, int *icid)

static inline void set_xim_name ();
static inline void set_local_ctype ();
static inline void set_input_styles ();
static inline void set_encoding_list ();

//implementation details for quick access
static GHashTable*  connections_ht = NULL; // connect_id --> SimpleConn
static GHashTable*  ic_ht = NULL;          //icic --> SimpleIC

/*
 *	
 */
void
xim_init ()
{
    //0.
    connections_ht = g_hash_table_new (g_direct_hash, g_direct_equal);
    ic_ht = g_hash_table_new (g_direct_hash, g_direct_equal);
    //1.
    create_server_window ();
    set_xim_name ();
    set_local_ctype ();
    set_input_styles ();
    //2.
    g_xims = IMOpenIM(g_display,
                      IMServerWindow, g_server_window,
                      IMModifiers, "Xi18n",
                      IMServerName, g_xim_name,
                      IMLocale, g_locale_ctype,
                      IMServerTransport, DEFAULT_XIM_TRANSPORT,
                      IMInputStyles, &g_ims_styles,
                      IMEncodingList, &g_ims_encodings,
                      IMProtocolHandler, ims_protocol_handler, /*TODO*/
                      IMFilterEventMask, KeyPressMask|KeyReleaseMask, /*do we need KeyReleaseMask*/
                      /*IMOnKeysList, xxx,*/
                      NULL);
}
static int
ims_protocol_handler (XIMS xims, IMProtocol *call_data)
{
    if (xims == NULL || call_data == NULL)
	return True;

    switch (call_data->major_code) {
    case XIM_OPEN:
        return xim_open_handler (xims, (IMOpenStruct *)call_data);
    case XIM_CLOSE:
        return xim_close_handler (xims, (IMCloseStruct *)call_data);
    case XIM_DISCONNECT:
        return xim_disconnect_ic_handler (xims, (IMDisConnectStruct *)call_data);
    case XIM_CREATE_IC:
        return xim_create_ic_handler (xims, (IMChangeICStruct *)call_data);
    case XIM_DESTROY_IC:
        return xim_destroy_ic_handler (xims, (IMChangeICStruct *)call_data);
    case XIM_SET_IC_VALUES:
        return xim_set_ic_values_handler (xims, (IMChangeICStruct *)call_data);
    case XIM_GET_IC_VALUES:
        return xim_get_ic_values_handler (xims, (IMChangeICStruct *)call_data);
    case XIM_FORWARD_EVENT:
        return xim_forward_event_handler (xims, (IMForwardEventStruct *)call_data);
    case XIM_SET_IC_FOCUS:
        return xim_set_ic_focus_handler (xims, (IMChangeFocusStruct *)call_data);
    case XIM_UNSET_IC_FOCUS:
        return xim_unset_ic_focus_handler (xims, (IMChangeFocusStruct *)call_data);
    case XIM_RESET_IC:
        //return xim_reset_ic (xims, (IMResetICStruct *)call_data);
    case XIM_TRIGGER_NOTIFY:
        g_debug ("request: XIM_TRIGGER_NOTIFY");
        return 0;
    case XIM_PREEDIT_START_REPLY:
        g_debug ("request: XIM_PREEDIT_START_REPLY");
        return 0;
    case XIM_PREEDIT_CARET_REPLY:
        g_debug ("request: XIM_PREEDIT_CARET_REPLY");
        return 0;
    case XIM_SYNC_REPLY:
        g_debug ("request: XIM_SYNC_REPLY");
        return 0;
    default:
        g_debug ("Unknown (%d)", call_data->major_code);
        return 0;
    }
}
/*
 *	add a new connection to the central registry if not exist.
 */
static int 
xim_open_handler (XIMS xims, IMOpenStruct *call_data)
{
    g_debug ("request: XIM_OPEN connect_id: %d", call_data->connect_id);
    
    SimpleConn* conn = NULL;
    conn = g_hash_table_lookup (connections_ht,
                                GINT_TO_POINTER ((gint) call_data->connect_id));
    if (conn == NULL)
    {
	conn = g_slice_new0 (SimpleConn);
	g_hash_table_insert (connections_ht,
                             (gpointer)(unsigned long)call_data->connect_id,
                             (gpointer)conn);
    }
		
    return True;
}

static void
_free_client_ic (gpointer user_data)
{
}
/*
 *	remove this connection
 */
static int 
xim_close_handler (XIMS ims, IMCloseStruct *call_data)
{
    g_debug ("request: XIM_CLOSE connect_id=%d", call_data->connect_id);

    SimpleConn* conn;
    conn = g_hash_table_lookup (connections_ht, 
	                        GINT_TO_POINTER ((gint) call_data->connect_id));
    if (conn == NULL)
	return False;

    g_list_free_full (conn->client_ics, _free_client_ic);

    g_hash_table_remove (connections_ht,
                         GINT_TO_POINTER ((gint) connect_id));

    g_slice_free (SimpleConn, conn);

    return True;
}

//this function is barely called.
#if 1
static int 
xim_disconnect_ic_handler (XIMS xims, IMDisConnectStruct *call_data)
{
    g_debug ("request: XIM_DISCONNECT");
    return False;
}
#endif

static int
_xim_store_ic_values (SimpleIC* simpleic, IMChangeICStruct *call_data)
{
    XICAttribute *ic_attr = call_data->ic_attr;
    XICAttribute *pre_attr = call_data->preedit_attr;
    XICAttribute *sts_attr = call_data->status_attr;

    gint i;
    guint32 attrs = 1;

    g_return_val_if_fail (simpleic != NULL, 0);
    for (i = 0; i < (int)call_data->ic_attr_num; ++i, ++ic_attr) 
    {
        if (g_strcmp0 (XNInputStyle, ic_attr->name) == 0) 
	{
            simpleic->input_style = *(gint32 *) ic_attr->value;
        }
        else if (g_strcmp0 (XNClientWindow, ic_attr->name) == 0) 
	{
            simpleic->client_window =  (Window)(*(CARD32 *) call_data->ic_attr[i].value);
        }
        else if (g_strcmp0 (XNFocusWindow, ic_attr->name) == 0) 
	{
            simpleic->focus_window =  (Window)(*(CARD32 *) call_data->ic_attr[i].value);
        }
        else 
	{
            g_debug ("Unknown ic attribute: %s", ic_attr->name);
        }
    }

    for (i = 0; i < (int)call_data->preedit_attr_num; ++i, ++pre_attr) 
    {
        if (g_strcmp0 (XNSpotLocation, pre_attr->name) == 0) 
	{
            simpleic->has_preedit_area = TRUE;
            simpleic->preedit_area.x = ((XPoint *)pre_attr->value)->x;
            simpleic->preedit_area.y = ((XPoint *)pre_attr->value)->y;
        }
        else 
	{
            g_debug("Unknown preedit attribute: %s", pre_attr->name);
        }
    }

    for (i=0; i< (int) call_data->status_attr_num; ++i, ++sts_attr) 
    {
        g_debug("Unknown status attribute: %s", sts_attr->name);
    }

    return attrs;
}
/*
 *	create a new Input Context and add it to the corresponding 
 *	connection.
 */
static int 
xim_create_ic_handler (XIMS ims, IMChangeICStruct *call_data, int *icid)
{
    static int base_icid = 1;
    SimpleIC* simpleic;

    //1.
    call_data->icid = base_icid ++;

    g_debug ("request: XIM_CREATE_IC ic=%d connect_id=%d",
            call_data->icid, call_data->connect_id);

    simpleic = g_slice_new0 (SimpleIC);
    g_return_val_if_fail (simpleic != NULL, 0);

    simpleic->icid = call_data->icid;
    simpleic->connect_id = call_data->connect_id;
    simpleic->conn = (SimpleConn*)g_hash_table_lookup (connections_ht,
                                                       GINT_TO_POINTER ((gint)call_data->connect_id));
    if (simpleic->conn == NULL) {
        g_slice_free (SimpleIC, simpleic);
        g_return_val_if_reached (0);
    }

    _xim_store_ic_values (simpleic, call_data);
#if 0
    x11ic->context = ibus_bus_create_input_context (_bus, "xim");

    if (x11ic->context == NULL) {
        g_slice_free (X11IC, x11ic);
        g_return_val_if_reached (0);
    }

    g_signal_connect (x11ic->context, "commit-text",
                        G_CALLBACK (_context_commit_text_cb), x11ic);
    g_signal_connect (x11ic->context, "forward-key-event",
                        G_CALLBACK (_context_forward_key_event_cb), x11ic);

    g_signal_connect (x11ic->context, "update-preedit-text",
                        G_CALLBACK (_context_update_preedit_text_cb), x11ic);
    g_signal_connect (x11ic->context, "show-preedit-text",
                        G_CALLBACK (_context_show_preedit_text_cb), x11ic);
    g_signal_connect (x11ic->context, "hide-preedit-text",
                        G_CALLBACK (_context_hide_preedit_text_cb), x11ic);
    g_signal_connect (x11ic->context, "enabled",
                        G_CALLBACK (_context_enabled_cb), x11ic);
    g_signal_connect (x11ic->context, "disabled",
                        G_CALLBACK (_context_disabled_cb), x11ic);


    if (x11ic->input_style & XIMPreeditCallbacks) {
        ibus_input_context_set_capabilities (x11ic->context, IBUS_CAP_FOCUS | IBUS_CAP_PREEDIT_TEXT);
    }
    else {
        ibus_input_context_set_capabilities (x11ic->context, IBUS_CAP_FOCUS);
    }
#endif

    g_hash_table_insert (ic_ht, GINT_TO_POINTER (simpleic->icid), (gpointer)simpleic);

    simpleic->conn->clients = g_list_append (simpleic->conn->clients, (gpointer)simpleic);

    return True;
}

static int 
xim_destroy_ic_handler(XIMS ims, IMDestroyICStruct *call_data, int *icid)
{
    g_debug ("request: XIM_DESTROY_IC ic=%d connect_id=%d",
             call_data->icid, call_data->connect_id);

    SimpleIC*  simpleic;

    simpleic = (SimpleIC*)g_hash_table_lookup (ic_ht,
                                               GINT_TO_POINTER ((gint) call_data->icid));
    g_return_val_if_fail (simpleic != NULL, False);
#if 0
    if (x11ic->context) {
        ibus_proxy_destroy ((IBusProxy *)x11ic->context);
        g_object_unref (x11ic->context);
        x11ic->context = NULL;
    }
#endif

    g_hash_table_remove (ic_ht, GINT_TO_POINTER ((gint) call_data->icid));
    simpleic->conn->clients = g_list_remove (simpleic->conn->clients, (gconstpointer)simpleic);
#if 0
    g_free (x11ic->preedit_string);
    x11ic->preedit_string = NULL;

    if (x11ic->preedit_attrs) {
        g_object_unref (x11ic->preedit_attrs);
        x11ic->preedit_attrs = NULL;
    }
#endif

    g_slice_free (SimpleIC, simpleic);

    return True;
}

static void
_xim_set_cursor_location (SimpleIC* simpleic)
{
    g_return_if_fail (x11ic != NULL);

    XRectangle preedit_area = simpleic->preedit_area;

    Window w = simpleic->focus_window ?
               simpleic->focus_window : simpleic->client_window;

    if (w) 
    {
        XWindowAttributes xwa;
        Window child;

        XGetWindowAttributes (g_display, w, &xwa);
        if (preedit_area.x <= 0 && preedit_area.y <= 0) 
	{
	    XTranslateCoordinates (g_display, w, xwa.root,
                                   0, xwa.height,
                                   &preedit_area.x, &preedit_area.y,
                                   &child);
        }
        else 
	{
	    XTranslateCoordinates (g_display, w, xwa.root,
				   preedit_area.x, preedit_area.y,
                                   &preedit_area.x, &preedit_area.y,
                                   &child);
        }
    }
#if 0
    ibus_input_context_set_cursor_location (x11ic->context,
            preedit_area.x,
            preedit_area.y,
            preedit_area.width,
            preedit_area.height);
#endif 
}

static int 
xim_set_ic_values_handler (XIMS ims, IMChangeICStruct *call_data, int *icid)
{
    g_debug ("request: XIM_SET_IC_VALUES ic=%d connect_id=%d",
             call_data->icid, call_data->connect_id);

    SimpleIC* simpleic;
    simpleic = (SimpleIC*) g_hash_table_lookup (ic_ht, GINT_TO_POINTER ((gint) call_data->icid));
    g_return_val_if_fail (simpleic != NULL, 0);

    if (_xim_store_ic_values (simpleic, call_data)) 
    {
        _xim_set_cursor_location (simpleic);
    }

    return True;
}

static int 
xim_get_ic_values_handler (XIMS ims, IMChangeICStruct *call_data, int *icid)
{
    g_debug ("request: XIM_GET_IC_VALUES ic=%d connect_id=%d",
             call_data->icid, call_data->connect_id);

    gint i;
    SimpleIC* simpleic;

    simpleic = (SimpleIC*) g_hash_table_lookup (ic_ht,
                                                GINT_TO_POINTER ((gint) call_data->icid));
    g_return_val_if_fail (simpleic!= NULL, 0);

    XICAttribute *ic_attr = call_data->ic_attr;
    for (i = 0; i < (int) call_data->ic_attr_num; ++i, ++ic_attr) 
    {
        if (g_strcmp0 (XNFilterEvents, ic_attr->name) == 0) 
	{
            ic_attr->value = (void *) malloc (sizeof (CARD32));
            *(CARD32 *) ic_attr->value = KeyPressMask | KeyReleaseMask;
            ic_attr->value_length = sizeof (CARD32);
        }
    }

    return False;
}

static int 
xim_forward_event_handler (XIMS ims, IMForwardEventStruct *call_data, int *icid)
{
    g_debug ("request: XIM_FORWARD_EVENT ic=%d connect_id=%d",
             call_data->icid, call_data->connect_id);

    gboolean retval;

    SimpleIC* simpleic;
    simpleic= (SimplIC*) g_hash_table_lookup (ic_ht,
                                              GINT_TO_POINTER ((gint) call_data->icid));
    g_return_val_if_fail (simpleic != NULL, 0);

    XKeyEvent *xevent;
    xevent = (XKeyEvent*) &(call_data->event);
#if 0
    translate_key_event (gdk_display_get_default (),
        (GdkEvent *)&event, (XEvent *)xevent);

    event.send_event = xevent->send_event;
    event.window = NULL;

    if (event.type == GDK_KEY_RELEASE) {
        event.state |= IBUS_RELEASE_MASK;
    }
#endif
    if (_use_sync_mode) 
    {
        retval = ibus_input_context_process_key_event (
                                      x11ic->context,
                                      event.keyval,
                                      event.hardware_keycode - 8,
                                      event.state);
        if (retval) 
	{
            if (! x11ic->has_preedit_area) 
	    {
                _xim_set_cursor_location (x11ic);
            }
            return 1;
        }

        IMForwardEventStruct fe;
        memset (&fe, 0, sizeof (fe));

        fe.major_code = XIM_FORWARD_EVENT;
        fe.icid = x11ic->icid;
        fe.connect_id = x11ic->connect_id;
        fe.sync_bit = 0;
        fe.serial_number = 0L;
        fe.event = call_data->event;

        IMForwardEvent (_xims, (XPointer) &fe);

        retval = 1;
    }
    else  //asychronous mode
    {
        IMForwardEventStruct *pfe;

        pfe = g_slice_new0 (IMForwardEventStruct);
        pfe->major_code = XIM_FORWARD_EVENT;
        pfe->icid = x11ic->icid;
        pfe->connect_id = x11ic->connect_id;
        pfe->sync_bit = 0;
        pfe->serial_number = 0L;
        pfe->event = call_data->event;

        ibus_input_context_process_key_event_async (
                                      x11ic->context,
                                      event.keyval,
                                      event.hardware_keycode - 8,
                                      event.state,
                                      -1,
                                      NULL,
                                      _process_key_event_done,
                                      pfe);
        retval = 1;
    }
    return retval;
}

static int 
xim_set_ic_focus_handler (XIMS ims, IMChangeFocusStruct *call_data, int *icid)
{
    g_debug ("request: XIM_SET_IC_FOCUS ic=%d connect_id=%d",
             call_data->icid, call_data->connect_id);

    SimpleIC* simpleic;
    simpleic = (SimpleIC*) g_hash_table_lookup (ic_ht,
                                                GINT_TO_POINTER ((gint) call_data->icid));
    g_return_val_if_fail (simpleic != NULL, 0);
#if 0
    ibus_input_context_focus_in (x11ic->context);
#endif

    _xim_set_cursor_location (x11ic);

    return True;
}
static int 
xim_unset_ic_focus_handler (XIMS ims, IMChangeFocusStruct *call_data, int *icid)
{
    g_debug ("request: XIM_UNSET_IC_FOCUS ic=%d connect_id=%d",
             call_data->icid, call_data->connect_id);

    SimpleIC* simpleic;
    simpleic = (SimpleIC*) g_hash_table_lookup (ic_ht,
                                                GINT_TO_POINTER ((gint) call_data->icid));
    g_return_val_if_fail (simpleic != NULL, 0);
#if 0
    ibus_input_context_focus_out (x11ic->context);
#endif

    return True;
}

//static int xim_trigger_handler(XIMS ims, IMTriggerNotifyStruct *call_data, int *icid);
//static int xim_sync_reply_handler(XIMS ims, IMSyncXlibStruct *call_data, int *icid)

// inline functions
static inline void 
set_xim_name ()
{
    strncpy(g_xim_name, DEFAULT_XIM_NAME, sizeof(g_xim_name));
}

static inline void 
set_local_ctype ()
{
    strncpy(g_locale_ctype, DEFAULT_LOCALES_CTYPE, sizeof(g_locale_ctype));
}

static inline void 
set_input_styles ()
{
    g_ims_styles.count_styles = sizeof (_ims_styles)/sizeof (XIMStyle) -1;
    g_ims_styles.supported_styles = _ims_styles;
}

static inline void
set_encoding_list ()
{
    g_ims_encodings.count_encodings = sizeof (_ims_encodings)/sizeof (XIMEncoding) -1;
    g_ims_encodings.supported_encodings = _ims_encodings;
}
