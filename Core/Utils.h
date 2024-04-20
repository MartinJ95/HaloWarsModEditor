#pragma once

struct Vec3
{
    float x;
    float y;
    float z;
};

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

void GetAllStringsInLine(const std::string& Line, std::vector<std::string>& Strings, const std::pair<char, char> TestingChars = { '"', '"' }, bool writing = false, bool alwaysWriting = false)
{
    Strings.clear();

    std::string buildString = "";

    for (std::string::const_iterator it = Line.begin(); it != Line.end(); it++)
    {
        if (*it == TestingChars.second && writing == true)
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