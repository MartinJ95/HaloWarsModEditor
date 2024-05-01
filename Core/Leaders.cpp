#include "Leaders.h"
#include "LeaderUIHandler.h"

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

void InitialLeaderVals::Load(const std::string& str)
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
    leaderPickOrder = std::stoi(vals[2 + hasAlpha]);
    statsID = std::stoi(vals[3 + hasAlpha]);
    defaultPlayerSlotFlags = vals[4 + hasAlpha];
}

void InitialLeaderVals::Save(const std::string& line, std::string& saveBuildString)
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

    saveBuildString += (subStrings[2 + hasAlpha] + Stringify(leaderPickOrder) + subStrings[3 + hasAlpha] +
        Stringify(statsID) + subStrings[4 + hasAlpha] + Stringify(defaultPlayerSlotFlags) + subStrings[5 + hasAlpha] + "\n");
}

void CivDetails::Load(std::string& str, std::ifstream& Stream)
{
    std::vector<std::string> vals;
    GetAllStringsInLine(str, vals, { '>', '<' });
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

void CivDetails::Save(std::ifstream& file, std::string& line, std::string& saveBuildString)
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

void Leader::RemoveLeader(LeaderUIHandler* ui)
{
    for (std::vector<Leader>::iterator it = ui->leaders.begin(); it != ui->leaders.end(); it++)
    {
        if (&*it == this)
        {
            ui->leaders.erase(it);
            break;
        }
    }
}
