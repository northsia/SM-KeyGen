# 🔑 SM-KeyGen

**Simple Key Generator — Educational Reverse Engineering Project**

---

## 📌 About

**SM-KeyGen** is an educational reverse engineering project that demonstrates how a key verification algorithm can be analyzed, understood, and reconstructed from a compiled binary using **IDA Pro**.

This repository shows the full process of:
- Inspecting a Windows binary in IDA
- Understanding the key-check logic
- Rewriting the algorithm as a working KeyGen

> ⚠️ **IMPORTANT**
>
> This project is provided **for educational purposes only**.  
> It is intended for learning reverse engineering techniques and must **not** be used for piracy or illegal activities.

---

## 🧠 Reverse Engineering Overview

During static analysis with **IDA Pro**, the application's main window procedure was identified as the location responsible for validating the user-provided **Name** and **Key**.

### Observed behavior:

- Reads a **Name** string from the input field
- Reads a **Key** value in hexadecimal format
- Executes a custom verification algorithm
- Compares the computed result with the provided key
- Displays either a success or failure message

---

## 📄 Decompiled Code (IDA Output)

<img width="1920" height="1080" alt="{5F245E5C-211D-48DB-ACC2-512053D44ECA}" src="https://github.com/user-attachments/assets/e6539c9b-fce2-4485-b572-a87c8c1fb2b9" />

```c

LRESULT __fastcall sub_140001A40(HWND hWnd, UINT a2, WPARAM a3, LPARAM a4)
{
  int v6; // eax
  unsigned __int8 left; // r8
  int v8; // eax
  int v9; // ecx
  int v10; // edi
  __int64 v11; // rdx
  int v12; // edi
  int X; // edi
  int Y; // ebx
  HMODULE ModuleHandleA; // rax
  const CHAR *v18; // rdx
  const CHAR *v19; // r8
  HWND v20; // rcx
  UINT v21; // r9d
  struct tagRECT String[16]; // [rsp+60h] [rbp-238h] BYREF
  CHAR v23[256]; // [rsp+160h] [rbp-138h] BYREF

  switch ( a2 )
  {
    case 0x111u:
      if ( (_WORD)a3 == 2 )
      {
        GetWindowRect(hWnd, String);
        X = String[0].left
          + ((int)(String[0].right
                 - String[0].left
                 + ((unsigned int)(String[0].right - String[0].left - 300) >> 31)
                 - 300) >> 1);
        Y = String[0].top
          + ((int)(String[0].bottom
                 - String[0].top
                 + ((unsigned int)(String[0].bottom - String[0].top - 250) >> 31)
                 - 250) >> 1);
        ModuleHandleA = GetModuleHandleA(0LL);
        CreateWindowExA(0, "AboutWndClass", "About", 0x90C80000, X, Y, 300, 250, hWnd, 0LL, ModuleHandleA, 0LL);
      }
      else if ( (unsigned __int16)a3 == 1 )
      {
        GetWindowTextA(::hWnd, (LPSTR)String, 256);
        GetWindowTextA(qword_14037EF58, v23, 256);
        v6 = strlen((const char *)String);
        left = String[0].left;
        if ( LOBYTE(String[0].left) )
        {
          v8 = -v6;
          v9 = -559038242;
          v10 = 1431655765;
          v11 = 0LL;
          do
          {
            if ( (left & 1) != 0 )
              v12 = left + __ROL4__(v10, 29) - 439041101;
            else
              v12 = (left ^ __ROL4__(v10, 12)) - 1863314892;
            v9 = (v11 + v9) ^ (v11 + v9 + v8);
            v10 = v9 ^ v12;
            left = *((_BYTE *)&String[0].left + ++v11);
          }
          while ( left );
        }
        else
        {
          v10 = 1431655765;
          v9 = -559038242;
          LODWORD(v11) = 0;
        }
        if ( ((255 * (_DWORD)v11 * (_DWORD)v11) ^ (v9 + __ROL4__(v10, 29))) == strtoul(v23, 0LL, 16) )
        {
          v18 = "Access Granted!";
          v19 = "Success";
          v20 = hWnd;
          v21 = 64;
        }
        else
        {
          v18 = "Invalid Key";
          v19 = "Failed";
          v20 = hWnd;
          v21 = 16;
        }
        MessageBoxA(v20, v18, v19, v21);
      }
      break;
    case 2u:
      PostQuitMessage(0);
      break;
    case 1u:
      CreateWindowExA(0, "STATIC", "Name:", 0x50000000u, 20, 20, 70, 20, hWnd, 0LL, 0LL, 0LL);
      ::hWnd = CreateWindowExA(0, "EDIT", &byte_140005161, 0x50800080u, 100, 20, 200, 25, hWnd, (HMENU)3, 0LL, 0LL);
      CreateWindowExA(0, "STATIC", "Key:", 0x50000000u, 20, 60, 70, 20, hWnd, 0LL, 0LL, 0LL);
      qword_14037EF58 = CreateWindowExA(0, "EDIT", &byte_140005161, 0x50800080u, 100, 60, 200, 25, hWnd, 0LL, 0LL, 0LL);
      CreateWindowExA(0, "BUTTON", "CHECK KEY", 0x50000001u, 100, 100, 100, 30, hWnd, (HMENU)1, 0LL, 0LL);
      CreateWindowExA(0, "BUTTON", "About", 0x50000000u, 210, 100, 80, 30, hWnd, (HMENU)2, 0LL, 0LL);
      break;
    default:
      return DefWindowProcA(hWnd, a2, a3, a4);
  }
  return 0LL;
}

```

⚙️ Key Verification Algorithm
After analyzing the decompiled code, the verification logic can be summarized as follows:

Iterates over each character of the input name

Applies different operations depending on character parity

Uses bitwise operations:

XOR

Rotate Left (ROL)

Relies on fixed seed constants

Constants used:
text
Copy code
0xDEADBEEF
0x55555555
🛠️ Reconstructed KeyGen Implementation
Based on the extracted logic, the key generation algorithm was reconstructed as shown below:

```
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

```


KeyGen.exe :

<img width="547" height="333" alt="{60BA6D9F-73A4-43DB-8093-BB5A2E9BA20B}" src="https://github.com/user-attachments/assets/5c9f2689-d343-4a2d-ae44-e857a1a52b2e" />



✅ Result
Using this KeyGen implementation:

A valid Key can be generated for any Name


<img width="496" height="307" alt="{BDCE4202-F959-4FF4-92A4-AB4766CEAAB3}" src="https://github.com/user-attachments/assets/71c9369e-ba1b-4fdc-aa67-76b2bc650b82" />


The target application accepts the generated key

The following message is displayed:

Access Granted!
🎉 The application is successfully cracked (for educational purposes).
