#ifndef _MAUD_STRING_H
#define _MAUD_STRING_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <stdbool.h>
#include <ctype.h>
#include <wctype.h>
#ifdef _WIN32
#include <windows.h>
#endif

char* maud_stringtolower(char** string, size_t wlen);
wchar_t* maud_widetolower(wchar_t** wstring, size_t len);
wchar_t* maud_stringtowide(const char* string);
char* maud_widetoutf8(const wchar_t* wstring);
char* maud_strcasestr(char* haystack, char* needle);
char* maud_dupstr(const char* string, size_t len);
char* maud_getutf8_char(const char* utf8_string, size_t* index, size_t utf8_stringlen);
bool maud_concatstr(char** destination_string, const char* source_string);
bool maud_isascii(int c);
#endif