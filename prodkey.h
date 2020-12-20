#ifndef __PRODKEY_H__
#define __PRODKEY_H__

#ifdef UNICODE
#define REGKEY_SUBKEY              L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"
#define REGKEY_DATA_NAME           L"DigitalProductId"
#else
#define REGKEY_SUBKEY              "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"
#define REGKEY_DATA_NAME           "DigitalProductId"
#endif /* UNICODE */

#define REGKEY_DATA_SIZE           164
#define REGKEY_DATA_START           52

#define KEY_DECODED_ALNUM_STRLEN    25
#define KEY_DECODED_STRLEN          29
#define KEY_ENCODED_SIZE            15
#define KEY_DECODED_PARTSZ           5

#ifdef __cplusplus
extern "C" {
#endif

extern DWORD prodkey_read(LPBYTE);
extern LPSTR prodkey_decode(LPCBYTE, LPSTR);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PRODKEY_H__ */