#define UNICODE
#define _UNICODE

#include <windows.h>
#include <string>
#include "Dictionary.h"

extern Dictionary g_Dictionary;

// Идентификаторы элементов
#define IDC_TEXT_WORD       4001
#define IDC_BUTTON_CHECK    4002
#define IDC_BUTTON_CORRECT  4003
#define IDC_BUTTON_WRONG    4004
#define IDC_BUTTON_FINISH   4005

// Глобальные указатели
HWND g_hTrainWnd = NULL;
HWND g_hTextWord = NULL;
HWND g_hButtonCheck = NULL;
HWND g_hButtonCorrect = NULL;
HWND g_hButtonWrong = NULL;

int g_currentIndex = -1;          // индекс текущего слова
bool g_isChecked = false;         // проверено ли текущее слово

LRESULT CALLBACK TrainProc(HWND, UINT, WPARAM, LPARAM);
void ShowNewWord();
void OnCheck(HWND hWnd);
void OnCorrect(HWND hWnd);
void OnWrong(HWND hWnd);
void OnFinish(HWND hWnd);

// Функция показа окна тренировки
void ShowTrainDialog(HWND hParent)
{
    static bool registered = false;
    if (!registered)
    {
        WNDCLASSEX wc = {};
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = TrainProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = L"TrainClass";
        RegisterClassEx(&wc);
        registered = true;
    }

    HWND hWnd = CreateWindowEx(
        0,
        L"TrainClass",
        L"Тренировка",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        400, 300,
        hParent, NULL, GetModuleHandle(NULL), NULL
    );

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);
}

LRESULT CALLBACK TrainProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        g_hTrainWnd = hWnd;

        // Создаём статический текст для отображения слова
        g_hTextWord = CreateWindow(L"STATIC", L"",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            50, 30, 300, 50,
            hWnd, (HMENU)IDC_TEXT_WORD, GetModuleHandle(NULL), NULL);

        // Кнопка "Проверить"
        g_hButtonCheck = CreateWindow(L"BUTTON", L"Проверить",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            150, 100, 100, 35,
            hWnd, (HMENU)IDC_BUTTON_CHECK, GetModuleHandle(NULL), NULL);

        // Кнопки "Верно" и "Неверно" (пока скрыты)
        g_hButtonCorrect = CreateWindow(L"BUTTON", L"Верно",
            WS_CHILD | BS_PUSHBUTTON,
            80, 160, 100, 35,
            hWnd, (HMENU)IDC_BUTTON_CORRECT, GetModuleHandle(NULL), NULL);

        g_hButtonWrong = CreateWindow(L"BUTTON", L"Неверно",
            WS_CHILD | BS_PUSHBUTTON,
            220, 160, 100, 35,
            hWnd, (HMENU)IDC_BUTTON_WRONG, GetModuleHandle(NULL), NULL);

        // Кнопка "Финиш"
        CreateWindow(L"BUTTON", L"Финиш",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            150, 220, 100, 35,
            hWnd, (HMENU)IDC_BUTTON_FINISH, GetModuleHandle(NULL), NULL);

        // Показываем первое слово
        ShowNewWord();
    }
    break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDC_BUTTON_CHECK:  OnCheck(hWnd); break;
        case IDC_BUTTON_CORRECT: OnCorrect(hWnd); break;
        case IDC_BUTTON_WRONG:  OnWrong(hWnd); break;
        case IDC_BUTTON_FINISH: OnFinish(hWnd); break;
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

// Показывает новое случайное слово
void ShowNewWord()
{
    g_currentIndex = g_Dictionary.getRandomWordIndex();
    g_isChecked = false;

    if (g_currentIndex == -1)
    {
        SetWindowText(g_hTextWord, L"Нет слов для тренировки!");
        ShowWindow(g_hButtonCheck, SW_HIDE);
        ShowWindow(g_hButtonCorrect, SW_HIDE);
        ShowWindow(g_hButtonWrong, SW_HIDE);
        return;
    }

    auto& words = g_Dictionary.getAllWords();
    std::wstring word = words[g_currentIndex].unknown;
    SetWindowText(g_hTextWord, word.c_str());

    // Показываем кнопку "Проверить", скрываем "Верно"/"Неверно"
    ShowWindow(g_hButtonCheck, SW_SHOW);
    ShowWindow(g_hButtonCorrect, SW_HIDE);
    ShowWindow(g_hButtonWrong, SW_HIDE);

    // Делаем кнопку "Проверить" доступной
    EnableWindow(g_hButtonCheck, TRUE);
}

// Обработчик кнопки "Проверить"
void OnCheck(HWND hWnd)
{
    if (g_currentIndex == -1) return;
    if (g_isChecked) return; // уже проверено

    auto& words = g_Dictionary.getAllWords();
    std::wstring translation = words[g_currentIndex].translation;
    std::wstring msg = L"Правильный перевод: " + translation;
    MessageBox(hWnd, msg.c_str(), L"Проверка", MB_OK);

    g_isChecked = true;

    // Скрываем кнопку "Проверить", показываем "Верно" и "Неверно"
    ShowWindow(g_hButtonCheck, SW_HIDE);
    ShowWindow(g_hButtonCorrect, SW_SHOW);
    ShowWindow(g_hButtonWrong, SW_SHOW);
}

// Обработчик кнопки "Верно"
void OnCorrect(HWND hWnd)
{
    if (g_currentIndex == -1) return;
    g_Dictionary.markCorrect(g_currentIndex);
    ShowNewWord();
}

// Обработчик кнопки "Неверно"
void OnWrong(HWND hWnd)
{
    if (g_currentIndex == -1) return;
    g_Dictionary.markWrong(g_currentIndex);
    ShowNewWord();
}

// Обработчик кнопки "Финиш"
void OnFinish(HWND hWnd)
{
    DestroyWindow(hWnd);
}