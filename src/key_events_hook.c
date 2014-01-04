/**
 * @file key_event_hook.c
 * @author Mozzhuhin Andrey (nopscmn@gmail.com)
 *
 * Hook GDK key press/release events and replace non-Latin keyvals
 * with whey Latin equivalents
 */

/* To allow defining of RTLD_NEXT in dlfcn.h */
#define _GNU_SOURCE

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

//
// Private Definitions
//

#if ENABLE_DEBUG
#define PRINT_DEBUG(message, ...)			printf("%s:%d: " message, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define PRINT_DEBUG(message, ...)
#endif

//
// Private Types
//

/**
 * Prototype of gdk_event_handler_set function
 */
typedef void (*GdkEventHandlerSet)(GdkEventFunc func, gpointer data, GDestroyNotify  notify);

//
// Private Variables
//

/**
 * Pointer to original gdk_event_handler_set function
 */
static GdkEventHandlerSet gdk_event_handler_set_orig = NULL;

/**
 * Application handler of GDK events
 */
static GdkEventFunc gdk_event_handler = NULL;

//
// Private Services
//

static gboolean is_latin_keyval(guint keyval)
{
	return ((GDK_0 <= keyval && keyval <= GDK_9)
			|| (GDK_A <= keyval && keyval <= GDK_Z)
			|| (GDK_a <= keyval && keyval <= GDK_z));
}

static void key_events_hook(GdkEvent *event, gpointer data)
{
	if (event->type == GDK_KEY_PRESS || event->type == GDK_KEY_RELEASE)
	{
		if ((event->key.state & GDK_CONTROL_MASK) && !event->key.is_modifier && !is_latin_keyval(event->key.keyval))
		{
			PRINT_DEBUG("non-Latin hotkey event with keyval=%d\n", event->key.keyval);
			GdkKeymapKey *keys;
			guint *keyvals;
			gint n_entries;
			gint level;

			if (gdk_keymap_translate_keyboard_state(NULL, event->key.hardware_keycode, event->key.state,
					event->key.group, NULL, NULL, &level, NULL)
					&& gdk_keymap_get_entries_for_keycode(NULL, event->key.hardware_keycode,
							&keys, &keyvals, &n_entries))
			{
				gint i;
				for (i = 0; i < n_entries; i++)
				{
					if (keys[i].group != event->key.group && keys[i].level == level	&& is_latin_keyval(keyvals[i]))
					{
						// found Latin key - modify event data
						PRINT_DEBUG("found Latin keyval=%d\n", keyvals[i]);
						event->key.group = keys[i].group;
						event->key.keyval = keyvals[i];
						break;
					}
				}
				g_free(keys);
				g_free(keyvals);
			}
		}
	}

	// call user application GDK event handler
	if (gdk_event_handler != NULL)
		gdk_event_handler(event, data);
}

//
// Public Services
//

void gdk_event_handler_set(GdkEventFunc func, gpointer data, GDestroyNotify notify)
{
	PRINT_DEBUG("enter in gdk_event_handler_set(%p, %p, %p)\n", func, data, notify);
	if (gdk_event_handler_set_orig == NULL)
	{
		gdk_event_handler_set_orig = dlsym(RTLD_NEXT, "gdk_event_handler_set");
		PRINT_DEBUG("found original gdk_event_handler_set at %p\n", gdk_event_handler_set_orig);
	}
	// save user application GDK event handler
	gdk_event_handler = func;
	// setup our GDK event handler
	PRINT_DEBUG("setup our GDK event handler\n");
	gdk_event_handler_set_orig(key_events_hook, data, notify);
	PRINT_DEBUG("leave in gdk_event_handler_set\n");
}
