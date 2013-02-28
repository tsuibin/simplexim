/*
 *	
 */
#include <gtk/gtk.h>

#include "dp-xims.h"
#include "dp-xims-window.h"

int 
main (int argc, char **argv)
{
    gtk_init (&argc, &argv);
    g_setenv ("G_MESSAGES_DEBUG", "all", FALSE);

    dp_xims_init ();

    dp_event_loop ();
}

