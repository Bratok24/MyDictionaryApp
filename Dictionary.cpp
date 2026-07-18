#include "Dictionary.h"
#include <windows.h> 
#include <random>


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

//Загрузка из файла 

void Dictionary::loadFromFile()
{
    words.clear();
    std::ifstream file("words.txt");
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line))
    {
        size_t pos1 = line.find('|');
        if (pos1 == std::string::npos) continue;
        size_t pos2 = line.find('|', pos1 + 1);
        if (pos2 == std::string::npos) continue;

        std::string unknown_utf8 = line.substr(0, pos1);
        std::string translation_utf8 = line.substr(pos1 + 1, pos2 - pos1 - 1);
        std::string stats_utf8 = line.substr(pos2 + 1);

        WordEntry entry;
        entry.unknown = UTF8ToWString(unknown_utf8);
        entry.translation = UTF8ToWString(translation_utf8);

        // Парсим статистику: correct|wrong
        size_t sep = stats_utf8.find('|');
        if (sep != std::string::npos)
        {
            entry.correct = std::stoi(stats_utf8.substr(0, sep));
            entry.wrong = std::stoi(stats_utf8.substr(sep + 1));
        }
        words.push_back(entry);
    }
    file.close();
}

// Перезапись файла

void Dictionary::rewriteFile()
{
    std::ofstream file("words.txt", std::ios::trunc);
    if (!file.is_open()) return;

    for (const auto& entry : words)
    {
        std::string unknown_utf8 = WStringToUTF8(entry.unknown);
        std::string translation_utf8 = WStringToUTF8(entry.translation);
        file << unknown_utf8 << "|" << translation_utf8 << "|"
             << entry.correct << "|" << entry.wrong << std::endl;
    }
    file.close();
}

// Добавление, удаление, изменение 

void Dictionary::saveWord(const std::wstring& unknown, const std::wstring& translation)
{
    WordEntry entry;
    entry.unknown = unknown;
    entry.translation = translation;
    entry.correct = 0;
    entry.wrong = 0;
    words.push_back(entry);
    rewriteFile();
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
    words[index].unknown = newUnknown;
    words[index].translation = newTranslation;
    rewriteFile();
}

//Методы для тренировки

int Dictionary::getRandomWordIndex() const
{
    if (words.empty()) return -1;

    // Вычисляем веса: чем больше wrong, тем выше шанс
    std::vector<int> weights;
    for (const auto& entry : words)
    {
        int weight = entry.wrong * 2 + 1; 
        weights.push_back(weight);
    }

    // Считаем общий вес
    int total = 0;
    for (int w : weights) total += w;

    // Случайное число от 0 до total-1
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, total - 1);
    int r = dist(gen);

    // Выбираем индекс
    int cumulative = 0;
    for (size_t i = 0; i < weights.size(); ++i)
    {
        cumulative += weights[i];
        if (r < cumulative)
            return (int)i;
    }
    return (int)weights.size() - 1;
}

void Dictionary::markCorrect(int index)
{
    if (index < 0 || index >= (int)words.size()) return;
    words[index].correct++;
    rewriteFile();
}

void Dictionary::markWrong(int index)
{
    if (index < 0 || index >= (int)words.size()) return;
    words[index].wrong++;
    rewriteFile();
}

const std::vector<WordEntry>& Dictionary::getAllWords() const
{
    return words;
}