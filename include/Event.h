#pragma once
#include <windows.h>
#include <evntcons.h>
#include <string>
#include <cstdio>
#include <vector>
class EventHeader
{
    unsigned short m_size;
    unsigned short m_type;
    unsigned short m_flags;
    unsigned short m_property;
    unsigned long m_tid;
    unsigned long m_pid;
    LARGE_INTEGER m_timestamp;
    GUID m_providerid;
    unsigned short m_id;
    unsigned char m_version;
    unsigned char m_channel;
    unsigned char m_level;
    unsigned char m_opcode;
    unsigned short m_task;
    unsigned long long m_keyword;

public:
    EventHeader(EVENT_HEADER* header) : m_size(header->Size), m_type(header->HeaderType),
    m_flags(header->Flags), m_property(header->EventProperty), m_tid(header->ThreadId == -1 ? 0 : header->ThreadId),
    m_pid(header->ProcessId == -1 ? 0 : header->ProcessId), m_timestamp(header->TimeStamp), m_providerid(header->ProviderId),
    m_id(header->EventDescriptor.Id), m_version(header->EventDescriptor.Version),
    m_channel(header->EventDescriptor.Channel), m_level(header->EventDescriptor.Level),
    m_opcode(header->EventDescriptor.Opcode), m_task(header->EventDescriptor.Task),
    m_keyword(header->EventDescriptor.Keyword)
    {

    }
    unsigned short get_size()
    {
        return m_size;
    }
    unsigned short get_flags()
    {
        return m_flags;
    }
    unsigned short get_property()
    {
        return m_property;
    }
    unsigned long get_tid()
    {
        return m_tid;
    }
    unsigned long get_pid()
    {
        return m_pid;
    }
    GUID get_providerId()
    {
        return m_providerid;
    }
    unsigned short get_id()
    {
        return m_id;
    }
    unsigned char get_vers()
    {
        return m_version;
    }
    unsigned char get_channel()
    {
        return m_channel;
    }
    unsigned char get_level()
    {
        return m_level;
    }
    unsigned char get_opcode()
    {
        return m_opcode;
    }
    unsigned short get_task()
    {
        return m_task;
    }
    unsigned long long get_keywords()
    {
        return m_keyword;
    }
    LARGE_INTEGER get_timestamp()
    {
        return m_timestamp;
    }
};

class Event
{
    EventHeader m_header;
    ETW_BUFFER_CONTEXT m_context;
    unsigned char* m_data;
    unsigned short m_length;
    unsigned short m_extcount;
public:
    Event(EVENT_RECORD* record) : m_header(&record->EventHeader), m_data(nullptr),
    m_length(record->UserDataLength), m_context(record->BufferContext), m_extcount(record->ExtendedDataCount)
    {
        m_data = new unsigned char[m_length+1]{0};
        memcpy_s(m_data, m_length, record->UserData, m_length);
    }
    ~Event()
    {
        delete[] m_data;
    }

    unsigned short get_extdata_count()
    {
        return m_extcount;
    }
    unsigned short get_data_length()
    {
        return m_length;
    }
    const ETW_BUFFER_CONTEXT* get_context()
    {
        return &m_context;
    }
    bool get_hex_dump(std::vector<unsigned char>& bytes)
    {
        for(int i = 0; i < m_length; i++)
            bytes.push_back(m_data[i]);
        return true;
    }
    bool get_str(std::string& str)
    {
        char buffer[1024] = {0};

        SYSTEMTIME st;
        FILETIME ft;
        LARGE_INTEGER li = m_header.get_timestamp();
        ft.dwLowDateTime = li.LowPart;
        ft.dwHighDateTime = li.HighPart;
        FileTimeToSystemTime(&ft, &st);
        std::string guid;
        get_providerId(guid);
        sprintf_s(buffer, 1024, " %d-%02d-%02d %02d:%02d:%02d.%03d %04x.%04x %s",
            st.wYear,
            st.wMonth,
            st.wDay,
            st.wHour,
            st.wMinute,
            st.wSecond,
            st.wMilliseconds,
            m_header.get_pid(),
            m_header.get_tid(),
            guid.c_str());

        str = buffer;
        return true;
    }
    unsigned short get_size()
    {
        return m_header.get_size();
    }
    unsigned short get_flags()
    {
        return m_header.get_flags();
    }
    unsigned short get_property()
    {
        return m_header.get_property();
    }
    unsigned long get_tid()
    {
        return m_header.get_tid();
    }
    unsigned long get_pid()
    {
        return m_header.get_pid();
    }
    void get_timestamp(std::string& str)
    {
        char buffer[1024]{0};

        SYSTEMTIME st;
        FILETIME ft;
        LARGE_INTEGER li = m_header.get_timestamp();
        ft.dwLowDateTime = li.LowPart;
        ft.dwHighDateTime = li.HighPart;
        FileTimeToSystemTime(&ft, &st);

        sprintf_s(buffer, 1024, "%d-%02d-%02d %02d:%02d:%02d.%03d",
            st.wYear,
            st.wMonth,
            st.wDay,
            st.wHour,
            st.wMinute,
            st.wSecond,
            st.wMilliseconds);

        str = buffer;
    }

    void get_providerId(std::string& guid_str)
    {
        char buffer[1024]{0};
        GUID guid = m_header.get_providerId();
        sprintf_s(buffer, 1024, "{%08lX-%04hX-%04hX-%02hX%02hX-%02hX%02hX%02hX%02hX%02hX%02hX}",
              guid.Data1, guid.Data2, guid.Data3,
              guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
              guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
        guid_str = buffer;
    }

    unsigned short get_id()
    {
        return m_header.get_id();
    }

    unsigned char get_vers()
    {
        return m_header.get_vers();
    }

    unsigned char get_channel()
    {
        return m_header.get_channel();
    }

    unsigned char get_level()
    {
        return m_header.get_level();
    }

    unsigned char get_opcode()
    {
        return m_header.get_opcode();
    }

    unsigned short get_task()
    {
        return m_header.get_task();
    }

    unsigned long long get_keywords()
    {
        return m_header.get_keywords();
    }

};