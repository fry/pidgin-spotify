#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

struct SpotifyInfo {
public:
	DWORD ProcessID;
	HWND Hwnd;
	std::wstring WindowTitle;
	std::string Artist;
	std::string Title;
	bool IsPlaying;

	SpotifyInfo();

	// Parse "Spotify - Artist <EN DASH> Title"
	bool parse_title(const std::wstring& window_title);
	bool refresh();

protected:
	bool find_process();
	bool find_window();
};
