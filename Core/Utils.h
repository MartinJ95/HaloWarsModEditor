#pragma once
#include <iostream>
#include <stack>
#include "wx/wx.h"

enum
{
    ID_Hello = 1,
    ID_LoadLeaders = 2,
    ID_DisplayLeaders = 3,
    ID_SaveLeaders = 4,
    ID_BackupLeaders = 5,
    ID_AddLeader = 6
};

enum class VarType
{
    eString,
    eBool,
    eInt
};

enum class WindowType
{
    eLeaders = 0
};

struct EditBoxVals
{
    std::string name;
    std::string existingValue;
    void* val;
    VarType type;
    wxPoint position;
    wxWindow* frame;
    int ID;
};

class EditBox
{
public:
    EditBox()
    {}
    EditBox(EditBoxVals Vals)
    {
        vals = Vals;
        name = new wxStaticText(vals.frame, vals.ID, vals.name, vals.position);
        value = new wxTextCtrl(vals.frame, vals.ID + 150, vals.existingValue, vals.position + wxPoint(200, 0));
    }
    inline void ApplyChange()
    {
        if (vals.val != nullptr)
        {
            switch (vals.type)
            {
            case VarType::eString:
                *(std::string*)vals.val = std::string(value->GetValue().mb_str());
                break;
            case VarType::eBool:
                *(bool*)vals.val = std::string(value->GetValue().mb_str()) == "true" ? true : false;
                break;
            case VarType::eInt:
                value->GetValue().ToInt((int*)vals.val);
                //*(int*)vals.val = value->GetValue().ToInt();
                break;
            default:
                break;
            }
        }
    }
    ~EditBox()
    {

    }
    wxStaticText* name = nullptr;
    wxTextCtrl* value = nullptr;
    EditBoxVals vals;
};


struct AddableType
{
    wxWindow* containedWindow;
    std::vector<AddableType>* owningVec;
    wxButton* subtractionButton;
    std::vector<EditBox> editBoxes;
    /*const bool operator==(const AddableType& other)
    {
        return ID == other.ID;
    }*/
    AddableType(wxWindow* window, std::vector<AddableType>* OwningVec, const std::vector<EditBoxVals>& vals) : containedWindow(window), owningVec(OwningVec)
    {
        for (std::vector<EditBoxVals>::const_iterator it = vals.begin(); it != vals.end(); it++)
        {
            editBoxes.emplace_back(*it);
        }
    }
    void TakeAwayType(wxCommandEvent &event)
    {
        for (std::vector<AddableType>::iterator it = owningVec->begin(); it != owningVec->end(); it++)
        {
            if (&*it == &*this)
            {
                CleanUp();
                owningVec->erase(it);
                return;
            }
        }
    }
    void CleanUp()
    {
        for (std::vector<EditBox>::iterator it = editBoxes.begin(); it != editBoxes.end(); it++)
        {
            delete it->name;
            delete it->value;
        }
        delete subtractionButton;
        containedWindow->FitInside();
        containedWindow->Show();
    }
};

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

inline bool StringContainsSubString(const std::string& Main, const std::string& SubString)
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

inline void GetAllStringsInLine(const std::string& Line, std::vector<std::string>& Strings, const std::pair<char, char> TestingChars = { '"', '"' }, bool writing = false, const bool alwaysWriting = false)
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

inline void GetAllStringsInLine(const std::string& line, std::vector<std::string>& strings, const char TestingChar, bool writing = false, const bool alwaysWriting = false)
{
    GetAllStringsInLine(line, strings, { TestingChar, TestingChar }, writing, alwaysWriting);
}