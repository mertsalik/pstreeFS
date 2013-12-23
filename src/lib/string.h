#include <string.h>

/**
 *	Removes character from char array
 *	@param char* str, stack
 *	@param char* garbage, needle
 */
void removeChar(char *str, char garbage) {
    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}

/**
 *	Removes substring from character array
 *	@param char* s, stack
 *	@param const char*, needle
 */
void removeSubstring(char *s, const char *toremove){
        while(s=strstr(s,toremove))
                memmove(s, s+strlen(toremove),1+strlen(s+strlen(toremove)));
}
