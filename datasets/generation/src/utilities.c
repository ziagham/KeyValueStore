#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "base64.h"

// Following function extracts characters present in `src`
// between `m` and `n` (excluding `n`)
char* substr(const char *src, int m, int n)
{
    // get the length of the destination string
    int len = n - m;
 
    // allocate (len + 1) chars for destination (+1 for extra null character)
    char *dest = (char*)malloc(sizeof(char) * (len + 1));
 
    // extracts characters between m'th and n'th index from source string
    // and copy them into the destination string
    for (int i = m; i < n && (*(src + i) != '\0'); i++)
    {
        *dest = *(src + i);
        dest++;
    }
 
    // null-terminate the destination string
    *dest = '\0';
 
    // return the destination string
    return dest - len;
}

char* removeSpecialCharacter(char *s)
{
    int j = 0;
    for (int i = 0; i < strlen(s); i++) {
         
        // Store only valid characters
        if ((s[i] >= 'A' && s[i] <= 'Z') ||
            (s[i] >='a' && s[i] <= 'z'))
        {
            s[j] = s[i];
            j++;
        }
    }
    return substr(s, 0, j);
}

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char *repeatStr (char *str, size_t count) {
    if (count == 0) return NULL;
    char *ret = malloc (strlen (str) * count + count);
    if (ret == NULL) return NULL;
    strcpy (ret, str);
    while (--count > 0) {
        strcat (ret, str);
    }
    return ret;
}

char* generateAlphabetKey(char* input)
{
	char *result;
	
	char *numberPart = substr(input, 4, strlen(input));
	
	char *concatNums = repeatStr(numberPart, 2);
	int encodelen = strlen(concatNums);
	
	char *encode_out;
	encode_out = malloc(BASE64_ENCODE_OUT_SIZE(encodelen));

	base64_encode((unsigned char*)concatNums, encodelen, encode_out);
	
	result = removeSpecialCharacter(encode_out);
	free(encode_out);
	
	return result;
}