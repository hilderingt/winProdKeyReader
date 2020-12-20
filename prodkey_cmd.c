#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <stdio.h>

#include "prodkey.h"

int main(int argc, char** argv)
{
    char decoded[KEY_DECODED_STRLEN + 1];
    LPBYTE encoded;
    DWORD err;
    
    if ((err = prodkey_read(encoded)) != ERROR_SUCCESS)
        on_error(err);    

    puts(prodkey_decode(encoded, decoded));
    free(encoded);

    return (0);
}

static void on_error(DWORD err) 
{
    LPTSTR msg;

    if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                      FORMAT_MESSAGE_FROM_SYSTEM | 
                      FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL,
                      err,
                      0,
                      (LPTSTR)&msg,
                      0,
                      NULL) == 0)
        exit(GetLastError());

    wprintf(L"System Error Code: 0x%04x\n%ls", err, msg);

    exit(err);
}