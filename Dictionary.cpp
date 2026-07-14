#include "Dictionary.h"
#include <windows.h> 


std::wstring Dictionary::UTF8ToWString(const std::string& str)
{
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string Dictionary::WStringToUTF8(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}


Dictionary::Dictionary()
{
    loadFromFile();
}


void Dictionary::loadFromFile()
{
    words.clear(); 
    std::ifstream file("words.txt");
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line))
    {
        size_t pos = line.find('|');
        if (pos != std::string::npos)
        {
            std::string unknown_utf8 = line.substr(0, pos);
            std::string translation_utf8 = line.substr(pos + 1);
            std::wstring unknown = UTF8ToWString(unknown_utf8);
            std::wstring translation = UTF8ToWString(translation_utf8);
            words.push_back({unknown, translation});
        }
    }
    file.close();
}

void Dictionary::saveWord(const std::wstring& unknown, const std::wstring& translation)
{
    // Добавляем слово в вектор
    words.push_back({unknown, translation});

    // Открываем файл для добавления
    std::ofstream file("words.txt", std::ios::app);
    if (!file.is_open()) return;

    // Преобразуем в UTF-8 и записываем
    std::string unknown_utf8 = WStringToUTF8(unknown);
    std::string translation_utf8 = WStringToUTF8(translation);
    file << unknown_utf8 << "|" << translation_utf8 << std::endl;

    file.close();
}

const std::vector<std::pair<std::wstring, std::wstring>>& Dictionary::getAllWords() const
{
    return words;
}
void Dictionary::rewriteFile()
{
    std::ofstream file("words.txt", std::ios::trunc); // открываем с очисткой
    if (!file.is_open()) return;

    for (const auto& pair : words)
    {
        std::string unknown_utf8 = WStringToUTF8(pair.first);
        std::string translation_utf8 = WStringToUTF8(pair.second);
        file << unknown_utf8 << "|" << translation_utf8 << std::endl;
    }
    file.close();
}

void Dictionary::deleteWord(int index)
{
    if (index < 0 || index >= (int)words.size()) return;
    words.erase(words.begin() + index);
    rewriteFile();
}

void Dictionary::editWord(int index, const std::wstring& newUnknown, const std::wstring& newTranslation)
{
    if (index < 0 || index >= (int)words.size()) return;
    words[index] = {newUnknown, newTranslation};
    rewriteFile();
}