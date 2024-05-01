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
#include "Leaders.h"
#include "ElementWindow.h"
#include "LeaderUIHandler.h"

class MyFrame : public wxFrame
{
protected:
    wxMenu* menuFile;
    wxMenu* menuHelp;
    wxMenu* leaderMenu;

    ElementWindow window;
    wxBoxSizer* sizer;

    wxMenuBar* menuBar;

    //std::vector<Leader> leaders;
public:
    MyFrame() :
        wxFrame(nullptr, wxID_ANY, "Halo Wars Mod Editor"), window(this)
    {
        window.leaders.leaders.reserve(10);
        window.leaders.leaderPanes.reserve(10);

        leaderMenu = new wxMenu;
        leaderMenu->Append(ID_LoadLeaders, "&Load Leaders");
        leaderMenu->Append(ID_DisplayLeaders, "&Display Leaders");
        leaderMenu->Append(ID_SaveLeaders, "&Save Leaders");
        leaderMenu->Append(ID_BackupLeaders, "&Backup Leaders");
        leaderMenu->Append(ID_AddLeader, "&Add Leader");

        menuBar = new wxMenuBar;

        menuBar->Append(leaderMenu, "Leaders");

        SetMenuBar(menuBar);



        window.FitInside();
        window.Show();

        sizer = new wxBoxSizer(wxHORIZONTAL);

        sizer->Add((wxWindow*)&window, wxEXPAND);

        this->SetSizer(sizer);

        this->FitInside();
        this->Show();

        CreateStatusBar();
        SetStatusText("Welcome to wxWidgets!");

        Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
        Bind(wxEVT_MENU, &MyFrame::OnLeaderLoad, this, ID_LoadLeaders);
        Bind(wxEVT_MENU, &MyFrame::OnLeaderDisplay, this, ID_DisplayLeaders);
        Bind(wxEVT_MENU, &MyFrame::OnLeaderSave, this, ID_SaveLeaders);
        Bind(wxEVT_MENU, &MyFrame::OnLeaderBackUp, this, ID_BackupLeaders);
        Bind(wxEVT_MENU, &MyFrame::OnLeaderAdd, this, ID_AddLeader);
    }
private:
    void OnExit(wxCommandEvent& event)
    {
        Close(true);
    }
    
    void OnLeaderLoad(wxCommandEvent& event)
    {
        window.leaders.LeaderLoad();
    }

    void OnLeaderSave(wxCommandEvent& event)
    {
        ApplyChangedValues(window.leaders.leaderPanes);
        SaveLeaders(window.leaders.leaders);
    }

    void OnLeaderBackUp(wxCommandEvent& event)
    {
        BackUpLeaders();
    }

    void OnLeaderDisplay(wxCommandEvent& event)
    {
        window.leaders.DisplayLeaders();
    }

    void OnLeaderAdd(wxCommandEvent& event)
    {
        window.leaders.leaders.emplace_back();
        OnLeaderDisplay(event);
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