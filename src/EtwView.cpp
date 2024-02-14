#include "EtwView.h"
#include "imgui.h"
#include <windows.h>
#include <atlbase.h>
#include <shobjidl_core.h>
#include <vector>

bool is_printable(unsigned char value)
{
    return value >= 32 && value <= 126;
}

EtwView::EtwView() : m_running(true), m_view(View::Data)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if(FAILED(hr))
    {
        // TODO(will): Handle if this fails...
    }
}

EtwView::~EtwView()
{
    CoUninitialize();
}

void EtwView::show_view()
{
    static bool show_demo = false;
    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->WorkPos);
    ImGui::SetNextWindowSize(vp->WorkSize);
    ImGui::Begin("Internal Window", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNav);
    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("Open"))
            {
                load_file();
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Help"))
        {
#if _DEBUG
            ImGui::MenuItem("Demo", "", &show_demo);
#endif
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    switch(m_view)
    {
    case View::Landing:
        show_landing();
        break;
    case View::Data:
        show_data();
        break;
    default:
        break;
    }

#if _DEBUG
    if(show_demo)
        ImGui::ShowDemoWindow(&show_demo);
#endif

    ImGui::End();
}

void EtwView::show_data()
{
    if(!m_trace.data_available())
    {
        ImGui::Text("No data available");
        return;
    }

    static int selected_index = 0;
    const auto& records = m_trace.get_records();
    if(ImGui::BeginListBox("##Items", ImVec2(-FLT_MIN, 20 * ImGui::GetTextLineHeightWithSpacing())))
    {
        for(int i = 0; i < records.size(); i++)
        {
            std::string buffer;
            records[i]->get_str(buffer);
            buffer = std::to_string(i+1) + buffer;
            const bool is_selected = selected_index == i;
            if(ImGui::Selectable(buffer.c_str(), is_selected))
                selected_index = i;
            if(is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }

    ImGui::SeparatorText("Details");
    if(ImGui::BeginChild("DetailTree", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f,
        ImGui::GetContentRegionAvail().y), true ))
    {
        if(ImGui::TreeNode("Etw Structure"))
        {
            if(ImGui::TreeNode("Header"))
            {
                ImGui::Text("Size: %d (0x%x)",
                    records[selected_index]->get_size(),
                    records[selected_index]->get_size());
                ImGui::Text("Header Type: 0 (0x0)");
                ImGui::Text("Flags: %d (0x%x)",
                    records[selected_index]->get_flags(),
                    records[selected_index]->get_flags());
                ImGui::Text("Property: %d (0x%x)",
                    records[selected_index]->get_property(),
                    records[selected_index]->get_property());
                ImGui::Text("Thread Id: %d (0x%x)",
                    records[selected_index]->get_tid(),
                    records[selected_index]->get_tid());
                ImGui::Text("Process Id: %d (0x%x)",
                    records[selected_index]->get_pid(),
                    records[selected_index]->get_pid());
                std::string ts;
                records[selected_index]->get_timestamp(ts);
                ImGui::Text("Timestamp: %s", ts.c_str());
                std::string guid;
                records[selected_index]->get_providerId(guid);
                ImGui::Text("Provider Id: %s", guid.c_str());
                if(ImGui::TreeNode("Descriptor"))
                {
                    ImGui::Text("Id %d (0x%x)", records[selected_index]->get_id(),
                        records[selected_index]->get_id());
                    ImGui::Text("Version: %d (0x%x)", records[selected_index]->get_vers(),
                        records[selected_index]->get_vers());
                    ImGui::Text("Channel %d (0x%x)", records[selected_index]->get_channel(),
                        records[selected_index]->get_channel());
                    ImGui::Text("Level: %d", records[selected_index]->get_level());
                    ImGui::Text("Opcode: %d", records[selected_index]->get_opcode());
                    ImGui::Text("Task %d (0x%x)", records[selected_index]->get_task(),
                        records[selected_index]->get_task());
                    ImGui::Text("Keywords: %d (0x%x)", records[selected_index]->get_keywords(),
                        records[selected_index]->get_keywords());
                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }

            if(ImGui::TreeNode("Buffer Context"))
            {
                auto context = records[selected_index]->get_context();
                ImGui::Text("Processor Number: %d (0x%x)",
                    context->ProcessorNumber,
                    context->ProcessorNumber);
                ImGui::Text("Alignment: %d (0x%x)",
                    context->Alignment,
                    context->Alignment);
                ImGui::Text("Logger Id: %d (0x%x)",
                    context->LoggerId,
                    context->LoggerId);
                ImGui::TreePop();
            }

            ImGui::Text("Extended Data Count: %d (0x%x)",
                records[selected_index]->get_extdata_count(),
                records[selected_index]->get_extdata_count());
            ImGui::Text("User Data Length: %d (0x%x)",
                records[selected_index]->get_data_length(),
                records[selected_index]->get_data_length());
            ImGui::TreePop();
        }
        ImGui::EndChild();
    }

    ImGui::SameLine();

    std::vector<unsigned char> buffer;
    records[selected_index]->get_hex_dump(buffer);
    if(ImGui::BeginChild("Hex", ImVec2(0, ImGui::GetContentRegionAvail().y), true, ImGuiWindowFlags_HorizontalScrollbar))
    {
        size_t j = 0;
        size_t i = 0;
        while(i < buffer.size())
        {
            if(i % 0x10 == 0)
            {
                while(j < i)
                {
                    ImGui::SameLine();
                    if(is_printable(buffer[j]))
                        ImGui::Text(" %c", buffer[j]);
                    else
                        ImGui::Text(" .");
                    j++;
                }

                ImGui::Text("%04X ", i);
                ImGui::SameLine();
            }
            ImGui::Text("%02X ", buffer[i]);

            if(i % 0x10 != 0x10 - 1)
                ImGui::SameLine();
            i++;
        }
        // Get the last few bits
        for(size_t k = i; k % 0x10 != 0; k++)
        {
            ImGui::SameLine();
            ImGui::Text("   ");
        }
        for(size_t k = j; k < i; k++)
        {
            ImGui::SameLine();
            if(is_printable(buffer[k]))
                ImGui::Text(" %c", buffer[k]);
            else
                ImGui::Text(" .");
        }
        ImGui::EndChild();
    }
}

void EtwView::show_landing()
{

}

void EtwView::load_file()
{
    constexpr COMDLG_FILTERSPEC filter =  { L"Etl Trace (*.etl)", L"*.etl"} ;
    CComPtr<IFileOpenDialog> open_dlg = nullptr;
    HRESULT hr = open_dlg.CoCreateInstance(__uuidof(FileOpenDialog));
    if(SUCCEEDED(hr))
    {
        open_dlg->SetFileTypes(1, &filter);
        if(SUCCEEDED(open_dlg->Show(NULL)))
        {
            CComPtr<IShellItem> selection = nullptr;
            if(SUCCEEDED(open_dlg->GetResult(&selection)))
            {
                wchar_t* path = nullptr;
                if(SUCCEEDED(selection->GetDisplayName(SIGDN_FILESYSPATH, &path)))
                {
                    m_trace.load_file(path);
                }
            }
        }

    }
}
