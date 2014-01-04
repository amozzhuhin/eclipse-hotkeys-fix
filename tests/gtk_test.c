/**
 * @file gtk_test.c
 * @author Mozzhuhin Andrey (nopscmn@gmail.com)
 *
 * Test program to capture GDK key events in GTK+ application
 */

#include <gtk/gtk.h>
#include <glib.h>
#include <gdk/gdkkeysyms.h>
#include <stdlib.h>
#include <stdio.h>

//
// Private Services
//

static void dump_key_event(GdkEventKey *event)
{
	printf( "    type %d window %p send_event %d\n"
			"    time %u state %08x keyval %u\n"
			"    length %d string \"%s\" hardware_keycode %u\n"
			"    group %u is_modifier %u\n",
			event->type, event->window, event->send_event,
			event->time, event->state, event->keyval,
			event->length, event->string, event->hardware_keycode,
			event->group, event->is_modifier);
}

static gboolean key_press_event_cb(GtkWidget *widget, GdkEventKey *event)
{
	printf("key_press_event\n");
	dump_key_event(event);
	return TRUE;
}

static gboolean key_release_event_cb(GtkWidget *widget, GdkEventKey *event)
{
	printf("key_release_event\n");
	dump_key_event(event);
	return TRUE;
}

//
// Public Services
//

int main(int argc, char *argv[])
{
	GtkWidget *window;
	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_widget_show(window);
	g_signal_connect(window, "key_press_event", G_CALLBACK(key_press_event_cb), NULL);
	g_signal_connect(window, "key_release_event", G_CALLBACK(key_release_event_cb), NULL);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_main();
	return EXIT_SUCCESS;
}
