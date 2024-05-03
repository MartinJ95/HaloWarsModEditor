#pragma once
#include <unordered_map>
#include "Leaders.h"
#include "Utils.h"
#include "wx/wx.h"

class LeaderPane : public wxScrolledWindow
{
public:
    LeaderPane(Leader* Ref, wxWindow* parent, wxWindowID ID);
    LeaderPane(Leader* Ref, wxWindow* parent, wxWindowID ID, const std::vector<EditBoxVals>& Vals);
    void AddBoxesToSizer();
    void LoadPopulations(Leader& leader);
    void LoadStartingSquads(Leader& leader);
    void ApplyChanges();
    void AddPopulation(wxCommandEvent& event);
    void AddStartingSquad(wxCommandEvent& event);
    void RemoveLeader(wxCommandEvent& event);
    Leader* ref;
    wxButton* removeLeader;
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

    bool savingLeader = false;

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
                    leaderMap.find(subStrings[0])->second.Save(line, str);
                    leaderMap.erase(subStrings[0]);
                    //leaderMap.erase(subStrings[0]);
                    while (!StringContainsSubString(line, "</Leader"))
                    {
                        std::getline(file, line);
                    }
                }
            }
            //leader not editable
            while (!StringContainsSubString(line, "</Leader"))
            {
                str += line + '\n';
                std::getline(file, line);
            }
        }
        //no leader found
        if (StringContainsSubString(line, "</Leaders>"))
        {
            for (std::unordered_map<std::string, Leader&>::iterator it = leaderMap.begin(); it != leaderMap.end(); it++)
            {
                it->second.Save(line, str);
                str += std::string("\t\t") + "</Leader>" + '\n';
            }
            str += line + '\n';
            std::getline(file, line);
            continue;
        }

        if (file.eof())
        {
            str += line;
            break;
        }
        str += line + '\n';
    }
    /*
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
    }*/
}


inline void SaveLeaders(std::vector<Leader>& leaders)
{
    std::string saveBuildString = "";
    BuildSaveString(saveBuildString, leaders);
    std::ofstream file(leadersPath, std::ios_base::binary);
    //saveBuildString.erase(saveBuildString.begin() + saveBuildString.size() - 1);
    //file << saveBuildString;
    file.write(saveBuildString.c_str(), saveBuildString.size());
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