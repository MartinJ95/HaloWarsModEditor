#include "LeaderUIHandler.h"
#include "ElementWindow.h"

LeaderPane::LeaderPane(Leader* Ref, wxWindow* parent, wxWindowID ID) : wxScrolledWindow(parent, ID, wxDefaultPosition, wxSize(200, 800)), ref(Ref)
{
    sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(sizer);
    this->FitInside();
    this->SetScrollRate(0, 5);
}

LeaderPane::LeaderPane(Leader* Ref, wxWindow* parent, wxWindowID ID, const std::vector<EditBoxVals>& Vals) : wxScrolledWindow(parent, ID, wxDefaultPosition, wxSize(200, 600)), ref(Ref)
{
    sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(sizer);
    this->FitInside();
    this->SetScrollRate(0, 5);

    removeLeader = new wxButton(this, wxID_ANY, "remove leader");
    sizer->Add(removeLeader);
    removeLeader->Bind(wxEVT_BUTTON, &LeaderPane::RemoveLeader, this);

    for (int j = 0; j < Vals.size(); j++)
    {
        EditBoxVals val;
        //final.frame = Vals[j].frame;
        //final.frame = scrolledWindow;
        //val.frame = leaderPanes.back();
        val.frame = this;
        val.name = Vals[j].name;
        val.existingValue = Vals[j].existingValue;
        val.type = Vals[j].type;
        val.val = Vals[j].val;
        val.ID = wxID_ANY;
        val.position = wxPoint();
        //val.ID = 50 + j + (50 * i);
        //val.position = wxPoint(x, y + y * j);
        leaderEditBoxes.emplace_back(val);
        //panes.back()->sizer->Add(panes.back()->leaderEditBoxes.back().name);
        //panes.back()->sizer->Add(panes.back()->leaderEditBoxes.back().value);
    }
}

void LeaderPane::AddBoxesToSizer()
{
    for (std::vector<EditBox>::iterator it = leaderEditBoxes.begin(); it != leaderEditBoxes.end(); it++)
    {
        sizer->Add(it->name, wxEXPAND);
        sizer->Add(it->value, wxEXPAND);
    }
    FitInside();
    Show();
}

void LeaderPane::LoadPopulations(Leader& leader)
{
    for (std::vector<PopDefine>::iterator it = leader.populations.begin(); it != leader.populations.end(); it++)
    {
        populations.emplace_back(this, &populations,
            std::vector<EditBoxVals>{
            EditBoxVals{ "population type :", it->popType, &it->popType, VarType::eString, wxPoint(), (wxFrame*)this, 0 },
                EditBoxVals{ "population max :", std::to_string(it->max), &it->max, VarType::eInt, wxPoint(), (wxFrame*)this, 0 },
                EditBoxVals{ "population current :", std::to_string(it->current), &it->current, VarType::eInt, wxPoint(), (wxFrame*)this, 0 }
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

void LeaderPane::LoadStartingSquads(Leader& leader)
{
    for (std::vector<StartingSquad>::iterator it = leader.startProperties.startingSquads.begin(); it != leader.startProperties.startingSquads.end(); it++)
    {
        squads.emplace_back(this, &squads,
            std::vector<EditBoxVals>{
            EditBoxVals{ "squad fly in :" , it->flyIn ? "true" : "false", &it->flyIn, VarType::eBool, wxPoint(), (wxFrame*)this, 0 },
                EditBoxVals{ "squad offset x:", std::to_string(it->offset.x), &it->offset.x, VarType::eInt, wxPoint(), (wxFrame*)this, 0 },
                EditBoxVals{ "squad offset y:", std::to_string(it->offset.y), &it->offset.y, VarType::eInt, wxPoint(), (wxFrame*)this, 0 },
                EditBoxVals{ "squad offset z:", std::to_string(it->offset.z), &it->offset.z, VarType::eInt, wxPoint(), (wxFrame*)this, 0 },
                EditBoxVals{ "squad id :", it->unitID, &it->unitID, VarType::eString, wxPoint(), (wxFrame*)this, 0 }
        });
        squads.back().subtractionButton = new wxButton(this, wxID_ANY, std::string("remove squad"));
        sizer->Add(squads.back().subtractionButton);
        for (std::vector<EditBox>::iterator it2 = squads.back().editBoxes.begin(); it2 != squads.back().editBoxes.end(); it2++)
        {
            sizer->Add(it2->name);
            sizer->Add(it2->value);
        }
    }
    for (std::vector<AddableType>::iterator it = squads.begin(); it != squads.end(); it++)
    {
        it->subtractionButton->Bind(wxEVT_BUTTON, &AddableType::TakeAwayType, &*it);
    }
    FitInside();
    Show();
}

void LeaderPane::ApplyChanges()
{
    for (std::vector<EditBox>::iterator it = leaderEditBoxes.begin(); it != leaderEditBoxes.end(); it++)
    {
        it->ApplyChange();
    }
    if (ref != nullptr)
    {
        ref->populations.clear();
        for (std::vector<AddableType>::iterator it = populations.begin(); it != populations.end(); it++)
        {
            ref->populations.emplace_back(
                PopDefine{
                    it->editBoxes[0].value->GetValue().ToStdString(),
                    std::stoi(it->editBoxes[1].value->GetValue().ToStdString()),
                    std::stoi(it->editBoxes[2].value->GetValue().ToStdString()) }
            );
        }
        ref->startProperties.startingSquads.clear();
        for (std::vector<AddableType>::iterator it = squads.begin(); it != squads.end(); it++)
        {
            ref->startProperties.startingSquads.emplace_back(
                StartingSquad{
                    it->editBoxes[0].value->GetValue().ToStdString() == "true" ? true : false,
                    std::stoi(it->editBoxes[1].value->GetValue().ToStdString()),
                    std::stoi(it->editBoxes[2].value->GetValue().ToStdString()),
                    std::stoi(it->editBoxes[3].value->GetValue().ToStdString()),
                    it->editBoxes[4].value->GetValue().ToStdString()
                }
            );
        }
    }
}

void LeaderPane::AddPopulation(wxCommandEvent& event)
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

void LeaderPane::AddStartingSquad(wxCommandEvent& event)
{
    squads.emplace_back(this, &squads,
        std::vector<EditBoxVals>{
        EditBoxVals{ "Starting Squad fly in :", "true", nullptr, VarType::eBool, wxPoint(), (wxFrame*)this, 0 },
            EditBoxVals{ "squad offset x:", "0", nullptr, VarType::eInt, wxPoint(), (wxFrame*)this, 0 },
            EditBoxVals{ "squad offset y:", "0", nullptr, VarType::eInt, wxPoint(), (wxFrame*)this, 0 },
            EditBoxVals{ "squad offset z:", "0", nullptr, VarType::eInt, wxPoint(), (wxFrame*)this, 0 },
            EditBoxVals{ "squad id :", "unit id", nullptr, VarType::eString, wxPoint(), (wxFrame*)this, 0 }
    });
    squads.back().subtractionButton = new wxButton(this, wxID_ANY, std::string("remove squad: "));

    sizer->Add(squads.back().subtractionButton);
    for (std::vector<EditBox>::iterator it = squads.back().editBoxes.begin(); it != squads.back().editBoxes.end(); it++)
    {
        sizer->Add(it->name);
        sizer->Add(it->value);
    }

    for (std::vector<AddableType>::iterator it = squads.begin(); it != squads.end(); it++)
    {
        it->subtractionButton->Bind(wxEVT_BUTTON, &AddableType::TakeAwayType, &*it);
    }
    FitInside();
    Show();
}

LeaderUIHandler::LeaderUIHandler(ElementWindow* Parent) : parent(Parent)
{
    leaders.reserve(10);
    leaderPanes.reserve(10);
}

void LeaderUIHandler::ShowLeaderDetails(std::vector<wxTextCtrl*>& texts, const std::vector<std::string>& strings, int x, int y, int i)
{
    for (int j = 0; j < strings.size(); j++)
    {
        texts.emplace_back();
        texts.back() = new wxTextCtrl(parent, 50 + j + (50 * i), strings[j], wxPoint(x, y + y * j));
    }
}

void LeaderUIHandler::ShowLeaderDetails(std::vector<EditBox>& EditValues, const std::vector<EditBoxVals>& Vals, int x, int y, int i)
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
        parent->sizer->Add(EditValues.back().name);
        parent->sizer->Add(EditValues.back().value);
    }
}

void LeaderUIHandler::ShowLeaderDetails(std::vector<LeaderPane*>& panes, const std::vector<EditBoxVals>& Vals, int x, int y, int i, Leader& refferedLeader)
{
    panes.emplace_back();
    panes.back() = new LeaderPane(&refferedLeader, parent, wxID_ANY, Vals);

    panes.back()->AddBoxesToSizer();
    panes.back()->addPopulation = new wxButton(panes.back(), wxID_ANY, "add new population");
    panes.back()->sizer->Add(panes.back()->addPopulation);
    LeaderPane* p = panes.back();
    panes.back()->addPopulation->Bind(wxEVT_BUTTON, &LeaderPane::AddPopulation, p);
    panes.back()->LoadPopulations(refferedLeader);
    panes.back()->addStartingSquad = new wxButton(panes.back(), wxID_ANY, "add new starting squad");
    panes.back()->sizer->Add(panes.back()->addStartingSquad);
    panes.back()->addStartingSquad->Bind(wxEVT_BUTTON, &LeaderPane::AddStartingSquad, p);
    panes.back()->LoadStartingSquads(refferedLeader);
    parent->sizer->Add(panes.back(), wxEXPAND);
    parent->FitInside();
    parent->Show();
}

void LeaderUIHandler::DisplayLeaders()
{
    {
        for (std::vector<LeaderPane*>::reverse_iterator rit = leaderPanes.rbegin(); rit != leaderPanes.rend(); rit++)
        {
            delete* rit;
        }
        leaderPanes.clear();

        for (int i = 0; i < leaders.size(); i++)
        {
            std::vector<EditBoxVals> vals{
                { "name: ", leaders[i].initialValues.name, &leaders[i].initialValues.name, VarType::eString, wxPoint(), (wxFrame*)parent, 0 },
                {"icon path: ", leaders[i].initialValues.iconPath,&leaders[i].initialValues.iconPath, VarType::eString, wxPoint(), (wxFrame*)parent, 0},
                {"leader pick order: ", std::to_string(leaders[i].initialValues.leaderPickOrder), &leaders[i].initialValues.leaderPickOrder, VarType::eInt, wxPoint(), (wxFrame*)parent, 0},
                {"stats id: ", std::to_string(leaders[i].initialValues.statsID), &leaders[i].initialValues.statsID, VarType::eInt, wxPoint(), (wxFrame*)parent, 0},
                { "default player slot flags: ", leaders[i].initialValues.defaultPlayerSlotFlags, &leaders[i].initialValues.defaultPlayerSlotFlags, VarType::eString, wxPoint(), (wxFrame*)parent, 0},
                {"civ: ", leaders[i].civDetails.civ, &leaders[i].civDetails.civ, VarType::eString, wxPoint(), (wxFrame*)parent, 0},
                {"tech: ", leaders[i].civDetails.tech, &leaders[i].civDetails.tech, VarType::eString, wxPoint(), (wxFrame*)parent, 0},
                {"name id: ", std::to_string(leaders[i].civDetails.nameID), &leaders[i].civDetails.nameID, VarType::eInt, wxPoint(), (wxFrame*)parent, 0},
                { "descriptionID: ", std::to_string(leaders[i].civDetails.descriptionID), &leaders[i].civDetails.descriptionID, VarType::eInt, wxPoint(), (wxFrame*)parent, 0},
                {"flashID: ", std::to_string(leaders[i].flashDetails.flashID), &leaders[i].flashDetails.flashID, VarType::eInt, wxPoint(), (wxFrame*)parent, 0},
                {"flash image ", leaders[i].flashDetails.flashImg, &leaders[i].flashDetails.flashImg , VarType::eString, wxPoint(), (wxFrame*)parent, 0},
                {"flash portrait: ", leaders[i].flashDetails.flashPortrait, &leaders[i].flashDetails.flashPortrait, VarType::eString, wxPoint(), (wxFrame*)parent, 0},
                {"resource: " + leaders[i].startProperties.leaderResources[0].first, std::to_string(leaders[i].startProperties.leaderResources[0].second), &leaders[i].startProperties.leaderResources[0].second, VarType::eInt, wxPoint(), (wxFrame*)parent, 0},
                { "resource: " + leaders[i].startProperties.leaderResources[1].first, std::to_string(leaders[i].startProperties.leaderResources[1].second), &leaders[i].startProperties.leaderResources[1].second, VarType::eInt, wxPoint(), (wxFrame*)parent, 0 },
                {"resource: " + leaders[i].startProperties.leaderResources[2].first, std::to_string(leaders[i].startProperties.leaderResources[2].second), &leaders[i].startProperties.leaderResources[2].second, VarType::eInt, wxPoint(), (wxFrame*)parent, 0},
                {"resource: " + leaders[i].startProperties.leaderResources[3].first, std::to_string(leaders[i].startProperties.leaderResources[3].second), &leaders[i].startProperties.leaderResources[3].second, VarType::eInt, wxPoint(), (wxFrame*)parent, 0},
                {"rally point offset.x: ", std::to_string(leaders[i].startProperties.rallyPointOffset.x), &leaders[i].startProperties.rallyPointOffset.x, VarType::eInt, wxPoint(), (wxFrame*)parent, 0},
                { "rally point offset.y: ", std::to_string(leaders[i].startProperties.rallyPointOffset.y), &leaders[i].startProperties.rallyPointOffset.y, VarType::eInt, wxPoint(), (wxFrame*)parent, 0 },
                { "rally point offset.z: ", std::to_string(leaders[i].startProperties.rallyPointOffset.z), &leaders[i].startProperties.rallyPointOffset.z, VarType::eInt, wxPoint(), (wxFrame*)parent, 0 },
                {"starting unit Socket: ", leaders[i].startProperties.startUnit.socket, &leaders[i].startProperties.startUnit.socket , VarType::eString, wxPoint(), (wxFrame*)parent, 0}
            };
            ShowLeaderDetails(leaderPanes, vals,
                400 * i,
                20,
                i,
                leaders[i]
            );
            parent->FitInside();
            parent->Show();
        }
        //parent->FitInside();
        for (std::vector<LeaderPane*>::iterator it = leaderPanes.begin(); it != leaderPanes.end(); it++)
        {
            LeaderPane* ref = *it;
            ref->FitInside();
        }
        parent->FitInside();
        parent->Show();
        parent->GetParent()->FitInside();
        parent->GetParent()->Show();
    }
}

void LeaderPane::RemoveLeader(wxCommandEvent& event)
{
    ElementWindow* parentWindow = (ElementWindow*)GetParent();
    ref->RemoveLeader(&parentWindow->leaders);
    for (std::vector<LeaderPane*>::iterator it = parentWindow->leaders.leaderPanes.begin(); it != parentWindow->leaders.leaderPanes.end(); it++)
    {
        if (*it == this)
        {
            delete* it;
            parentWindow->leaders.leaderPanes.erase(it);
            parentWindow->FitInside();
            parentWindow->Show();
            break;
        }
    }
}
