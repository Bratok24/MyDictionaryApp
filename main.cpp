#define UNICODE
#define _UNICODE
#include <windows.h>

// Объявление функции из другого файла
void ShowAddWordDialog(HWND hParent);

// Идентификаторы для кнопок
#define ID_BUTTON_ADD     1001
#define ID_BUTTON_EDIT    1002
#define ID_BUTTON_TRAIN   1003
#define ID_BUTTON_EXIT    1004

// Прототипы функций
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void OnButtonAdd(HWND);
void OnButtonEdit(HWND);
void OnButtonTrain(HWND);
void OnButtonExit(HWND);

// Точка входа
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Регистрация класса окна
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"MyDictionaryClass";

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, L"Не удалось зарегистрировать класс окна!", L"Ошибка", MB_ICONERROR);
        return 1;
    }

    // Создание окна (поверх всех окон)
    HWND hWnd = CreateWindowEx(
        WS_EX_TOPMOST,
        L"MyDictionaryClass",
        L"Мой словарь",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        400, 350,
        NULL, NULL, hInstance, NULL
    );

    if (!hWnd)
    {
        MessageBox(NULL, L"Не удалось создать окно!", L"Ошибка", MB_ICONERROR);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Цикл обработки сообщений
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// Оконная процедура
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        CreateWindow(L"BUTTON", L"Добавление слов",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            50, 40, 280, 50,
            hWnd, (HMENU)ID_BUTTON_ADD, GetModuleHandle(NULL), NULL);

        CreateWindow(L"BUTTON", L"Редактирование слов",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            50, 110, 280, 50,
            hWnd, (HMENU)ID_BUTTON_EDIT, GetModuleHandle(NULL), NULL);

        CreateWindow(L"BUTTON", L"Тренировка",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            50, 180, 280, 50,
            hWnd, (HMENU)ID_BUTTON_TRAIN, GetModuleHandle(NULL), NULL);

        CreateWindow(L"BUTTON", L"Выход",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            50, 250, 280, 50,
            hWnd, (HMENU)ID_BUTTON_EXIT, GetModuleHandle(NULL), NULL);
    }
    break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case ID_BUTTON_ADD: OnButtonAdd(hWnd); break;
        case ID_BUTTON_EDIT: OnButtonEdit(hWnd); break;
        case ID_BUTTON_TRAIN: OnButtonTrain(hWnd); break;
        case ID_BUTTON_EXIT: OnButtonExit(hWnd); break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчики кнопок
void OnButtonAdd(HWND hWnd)
{
    ShowAddWordDialog(hWnd); // открываем окно добавления
}

void OnButtonEdit(HWND hwnd)  // ← исправлено имя!
{
    MessageBox(hwnd, L"Здесь будет окно редактирования слов", L"Редактирование", MB_OK);
}

void OnButtonTrain(HWND hwnd)
{
    MessageBox(hwnd, L"Здесь будет окно тренировки", L"Тренировка", MB_OK);
}

void OnButtonExit(HWND hwnd)
{
    PostMessage(hwnd, WM_DESTROY, 0, 0);
}