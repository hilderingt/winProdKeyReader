#include <string.h>
#include <windows.h>

#include "prodkey.h"

static const char key_char[] = { 'B','C','D','F','G','H','J','K',
                                 'M','P','Q','R','T','V','W','X',
                                 'Y','2','3','4','6','7','8','9' };

extern DWORD prodkey_read(LPBYTE regdata)
{
    DWORD size = REGKEY_DATA_SIZE, err;
    LPVOID p_new;
    HKEY hd;    

    if ((err = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
                            REGKEY_SUBKEY,
                            0,
                            KEY_QUERY_VALUE | KEY_WOW64_64KEY,
                            &hd)) != ERROR_SUCCESS)
        return (err);

    regdata = (LPBYTE)malloc(REGKEY_DATA_SIZE);

    if (regdata == NULL)
        return (errno);

    if ((err = RegQueryValueEx(hd,
                               REGKEY_DATA_NAME,
                               NULL,
                               NULL,
                               regdata,
                               &size)) != ERROR_SUCCESS) {
        free(regdata);
        return (err);
    }

    memmove(regdata, &regdata[REGKEY_DATA_START], KEY_ENCODED_SIZE);
    p_new = realloc(regdata, KEY_ENCODED_SIZE);

    if (p_new == NULL)
        return (errno);    

    return (ERROR_SUCCESS);
}

#define BYTE_BIT 8

extern char* prodkey_decode(LPCBYTE encoded, LPSTR decoded)
{    
    unsigned char key[KEY_ENCODED_SIZE];
    unsigned int ch;
    int iter, byte;
    
    *(decoded += KEY_DECODED_STRLEN) = '\0';
    iter = KEY_DECODED_ALNUM_STRLEN - 1;

    memcpy(key, encoded, KEY_ENCODED_SIZE);    

    do {        
        byte = KEY_ENCODED_SIZE - 1;
        ch = 0;

        do {
            ch = ch << BYTE_BIT ^ key[byte];
            key[byte] = ch / sizeof(key_char);
            ch = ch % sizeof(key_char);
        } while (--byte >= 0);      

        *--decoded = key_char[ch];

        if (iter != 0 && iter % KEY_DECODED_PARTSZ == 0)
            *--decoded = '-';
    } while (--iter >= 0);

    return (decoded);
}