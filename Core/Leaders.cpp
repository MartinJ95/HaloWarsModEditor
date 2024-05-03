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
    if (StringContainsSubString(line, "Alpha"))
    {
        hasAlpha = true;
    }

    std::vector<std::string> subStrings;
    GetAllStringsInLine(line, subStrings, { '"', '"' }, true);
    /*
    saveBuildString += (subStrings[0] + Stringify(name) + subStrings[1] + Stringify(iconPath));
    <Leader Name = "Cutter" Icon = "ui\game\icon\unsc\leader\Captain Cutter" LeaderPickerOrder = "2" StatsID = "1" DefaultPlayerSlotFlags = "0x81">
        saveBuildString += "\t" + "<Leader Name = " + Stringify(name) + " Icon = " + Stringify(iconPath);
    */
    //<Leader Name = "Cutter" Icon = "ui\game\icon\unsc\leader\Captain Cutter" LeaderPickerOrder = "2" StatsID = "1" DefaultPlayerSlotFlags = "0x81">
    saveBuildString += "\t\t" + std::string("<Leader Name=") + Stringify(name) + " Icon=" + Stringify(iconPath);
    if (hasAlpha)
    {
        //saveBuildString += (subStrings[2] + Stringify("0"));
        saveBuildString += (" Alpha=" + Stringify("0"));
    }
    saveBuildString += " LeaderPickerOrder=" + Stringify(leaderPickOrder) + " StatsID=" + Stringify(statsID) + " DefaultPlayerSlotsFlags=" + Stringify(defaultPlayerSlotFlags) + '>' + '\n';
    /*saveBuildString += (subStrings[2 + hasAlpha] + Stringify(leaderPickOrder) + subStrings[3 + hasAlpha] +
        Stringify(statsID) + subStrings[4 + hasAlpha] + Stringify(defaultPlayerSlotFlags) + subStrings[5 + hasAlpha] + "\n");*/
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

    /*
    SaveMultipleValues(file, line, saveBuildString,
        std::move(
            std::vector<std::string>{civ, tech}
    )
    );
    */
    std::vector<std::pair<std::string, std::string>> vals{
        {"Civ", civ}, {"Tech", tech}, {"NameID", std::to_string(nameID)}, {"DescriptionID", std::to_string(descriptionID)}
    };
    for (std::vector<std::pair<std::string, std::string>>::iterator it = vals.begin(); it != vals.end(); it++)
    {
        saveBuildString += "\t\t" + Fieldify(it->first) + it->second + Fieldify('/' + it->first) + "\n";
    }
    /*
    SaveMultipleValues(file, line, saveBuildString,
        std::move(
            std::vector<int>{nameID, descriptionID}
    )
    );
    */
}

void Leader::Load(std::ifstream& file, std::string& line)
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

void Leader::Save(std::ifstream& file, std::string& line, std::string& saveBuildString)
{
    initialValues.Save(line, saveBuildString);
    std::getline(file, line);
    civDetails.Save(file, line, saveBuildString);
    flashDetails.Save(file, line, saveBuildString);
    startProperties.Save(file, line, saveBuildString);
    repairProperties.Save(file, line, saveBuildString);
    if (hasReverseDrop)
    {
        /*
        std::vector<std::string> strings;
        std::vector<std::string> nstrings;

        GetAllStringsInLine(line, strings, '"', true, true);
        GetAllStringsInLine(line, nstrings, { '>',  '<' }, true, true);

        //saveBuildString += strings[0] + Stringify(reverseDropCost.first) + Valuefy(reverseDropCost.second) + nstrings[1] + "\n";
        saveBuildString += strings[0] + Stringify(reverseDropCost.first) + '>' + std::to_string(reverseDropCost.second) + Fieldify(nstrings[3]) + "\n";

        std::getline(file, line);
        */
        //<ReverseHotDropCost Type = "Supplies">100 < / ReverseHotDropCost >
        saveBuildString += "\t\t" + std::string("<ReverseHotDropCost Type=") + Stringify(reverseDropCost.first) + ">" + std::to_string(reverseDropCost.second) + "</ReverseHotDropCost>" + "\n";
    }

    for (std::vector<PopDefine>::iterator it = populations.begin(); it != populations.end(); it++)
    {
        saveBuildString += "\t\t" + std::string("<Pop Type=") + Stringify(it->popType) + std::string(" Max=") + Stringify(std::to_string(it->max)) + '>' + std::to_string(it->current) + std::string("</Pop>\n");
    }
    /*
    while (StringContainsSubString(line, "<Pop"))
    {

        std::getline(file, line);
    }
    */
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

void FlashDetails::Load(std::string& str, std::ifstream& Stream)
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

void FlashDetails::Save(std::ifstream& file, std::string& line, std::string& saveBuildString)
{
    {
        std::vector<std::string> substrings;
        /*
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
        */
        std::vector<std::pair<std::string, std::string>>vals{
            {"FlashCivID", std::to_string(flashID)}, {"FlashImg", flashImg}, {"FlashPortrait", flashPortrait}, {"UIControlBackground", UIControlBackground}
        };

        for (std::vector<std::pair<std::string, std::string>>::iterator it = vals.begin(); it != vals.end(); it++)
        {
            saveBuildString += "\t\t" + Fieldify(it->first) + it->second + Fieldify('/' + it->first) + '\n';
        }
    }
}

void StartingUnit::Load(const std::string& str, std::vector<std::string>& strings, std::vector<std::string>& nstrings)
{
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
}

void StartingUnit::Save(const std::string& line, std::string& buildSaveString)
{
    std::vector<std::string> strings;
    std::vector<std::string> nstrings;

    GetAllStringsInLine(line, strings, '"', true, true);
    GetAllStringsInLine(line, nstrings, { '>', '<' }, true, true);
    //buildSaveString += strings[0] + Stringify(offset.GetStringVersion()) + strings[2] + Stringify(buildOther) + strings[4] + Stringify(doppleOnStart ? "true" : "false") + '>' + socket + Fieldify(nstrings[3]) + "\n";
    //<StartingUnit Offset = "0,0,0" BuildOther = "cov_bldg_builder_02" DoppleOnStart = "true">game_base_socket_01< / StartingUnit>
    buildSaveString += "\t\t" + std::string("<StartingUnit Offset=") + Stringify(offset.GetStringVersion()) + " BuildOther=" + Stringify(buildOther) + " DoppleOnStart=" + Stringify(doppleOnStart ? "true" : "false") +
        '>' + socket + "</StartingUnit>" + "\n";
}

void StartingSquad::Load(const std::string& str, std::vector<std::string>& strings, std::vector<std::string>& nstrings)
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

void StartingSquad::Save(const std::string& line, std::string& buildSaveString)
{
    buildSaveString += "\t\t" + std::string("<StartingSquad FlyIn=") + Stringify(flyIn ? "true" : "false") + " Offset=" + Stringify(offset.GetStringVersion()) + '>' + unitID + "</StartingSquad>\n";
    /*std::vector<std::string> strings;
    std::vector<std::string> nstrings;

    GetAllStringsInLine(line, strings, '"', true, true);
    GetAllStringsInLine(line, nstrings, { '>', '<' }, true, true);

    buildSaveString += strings[0] + Stringify(flyIn ? "true" : "false") + strings[2] + Stringify(offset.GetStringVersion()) + '>' + unitID + Fieldify(nstrings[3]) + "\n";
    */
}

void StartingProperties::Load(std::string& str, std::ifstream& Stream)
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

    while (StringContainsSubString(str, "StartingSquad"))
    {
        startingSquads.emplace_back();
        startingSquads.back().Load(str, strings, nstrings);

        std::getline(Stream, str);
    }

    /*
    startingSquads.emplace_back();
    startingSquads.back().Load(str, strings, nstrings);

    std::getline(Stream, str);
    */

    std::vector<std::string> vecVals;

    GetAllStringsInLine(str, nstrings, { '>', '<' });
    GetAllStringsInLine(nstrings[0], vecVals, { ',', ',' }, true, true);

    rallyPointOffset.x = std::stoi(vecVals[0]);
    rallyPointOffset.y = std::stoi(vecVals[1]);
    rallyPointOffset.z = std::stoi(vecVals[2]);
}

void StartingProperties::Save(std::ifstream& file, std::string& line, std::string& saveBuildString)
{
    std::vector<std::string> nstrings;

    for (int i = 0; i < 4; i++)
    {
        /*
        GetAllStringsInLine(line, nstrings, { '>', '<' }, true, true);
        saveBuildString += nstrings[0] + Fieldify(nstrings[1]) + std::to_string(leaderResources[i].second) + Fieldify(nstrings[3]) + "\n";
        std::getline(file, line);
        */
        //<Resource Type="Supplies">800</Resource>
        saveBuildString += "\t\t" + std::string("<Resource Type=") + Stringify(leaderResources[i].first) + '>' + std::to_string(leaderResources[i].second) + "</Resource>" + "\n";
    }

    startUnit.Save(line, saveBuildString);
    //std::getline(file, line);
    for (std::vector<StartingSquad>::iterator it = startingSquads.begin(); it != startingSquads.end(); it++)
    {
        it->Save(line, saveBuildString);
        //std::getline(file, line);
    }
    while (StringContainsSubString(line, "StartingSquad"))
    {
        //std::getline(file, line);
    }
    GetAllStringsInLine(line, nstrings, { '>', '<' }, true, true);
    //saveBuildString += nstrings[0] + Fieldify(nstrings[1]) + rallyPointOffset.GetStringVersion() + Fieldify(nstrings[3]) + "\n";
    //std::getline(file, line);
    //<RallyPointOffset>50, 0, 0 < / RallyPointOffset >
    saveBuildString += "\t\t" + std::string("<RallyPointOffset>") + rallyPointOffset.GetStringVersion() + "</RallyPointOffset>" + "\n";
}

void RepairProperties::Load(std::string& str, std::ifstream& Stream)
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

void RepairProperties::Save(std::ifstream& file, std::string& line, std::string& saveBuildString)
{
    std::vector<std::string> strings;
    std::vector<std::string> nstrings;
    /*
    auto save = [](const std::string& str, std::string& line, std::ifstream& f, std::string& sbs, std::vector<std::string>& nstrings) {
        GetAllStringsInLine(line, nstrings, { '>', '<' }, true, true);
        sbs += nstrings[0] + Fieldify(nstrings[1]) + str + Fieldify(nstrings[3]) + "\n";

        std::getline(f, line);
        };
    */

    auto save = [](const std::string& field, const std::string& value, std::string& saveBuildString)
        {
            saveBuildString += "\t\t" + Fieldify(field) + value + Fieldify("/" + field) + "\n";
        };

    //save(std::to_string(repairRate), line, file, saveBuildString, nstrings);
    //save(std::to_string(repairDelay), line, file, saveBuildString, nstrings);
    save(std::string("RepairRate"), std::to_string(repairRate), saveBuildString);
    save(std::string("RepairDelay"), std::to_string(repairDelay), saveBuildString);

    /*
    GetAllStringsInLine(line, strings, '"', true);
    GetAllStringsInLine(line, nstrings, { '>', '<' }, true, true);

    saveBuildString += strings[0] + Stringify(repairCost.first) + '>' + std::to_string(repairCost.second) + Fieldify(nstrings[3]) + "\n";
    std::getline(file, line);
    */
    //<RepairCost Type = "Supplies">35 < / RepairCost >
    saveBuildString += "\t\t" + std::string("<RepairCost Type=") + Stringify(repairCost.first) + ">" + std::to_string(repairCost.second) + "</RepairCost>" + "\n";

    //save(std::to_string(repairTime), line, file, saveBuildString, nstrings);
    save(std::string("RepairTime"), std::to_string(repairTime), saveBuildString);
}
