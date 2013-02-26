#include <stdio.h>

#include <X11/Xlib.h>

#include "simplexim.h"



Display* g_display = NULL;
int	 g_screen = 0;
Colormap g_colormap = 0;
int	 g_display_width;
int	 g_display_height;
int	 g_root_window;

int
main (int argc, char** argv)
{
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

}
