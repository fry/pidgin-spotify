#define PURPLE_PLUGINS

#include <glib.h>

#include "notify.h"
#include "plugin.h"
#include "version.h"

#include "SpotifyInfo.hpp"

// handle to the libpurple timer
guint spotify_handle;

SpotifyInfo spotify;
PurplePlugin* spotify_plugin = NULL;

// repeatedly called procedure that polls spotify for the song and modifies the status
static gboolean spotify_poll(gpointer data) {
	if (spotify.refresh()) {
		// magic UTF8 conversion
		gchar* artist = g_locale_to_utf8(spotify.Artist.c_str(), spotify.Artist.length(), NULL, NULL, NULL);
		gchar* title = g_locale_to_utf8(spotify.Title.c_str(), spotify.Title.length(), NULL, NULL, NULL);
		purple_util_set_current_song(title, artist, NULL);
		g_free(artist);
		g_free(title);
	} else {
		purple_util_set_current_song(NULL, NULL, NULL);
	}

	return TRUE;
}

static gboolean plugin_load(PurplePlugin *plugin) {
	guint poll_interval = 3000;
	spotify_plugin = plugin;

	// register the timeout
	spotify_handle = purple_timeout_add(poll_interval, spotify_poll, NULL);

	return TRUE;
}

static gboolean plugin_unload(PurplePlugin * plugin) {
	purple_timeout_remove(spotify_handle);
	// clear the currently playing song
	purple_util_set_current_song (NULL, NULL, NULL);
	return TRUE;
}

static void init_plugin(PurplePlugin *plugin) {                                  
}

static PurplePluginInfo info = {
    PURPLE_PLUGIN_MAGIC,
    PURPLE_MAJOR_VERSION,
    PURPLE_MINOR_VERSION,
    PURPLE_PLUGIN_STANDARD,
    NULL,
    0,
    NULL,
    PURPLE_PRIORITY_DEFAULT,

    "core-spotify-windows-scrobbler",
    "Spotify Scrobbler",
    "0.1",

    "Scrobbles Spotify",          
    "Sets the 'Now Listening' status message to Spotify's current track.",
    "Eike Siewertsen <e.siew@londeroth.org>",
    "http://londeroth.org",     
    
    plugin_load,                   
    plugin_unload,                          
    NULL,                          
                                   
    NULL,                          
    NULL,                          
    NULL,                        
    NULL,                   
    NULL,                          
    NULL,                          
    NULL,                          
    NULL                           
};                               
    
extern "C" {
	PURPLE_INIT_PLUGIN(hello_world, init_plugin, info)
}
