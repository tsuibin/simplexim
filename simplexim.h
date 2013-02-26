/*
 *	
 */
#ifndef _SIMPLE_XIM_H
#define _SIMPLE_XIM_H

//type defs
//1. a client connection to the IM server
typedef struct _SimpleConn SimpleConn;
struct _SimpleConn
{
    GList* client_ics;   // a list of client Input Contexts for this connection.
};
//2. a Input Context created by the IM server for a specific client
typedef struct _SimpleIC SimpleIC;
struct _SimpleIC
{
    //1. connection
    SimpleConn*	     conn;	//the connection this IC belongs
    gint             connect_id;//the connection ID

    //2. 
    gint             icid;	//an identifier assigned by the IM server for this IC

    //3. IC attributes
    gint32           input_style;  //XNInputStyle
    Window           client_window;//XNClientWindow
    Window           focus_window; //XNFocusWindow

    //4. Preedit attributes
    gboolean         has_preedit_area;
    XRectangle	     preedit_area;

    //others
    IBusInputContext *context;
    gchar           *lang;

    gchar           *preedit_string;
    IBusAttrList    *preedit_attrs;
    gint             preedit_cursor;
    gboolean         preedit_visible;
    gboolean         preedit_started;
    gint             onspot_preedit_length;
};

//Globals
extern Display* g_display;
extern int g_screen; //default screen
extern Colormap g_colormap; //default colormap
extern int g_display_width;
extern int g_display_height;
extern int g_root_window;

//IM related globals
#define DEFAULT_XIM_NAME "simplexim"
extern XIMS g_xims;
extern Window g_server_window;// window created by XIM server
#define DEFAULT_LOCALES_CTYPE "en,zh"
#define DEFAULT_XIM_TRANSPORT "X/"


//simplexim.c
extern void xim_init ();
//simplewindow.c
extern void create_server_window ();

#endif
