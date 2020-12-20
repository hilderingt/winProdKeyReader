#ifndef UNICODE
#define UNICODE
#endif

#include <windowsx.h>
#include <windows.h>
#include <stdint.h>
#include <stdio.h>

#include "prodkey.h"

#define IDC_BUTTON_READ  1
#define IDC_BUTTON_COPY  2
#define IDC_BUTTON_SAVE  3
#define IDC_STATIC_LABEL 4
#define IDC_EDIT_OUTPUT  5

LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
static void CopyToClipboard(HWND, LPCSTR);
static void SaveToFile(LPCSTR);
static LPBYTE ReadProductKey();
static void OnError(DWORD);
static int InitCtrl(HWND);

HINSTANCE g_instance = NULL;
HWND edit_output;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance,
                   LPSTR args, int show)
{
    WNDCLASSEX window_class;
    HANDLE mutex;
    HWND window;    
    HICON icon;    
    MSG msg;
    
    mutex = CreateMutexA(NULL, FALSE, "2f4t9l5");
    
    if (mutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
        OnError(GetLastError());
    
    g_instance = instance;    
    icon = LoadIcon(instance, MAKEINTRESOURCE(2));
    
    window_class.cbSize        = sizeof(WNDCLASSEX);
    window_class.style         = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc   = MainWndProc;
    window_class.cbClsExtra    = 0;
    window_class.cbWndExtra    = 0;
    window_class.hInstance     = instance;
    window_class.hIcon         = icon;
    window_class.hCursor       = LoadCursor(NULL, IDC_ARROW);
    window_class.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
    window_class.lpszMenuName  = NULL;
    window_class.lpszClassName = L"MainWindow";
    window_class.hIconSm       = icon;
    
    if (RegisterClassEx(&window_class) == 0)
        OnError(GetLastError());    
    
    if ((window = CreateWindowEx(WS_EX_APPWINDOW,
                                 L"MainWindow",
                                 L"Windows ProductKey Reader",
                                 WS_OVERLAPPED | WS_SYSMENU,
                                 CW_USEDEFAULT, 
                                 CW_USEDEFAULT, 
                                 315, 
                                 95,
                                 NULL, 
                                 NULL, 
                                 instance, 
                                 NULL)) == NULL)
        OnError(GetLastError());

    ShowWindow(window, show);
    UpdateWindow(window);
    
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (msg.wParam);
}

LRESULT CALLBACK MainWndProc(HWND window, UINT msg, WPARAM wparam,
                             LPARAM lparam)
{
    static char key[KEY_DECODED_STRLEN];
    static BOOL read_key = FALSE;
    static LPBYTE data;    

    switch(msg) {
        case WM_CREATE:            
            InitCtrl(window);
            break;
        case WM_COMMAND:
            switch (LOWORD(wparam)) {
                case IDC_BUTTON_READ:
                    switch (HIWORD(wparam)) {
                        case BN_CLICKED:
                            if (read_key == FALSE && 
                                (data = ReadProductKey()) != NULL) {
                                prodkey_decode(data + REGKEY_DATA_START, key);
                                SetWindowTextA(edit_output, key);
                                read_key = TRUE;
                            }
                        default:
                            return DefWindowProc(window, msg, wparam, lparam);
                    }
                    break;
                case IDC_BUTTON_COPY:
                    switch (HIWORD(wparam)) {
                        case BN_CLICKED:
                            if (read_key == TRUE)
                                CopyToClipboard(window, key);
                            break;                            
                        default:
                            return DefWindowProc(window, msg, wparam, lparam);
                    }
                    break;
                case IDC_BUTTON_SAVE:
                    switch (HIWORD(wparam)) {
                        case BN_CLICKED:
                            if (read_key == TRUE)
                                SaveToFile(key);
                            break;                            
                        default:
                            return DefWindowProc(window, msg, wparam, lparam);
                    }
                    break;
            }
            break;
        case WM_CLOSE:
            DestroyWindow(window);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(window, msg, wparam, lparam);
    }
    
    return (0);
}

static int InitCtrl(HWND parent)
{
    HFONT font;
    HWND button_read;    
    HWND button_copy;
    HWND button_save;
    HWND static_label;
    HICON image;

    font = GetStockObject(DEFAULT_GUI_FONT);                                                   
        
    button_read = CreateWindow(L"BUTTON", NULL,
                               WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_ICON,
                               226, 29, 22, 22, parent, (HMENU)IDC_BUTTON_READ, g_instance, NULL);

    image = LoadImage(g_instance, MAKEINTRESOURCE(3), IMAGE_ICON, 16, 16, 0);
    SendMessage(button_read, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)image);
    
    button_copy = CreateWindow(L"BUTTON", NULL,
                               WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_ICON,
                               250, 29, 22, 22, parent, (HMENU)IDC_BUTTON_COPY, g_instance, NULL);

    image = LoadImage(g_instance, MAKEINTRESOURCE(4), IMAGE_ICON, 16, 16, 0);
    SendMessage(button_copy, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)image);

    button_save = CreateWindow(L"BUTTON", NULL,
                               WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_ICON,
                               274, 29, 22, 22, parent, (HMENU)IDC_BUTTON_SAVE, g_instance, NULL);

    image = LoadImage(g_instance, MAKEINTRESOURCE(5), IMAGE_ICON, 16, 16, 0);
    SendMessage(button_save, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)image);

    static_label = CreateWindow(L"STATIC", L"Product Key:",
                                WS_CHILD | WS_VISIBLE,
                                11, 10, 100, 25, parent, (HMENU)IDC_STATIC_LABEL, g_instance, NULL);

    SendMessage(static_label, WM_SETFONT, (WPARAM)font, MAKELPARAM(TRUE, 0));

    edit_output = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", NULL,
                                 WS_CHILD | WS_VISIBLE | ES_CENTER,
                                 11, 30, 210, 20, parent, (HMENU)IDC_EDIT_OUTPUT, g_instance, NULL);

    SendMessage(edit_output, WM_SETFONT, (WPARAM)font, MAKELPARAM(TRUE, 0));
    Edit_SetReadOnly(edit_output, TRUE);
        
    return (0);
}

static LPBYTE ReadProductKey()
{
    DWORD size = REGKEY_DATA_SIZE, err;
    LPBYTE regdat;
    HKEY hd;

    if ((err = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
                            REGKEY_SUBKEY,
                            0,
                            KEY_QUERY_VALUE | KEY_WOW64_64KEY,
                            &hd)) != ERROR_SUCCESS)
        goto _ERROR;

    regdat = (LPBYTE)malloc(REGKEY_DATA_SIZE);

    if ((err = RegQueryValueEx(hd,
                               REGKEY_DATA_NAME,
                               NULL,
                               NULL,
                               regdat,
                               &size)) != ERROR_SUCCESS) {
        free(regdat);
        goto _ERROR;
    }

    return (regdat);

_ERROR:
    OnError(err);
    return (NULL);
}

static void CopyToClipboard(HWND hd, LPCSTR key)
{
    HGLOBAL memblk;
    LPVOID ptr;

    if (OpenClipboard(hd) == 0)
        goto _ERROR;        
    
    if (EmptyClipboard() == 0)
        goto _ERROR;

    if ((memblk = GlobalAlloc(GMEM_MOVEABLE, KEY_DECODED_STRLEN)) == NULL)
        goto _ERROR;

    if ((ptr = GlobalLock(memblk)) == NULL)
        goto _ERROR;

    memcpy(ptr, key, KEY_DECODED_STRLEN);
    GlobalUnlock(memblk);

    if (SetClipboardData(CF_TEXT, memblk) == NULL)
        goto _ERROR;

    if (CloseClipboard() == 0)
        goto _ERROR;

    return;

_ERROR:    
    OnError(GetLastError());
    return;
}

static void SaveToFile(LPCSTR key)
{    
    WCHAR path[MAX_PATH];
    OPENFILENAME sfn;
    DWORD err;
    FILE* file;

    memset(&sfn, 0, sizeof(OPENFILENAME));

    sfn.lStructSize = sizeof(OPENFILENAME);
    sfn.lpstrFilter = L"Text file (*.txt)\0*.txt\0All files (*.*)\0*.*\0";
    sfn.lpstrFile = path;
    sfn.nMaxFile = MAX_PATH;
    sfn.Flags = OFN_HIDEREADONLY;
    sfn.lpstrDefExt = L"txt";    

    if (GetSaveFileName(&sfn) > 0) {
        if ((file = _wfopen(sfn.lpstrFile, L"wt")) == NULL)
            goto _ERROR;    

        if (fwrite(key, 1, KEY_DECODED_STRLEN, file) != KEY_DECODED_STRLEN)
            goto _ERROR;

        if (fclose(file) != 0)
            goto _ERROR;
    } else if ((err = CommDlgExtendedError()) != 0)
        OnError(err);

    return;

_ERROR:
    OnError(errno);
    return;
}

static void OnError(DWORD err) 
{
    LPWSTR msg;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_FROM_SYSTEM | 
                  FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  err,
                  0,
                  (LPWSTR)&msg,
                  0,
                  NULL);

    MessageBox(NULL,
               msg,
               L"Error!",
               MB_OK | MB_ICONERROR | MB_APPLMODAL);
}
