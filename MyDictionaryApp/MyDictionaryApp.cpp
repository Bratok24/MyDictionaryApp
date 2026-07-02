#include <windows.h>
#include <string>

// Идентификаторы для кнопок (чтобы различать их в обработчике)
#define ID_BUTTON_ADD     1001
#define ID_BUTTON_EDIT    1002
#define ID_BUTTON_TRAIN   1003
#define ID_BUTTON_EXIT    1004

// Прототипы функций
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void OnButtonAdd(HWND hWnd);
void OnButtonEdit(HWND hWnd);
void OnButtonTrain(HWND hWnd);
void OnButtonExit(HWND hWnd);

// Точка входа в программу
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 1. Регистрация класса окна
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

    // 2. Создание окна с флагом "поверх всех окон" (WS_EX_TOPMOST)
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

    // 3. Цикл обработки сообщений
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// 4. Оконная процедура — обрабатывает все сообщения
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        // Создаём кнопки при создании окна
        // Три кнопки: Добавление, Редактирование, Тренировка
        // И кнопка "Выход"
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
        // Обработка нажатий на кнопки
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case ID_BUTTON_ADD:
            OnButtonAdd(hWnd);
            break;
        case ID_BUTTON_EDIT:
            OnButtonEdit(hWnd);
            break;
        case ID_BUTTON_TRAIN:
            OnButtonTrain(hWnd);
            break;
        case ID_BUTTON_EXIT:
            OnButtonExit(hWnd);
            break;
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

// 5. Обработчики нажатий на кнопки (пока просто показывают сообщения)
void OnButtonAdd(HWND hWnd)
{
    MessageBox(hWnd, L"Здесь будет окно добавления слов", L"Добавление", MB_OK);
}

void OnButtonEdit(HWND hWnd)
{
    MessageBox(hWnd, L"Здесь будет окно редактирования слов", L"Редактирование", MB_OK);
}

void OnButtonTrain(HWND hWnd)
{
    MessageBox(hWnd, L"Здесь будет окно тренировки", L"Тренировка", MB_OK);
}

void OnButtonExit(HWND hWnd)
{
    // Закрываем окно
    PostMessage(hWnd, WM_DESTROY, 0, 0);
}