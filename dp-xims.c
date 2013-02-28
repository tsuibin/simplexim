#include <X11/Xproto.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <XimProto.h>
#include <IMdkit.h>
#include <Xi18n.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <langinfo.h>
#include <iconv.h>
#include <signal.h>
#include <stdlib.h>
#include <getopt.h>

static int
xim_create_ic (XIMS xims, IMChangeICStruct *call_data)
{
    static int base_icid = 1;
    call_data->icid = base_icid ++;
    g_debug("XIM_CREATE_IC ic=%d connect_id=%d", call_data->icid, call_data->connect_id);
    return 1;
}
static int
xim_destroy_ic (XIMS xims, IMChangeICStruct *call_data)
{
    g_debug("XIM_DESTROY_IC ic=%d connect_id=%d", call_data->icid, call_data->connect_id);
    return 1;
}
static int
xim_set_ic_focus (XIMS xims, IMChangeFocusStruct *call_data)
{
    g_debug ("XIM_SET_IC_FOCUS ic=%d connect_id=%d", call_data->icid, call_data->connect_id);
    return 1;
}
static int
xim_unset_ic_focus (XIMS xims, IMChangeFocusStruct *call_data)
{
    g_debug("XIM_UNSET_IC_FOCUS ic=%d connect_id=%d", call_data->icid, call_data->connect_id);
    return 1;
}
static int
xim_forward_event (XIMS xims, IMForwardEventStruct *call_data)
{
    g_debug("XIM_FORWARD_EVENT ic=%d connect_id=%d", call_data->icid, call_data->connect_id);
    return 1;
}
static int
xim_open (XIMS xims, IMOpenStruct *call_data)
{
    g_debug("XIM_OPEN connect_id=%d", call_data->connect_id);
    return 1;
}
static int
xim_close (XIMS ims, IMCloseStruct *call_data)
{
    g_debug("XIM_CLOSE connect_id=%d", call_data->connect_id);
    return 1;
}
static int
xim_set_ic_values (XIMS xims, IMChangeICStruct *call_data)
{
    g_debug("XIM_SET_IC_VALUES ic=%d connect_id=%d", call_data->icid, call_data->connect_id);
    return 1;
}
static int
xim_get_ic_values (XIMS xims, IMChangeICStruct *call_data)
{
    g_debug("XIM_GET_IC_VALUES ic=%d connect_id=%d", call_data->icid, call_data->connect_id);
    return 1;
}
static int
xim_reset_ic (XIMS xims, IMResetICStruct *call_data)
{
    g_debug("XIM_RESET_IC ic=%d connect_id=%d", call_data->icid, call_data->connect_id);
    return 1;
}
int
ims_protocol_handler (XIMS xims, IMProtocol *call_data)
{
    g_debug ("ims_protocol_handler");
    g_return_val_if_fail (xims != NULL, 1);
    g_return_val_if_fail (call_data != NULL, 1);

    switch (call_data->major_code) {
    case XIM_OPEN:
        return xim_open (xims, (IMOpenStruct *)call_data);
    case XIM_CLOSE:
        return xim_close (xims, (IMCloseStruct *)call_data);
    case XIM_CREATE_IC:
        return xim_create_ic (xims, (IMChangeICStruct *)call_data);
    case XIM_DESTROY_IC:
        return xim_destroy_ic (xims, (IMChangeICStruct *)call_data);
    case XIM_SET_IC_VALUES:
        return xim_set_ic_values (xims, (IMChangeICStruct *)call_data);
    case XIM_GET_IC_VALUES:
        return xim_get_ic_values (xims, (IMChangeICStruct *)call_data);
    case XIM_FORWARD_EVENT:
        return xim_forward_event (xims, (IMForwardEventStruct *)call_data);
    case XIM_SET_IC_FOCUS:
        return xim_set_ic_focus (xims, (IMChangeFocusStruct *)call_data);
    case XIM_UNSET_IC_FOCUS:
        return xim_unset_ic_focus (xims, (IMChangeFocusStruct *)call_data);
    case XIM_RESET_IC:
        return xim_reset_ic (xims, (IMResetICStruct *)call_data);
    case XIM_TRIGGER_NOTIFY:
        g_debug("XIM_TRIGGER_NOTIFY");
        return 0;
    case XIM_PREEDIT_START_REPLY:
        g_debug("XIM_PREEDIT_START_REPLY");
        return 0;
    case XIM_PREEDIT_CARET_REPLY:
        g_debug("XIM_PREEDIT_CARET_REPLY");
        return 0;
    case XIM_SYNC_REPLY:
        g_debug("XIM_SYNC_REPLY");
        return 0;
    default:
        g_debug("Unknown (%d)", call_data->major_code);
        return 0;
    }
}

void
dp_xims_init ()
{
    XIMStyle ims_styles_onspot [] = {
        XIMPreeditPosition  | XIMStatusNothing,
        XIMPreeditCallbacks | XIMStatusNothing,
        XIMPreeditNothing   | XIMStatusNothing,
        XIMPreeditPosition  | XIMStatusCallbacks,
        XIMPreeditCallbacks | XIMStatusCallbacks,
        XIMPreeditNothing   | XIMStatusCallbacks,
        0
    };

    XIMEncoding ims_encodings[] = {
        "COMPOUND_TEXT",
        0
    };

    GdkWindowAttr window_attr = {
        .title              = "ibus-xim",
        .event_mask         = GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK,
        .wclass             = GDK_INPUT_OUTPUT,
        .window_type        = GDK_WINDOW_TOPLEVEL,
        .override_redirect   = 1,
    };

    XIMStyles styles;
    XIMEncodings encodings;

    GdkWindow *win;

    win = gdk_window_new (NULL, &window_attr, GDK_WA_TITLE);

    styles.count_styles =
        sizeof (ims_styles_onspot)/sizeof (XIMStyle) - 1;
    styles.supported_styles = ims_styles_onspot;

    encodings.count_encodings =
        sizeof (ims_encodings)/sizeof (XIMEncoding) - 1;
    encodings.supported_encodings = ims_encodings;

    g_debug("IMOpenIM");
    IMOpenIM(GDK_DISPLAY_XDISPLAY (gdk_display_get_default ()),
        IMModifiers, "Xi18n",
        IMServerWindow, gdk_x11_window_get_xid(win),
        IMServerName, "ibus",
        IMLocale, "en,zh",
        IMServerTransport, "X/",
        IMInputStyles, &styles,
        IMEncodingList, &encodings,
        IMProtocolHandler, ims_protocol_handler,
        IMFilterEventMask, KeyPressMask | KeyReleaseMask,
        NULL);
}
