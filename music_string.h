#ifndef _MUSIC_STRING_H
#define _MUSIC_STRING_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

char* mplayer_stringtolower(char** string, size_t wlen);
wchar_t* mplayer_widetolower(wchar_t** wstring, size_t len);
wchar_t* mplayer_stringtowide(const char* string);
char* mplayer_widetoutf8(wchar_t* wstring);
char* mplayer_strcasestr(char* haystack, char* needle);
char* mplayer_dupstr(char* string, size_t len);
#endif