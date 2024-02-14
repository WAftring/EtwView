#include "EtwTrace.h"

void record_callback(EVENT_RECORD* record);
DWORD WINAPI process_thread(LPVOID param);
EtwTrace::EtwTrace() : m_path(nullptr), m_tracefile({0}), m_handle(INVALID_PROCESSTRACE_HANDLE),
m_thread(NULL), m_data_available(false)
{

}
EtwTrace::~EtwTrace()
{
    if(NULL != m_thread)
        CloseHandle(m_thread);
    if(INVALID_PROCESSTRACE_HANDLE != m_handle)
        CloseTrace(m_handle);
}

void EtwTrace::load_file(const wchar_t* path)
{
    int path_len = static_cast<int>(wcslen(path));
    int size = WideCharToMultiByte(CP_UTF8, 0, path, path_len, nullptr, 0, nullptr, nullptr);
    m_path = new char[size+1]{0};
    WideCharToMultiByte(CP_UTF8, 0, path, path_len, const_cast<LPSTR>(m_path), size, nullptr, nullptr);
    m_tracefile.LogFileName = const_cast<LPSTR>(m_path);
    m_tracefile.ProcessTraceMode = PROCESS_TRACE_MODE_EVENT_RECORD;
    m_tracefile.EventRecordCallback = record_callback;
    m_tracefile.Context = reinterpret_cast<PVOID>(this);
    m_handle = OpenTraceA(&m_tracefile);
    if(INVALID_PROCESSTRACE_HANDLE == m_handle)
    {
        // TODO(will): Handle this...
    }

    m_thread = CreateThread(nullptr,
        0,
        process_thread,
        reinterpret_cast<LPVOID>(this),
        0,
        nullptr);

}

void record_callback(EVENT_RECORD* record)
{
    EtwTrace* trace = reinterpret_cast<EtwTrace*>(record->UserContext);
    trace->m_records.push_back(std::make_shared<Event>(record));
}

DWORD WINAPI process_thread(LPVOID param)
{
    EtwTrace* trace = reinterpret_cast<EtwTrace*>(param);
    DWORD error = ProcessTrace(&trace->m_handle, 1, nullptr, nullptr);
    if(ERROR_SUCCESS != error)
    {
        // TODO(will): Handle this
    }
    else
        trace->m_data_available = true;
    return error;
}