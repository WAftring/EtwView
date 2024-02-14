#pragma once
#include "Event.h"
#include <windows.h>
#include <evntcons.h>
#include <vector>
#include <memory>

class EtwTrace
{
public:
    EtwTrace();
    ~EtwTrace();
    void load_file(const wchar_t* path);
    bool data_available()
    {
        if(m_data_available && NULL != m_thread)
        {
            CloseHandle(m_thread);
            m_thread = NULL;
        }
        return m_data_available;
    }
    size_t get_record_count()
    {
        return m_records.size();
    }

    std::vector<std::shared_ptr<Event>> get_records()
    {
        return m_records;
    }
private:
    friend void record_callback(EVENT_RECORD* record);
    friend DWORD WINAPI process_thread(LPVOID param);
    const char* m_path;


    bool m_data_available;
    EVENT_TRACE_LOGFILEA m_tracefile;
    TRACEHANDLE m_handle;
    HANDLE m_thread;
    std::vector<std::shared_ptr<Event>> m_records;
};