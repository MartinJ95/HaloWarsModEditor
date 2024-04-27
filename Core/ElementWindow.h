#pragma once
#include "Leaders.h"
#include "LeaderUIHandler.h"
#include "Utils.h"
#include "wx/wx.h"

class ElementWindow : public wxScrolledWindow
{
public:
    LeaderUIHandler leaders;
    wxBoxSizer* sizer;
    ElementWindow(wxFrame* frame) : leaders(this), wxScrolledWindow(frame, wxID_ANY)
    {
        sizer = new wxBoxSizer(wxHORIZONTAL);
        this->SetSizer(sizer);
        this->FitInside();
        this->SetScrollRate(5, 0);
        this->Show();
    }
    void MakeWindow(const WindowType elementType)
    {
        switch (elementType)
        {
        case WindowType::eLeaders:
            leaders.DisplayLeaders();
        default:
            break;
        }
        FitInside();
        Show();
    }
};