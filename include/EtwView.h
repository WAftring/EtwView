// EtwView.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include "EtwTrace.h"
#include <iostream>

enum class View
{
    Landing,
    Data
};

class EtwView
{
public:
    EtwView();
    ~EtwView();
    void show_view();
    bool is_running() { return m_running; }
private:
    View m_view;
    EtwTrace m_trace;
    bool m_running;
    void show_data();
    void show_landing();
    void load_file();
};