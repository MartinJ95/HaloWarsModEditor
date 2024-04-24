#pragma once

struct Vec3
{
    int x;
    int y;
    int z;
    std::string GetStringVersion()
    {
        return std::to_string(x) + ',' + std::to_string(y) + ',' + std::to_string(z);
    }
};

inline std::string Stringify(const std::string& str)
{
    return '"' + str + '"';
}

inline std::string Stringify(const int value)
{
    return '"' + std::to_string(value) + '"';
}

inline std::string Valuefy(const std::string& str)
{
    return '>' + str + '<';
}

inline std::string Valuefy(const int value)
{
    return '>' + std::to_string(value) + '<';
}

inline std::string Fieldify(const std::string& str)
{
    return '<' + str + '>';
}

inline std::string Fieldify(const int value)
{
    return Fieldify(std::to_string(value));
}

bool StringContainsSubString(const std::string& Main, const std::string& SubString)
{
    std::stack<char> lettersToTest;
    std::stack<char> foundLetters;

    for (std::string::const_reverse_iterator rit = SubString.rbegin(); rit != SubString.rend(); rit++)
    {
        lettersToTest.emplace(*rit);
    }

    for (std::string::const_iterator it = Main.begin(); it != Main.end(); it++)
    {
        if (lettersToTest.top() == *it)
        {
            foundLetters.emplace(lettersToTest.top());
            lettersToTest.pop();
            if (lettersToTest.empty())
            {
                return true;
            }
            continue;
        }
        while (!foundLetters.empty())
        {
            lettersToTest.emplace(foundLetters.top());
            foundLetters.pop();
        }
    }
    return false;
}

void GetAllStringsInLine(const std::string& Line, std::vector<std::string>& Strings, const std::pair<char, char> TestingChars = { '"', '"' }, bool writing = false, const bool alwaysWriting = false)
{
    Strings.clear();

    std::string buildString = "";

    for (std::string::const_iterator it = Line.begin(); it != Line.end(); it++)
    {
        if (*it == TestingChars.second && writing == true || *it == TestingChars.first && alwaysWriting)
        {
            writing = alwaysWriting ? true : false;
            Strings.emplace_back(buildString);
            buildString.clear();
            continue;
        }
        else if (*it == TestingChars.first && writing == false)
        {
            writing = true;
            continue;
        }
        if (writing)
        {
            buildString += *it;
        }
    }
    if (writing)
    {
        Strings.emplace_back(buildString);
    }
}

void GetAllStringsInLine(const std::string& line, std::vector<std::string>& strings, const char TestingChar, bool writing = false, const bool alwaysWriting = false)
{
    GetAllStringsInLine(line, strings, { TestingChar, TestingChar }, writing, alwaysWriting);
}