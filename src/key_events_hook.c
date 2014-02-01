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

/**
 * Key group of Latin layout
 */
static gint latin_key_group = -1;

//
// Private Services
//

/**
 * Determine key group of Latin layout
 * @return Latin key group
 */
static gint find_latin_key_group(void)
{
	gint result = 0;
	GArray *group_keys_count = g_array_new(FALSE, TRUE, sizeof(gint));

	// count all key groups for Latin alphabet
	for (guint keyval = GDK_KEY_a; keyval <= GDK_KEY_z; keyval++)
	{
		GdkKeymapKey* keys;
		gint n_keys;
		if (gdk_keymap_get_entries_for_keyval(gdk_keymap_get_default(), keyval, &keys, &n_keys))
		{
			for (gint key = 0; key < n_keys; key++)
			{
				if (keys[key].group >= 0)
				{
					if ((guint) keys[key].group >= group_keys_count->len)
						g_array_set_size(group_keys_count, keys[key].group + 1);
					g_array_index(group_keys_count, gint, keys[key].group)++;
				}
			}
			g_free(keys);
		}
	}

	// group with maximum keys count is Latin
	int max_keys_count = 0;
	for (guint group = 0; group < group_keys_count->len; group++)
	{
		int keys_count = g_array_index(group_keys_count, gint, group);
		PRINT_DEBUG("group %d has %d Latin keys\n", group, keys_count);
		if (keys_count > max_keys_count)
		{
			result = group;
			max_keys_count = keys_count;
		}
	}

	g_array_free(group_keys_count, TRUE);

	return result;
}

static void on_gdk_keys_changed(GdkKeymap *keymap, gpointer user_data)
{
	PRINT_DEBUG("on_gdk_keys_changed\n");
	latin_key_group = find_latin_key_group();
	PRINT_DEBUG("Latin key group is %d\n", latin_key_group);
}

/**
 * Key events filter. Replace key group and keyval with Latin equivalents when key pressed with Alt or Ctrl modifiers.
 */
static void key_events_hook(GdkEvent *event, gpointer data)
{
	if (event->type == GDK_KEY_PRESS || event->type == GDK_KEY_RELEASE)
	{
		if (latin_key_group == -1)
		{
			latin_key_group = find_latin_key_group();
			PRINT_DEBUG("Latin key group is %d\n", latin_key_group);
			g_signal_connect(G_OBJECT(gdk_keymap_get_default()), "keys-changed", G_CALLBACK(on_gdk_keys_changed), NULL);
		}
		if ((event->key.state & (GDK_CONTROL_MASK | GDK_MOD1_MASK)) && !event->key.is_modifier
				&& event->key.group != latin_key_group)
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
					PRINT_DEBUG("group: %d, level: %d, keycode: %d\n", keys[i].group, keys[i].level, keys[i].keycode);
					if (keys[i].group == latin_key_group && keys[i].level == level)
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

/**
 * Hook of GDK gdk_event_handler_set function
 */
void gdk_event_handler_set(GdkEventFunc func, gpointer data, GDestroyNotify notify)
{
	PRINT_DEBUG("enter in gdk_event_handler_set(%p, %p, %p)\n", func, data, notify);
	if (gdk_event_handler_set_orig == NULL)
	{
		// search for original 'gdk_event_handler_set'
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
