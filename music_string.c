#include "music_string.h"

char* mplayer_stringtolower(char** string, size_t wlen) {
    char* new_string = calloc(wlen+1, sizeof(char));
    for(size_t i=0;i<wlen;i++) {
        new_string[i] = tolower((*string)[i]);
    }
    free(*string); *string = new_string;
    return new_string;
}

wchar_t* mplayer_widetolower(wchar_t** wstring, size_t len) {
    wchar_t* new_wstring = calloc(len+1, sizeof(wchar_t));
    for(size_t i=0;i<len;i++) {
        new_wstring[i] = towlower((*wstring)[i]);
    }
    free(*wstring); *wstring = new_wstring;
    return new_wstring;
}

wchar_t* mplayer_stringtowide(const char* string) {
    if(!string) {
        printf("From mplayer string to wide(): the string give as a parameter is NULL\n");
        return NULL;
    }
    size_t wstr_len = mbstowcs(NULL, string, 0)+1; // get the length of the string in wide characters
    size_t str_len = strlen(string);
    if(wstr_len == -1) {
        printf("mbstowcs(): failed at first\n");
        wstr_len = str_len;
    }
    wchar_t* wstring = calloc(wstr_len+1, sizeof(wchar_t));
    size_t ret = mbstowcs(wstring, string, wstr_len);
    if(ret == -1) {
        printf("mbstowc(): failed again\n");
        wchar_t wc = 0;
        for(size_t i=0;i<wstr_len;i++) {
            if(mbtowc(&wc, &string[i], 1) < 0) {
                printf("Failed to convert to from string to wide as mbtowc() failed\n");
            }
            wstring[i] = wc;
        }
    }
    return wstring;
}

char* mplayer_widetoutf8(wchar_t* wstring) {
    size_t len_wstr = 0;
    char* string = NULL;
    #ifdef _WIN32
    len_wstr = WideCharToMultiByte(CP_UTF8, 0, wstring, -1, NULL, 0, NULL, NULL);
    string = calloc(len_wstr+1, sizeof(char));
    WideCharToMultiByte(CP_UTF8, 0, wstring, -1, string, len_wstr, NULL, NULL);
    #else
    len_wstr = wcstombs(NULL, wstring, 0);
    string = calloc(len_wstr+1, sizeof(char));
    wcstombs(string, wstring, len_wstr);
    #endif
    return string;
}

char* mplayer_strcasestr(char* haystack, char* needle) {
    size_t haystack_len = 0, needle_len = 0, match_substrlen = 0;
    if(!haystack) {
        return NULL;
    }
    if(!needle) {
        return NULL;
    }
    haystack_len = strlen((char*)haystack), needle_len = strlen((char*)needle);
    char *haystack_dup = (char*)mplayer_dupstr(haystack, haystack_len),
         *needle_dup = (char*)mplayer_dupstr(needle, needle_len);
    mplayer_stringtolower(&haystack_dup, haystack_len);
    mplayer_stringtolower(&needle_dup, needle_len);
    char* match_substr = strstr(haystack_dup, needle_dup);
    if(match_substr) {
        match_substrlen = strlen(match_substr);
        match_substr = (char*)mplayer_dupstr(match_substr, match_substrlen);
    }
    free(haystack_dup); haystack_dup = NULL;
    free(needle_dup); needle_dup = NULL;
    return match_substr;
}

char* mplayer_dupstr(const char* string, size_t len) {
    if(!string) {
        return NULL;
    }
    char *new_dupstr = (char*)calloc(len+1, sizeof(char));
    strncpy(new_dupstr, (char*)string, len);
    return new_dupstr;
}

char* mplayer_getutf8_charunixlike(const char* utf8_string, size_t* index, size_t utf8_stringlen) {
    mbstate_t state = {0};
    wchar_t widechar = L'\0';
    int num_bytes = 0;
    char* utf8_char = NULL;
    if(*index < utf8_stringlen) {
        num_bytes = mbrtowc(&widechar, utf8_string+(*index), utf8_stringlen, &state);
        utf8_char = (num_bytes) ? (char*)malloc(num_bytes+1) : NULL;
    }
    if(utf8_char) {
        strncpy(utf8_char, utf8_string+(*index), num_bytes);
        utf8_char[num_bytes] = '\0';
        (*index) += num_bytes-1;
    }
    return utf8_char;
}

char* mplayer_getutf8_charwindows(const char* utf8_string, size_t* index, size_t utf8_stringlen) {
    wchar_t wchar = L'\0';
    int num_bytes = 0;
    char* utf8_char = NULL;
    if(*index >= utf8_stringlen) {
        return NULL;
    }
    // Determine the length of the UTF-8 character
    if ((utf8_string[*index] & 0x80) == 0) {
        num_bytes = 1; // 1 byte character (ASCII)
    } else if ((utf8_string[*index] & 0xE0) == 0xC0) {
        num_bytes = 2; // 2 byte character
    } else if ((utf8_string[*index] & 0xF0) == 0xE0) {
        num_bytes = 3; // 3 byte character
    } else if ((utf8_string[*index] & 0xF8) == 0xF0) {
        num_bytes = 4; // 4 byte character
    } else {
        return NULL; // Invalid UTF-8 start byte
    }
    utf8_char = (char*)malloc(num_bytes+1);
    if(utf8_char) {
        strncpy(utf8_char, utf8_string+(*index), num_bytes);
        utf8_char[num_bytes] = '\0';
    }
    (*index) += num_bytes-1;
    return utf8_char;
}

char* mplayer_getutf8_char(const char* utf8_string, size_t* index, size_t utf8_stringlen) {
    char* utf8_char = NULL;
    #ifdef _WIN32
    utf8_char = mplayer_getutf8_charwindows(utf8_string, index, utf8_stringlen);
    #else
    utf8_char = mplayer_getutf8_charunixlike(utf8_string, index, utf8_stringlen);
    #endif
    return utf8_char;
}

bool mplayer_isascii(int c) {
    return !(c & 0x80);
}

bool mplayer_concatstr(char** destination_string, const char* source_string) {
    size_t destination_stringlen = 0, source_stringlen = 0;
    if(!(*destination_string) && !source_string) {
        return true;
    }
    if(source_string) {
        source_stringlen = strlen(source_string);
    }
    if(*destination_string) {
        destination_stringlen = strlen(*destination_string);
    }
    size_t new_size = source_stringlen + destination_stringlen;
    char* new_ptr = (char*)realloc(*destination_string, new_size+1);
    if(!new_ptr) {
        return false;
    }
    strncpy(new_ptr+destination_stringlen, source_string, source_stringlen);
    new_ptr[new_size] = '\0';
    *destination_string = new_ptr;
    return true;
}