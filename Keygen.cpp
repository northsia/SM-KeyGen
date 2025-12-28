#include <windows.h>
#include <stdio.h>

HWND hNameEdit, hKeyEdit;

static inline unsigned int ROL(unsigned int x, int r)
{
    return (x << r) | (x >> (32 - r));
}

unsigned int GenerateKey(const char* name)
{
    int len = lstrlenA(name);
    int v8  = -len;
    int v9  = 0xDEADBEEF;
    int v10 = 0x55555555;
    int i   = 0;

    if (len == 0)
        return (255 * 0 * 0) ^ (v9 + ROL(v10, 29));

    while (name[i])
    {
        unsigned char c = (unsigned char)name[i];
        int v12;

        if (c & 1)
            v12 = c + ROL(v10, 29) - 439041101;
        else
            v12 = (c ^ ROL(v10, 12)) - 1863314892;

        v9  = (i + v9) ^ (i + v9 + v8);
        v10 = v9 ^ v12;

        i++;
    }

    return (255 * i * i) ^ (v9 + ROL(v10, 29));
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        CreateWindowA("STATIC", "Name:",
            WS_VISIBLE | WS_CHILD,
            20, 20, 60, 20,
            hWnd, 0, 0, 0);

        hNameEdit = CreateWindowA("EDIT", "",
            WS_VISIBLE | WS_CHILD | WS_BORDER,
            90, 20, 200, 25,
            hWnd, 0, 0, 0);

        CreateWindowA("STATIC", "Key:",
            WS_VISIBLE | WS_CHILD,
            20, 60, 60, 20,
            hWnd, 0, 0, 0);

        hKeyEdit = CreateWindowA("EDIT", "",
            WS_VISIBLE | WS_CHILD | WS_BORDER,
            90, 60, 200, 25,
            hWnd, 0, 0, 0);

        CreateWindowA("BUTTON", "CHECK KEY",
            WS_VISIBLE | WS_CHILD,
            40, 110, 100, 30,
            hWnd, (HMENU)1, 0, 0);

        CreateWindowA("BUTTON", "GENERATE",
            WS_VISIBLE | WS_CHILD,
            170, 110, 100, 30,
            hWnd, (HMENU)2, 0, 0);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) // CHECK
        {
            char name[256], key[256];
            GetWindowTextA(hNameEdit, name, 256);
            GetWindowTextA(hKeyEdit, key, 256);

            unsigned int gen = GenerateKey(name);
            unsigned int user = strtoul(key, 0, 16);

            if (gen == user)
                MessageBoxA(hWnd, "Access Granted!", "Success", MB_ICONINFORMATION);
            else
                MessageBoxA(hWnd, "Invalid Key", "Failed", MB_ICONERROR);
        }
        else if (LOWORD(wParam) == 2) // GENERATE
        {
            char name[256], out[64];
            GetWindowTextA(hNameEdit, name, 256);

            unsigned int gen = GenerateKey(name);
            wsprintfA(out, "%X", gen);

            SetWindowTextA(hKeyEdit, out);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcA(hWnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
    WNDCLASSA wc = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = "KeygenWnd";

    RegisterClassA(&wc);

    HWND hWnd = CreateWindowA(
        "KeygenWnd",
        "Crackme Keygen Test",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        500, 300, 330, 200,
        0, 0, hInst, 0);

    ShowWindow(hWnd, SW_SHOW);

    MSG msg;
    while (GetMessageA(&msg, 0, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return 0;
}
