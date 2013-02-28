#include <stdio.h>

#include <X11/Xlib.h>
#include <gtk/gtk.h>

#include "dp-xims.h"
#include "dp-xims-window.h"

int 
main (int argc, char **argv)
{
    gtk_init (&argc, &argv);
    g_setenv ("G_MESSAGES_DEBUG", "all", FALSE);

    dp_xims_init ();
//event loop
    dp_event_loop ();
}


#if 0
Display* g_display = NULL;
int	 g_screen = 0;
Colormap g_colormap = 0;
int	 g_display_width;
int	 g_display_height;
int	 g_root_window;

int
main (int argc, char** argv)
{
    g_setenv ("G_MESSAGES_DEBUG", "all", FALSE);
    g_display = XOpenDisplay (NULL);
    if (!g_display)
    {
	fprintf (stderr, "Cannot Open Display");
    }
    g_screen = DefaultScreen(g_display);
    g_colormap = DefaultColormap(g_display, g_screen);
    g_display_width = DisplayWidth(g_display, g_screen);
    g_display_height = DisplayHeight(g_display, g_screen);
    g_root_window = RootWindow(g_display, g_screen);

    xim_init ();
    event_loop ();

    return 0;
}
#endif
