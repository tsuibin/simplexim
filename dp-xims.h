#ifndef _DP_XIMS_H_
#define _DP_XIMS_H_

#include <X11/Xlib.h>
//IMdkit
#include <IMdkit.h>
#include <Xi18n.h>

#include <gtk/gtk.h>
/*
 *	Macros
 */
#define DEFAULT_MODIFIERS	"Xi18n"
#define DEFAULT_SERVER_NAME	"dp-xims"
#define DEFAULT_LOCALE		"en,zh"
#define DEFAULT_SERVER_TRANSPORT "X/"	//so XIM events are processed at XNextEvent.

/*
 *	type definitions
 */
//0. global data 
typedef struct _DpXIMServerData DpXIMServerData;
struct _DpXIMServerData
{
    XIMS	xims;
    GdkDisplay* gdk_display;
    GdkWindow*  server_window;
};

//1. a client connection to the IM server
//   a list of client Input Contexts for this connection.
typedef struct _DpXIMConnection DpXIMConnection;
struct _DpXIMConnection
{
    GList* client_ics;   
};

//2. a Input Context created by the IM server for a specific client
typedef struct _DpXIMIC DpXIMIC;
struct _DpXIMIC
{
    //1. connection
    DpXIMConnection*	connection; //the connection this IC belongs
    gint                connect_id; //the connection ID

    //2. 
    gint                ic_id;	//an identifier assigned by the IM server for this IC

    //3. IC attributes
    gint32           input_style;  //XNInputStyle
    Window           client_window;//XNClientWindow
    Window           focus_window; //XNFocusWindow

    //4. Preedit attributes
    gboolean         has_preedit_area;
    XRectangle	     preedit_area;
    gint             preedit_cursor;
    gboolean         preedit_started;
    gboolean         preedit_visible;
    gint             onspot_preedit_length;
    gchar           *preedit_string;
    //others
    //gchar           *lang;
};
/*
 *	global variables	
 */
DpXIMServerData* dp_xims_data;
//
extern void dp_xims_init ();

#endif //_DP_XIMS_H_
