#define UNICODE
#define _UNICODE

#include <windows.h>
#include <string>
#include <fstream>
#include "Dictionary.h"

extern Dictionary g_Dictionary;

#define IDC_EDIT_UNKNOWN     2001
#define IDC_EDIT_TRANSLATION 2002
#define IDC_BUTTON_SAVE      2003
#define IDC_BUTTON_DONE      2004

std::wstring g_unknown;
std::wstring g_translation;

LRESULT CALLBACK AddWordProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void OnSave(HWND hWnd);
void OnDone(HWND hWnd);

//Создание и показ окна добавления
void ShowAddWordDialog(HWND hParent)
{
    static bool registered = false;
    if (!registered)
    {
        WNDCLASSEX wc = {};
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = AddWordProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = L"AddWordClass";
        RegisterClassEx(&wc);
        registered = true;
    }
    //Создаём окно (дочернее по отношению к главному)
    HWND hWnd = CreateWindowEx(
        0,
        L"AddWordClass",
        L"Добавление слов",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        400, 300,
        hParent,
        NULL, GetModuleHandle(NULL), NULL
    );

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);
}

LRESULT CALLBACK AddWordProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        CreateWindow(L"STATIC", L"Unknown (слово):",
            WS_CHILD | WS_VISIBLE,
            20, 20, 120, 35,
            hWnd, NULL, GetModuleHandle(NULL), NULL);

        CreateWindow(L"STATIC", L"Translation (перевод):",
            WS_CHILD | WS_VISIBLE,
            20, 70, 120, 35,
            hWnd, NULL, GetModuleHandle(NULL), NULL);

        CreateWindow(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            150, 20, 200, 25,
            hWnd, (HMENU)IDC_EDIT_UNKNOWN, GetModuleHandle(NULL), NULL);

        CreateWindow(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            150, 70, 200, 25,
            hWnd, (HMENU)IDC_EDIT_TRANSLATION, GetModuleHandle(NULL), NULL);

        CreateWindow(L"BUTTON", L"Подтвердить",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            80, 130, 100, 35,
            hWnd, (HMENU)IDC_BUTTON_SAVE, GetModuleHandle(NULL), NULL);

        CreateWindow(L"BUTTON", L"Готово",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            220, 130, 100, 35,
            hWnd, (HMENU)IDC_BUTTON_DONE, GetModuleHandle(NULL), NULL);
        break;
    }

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDC_BUTTON_SAVE:
        {
            OnSave(hWnd);
            break;
        }
        case IDC_BUTTON_DONE:
        {
            OnDone(hWnd);
            break;
        }
        default:
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        }
        break;
    }

    case WM_CLOSE:
    {
        DestroyWindow(hWnd);
        break;
    }

    default:
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    }
    return 0;
}

void OnSave(HWND hWnd)
{
    //Читаем текст из полей ввода
    wchar_t bufferUnknown[256] = {};
    wchar_t bufferTranslation[256] = {};
    GetWindowText(GetDlgItem(hWnd, IDC_EDIT_UNKNOWN), bufferUnknown, 256);
    GetWindowText(GetDlgItem(hWnd, IDC_EDIT_TRANSLATION), bufferTranslation, 256);

    std::wstring unknown = bufferUnknown;
    std::wstring translation = bufferTranslation;

    if (unknown.empty() || translation.empty())
    {
        MessageBox(hWnd, L"Заполните оба поля!", L"Ошибка", MB_OK);
        return;
    }
    //Сохраняем через класс Dictionary
    g_Dictionary.saveWord(unknown, translation);
    MessageBox(hWnd, L"Слово сохранено в файл!", L"Успех", MB_OK);

    SetWindowText(GetDlgItem(hWnd, IDC_EDIT_UNKNOWN), L"");
    SetWindowText(GetDlgItem(hWnd, IDC_EDIT_TRANSLATION), L"");
}

void OnDone(HWND hWnd)
{
    DestroyWindow(hWnd);
}