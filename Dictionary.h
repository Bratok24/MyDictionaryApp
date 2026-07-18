#pragma once
#include <vector>
#include <string>
#include <fstream>

// Структура для хранения слова с его статистикой
struct WordEntry {
    std::wstring unknown;
    std::wstring translation;
    int correct = 0;
    int wrong = 0;
};

class Dictionary
{
private:
    std::vector<WordEntry> words;

    // Вспомогательные методы для работы с кодировкой
    std::wstring UTF8ToWString(const std::string& str);
    std::string WStringToUTF8(const std::wstring& wstr);

    void rewriteFile(); // перезаписывает файл из вектора

public:
    Dictionary();

    void loadFromFile();
    void saveWord(const std::wstring& unknown, const std::wstring& translation);
    void deleteWord(int index);
    void editWord(int index, const std::wstring& newUnknown, const std::wstring& newTranslation);

    // Методы для тренировки
    int getRandomWordIndex() const;          // выбирает случайный индекс с учётом веса
    void markCorrect(int index);             // увеличивает счётчик правильных ответов
    void markWrong(int index);               // увеличивает счётчик неправильных ответов

    // Возвращает все слова 
    const std::vector<WordEntry>& getAllWords() const;
};