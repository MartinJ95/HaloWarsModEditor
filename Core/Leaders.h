#pragma once
#include <vector>
#include <fstream>
#include <string>
#include "Utils.h"

const std::string LeaderResources[4] = {
    "Supplies",
    "CampaignFoo",
    "Collectable",
    "Power"
};

const std::string leadersPath = ("..\\ModData\\data\\leaders.xml");
const std::string leadersBackUpPath("..\\ModData\\data\\leadersBackUp.xml");

inline void SaveMultipleValues(std::ifstream& file, std::string& line, std::string& saveBuildString, const std::vector<std::string>&& values);
inline void SaveMultipleValues(std::ifstream& file, std::string& line, std::string& saveBuildString, const std::vector<int>&& values);


struct InitialLeaderVals
{
    std::string name;
    std::string originalName;
    std::string iconPath;
    int leaderPickOrder;
    int statsID;
    std::string defaultPlayerSlotFlags;
    void Load(const std::string& str);
    void Save(const std::string& line, std::string& saveBuildString);
};

struct CivDetails
{
    std::string civ;
    std::string tech;
    int nameID;
    int descriptionID;
    void Load(std::string& str, std::ifstream& Stream);
    void Save(std::ifstream& file, std::string& line, std::string& saveBuildString);
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
            leaderResources[i] = std::pair<std::string, int>(LeaderResources[i], std::stoi(nstrings[0]));
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
            GetAllStringsInLine(line, nstrings, { '>', '<' });

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