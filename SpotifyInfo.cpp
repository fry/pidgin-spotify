#include "SpotifyInfo.hpp"
#include <tlhelp32.h>
#include <iostream>

BOOL CALLBACK enum_windows_proc(HWND hwnd, LPARAM lParam) {
	DWORD process_id;
	GetWindowThreadProcessId(hwnd, &process_id);

	SpotifyInfo* info = reinterpret_cast<SpotifyInfo*>(lParam);
	if (info->ProcessID == process_id) {
		char buffer[512];
		if (GetClassName(hwnd, buffer, 512) && strcmp(buffer, "SpotifyMainWindow") == 0) {
			info->Hwnd = hwnd;
			return false;
		}
	}

	return true;
}

SpotifyInfo::SpotifyInfo(): Hwnd(NULL), ProcessID(0), IsPlaying(false) {}

bool SpotifyInfo::parse_title(const std::string& window_title) {
	WindowTitle = window_title;
	// that special EN DASH separating artist and title
	std::size_t dash_index = window_title.find_last_of("\x96");

	if (dash_index == std::string::npos) {
		Artist = "";
		Title = "";
		IsPlaying = false;
		return false;
	}
	// skip the space at the end of the artist
	Artist = window_title.substr(10, dash_index - 1 - 10);
	// skip the dash and space at the start of the title
	Title = window_title.substr(dash_index + 2);
	IsPlaying = true;
	return true;
}

bool SpotifyInfo::find_process() {
	ProcessID = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process;
	process.dwSize = sizeof(PROCESSENTRY32);

	bool success = Process32First(snapshot, &process);

	while (success) {
		if (strstr(process.szExeFile, "spotify.exe")) {
			ProcessID = process.th32ProcessID;
			CloseHandle(snapshot);
			return true;
		}
		success = Process32Next(snapshot, &process);
	} 

	CloseHandle(snapshot);

	return false;
}

bool SpotifyInfo::find_window() {
	Hwnd = NULL;
	EnumWindows(enum_windows_proc, (LPARAM)this);
	return Hwnd != NULL;
}

bool SpotifyInfo::refresh() {
	if (ProcessID == 0 && !find_process())
		return false;

	if (Hwnd == NULL && !find_window())
		return false;

	char buffer[512];
	GetWindowText(Hwnd, buffer, 512);
	return parse_title(buffer);
}
