#include <windows.h>
#include <string>

// Идентификаторы для элементов управления (кнопок и полей)
#define IDC_EDIT_UNKNOWN   2001
#define IDC_EDIT_TRANSLATION 2002
#define IDC_BUTTON_SAVE    2003
#define IDC_BUTTON_DONE    2004

// Глобальные переменные для хранения введённых данных (пока просто для примера)
std::wstring g_unknown;
std::wstring g_translation;

// Прототипы функций (объявляем, что они будут ниже)
LRESULT CALLBACK AddWordProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void OnSave(HWND hWnd);
void OnDone(HWND hWnd);

// Функция, которая создаёт и показывает окно добавления слов
void ShowAddWordDialog(HWND hParent)
{
    // Регистрируем класс окна (если ещё не зарегистрирован)
    static bool registered = false;
    if (!registered)
    {
        WNDCLASSEX wc = {};
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = AddWordProc;  // Указываем, что обработчик сообщений — наша функция
        wc.hInstance = GetModuleHandle(NULL);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = L"AddWordClass";
        RegisterClassEx(&wc);
        registered = true;
    }

    // Создаём само окно (дочернее по отношению к главному)
    HWND hWnd = CreateWindowEx(
        0,                          // без расширенных стилей
        L"AddWordClass",            // имя класса
        L"Добавление слов",         // заголовок окна
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, // обычное окно без изменения размера
        CW_USEDEFAULT, CW_USEDEFAULT, // позиция X, Y (автоматическая)
        400, 300,                   // ширина, высота
        hParent,                    // родительское окно (главное)
        NULL, GetModuleHandle(NULL), NULL
    );

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);
}

// Оконная процедура для окна добавления
LRESULT CALLBACK AddWordProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        // Создаём надписи (статический текст)
        CreateWindow(L"STATIC", L"Unknown (слово):",
            WS_CHILD | WS_VISIBLE,
            20, 20, 120, 45,
            hWnd, NULL, GetModuleHandle(NULL), NULL);

        CreateWindow(L"STATIC", L"Translation (перевод):",
            WS_CHILD | WS_VISIBLE,
            20, 70, 120, 45,
            hWnd, NULL, GetModuleHandle(NULL), NULL);

        // Создаём поля для ввода текста
        CreateWindow(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            150, 20, 200, 25,
            hWnd, (HMENU)IDC_EDIT_UNKNOWN, GetModuleHandle(NULL), NULL);

        CreateWindow(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            150, 70, 200, 25,
            hWnd, (HMENU)IDC_EDIT_TRANSLATION, GetModuleHandle(NULL), NULL);

        // Создаём кнопки
        CreateWindow(L"BUTTON", L"Подтвердить",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            80, 130, 100, 35,
            hWnd, (HMENU)IDC_BUTTON_SAVE, GetModuleHandle(NULL), NULL);

        CreateWindow(L"BUTTON", L"Готово",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            220, 130, 100, 35,
            hWnd, (HMENU)IDC_BUTTON_DONE, GetModuleHandle(NULL), NULL);
    }
    break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDC_BUTTON_SAVE:
            OnSave(hWnd);
            break;
        case IDC_BUTTON_DONE:
            OnDone(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
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

// Обработчик кнопки "Подтвердить"
void OnSave(HWND hWnd)
{
    // Получаем текст из поля "Unknown"
    HWND hEditUnknown = GetDlgItem(hWnd, IDC_EDIT_UNKNOWN);
    wchar_t bufferUnknown[256] = {};
    GetWindowText(hEditUnknown, bufferUnknown, 256);

    // Получаем текст из поля "Translation"
    HWND hEditTranslation = GetDlgItem(hWnd, IDC_EDIT_TRANSLATION);
    wchar_t bufferTranslation[256] = {};
    GetWindowText(hEditTranslation, bufferTranslation, 256);

    // Сохраняем в глобальные переменные (позже заменим на сохранение в файл)
    g_unknown = bufferUnknown;
    g_translation = bufferTranslation;

    // Показываем сообщение, что слово добавлено (временно)
    std::wstring msg = L"Слово \"" + g_unknown + L"\" добавлено!";
    MessageBox(hWnd, msg.c_str(), L"Успех", MB_OK);

    // Очищаем поля для следующего ввода
    SetWindowText(hEditUnknown, L"");
    SetWindowText(hEditTranslation, L"");
}

// Обработчик кнопки "Готово"
void OnDone(HWND hWnd)
{
    // Закрываем окно добавления
    DestroyWindow(hWnd);
}