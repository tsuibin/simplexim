#include <X11/Xlib.h>

#include <gtk/gtk.h>

#include "dp-xims.h"
#include "dp-xims-window.h"

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
