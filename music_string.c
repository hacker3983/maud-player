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

char* mplayer_getutf8_char(const char* utf8_string, size_t* index, size_t utf8_stringlen) {
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
        (*index) += num_bytes;
    }
    return utf8_char;
}

bool mplayer_isascii(int c) {
    return !(c & 0x80);
}