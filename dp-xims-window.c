#include <X11/Xlib.h>

#include <gdk/gdkx.h>
#include <gtk/gtk.h>

#include "dp-xims.h"
#include "dp-xims-window.h"

GdkWindow*
dp_xims_window_new ()
{
    GdkWindowAttr window_attr = {
        .title              = "dp-xim",
        .event_mask         = GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK,
        .wclass             = GDK_INPUT_OUTPUT,
        .window_type        = GDK_WINDOW_TOPLEVEL,
        .override_redirect   = 1,
    };
    GdkWindow *win;
    win = gdk_window_new (NULL, &window_attr, GDK_WA_TITLE);
    return win;
}

Window
dp_xims_window_get_xid (GdkWindow* win)
{
    return gdk_x11_window_get_xid (win);
}

#if 0
void
create_server_window ()
{
    //TODO:
    unsigned long mask=0;
    mask = CWEventMask | CWOverrideRedirect;
    XSetWindowAttributes attr;
    attr.event_mask = KeyPressMask | KeyReleaseMask;
    attr.override_redirect = True;
    g_server_window = XCreateWindow (g_display, g_root_window, 
	                             1, 1, 1, 1, 
				     0, CopyFromParent,
	                             InputOutput, CopyFromParent, 
				     mask, &attr);
    XMapWindow(g_display, g_server_window);
    XFlush(g_display);

}
#endif

//an event loop
void 
dp_event_loop ()
{
    gtk_main ();
#if 0
    XEvent event;

    while(1) 
    {
	//1. check exit status
	
	//2
        XNextEvent(g_display, &event);
	if (XFilterEvent(&event, None) == True)
            continue;

	g_debug ("we got events: %d", event.type);
	switch (event.type)
       	{
	case Expose:
	    break;
	case GraphicsExpose:
	    break;
	case ConfigureNotify:
	    break;
	case ClientMessage:
	    break;
        }
    }
#endif
}
