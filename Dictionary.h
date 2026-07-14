#pragma once
#include <vector>
#include <string>
#include <fstream>

class Dictionary
{
private:
    // Вектор хранит пары (слово, перевод) в широких строках (wstring)
    std::vector<std::pair<std::wstring, std::wstring>> words;

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

    
    const std::vector<std::pair<std::wstring, std::wstring>>& getAllWords() const;
};