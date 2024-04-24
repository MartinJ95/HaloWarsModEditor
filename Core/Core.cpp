// Core.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <stack>
#include <map>
#include <unordered_map>
#include "Utils.h"
#include <wx/wx.h>

const std::string LeaderResources[4] = {
    "Supplies",
    "CampaignFoo",
    "Collectable",
    "Power"
};

const std::string leadersPath = ("..\\ModData\\data\\leaders.xml");
const std::string leadersBackUpPath("..\\ModData\\data\\leadersBackUp.xml");

inline void SaveMultipleValues(std::ifstream& file, std::string& line, std::string& saveBuildString, const std::vector<std::string>&& values)
{
    std::vector<std::string> substrings;

    for (std::vector<std::string>::const_iterator it = values.begin(); it != values.end(); it++)
    {
        GetAllStringsInLine(line, substrings, { '>', '<' }, true, true);
        //saveBuildString += substrings[0] + Valuefy(*it) + substrings[1];
        saveBuildString += substrings[0] + Fieldify(substrings[1]) + *it + Fieldify(substrings[3]) + "\n";

        std::getline(file, line);
    }
}

inline void SaveMultipleValues(std::ifstream& file, std::string& line, std::string& saveBuildString, const std::vector<int>&& values)
{
    std::vector<std::string> stringVersions;

    for (std::vector<int>::const_iterator it = values.begin(); it != values.end(); it++)
    {
        stringVersions.emplace_back(std::to_string(*it));
    }
    SaveMultipleValues(file, line, saveBuildString, std::move(stringVersions));
}


struct InitialLeaderVals
{
    std::string name;
    std::string originalName;
    std::string iconPath;
    int leaderPickOrder;
    int statsID;
    std::string defaultPlayerSlotFlags;
    void Load(const std::string& str)
    {
        std::vector<std::string> vals;

        bool hasAlpha = false;
        if (StringContainsSubString(str, "Alpha"))
        {
            hasAlpha = true;
        }

        GetAllStringsInLine(str, vals);
        name = vals[0];
        originalName = name;
        iconPath = vals[1];
        leaderPickOrder = std::stoi(vals[2+hasAlpha]);
        statsID = std::stoi(vals[3+hasAlpha]);
        defaultPlayerSlotFlags = vals[4+hasAlpha];
    }
    void Save(const std::string& line, std::string& saveBuildString)
    {
        bool hasAlpha = false;
        if (StringContainsSubString(line, "Alpha"))
        {
            hasAlpha = true;
        }

        std::vector<std::string> subStrings;
        GetAllStringsInLine(line, subStrings, { '"', '"' }, true);
        saveBuildString += (subStrings[0] + Stringify(name) + subStrings[1] + Stringify(iconPath));

        if (hasAlpha)
        {
            saveBuildString += (subStrings[2] + Stringify("0"));
        }

        saveBuildString += (subStrings[2+hasAlpha] + Stringify(leaderPickOrder) + subStrings[3 + hasAlpha] +
            Stringify(statsID) + subStrings[4 + hasAlpha] + Stringify(defaultPlayerSlotFlags) + subStrings[5 + hasAlpha] + "\n");
    }
};

struct CivDetails
{
    std::string civ;
    std::string tech;
    int nameID;
    int descriptionID;
    void Load(std::string& str, std::ifstream& Stream)
    {
        std::vector<std::string> vals;
        GetAllStringsInLine(str, vals, { '>', '<'});
        civ = vals[0];

        std::getline(Stream, str);
        GetAllStringsInLine(str, vals, { '>', '<' });
        tech = vals[0];

        std::getline(Stream, str);
        GetAllStringsInLine(str, vals, { '>', '<' });
        nameID = std::stoi(vals[0]);

        std::getline(Stream, str);
        GetAllStringsInLine(str, vals, { '>', '<' });
        descriptionID = std::stoi(vals[0]);
    }
    void Save(std::ifstream& file, std::string& line, std::string& saveBuildString)
    {
        std::vector<std::string> substrings;

        SaveMultipleValues(file, line, saveBuildString,
            std::move(
                std::vector<std::string>{civ, tech}
        )
        );
        SaveMultipleValues(file, line, saveBuildString,
            std::move(
                std::vector<int>{nameID, descriptionID}
            )
        );
    }
};

struct FlashDetails
{
    int flashID;
    std::string flashImg;
    std::string flashPortrait;
    std::string UIControlBackground;
    void Load(std::string& str, std::ifstream& Stream)
    {
        std::vector<std::string> vals;
        
        GetAllStringsInLine(str, vals, { '>', '<' });
        flashID = std::stoi(vals[0]);

        std::getline(Stream, str);
        GetAllStringsInLine(str, vals, { '>', '<' });
        flashImg = vals[0];

        std::getline(Stream, str);
        GetAllStringsInLine(str, vals, { '>', '<' });
        flashPortrait = vals[0];

        std::getline(Stream, str);
        GetAllStringsInLine(str, vals, { '>', '<' });
        UIControlBackground = vals[0];
    }
    void Save(std::ifstream& file, std::string& line, std::string& saveBuildString)
    {
        std::vector<std::string> substrings;

        GetAllStringsInLine(line, substrings, { '>', '<' }, true, true);
        //saveBuildString += substrings[0] + Valuefy(flashID) + substrings[1];
        saveBuildString += substrings[0] + Fieldify(substrings[1]) + std::to_string(flashID) + Fieldify(substrings[3]) + "\n";

        std::getline(file, line);

        SaveMultipleValues(file, line, saveBuildString, 
            std::move(
                std::vector<std::string>{
            flashImg, flashPortrait, UIControlBackground
        }
        ));
    }
};

struct StartingUnit
{
    Vec3 offset;
    std::string buildOther;
    bool doppleOnStart;
    std::string socket;
    void Load(const std::string& str, std::vector<std::string>& strings, std::vector<std::string>& nstrings)
    {
        GetAllStringsInLine(str, strings);
        GetAllStringsInLine(str, nstrings, { '>', '<' });

        std::vector<std::string> vecVals;
        GetAllStringsInLine(strings[0], vecVals, { ',', ',' }, true, true);
        offset.x = std::stoi(vecVals[0]);
        offset.y = std::stoi(vecVals[1]);
        offset.z = std::stoi(vecVals[2]);

        buildOther = strings[1];

        doppleOnStart = strings[2] == "false" ? false : true;

        socket = nstrings[0];
    }
    void Save(const std::string& line, std::string& buildSaveString)
    {
        std::vector<std::string> strings;
        std::vector<std::string> nstrings;

        GetAllStringsInLine(line, strings, '"', true, true);
        GetAllStringsInLine(line, nstrings, { '>', '<' }, true, true);
        buildSaveString += strings[0] + Stringify(offset.GetStringVersion()) + strings[2] + Stringify(buildOther) + strings[4] + Stringify(doppleOnStart ? "true" : "false") + '>' + socket + Fieldify(nstrings[3]) + "\n";
    }
};

struct StartingSquad
{
    bool flyIn;
    Vec3 offset;
    std::string unitID;
    void Load(const std::string& str, std::vector<std::string>& strings, std::vector<std::string>& nstrings)
    {
        GetAllStringsInLine(str, strings);
        GetAllStringsInLine(str, nstrings, { '>', '<' });

        flyIn = strings[0] == "false" ? false : true;

        std::vector<std::string> vecVals;
        GetAllStringsInLine(strings[1], vecVals, { ',', ',' }, true, true);
        offset.x = std::stoi(vecVals[0]);
        offset.y = std::stoi(vecVals[1]);
        offset.z = std::stoi(vecVals[2]);

        unitID = nstrings[0];
    }
    void Save(const std::string& line, std::string& buildSaveString)
    {
        std::vector<std::string> strings;
        std::vector<std::string> nstrings;

        GetAllStringsInLine(line, strings, '"', true, true);
        GetAllStringsInLine(line, nstrings, { '>', '<' }, true, true);

        buildSaveString += strings[0] + Stringify(flyIn ? "true" : "false") + strings[2] + Stringify(offset.GetStringVersion()) + '>' + unitID + Fieldify(nstrings[3]) + "\n";
    }
};

struct StartingProperties
{
    std::pair<std::string, int> leaderResources[4];
    StartingUnit startUnit;
    std::vector<StartingSquad> startingSquads;
    Vec3 rallyPointOffset;
    void Load(std::string& str, std::ifstream& Stream)
    {
        std::vector<std::string> strings;
        std::vector<std::string> nstrings;
        for (int i = 0; i < 4; i++)
        {
            GetAllStringsInLine(str, nstrings, { '>', '<' });
            leaderResources[i] = std::pair<std::string, int>( LeaderResources[i], std::stoi(nstrings[0]) );
            std::getline(Stream, str);
        }
        
        startUnit.Load(str, strings, nstrings);
        std::getline(Stream, str);

        startingSquads.emplace_back();
        startingSquads.back().Load(str, strings, nstrings);

        std::getline(Stream, str);

        std::vector<std::string> vecVals;

        GetAllStringsInLine(str, nstrings, { '>', '<' });
        GetAllStringsInLine(nstrings[0], vecVals, { ',', ',' }, true, true);

        rallyPointOffset.x = std::stoi(vecVals[0]);
        rallyPointOffset.y = std::stoi(vecVals[1]);
        rallyPointOffset.z = std::stoi(vecVals[2]);
    }
    void Save(std::ifstream& file, std::string& line, std::string& saveBuildString)
    {
        std::vector<std::string> nstrings;

        for (int i = 0; i < 4; i++)
        {
            GetAllStringsInLine(line, nstrings, { '>', '<' }, true, true);
            saveBuildString += nstrings[0] + Fieldify(nstrings[1]) + std::to_string(leaderResources[i].second) + Fieldify(nstrings[3]) + "\n";
            std::getline(file, line);
        }

        startUnit.Save(line, saveBuildString);
        std::getline(file, line);
        for (std::vector<StartingSquad>::iterator it = startingSquads.begin(); it != startingSquads.end(); it++)
        {
            it->Save(line, saveBuildString);
            std::getline(file, line);
        }
        GetAllStringsInLine(line, nstrings, { '>', '<' }, true, true);
        saveBuildString += nstrings[0] + Fieldify(nstrings[1]) + rallyPointOffset.GetStringVersion() + Fieldify(nstrings[3]) + "\n";
        std::getline(file, line);
    }
};

struct RepairProperties
{
    int repairRate;
    int repairDelay;
    std::pair<std::string, int> repairCost;
    int repairTime;
    void Load(std::string& str, std::ifstream& Stream)
    {
        std::vector<std::string> strings;
        std::vector<std::string> nstrings;

        GetAllStringsInLine(str, nstrings, { '>','<' });
        repairRate = std::stoi(nstrings[0]);

        std::getline(Stream, str);

        GetAllStringsInLine(str, nstrings, { '>','<' });
        repairDelay = std::stoi(nstrings[0]);

        std::getline(Stream, str);

        GetAllStringsInLine(str, nstrings, { '>','<' });
        GetAllStringsInLine(str, strings);

        repairCost = { strings[0], std::stoi(nstrings[0]) };

        std::getline(Stream, str);

        GetAllStringsInLine(str, nstrings, { '>', '<' });

        repairTime = std::stoi(nstrings[0]);
    }
    void Save(std::ifstream& file, std::string& line, std::string& saveBuildString)
    {
        std::vector<std::string> strings;
        std::vector<std::string> nstrings;

        auto save = [](const std::string& str, std::string& line, std::ifstream& f, std::string& sbs, std::vector<std::string>& nstrings) {
            GetAllStringsInLine(line, nstrings, { '>', '<' }, true, true);
            sbs += nstrings[0] + Fieldify(nstrings[1]) + str + Fieldify(nstrings[3]) + "\n";

            std::getline(f, line);
            };

        save(std::to_string(repairRate), line, file, saveBuildString, nstrings);
        save(std::to_string(repairDelay), line, file, saveBuildString, nstrings);

        GetAllStringsInLine(line, strings, '"', true);
        GetAllStringsInLine(line, nstrings, { '>', '<' }, true, true);

        saveBuildString += strings[0] + Stringify(repairCost.first) + '>' + std::to_string(repairCost.second) + Fieldify(nstrings[3]) + "\n";
        std::getline(file, line);

        save(std::to_string(repairTime), line, file, saveBuildString, nstrings);
    }
};

struct PopDefine
{
    std::string popType;
    int max;
    int current;
};

struct Leader
{
public:
    InitialLeaderVals initialValues;
    CivDetails civDetails;
    FlashDetails flashDetails;
    StartingProperties startProperties;
    RepairProperties repairProperties;
    std::pair<std::string, int> reverseDropCost;
    std::vector<PopDefine> populations;
    bool hasReverseDrop = false;
    void Load(std::ifstream& file, std::string& line)
    {
        initialValues.Load(line);
        std::getline(file, line);

        civDetails.Load(line, file);
        std::getline(file, line);
        
        flashDetails.Load(line, file);
        std::getline(file, line);

        startProperties.Load(line, file);
        std::getline(file, line);

        repairProperties.Load(line, file);
        std::getline(file, line);

        if (StringContainsSubString(line, "<ReverseHotDropCost"))
        {
            hasReverseDrop = true;
            std::vector<std::string> strings;
            std::vector<std::string> nstrings;

            GetAllStringsInLine(line, strings);
            GetAllStringsInLine(line, nstrings, { '>', '<' });

            reverseDropCost = { strings[0], std::stoi(nstrings[0]) };
            std::getline(file, line);
        }


        while (StringContainsSubString(line, "<Pop"))
        {
            std::vector<std::string> strings;
            std::vector<std::string> nstrings;

            GetAllStringsInLine(line, strings);
            GetAllStringsInLine(line, nstrings, {'>', '<'});

            populations.emplace_back();
            populations.back().popType = strings[0];
            populations.back().max = std::stoi(strings[1]);
            populations.back().current = std::stoi(nstrings[0]);

            std::getline(file, line);
        }
    }
    void Save(std::ifstream& file, std::string& line, std::string& saveBuildString)
    {
        initialValues.Save(line, saveBuildString);
        std::getline(file, line);
        civDetails.Save(file, line, saveBuildString);
        flashDetails.Save(file, line, saveBuildString);
        startProperties.Save(file, line, saveBuildString);
        repairProperties.Save(file, line, saveBuildString);
        if (hasReverseDrop)
        {
            std::vector<std::string> strings;
            std::vector<std::string> nstrings;

            GetAllStringsInLine(line, strings, '"', true, true);
            GetAllStringsInLine(line, nstrings, { '>',  '<' }, true, true);

            //saveBuildString += strings[0] + Stringify(reverseDropCost.first) + Valuefy(reverseDropCost.second) + nstrings[1] + "\n";
            saveBuildString += strings[0] + Stringify(reverseDropCost.first) + '>' + std::to_string(reverseDropCost.second) + Fieldify(nstrings[3]) + "\n";

            std::getline(file, line);
        }

        for (std::vector<PopDefine>::iterator it = populations.begin(); it != populations.end(); it++)
        {
            std::vector<std::string> strings;
            std::vector<std::string> nstrings;

            GetAllStringsInLine(line, strings, '"', true, true);
            GetAllStringsInLine(line, nstrings, { '>',  '<' }, true, true);

            //saveBuildString += strings[0] + Stringify(it->popType) + strings[1] + Stringify(it->max) + Valuefy(it->current) + nstrings[1] + "\n";
            saveBuildString += strings[0] + Stringify(it->popType) + strings[2] + Stringify(it->max) + '>' + std::to_string(it->current) + Fieldify(nstrings[3]) + "\n";

            std::getline(file, line);
        }
    }
};



enum
{
    ID_Hello = 1,
    ID_LoadLeaders = 2,
    ID_DisplayLeaders = 3,
    ID_SaveLeaders = 4,
    ID_BackupLeaders = 5
};

enum class VarType
{
    eString,
    eBool,
    eInt
};

class MyFrame;

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
        value = new wxTextCtrl(vals.frame, vals.ID+150, vals.existingValue, vals.position + wxPoint(200, 0));
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

class LeaderPane : public wxScrolledWindow
{
public:
    LeaderPane(wxWindow* parent, wxWindowID ID) : wxScrolledWindow(parent, ID, wxDefaultPosition, wxSize(200,600))
    {
        sizer = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(sizer);
        this->FitInside();
        this->SetScrollRate(0, 5);
    }
    void AddBoxesToSizer()
    {
        for (std::vector<EditBox>::iterator it = leaderEditBoxes.begin(); it != leaderEditBoxes.end(); it++)
        {
            // todo
            // for each box that is an addable or subtractable type such as populations or starting squads
            // find out that it is that type
            // add button to respond adding or removing the type
            // update pane when done
            sizer->Add(it->name, wxEXPAND);
            sizer->Add(it->value, wxEXPAND);
        }
        Show();
    }
    void ApplyChanges()
    {
        for (std::vector<EditBox>::iterator it = leaderEditBoxes.begin(); it != leaderEditBoxes.end(); it++)
        {
            it->ApplyChange();
        }
    }
    wxButton* addPopulation;
    wxButton* clearPopulations;
    wxButton* addStartingSquad;
    wxButton* clearStartingSquads;
    std::vector<EditBox> leaderEditBoxes;
    wxBoxSizer* sizer;
};

inline void LoadLeaders(std::vector<Leader>& leaders)
{
    leaders.clear();

    std::ifstream file(leadersPath);
    std::string line;
    if (!file)
    {
        std::cout << "error loading file" << std::endl;
        return;
    }
    //while (line.find("<Leader") != line.npos)
    while (!StringContainsSubString(line, "</Leader"))
    {
        if (file.eof())
        {
            return;
        }
        std::getline(file, line);
    }
    std::getline(file, line);
    while (!file.eof())
    {
        if (StringContainsSubString(line, "<Leader") && !StringContainsSubString(line, "Random") && !StringContainsSubString(line, "Test"))
        {
            leaders.emplace_back();
            leaders.back().Load(file, line);
        }
        std::getline(file, line);
    }
}

inline void BackUpLeaders()
{
    std::ifstream file(leadersPath);
    std::ofstream backup(leadersBackUpPath);
    if (!file)
    {
        return;
    }
    if (!backup)
    {
        return;
    }

    file.seekg(0, file.end);
    long size = file.tellg();
    file.seekg(0);

    char* buffer = new char[size];

    file.read(buffer, size);

    backup.write(buffer, size);

    delete[] buffer;

    file.close();

    backup.close();
}

inline void ApplyChangedValues(std::vector<LeaderPane*>& panes)
{
    for (std::vector<LeaderPane*>::iterator it = panes.begin(); it != panes.end(); it++)
    {
        LeaderPane* p = *it;
        p->ApplyChanges();
    }
}

inline void BuildSaveString(std::string& str, std::vector<Leader>& leaders)
{
    std::ifstream file(leadersPath);
    std::string line;
    if (!file)
    {
        std::cout << "failed to load file" << std::endl;
    }

    std::unordered_map<std::string, Leader&> leaderMap;
    for (std::vector<Leader>::iterator it = leaders.begin(); it != leaders.end(); it++)
    {
        leaderMap.emplace(std::pair<std::string, Leader&>( it->initialValues.originalName, *it) );
    }

    while (!file.eof())
    {
        std::getline(file, line);
        if (StringContainsSubString(line, "<Leader"))
        {
            std::vector<std::string> subStrings;
            GetAllStringsInLine(line, subStrings);
            if (!subStrings.empty()) {
                if (leaderMap.find(subStrings[0]) != leaderMap.end())
                {
                    //found existing leader to override
                    leaderMap.find(subStrings[0])->second.Save(file, line, str);
                    leaderMap.erase(subStrings[0]);
                    //leaderMap.erase(subStrings[0]);
                }
            }
            //leader not editable
        }
        //no leader found
        if (file.eof())
        {
            str += line;
            break;
        }
        str += line + "\n";
    }
}


inline void SaveLeaders(std::vector<Leader>& leaders)
{
    std::string saveBuildString = "";
    BuildSaveString(saveBuildString, leaders);
    std::ofstream file(leadersPath);
    //saveBuildString.erase(saveBuildString.begin() + saveBuildString.size() - 1);
    file << saveBuildString;
    file.close();

}

class MyFrame : public wxFrame
{
protected:
    wxMenu* menuFile;
    wxMenu* menuHelp;
    wxMenu* leaderMenu;

    wxScrolledWindow* scrolledWindow;
    wxBoxSizer* sizer;

    wxMenuBar* menuBar;

    std::vector<wxTextCtrl*> leaderDetails;
    std::vector<EditBox> leaderEditBoxes;
    std::vector<LeaderPane*> leaderPanes;

    std::vector<Leader> leaders;
public:
    MyFrame() :
        wxFrame(nullptr, wxID_ANY, "Halo Wars Mod Editor")
    {
        leaders.reserve(10);
        leaderPanes.reserve(10);

        /*menuFile = new wxMenu;
        menuFile->Append(ID_Hello, "&Hello...\tCtrl+H",
            "Help string shown in status bar for this menu item");
        menuFile->AppendSeparator();
        menuFile->Append(wxID_EXIT);

        menuHelp = new wxMenu;
        menuHelp->Append(wxID_ABOUT);
        */
        leaderMenu = new wxMenu;
        leaderMenu->Append(ID_LoadLeaders, "&Load Leaders");
        leaderMenu->Append(ID_DisplayLeaders, "&Display Leaders");
        leaderMenu->Append(ID_SaveLeaders, "&Save Leaders");
        leaderMenu->Append(ID_BackupLeaders, "&Backup Leaders");

        menuBar = new wxMenuBar;
        //menuBar->Append(menuFile, "&File");
        //menuBar->Append(menuHelp, "&Help");
        menuBar->Append(leaderMenu, "Leaders");

        SetMenuBar(menuBar);

        leaderDetails.emplace_back();
        leaderDetails.back() = new wxTextCtrl(this, 50, "test", wxPoint(120, 150));

        scrolledWindow = new wxScrolledWindow(this, wxID_ANY);
        scrolledWindow->SetScrollRate(FromDIP(10), 0);
        sizer = new wxBoxSizer(wxHORIZONTAL);

        scrolledWindow->SetSizer(sizer);
        scrolledWindow->FitInside();

        this->SetSizer(sizer);

        CreateStatusBar();
        SetStatusText("Welcome to wxWidgets!");

        Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
        Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
        Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
        Bind(wxEVT_MENU, &MyFrame::OnLeaderLoad, this, ID_LoadLeaders);
        Bind(wxEVT_MENU, &MyFrame::OnLeaderDisplay, this, ID_DisplayLeaders);
        Bind(wxEVT_MENU, &MyFrame::OnLeaderSave, this, ID_SaveLeaders);
        Bind(wxEVT_MENU, &MyFrame::OnLeaderBackUp, this, ID_BackupLeaders);

    }
private:
    void OnHello(wxCommandEvent& event)
    {
        wxLogMessage("Hello world from wxWidgets!");
    }
    void OnExit(wxCommandEvent& event)
    {
        Close(true);
    }
    void OnLeaderLoad(wxCommandEvent& event)
    {
        LoadLeaders(leaders);
    }

    inline void ShowLeaderDetails(std::vector<wxTextCtrl*>& texts, const std::vector<std::string>& strings, int x, int y, int i)
    {
        for (int j = 0; j < strings.size(); j++)
        {
            texts.emplace_back();
            texts.back() = new wxTextCtrl(this, 50 + j + (50 * i), strings[j], wxPoint(x, y + y * j));
        }
    }

    inline void ShowLeaderDetails(std::vector<EditBox>& EditValues, const std::vector<EditBoxVals>& Vals, int x, int y, int i)
    {
        for (int j = 0; j < Vals.size(); j++)
        {
            EditBoxVals final;
            final.frame = Vals[j].frame;
            //final.frame = scrolledWindow;
            //final.frame = &leaderPanes.back();
            final.name = Vals[j].name;
            final.existingValue = Vals[j].existingValue;
            final.type = Vals[j].type;
            final.val = Vals[j].val;
            final.ID = 50 + j + (50 * i);
            final.position = wxPoint(x, y + y * j);
            EditValues.emplace_back(final);
            sizer->Add(EditValues.back().name);
            sizer->Add(EditValues.back().value);
        }
    }
    
    inline void ShowLeaderDetails(std::vector<LeaderPane*>& panes, const std::vector<EditBoxVals>& Vals, int x, int y, int i)
    {
        panes.emplace_back();
        panes.back() = new LeaderPane(this, wxID_ANY);
        //panes.back()->SetSizer(sizer);
        panes.back()->FitInside();
        for (int j = 0; j < Vals.size(); j++)
        {
            EditBoxVals final;
            //final.frame = Vals[j].frame;
            //final.frame = scrolledWindow;
            final.frame = leaderPanes.back();
            final.name = Vals[j].name;
            final.existingValue = Vals[j].existingValue;
            final.type = Vals[j].type;
            final.val = Vals[j].val;
            final.ID = 50 + j + (50 * i);
            final.position = wxPoint(x, y + y * j);
            panes.back()->leaderEditBoxes.emplace_back(final);
            //panes.back()->sizer->Add(panes.back()->leaderEditBoxes.back().name);
            //panes.back()->sizer->Add(panes.back()->leaderEditBoxes.back().value);
        }
        panes.back()->AddBoxesToSizer();
        sizer->Add(panes.back(), wxEXPAND);
        this->FitInside();
        this->Show();
    }

    void OnLeaderSave(wxCommandEvent& event)
    {
        ApplyChangedValues(leaderPanes);
        SaveLeaders(leaders);
    }

    void OnLeaderBackUp(wxCommandEvent& event)
    {
        BackUpLeaders();
    }

    void OnLeaderDisplay(wxCommandEvent& event)
    {
        for (auto &t : leaderDetails)
        {
            delete t;
        }
        leaderDetails.clear();
        leaderEditBoxes.clear();
        for (std::vector<LeaderPane*>::reverse_iterator rit = leaderPanes.rbegin(); rit != leaderPanes.rend(); rit++)
        {
            delete* rit;
        }
        leaderPanes.clear();

        for (int i = 0; i < leaders.size(); i++)
        {
            std::vector<EditBoxVals> vals{
                { "name: ", leaders[i].initialValues.name, &leaders[i].initialValues.name, VarType::eString, wxPoint(), scrolledWindow, 0 },
                {"icon path: ", leaders[i].initialValues.iconPath,&leaders[i].initialValues.iconPath, VarType::eString, wxPoint(), scrolledWindow, 0},
                {"leader pick order: ", std::to_string(leaders[i].initialValues.leaderPickOrder), &leaders[i].initialValues.leaderPickOrder, VarType::eInt, wxPoint(), scrolledWindow, 0},
                {"stats id: ", std::to_string(leaders[i].initialValues.statsID), &leaders[i].initialValues.statsID, VarType::eInt, wxPoint(), scrolledWindow, 0},
                { "default player slot flags: ", leaders[i].initialValues.defaultPlayerSlotFlags, &leaders[i].initialValues.defaultPlayerSlotFlags, VarType::eString, wxPoint(), scrolledWindow, 0},
                {"civ: ", leaders[i].civDetails.civ, &leaders[i].civDetails.civ, VarType::eString, wxPoint(), scrolledWindow, 0},
                {"tech: ", leaders[i].civDetails.tech, &leaders[i].civDetails.tech, VarType::eString, wxPoint(), scrolledWindow, 0},
                {"name id: ", std::to_string(leaders[i].civDetails.nameID), &leaders[i].civDetails.nameID, VarType::eInt, wxPoint(), scrolledWindow, 0},
                { "descriptionID: ", std::to_string(leaders[i].civDetails.descriptionID), &leaders[i].civDetails.descriptionID, VarType::eInt, wxPoint(), scrolledWindow, 0},
                {"flashID: ", std::to_string(leaders[i].flashDetails.flashID), &leaders[i].flashDetails.flashID, VarType::eInt, wxPoint(), scrolledWindow, 0},
                {"flash image ", leaders[i].flashDetails.flashImg, &leaders[i].flashDetails.flashImg , VarType::eString, wxPoint(), scrolledWindow, 0},
                {"flash portrait: ", leaders[i].flashDetails.flashPortrait, &leaders[i].flashDetails.flashPortrait, VarType::eString, wxPoint(), scrolledWindow, 0},
                {"resource: " + leaders[i].startProperties.leaderResources[0].first, std::to_string(leaders[i].startProperties.leaderResources[0].second), &leaders[i].startProperties.leaderResources[0].second, VarType::eInt, wxPoint(), scrolledWindow, 0},
                { "resource: " + leaders[i].startProperties.leaderResources[1].first, std::to_string(leaders[i].startProperties.leaderResources[1].second), &leaders[i].startProperties.leaderResources[1].second, VarType::eInt, wxPoint(), scrolledWindow, 0 },
                {"resource: " + leaders[i].startProperties.leaderResources[2].first, std::to_string(leaders[i].startProperties.leaderResources[2].second), &leaders[i].startProperties.leaderResources[2].second, VarType::eInt, wxPoint(), scrolledWindow, 0},
                {"resource: " + leaders[i].startProperties.leaderResources[3].first, std::to_string(leaders[i].startProperties.leaderResources[3].second), &leaders[i].startProperties.leaderResources[3].second, VarType::eInt, wxPoint(), scrolledWindow, 0},
                {"rally point offset.x: ", std::to_string(leaders[i].startProperties.rallyPointOffset.x), &leaders[i].startProperties.rallyPointOffset.x, VarType::eInt, wxPoint(), scrolledWindow, 0},
                { "rally point offset.y: ", std::to_string(leaders[i].startProperties.rallyPointOffset.y), &leaders[i].startProperties.rallyPointOffset.y, VarType::eInt, wxPoint(), scrolledWindow, 0 },
                { "rally point offset.z: ", std::to_string(leaders[i].startProperties.rallyPointOffset.z), &leaders[i].startProperties.rallyPointOffset.z, VarType::eInt, wxPoint(), scrolledWindow, 0 },
                {"starting unit offset.x ", std::to_string(leaders[i].startProperties.startUnit.offset.x), &leaders[i].startProperties.startingSquads[0].offset.x, VarType::eInt, wxPoint(), scrolledWindow, 0},
                { "starting unit offset.y ", std::to_string(leaders[i].startProperties.startUnit.offset.y), &leaders[i].startProperties.startingSquads[0].offset.y, VarType::eInt, wxPoint(), scrolledWindow, 0 },
                { "starting unit offset.z ", std::to_string(leaders[i].startProperties.startUnit.offset.z), &leaders[i].startProperties.startingSquads[0].offset.z, VarType::eInt, wxPoint(), scrolledWindow, 0 },
                {"starting unit Socket: ", leaders[i].startProperties.startUnit.socket, &leaders[i].startProperties.startUnit.socket , VarType::eString, wxPoint(), scrolledWindow, 0}
            };
            for (std::vector<StartingSquad>::iterator it = leaders[i].startProperties.startingSquads.begin(); it != leaders[i].startProperties.startingSquads.end(); it++)
            {
                vals.emplace_back(EditBoxVals{ "start squad " + std::to_string(i) + " fly in:", std::string(it->flyIn ? "true" : "false"), &it->flyIn, VarType::eBool, wxPoint(), scrolledWindow, 0 });
                vals.emplace_back(EditBoxVals{"start squad " + std::to_string(i) + " offset.x:", std::to_string(it->offset.x), &it->offset.x, VarType::eInt, wxPoint(), scrolledWindow, 0});
                vals.emplace_back(EditBoxVals{"start squad " + std::to_string(i) + " offset.y:", std::to_string(it->offset.y), &it->offset.y, VarType::eInt, wxPoint(), scrolledWindow, 0});
                vals.emplace_back(EditBoxVals{ "start squad " + std::to_string(i) + " offset.z:", std::to_string(it->offset.z), &it->offset.z, VarType::eInt, wxPoint(), scrolledWindow, 0 });
                vals.emplace_back(EditBoxVals{ "start squad " + std::to_string(i) + " unit", it->unitID, &it->unitID, VarType::eString, wxPoint(), scrolledWindow, 0 });
            }
            for (std::vector<PopDefine>::iterator it = leaders[i].populations.begin(); it != leaders[i].populations.end(); it++)
            {
                vals.emplace_back(EditBoxVals{ "population type :", it->popType, &it->popType, VarType::eString, wxPoint(), scrolledWindow, 0 });
                vals.emplace_back(EditBoxVals{ "population max :", std::to_string(it->max), &it->max, VarType::eInt, wxPoint(), scrolledWindow, 0 });
                vals.emplace_back(EditBoxVals{ "population current :", std::to_string(it->current), &it->current, VarType::eInt, wxPoint(), scrolledWindow, 0 });
            }
            ShowLeaderDetails(leaderPanes, vals,
                400 * i,
                20,
                i
                );
        }
        scrolledWindow->FitInside();
        for (std::vector<LeaderPane*>::iterator it = leaderPanes.begin(); it != leaderPanes.end(); it++)
        {
            LeaderPane* ref = *it;
            ref->FitInside();
        }
        FitInside();
        Show();
    }

    void OnAbout(wxCommandEvent& event)
    {
        wxMessageBox("This is a wxWidgets Hello World example",
            "About Hello World", wxOK | wxICON_INFORMATION);
    }
};

class ModEditor : public wxApp
{
public:
    bool OnInit() override
    {
        MyFrame* frame = new MyFrame();
        frame->Show();
        return true;
    }
};

wxIMPLEMENT_APP(ModEditor);

/*int main()
{
    std::vector<Leader> leaders;

    LoadLeaders(leaders);

    std::cout << "Hello World!\n";
}*/