#pragma once
#include <unordered_map>
#include "Leaders.h"
#include "Utils.h"
#include "wx/wx.h"

class LeaderPane : public wxScrolledWindow
{
public:
    LeaderPane(wxWindow* parent, wxWindowID ID) : wxScrolledWindow(parent, ID, wxDefaultPosition, wxSize(200, 600))
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
            sizer->Add(it->name, wxEXPAND);
            sizer->Add(it->value, wxEXPAND);
        }
        FitInside();
        Show();
    }
    void LoadPopulations(Leader& leader)
    {
        int uniqueCount = 0;
        for (std::vector<PopDefine>::iterator it = leader.populations.begin(); it != leader.populations.end(); it++)
        {
            populations.emplace_back(this, &populations,
                std::vector<EditBoxVals>{
                EditBoxVals{ "population type :", it->popType, &it->popType, VarType::eString, wxPoint(), (wxFrame*)this, 0 },
                EditBoxVals{"population max :", std::to_string(it->max), &it->max, VarType::eInt, wxPoint(), (wxFrame*)this, 0},
                EditBoxVals{"population current :", std::to_string(it->current), &it->current, VarType::eInt, wxPoint(), (wxFrame*)this, 0}
            });
            populations.back().subtractionButton = new wxButton(this, wxID_ANY, std::string("remove pop: ") + leader.initialValues.name + ": " + it->popType);
            sizer->Add(populations.back().subtractionButton);
            //populations.back().subtractionButton->Bind(wxEVT_BUTTON, &AddableType::TakeAwayType, &populations.back());
            for (std::vector<EditBox>::iterator it2 = populations.back().editBoxes.begin(); it2 != populations.back().editBoxes.end(); it2++)
            {
                sizer->Add(it2->name);
                sizer->Add(it2->value);
            }
        }
        for (std::vector<AddableType>::iterator it = populations.begin(); it != populations.end(); it++)
        {
            it->subtractionButton->Bind(wxEVT_BUTTON, &AddableType::TakeAwayType, &*it);
        }
        FitInside();
        Show();
    }
    void ApplyChanges()
    {
        for (std::vector<EditBox>::iterator it = leaderEditBoxes.begin(); it != leaderEditBoxes.end(); it++)
        {
            it->ApplyChange();
        }
    }
    void AddPopulation(wxCommandEvent &event)
    {
        populations.emplace_back(this, &populations,
            std::vector<EditBoxVals>{
            EditBoxVals{ "population type :", "new population", nullptr, VarType::eString, wxPoint(), (wxFrame*)this, 0 },
            EditBoxVals{ "population max:", "1", nullptr, VarType::eInt, wxPoint(), (wxFrame*)this, 0 },
            EditBoxVals{ "population current :", "1", nullptr, VarType::eInt, wxPoint(), (wxFrame*)this, 0 }
        });
        populations.back().subtractionButton = new wxButton(this, wxID_ANY, std::string("remove pop: "));
        
        sizer->Add(populations.back().subtractionButton);
        for (int i = 0; i < 3; i++)
        {
            sizer->Add(populations.back().editBoxes[i].name);
            sizer->Add(populations.back().editBoxes[i].value);
        }

        for (std::vector<AddableType>::iterator it = populations.begin(); it != populations.end(); it++)
        {
            it->subtractionButton->Bind(wxEVT_BUTTON, &AddableType::TakeAwayType, &*it);
        }
        FitInside();
        Show();
    }
    void AddStartingSquad(wxCommandEvent& event)
    {

    }
    std::vector<EditBox> leaderEditBoxes;
    std::vector<AddableType> populations;
    wxButton* addPopulation;
    std::vector<AddableType> squads;
    wxButton* addStartingSquad;
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
        leaderMap.emplace(std::pair<std::string, Leader&>(it->initialValues.originalName, *it));
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

class ElementWindow;


class LeaderUIHandler
{
public:
    ElementWindow* parent;

    LeaderUIHandler(ElementWindow* Parent);
    void ShowLeaderDetails(std::vector<wxTextCtrl*>& texts, const std::vector<std::string>& strings, int x, int y, int i);

    void ShowLeaderDetails(std::vector<EditBox>& EditValues, const std::vector<EditBoxVals>& Vals, int x, int y, int i);

    void ShowLeaderDetails(std::vector<LeaderPane*>& panes, const std::vector<EditBoxVals>& Vals, int x, int y, int i, Leader& refferedLeader);
    void DisplayLeaders();
    
    void LeaderLoad()
    {
        LoadLeaders(leaders);
    }
    std::vector<Leader> leaders;
    std::vector<LeaderPane*> leaderPanes;
};