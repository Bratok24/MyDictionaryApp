#define UNICODE
#define _UNICODE

#include <windows.h>
#include <string>
#include <vector>
#include "Dictionary.h"

extern Dictionary g_Dictionary;

#define IDC_LIST_WORDS      3001
#define IDC_EDIT_UNKNOWN    3002
#define IDC_EDIT_TRANSLATION 3003
#define IDC_BUTTON_SAVE     3004
#define IDC_BUTTON_DELETE   3005
#define IDC_BUTTON_DONE     3006

// Глобальные указатели на элементы окна
HWND g_hEditWnd = NULL;
HWND g_hList = NULL;
HWND g_hEditUnknown = NULL;
HWND g_hEditTranslation = NULL;

// Прототипы
LRESULT CALLBACK EditWordProc(HWND, UINT, WPARAM, LPARAM);
void FillList();
void OnEditSelect(HWND hWnd);
void OnEditSave(HWND hWnd);
void OnEditDelete(HWND hWnd);
void OnEditDone(HWND hWnd);

void ShowEditWordDialog(HWND hParent)
{
    static bool registered = false;
    if (!registered)
    {
        WNDCLASSEX wc = {};
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = EditWordProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = L"EditWordClass";
        RegisterClassEx(&wc);
        registered = true;
    }

    HWND hWnd = CreateWindowEx(
        0,
        L"EditWordClass",
        L"Редактирование слов",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        500, 400,
        hParent, NULL, GetModuleHandle(NULL), NULL
    );

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);
}

LRESULT CALLBACK EditWordProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        g_hEditWnd = hWnd;

        // Список слов
        g_hList = CreateWindow(L"LISTBOX", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
            20, 20, 200, 200,
            hWnd, (HMENU)IDC_LIST_WORDS, GetModuleHandle(NULL), NULL);

        FillList();

        // Надписи и поля
        CreateWindow(L"STATIC", L"Unknown (слово):",
            WS_CHILD | WS_VISIBLE,
            240, 20, 120, 25,
            hWnd, NULL, GetModuleHandle(NULL), NULL);

        g_hEditUnknown = CreateWindow(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            240, 50, 200, 25,
            hWnd, (HMENU)IDC_EDIT_UNKNOWN, GetModuleHandle(NULL), NULL);

        CreateWindow(L"STATIC", L"Translation (перевод):",
            WS_CHILD | WS_VISIBLE,
            240, 90, 120, 25,
            hWnd, NULL, GetModuleHandle(NULL), NULL);

        g_hEditTranslation = CreateWindow(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            240, 120, 200, 25,
            hWnd, (HMENU)IDC_EDIT_TRANSLATION, GetModuleHandle(NULL), NULL);

        // Кнопки
        CreateWindow(L"BUTTON", L"Подтвердить",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            240, 180, 90, 35,
            hWnd, (HMENU)IDC_BUTTON_SAVE, GetModuleHandle(NULL), NULL);

        CreateWindow(L"BUTTON", L"Удалить",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            350, 180, 90, 35,
            hWnd, (HMENU)IDC_BUTTON_DELETE, GetModuleHandle(NULL), NULL);

        CreateWindow(L"BUTTON", L"Готово",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            300, 240, 90, 35,
            hWnd, (HMENU)IDC_BUTTON_DONE, GetModuleHandle(NULL), NULL);
    }
    break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);
        if (wmId == IDC_LIST_WORDS && wmEvent == LBN_SELCHANGE)
        {
            OnEditSelect(hWnd);
        }
        else switch (wmId)
        {
        case IDC_BUTTON_SAVE:   OnEditSave(hWnd); break;
        case IDC_BUTTON_DELETE: OnEditDelete(hWnd); break;
        case IDC_BUTTON_DONE:   OnEditDone(hWnd); break;
        default: return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void FillList()
{
    if (!g_hList) return;
    SendMessage(g_hList, LB_RESETCONTENT, 0, 0);
    const auto& words = g_Dictionary.getAllWords();
    for (size_t i = 0; i < words.size(); ++i)
    {
        // Используем поля unknown и translation
        std::wstring item = words[i].unknown + L" — " + words[i].translation;
        SendMessageW(g_hList, LB_ADDSTRING, 0, (LPARAM)item.c_str());
    }
}

void OnEditSelect(HWND hWnd)
{
    int index = (int)SendMessage(g_hList, LB_GETCURSEL, 0, 0);
    if (index == LB_ERR) return;
    const auto& words = g_Dictionary.getAllWords();
    if (index >= 0 && index < (int)words.size())
    {
        SetWindowText(g_hEditUnknown, words[index].unknown.c_str());
        SetWindowText(g_hEditTranslation, words[index].translation.c_str());
    }
}

void OnEditSave(HWND hWnd)
{
    int index = (int)SendMessage(g_hList, LB_GETCURSEL, 0, 0);
    if (index == LB_ERR)
    {
        MessageBox(hWnd, L"Выберите слово для редактирования!", L"Ошибка", MB_OK);
        return;
    }

    wchar_t bufferUnknown[256] = {};
    wchar_t bufferTranslation[256] = {};
    GetWindowText(g_hEditUnknown, bufferUnknown, 256);
    GetWindowText(g_hEditTranslation, bufferTranslation, 256);

    std::wstring newUnknown = bufferUnknown;
    std::wstring newTranslation = bufferTranslation;

    if (newUnknown.empty() || newTranslation.empty())
    {
        MessageBox(hWnd, L"Заполните оба поля!", L"Ошибка", MB_OK);
        return;
    }

    g_Dictionary.editWord(index, newUnknown, newTranslation);
    FillList();
    SetWindowText(g_hEditUnknown, L"");
    SetWindowText(g_hEditTranslation, L"");
    MessageBox(hWnd, L"Изменения сохранены!", L"Успех", MB_OK);
}

void OnEditDelete(HWND hWnd)
{
    int index = (int)SendMessage(g_hList, LB_GETCURSEL, 0, 0);
    if (index == LB_ERR)
    {
        MessageBox(hWnd, L"Выберите слово для удаления!", L"Ошибка", MB_OK);
        return;
    }

    if (MessageBox(hWnd, L"Удалить выбранное слово?", L"Подтверждение", MB_YESNO) == IDYES)
    {
        g_Dictionary.deleteWord(index);
        FillList();
        SetWindowText(g_hEditUnknown, L"");
        SetWindowText(g_hEditTranslation, L"");
    }
}

void OnEditDone(HWND hWnd)
{
    DestroyWindow(hWnd);
}