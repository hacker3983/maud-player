#ifndef _MUSIC_STRING_H
#define _MUSIC_STRING_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <stdbool.h>
#ifdef _WIN32
#include <windows.h>
#endif

char* mplayer_stringtolower(char** string, size_t wlen);
wchar_t* mplayer_widetolower(wchar_t** wstring, size_t len);
wchar_t* mplayer_stringtowide(const char* string);
char* mplayer_widetoutf8(wchar_t* wstring);
char* mplayer_strcasestr(char* haystack, char* needle);
char* mplayer_dupstr(const char* string, size_t len);
char* mplayer_getutf8_char(const char* utf8_string, size_t* index, size_t utf8_stringlen);
bool mplayer_isascii(int c);
#endif