#include <X11/Xlib.h>

#include "simplexim.h"


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

}


//an event loop
void 
event_loop ()
{
    XEvent event;

    while(1) 
    {
	//1. check exit status
	
	//2
        XNextEvent(g_display, &event);
	if (XFilterEvent(&event, None) == True)
            continue;

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
}
