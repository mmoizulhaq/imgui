// dear imgui
// (extra code)


#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "imgui.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui_internal.h"
#include "imgui_extra.h"

#ifndef IMGUI_DISABLE_API

//-----------------------------------------------------------------------------
// [SECTION] Utilities: AddInputString, GetTimeFloat
//-----------------------------------------------------------------------------
// - AddInputString()
// - GetTimeFloat()
//-----------------------------------------------------------------------------

void ImGui::AddInputString(const ImWchar* str)
{
    // We can't pass more wchars than ImGuiIO::InputCharacters[] can hold so don't convert more
    const int wchars_buf_len = sizeof(ImGui::GetIO().InputCharacters) / sizeof(ImWchar);
    const int input_buf_len  = ImStrlenW(str);
    for (int i = 0; i < wchars_buf_len && i < input_buf_len; ++i)
        ImGui::GetIO().AddInputCharacter(str[i]);
}

float ImGui::GetTimeFloat()
{
    return static_cast<float>(ImGui::GetTime());
}

//-----------------------------------------------------------------------------
// [SECTION] Widgets: SliderUnsignedInt, SliderLongInt, SliderUnsignedLongInt, etc.
//-----------------------------------------------------------------------------
// - SliderUnsignedInt()
// - SliderUnsignedInt2()
// - SliderUnsignedInt3()
// - SliderUnsignedInt4()
// - SliderUnsignedIntN()
// - SliderLongInt()
// - SliderLongInt2()
// - SliderLongInt3()
// - SliderLongInt4()
// - SliderLongIntN()
// - SliderUnsignedLongInt()
// - SliderUnsignedLongInt2()
// - SliderUnsignedLongInt3()
// - SliderUnsignedLongInt4()
// - SliderUnsignedLongIntN()
// - VSliderUnsignedInt()
// - VSliderLongInt()
// - VSliderUnsignedLongInt()
//-----------------------------------------------------------------------------

bool ImGui::SliderUnsignedInt(const char* label, unsigned int* v, unsigned int v_min, unsigned int v_max, const char* format)
{
    return SliderScalar(label, ImGuiDataType_U32, v, &v_min, &v_max, format);
}

bool ImGui::SliderUnsignedInt2(const char* label, unsigned int v[2], unsigned int v_min, unsigned int v_max, const char* format)
{
    return SliderScalarN(label, ImGuiDataType_U32, v, 2, &v_min, &v_max, format);
}

bool ImGui::SliderUnsignedInt3(const char* label, unsigned int v[3], unsigned int v_min, unsigned int v_max, const char* format)
{
    return SliderScalarN(label, ImGuiDataType_U32, v, 3, &v_min, &v_max, format);
}

bool ImGui::SliderUnsignedInt4(const char* label, unsigned int v[4], unsigned int v_min, unsigned int v_max, const char* format)
{
    return SliderScalarN(label, ImGuiDataType_U32, v, 4, &v_min, &v_max, format);
}

bool ImGui::SliderUnsignedIntN(const char* label, unsigned int* v, int components, unsigned int v_min, unsigned int v_max, const char* format)
{
    return SliderScalarN(label, ImGuiDataType_U32, v, components, &v_min, &v_max, format);
}

bool ImGui::SliderLongInt(const char* label, long long* v, long long v_min, long long v_max, const char* format)
{
    return SliderScalar(label, ImGuiDataType_S64, v, &v_min, &v_max, format);
}

bool ImGui::SliderLongInt2(const char* label, long long v[2], long long v_min, long long v_max, const char* format)
{
    return SliderScalarN(label, ImGuiDataType_S64, v, 2, &v_min, &v_max, format);
}

bool ImGui::SliderLongInt3(const char* label, long long v[3], long long v_min, long long v_max, const char* format)
{
    return SliderScalarN(label, ImGuiDataType_S64, v, 3, &v_min, &v_max, format);
}

bool ImGui::SliderLongInt4(const char* label, long long v[4], long long v_min, long long v_max, const char* format)
{
    return SliderScalarN(label, ImGuiDataType_S64, v, 4, &v_min, &v_max, format);
}

bool ImGui::SliderLongIntN(const char* label, long long* v, int components, long long v_min, long long v_max, const char* format)
{
    return SliderScalarN(label, ImGuiDataType_S64, v, components, &v_min, &v_max, format);
}

bool ImGui::SliderUnsignedLongInt(const char* label, unsigned long long* v, unsigned long long v_min, unsigned long long v_max, const char* format)
{
    return SliderScalar(label, ImGuiDataType_U64, v, &v_min, &v_max, format);
}

bool ImGui::SliderUnsignedLongInt2(const char* label, unsigned long long v[2], unsigned long long v_min, unsigned long long v_max, const char* format)
{
    return SliderScalarN(label, ImGuiDataType_U64, v, 2, &v_min, &v_max, format);
}

bool ImGui::SliderUnsignedLongInt3(const char* label, unsigned long long v[3], unsigned long long v_min, unsigned long long v_max, const char* format)
{
    return SliderScalarN(label, ImGuiDataType_U64, v, 3, &v_min, &v_max, format);
}

bool ImGui::SliderUnsignedLongInt4(const char* label, unsigned long long v[4], unsigned long long v_min, unsigned long long v_max, const char* format)
{
    return SliderScalarN(label, ImGuiDataType_U64, v, 4, &v_min, &v_max, format);
}

bool ImGui::SliderUnsignedLongIntN(const char* label, unsigned long long* v, int components, unsigned long long v_min, unsigned long long v_max, const char* format)
{
    return SliderScalarN(label, ImGuiDataType_U64, v, components, &v_min, &v_max, format);
}

bool ImGui::VSliderUnsignedInt(const char* label, const ImVec2& size, unsigned int* v, unsigned int v_min, unsigned int v_max, const char* format)
{
    return VSliderScalar(label, size, ImGuiDataType_U32, v, &v_min, &v_max, format);
}

bool ImGui::VSliderLongInt(const char* label, const ImVec2& size, long long* v, long long v_min, long long v_max, const char* format)
{
    return VSliderScalar(label, size, ImGuiDataType_S64, v, &v_min, &v_max, format);
}

bool ImGui::VSliderUnsignedLongInt(const char* label, const ImVec2& size, unsigned long long* v, unsigned long long v_min, unsigned long long v_max, const char* format)
{
    return VSliderScalar(label, size, ImGuiDataType_U64, v, &v_min, &v_max, format);
}

//-----------------------------------------------------------------------------
// [SECTION] Widgets: InputUnsignedInt, InputLongInt, InputUnsignedLongInt, etc.
//-----------------------------------------------------------------------------
// - InputUnsignedInt()
// - InputUnsignedInt2()
// - InputUnsignedInt3()
// - InputUnsignedInt4()
// - InputUnsignedIntN()
// - InputLongInt()
// - InputLongInt2()
// - InputLongInt3()
// - InputLongInt4()
// - InputLongIntN()
// - InputUnsignedLongInt()
// - InputUnsignedLongInt2()
// - InputUnsignedLongInt3()
// - InputUnsignedLongInt4()
// - InputUnsignedLongIntN()
//-----------------------------------------------------------------------------

bool ImGui::InputUnsignedInt(const char* label, unsigned int* v, unsigned int step, unsigned int step_fast, ImGuiInputTextFlags extra_flags)
{
    // Hexadecimal input provided as a convenience but the flag name is awkward. Typically you'd use InputText() to parse your own data, if you want to handle prefixes.
    const char* format = (extra_flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08X" : "%u";
    return InputScalar(label, ImGuiDataType_U32, (void*)v, (void*)(step>0 ? &step : NULL), (void*)(step_fast>0 ? &step_fast : NULL), format, extra_flags);
}

bool ImGui::InputUnsignedInt2(const char* label, unsigned int v[2], ImGuiInputTextFlags extra_flags)
{
    return InputScalarN(label, ImGuiDataType_U32, v, 2, NULL, NULL, "%u", extra_flags);
}

bool ImGui::InputUnsignedInt3(const char* label, unsigned int v[3], ImGuiInputTextFlags extra_flags)
{
    return InputScalarN(label, ImGuiDataType_U32, v, 3, NULL, NULL, "%u", extra_flags);
}

bool ImGui::InputUnsignedInt4(const char* label, unsigned int v[4], ImGuiInputTextFlags extra_flags)
{
    return InputScalarN(label, ImGuiDataType_U32, v, 4, NULL, NULL, "%u", extra_flags);
}

bool ImGui::InputUnsignedIntN(const char* label, unsigned int* v, int components, ImGuiInputTextFlags extra_flags)
{
    return InputScalarN(label, ImGuiDataType_U32, v, components, NULL, NULL, "%u", extra_flags);
}

bool ImGui::InputLongInt(const char* label, long long* v, long long step, long long step_fast, ImGuiInputTextFlags extra_flags)
{
    // Hexadecimal input provided as a convenience but the flag name is awkward. Typically you'd use InputText() to parse your own data, if you want to handle prefixes.
    const char* format = (extra_flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08llX" : "%lld";
    return InputScalar(label, ImGuiDataType_S64, (void*)v, (void*)(step>0 ? &step : NULL), (void*)(step_fast>0 ? &step_fast : NULL), format, extra_flags);
}

bool ImGui::InputLongInt2(const char* label, long long v[2], ImGuiInputTextFlags extra_flags)
{
    return InputScalarN(label, ImGuiDataType_S64, v, 2, NULL, NULL, "%lld", extra_flags);
}

bool ImGui::InputLongInt3(const char* label, long long v[3], ImGuiInputTextFlags extra_flags)
{
    return InputScalarN(label, ImGuiDataType_S64, v, 3, NULL, NULL, "%lld", extra_flags);
}

bool ImGui::InputLongInt4(const char* label, long long v[4], ImGuiInputTextFlags extra_flags)
{
    return InputScalarN(label, ImGuiDataType_S64, v, 4, NULL, NULL, "%lld", extra_flags);
}

bool ImGui::InputLongIntN(const char* label, long long* v, int components, ImGuiInputTextFlags extra_flags)
{
    return InputScalarN(label, ImGuiDataType_S64, v, components, NULL, NULL, "%lld", extra_flags);
}

bool ImGui::InputUnsignedLongInt(const char* label, unsigned long long* v, unsigned long long step, unsigned long long step_fast, ImGuiInputTextFlags extra_flags)
{
    // Hexadecimal input provided as a convenience but the flag name is awkward. Typically you'd use InputText() to parse your own data, if you want to handle prefixes.
    const char* format = (extra_flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08llX" : "%llu";
    return InputScalar(label, ImGuiDataType_U64, (void*)v, (void*)(step>0 ? &step : NULL), (void*)(step_fast>0 ? &step_fast : NULL), format, extra_flags);
}

bool ImGui::InputUnsignedLongInt2(const char* label, unsigned long long v[2], ImGuiInputTextFlags extra_flags)
{
    return InputScalarN(label, ImGuiDataType_U64, v, 2, NULL, NULL, "%llu", extra_flags);
}

bool ImGui::InputUnsignedLongInt3(const char* label, unsigned long long v[3], ImGuiInputTextFlags extra_flags)
{
    return InputScalarN(label, ImGuiDataType_U64, v, 3, NULL, NULL, "%llu", extra_flags);
}

bool ImGui::InputUnsignedLongInt4(const char* label, unsigned long long v[4], ImGuiInputTextFlags extra_flags)
{
    return InputScalarN(label, ImGuiDataType_U64, v, 4, NULL, NULL, "%llu", extra_flags);
}

bool ImGui::InputUnsignedLongIntN(const char* label, unsigned long long* v, int components, ImGuiInputTextFlags extra_flags)
{
    return InputScalarN(label, ImGuiDataType_U64, v, components, NULL, NULL, "%llu", extra_flags);
}

//-----------------------------------------------------------------------------
// [SECTION] Internal: PlotLinesWithGrid, PlotEx
//-----------------------------------------------------------------------------

// NOTE: This is an internal struct in imgui_widgets.cpp
struct ImGuiPlotArrayGetterData
{
    const float* Values;
    int Stride;

    ImGuiPlotArrayGetterData(const float* values, int stride) { Values = values; Stride = stride; }
};

// NOTE: This is internally defined in imgui_widgets.cpp
float Plot_ArrayGetter(void* data, int idx)
{
    ImGuiPlotArrayGetterData* plot_data = (ImGuiPlotArrayGetterData*)data;
    const float v = *(const float*)(const void*)((const unsigned char*)plot_data->Values + (size_t)idx * plot_data->Stride);
    return v;
}

void ImGui::PlotLinesWithGrid(const char* label, const float* values, int values_count, const int* grid, int grid_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride)
{
    ImGuiPlotArrayGetterData data(values, stride);
    PlotEx(ImGuiPlotType_Lines, label, &Plot_ArrayGetter, (void*)&data, values_count, grid, grid_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

void ImGui::PlotEx(ImGuiPlotType plot_type, const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, const int* grid, int grid_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    if (graph_size.x == 0.0f)
        graph_size.x = CalcItemWidth();
    if (graph_size.y == 0.0f)
        graph_size.y = label_size.y + (style.FramePadding.y * 2);

    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(graph_size.x, graph_size.y));
    const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0));
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, 0, &frame_bb))
        return;
    const bool hovered = ItemHoverable(inner_bb, 0);

    // Determine scale from values if not specified
    if (scale_min == FLT_MAX || scale_max == FLT_MAX)
    {
        float v_min = FLT_MAX;
        float v_max = -FLT_MAX;
        for (int i = 0; i < values_count; i++)
        {
            const float v = values_getter(data, i);
            v_min = ImMin(v_min, v);
            v_max = ImMax(v_max, v);
        }
        if (scale_min == FLT_MAX)
            scale_min = v_min;
        if (scale_max == FLT_MAX)
            scale_max = v_max;
    }

    RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

    if (values_count > 0)
    {
        int res_w = ImMin((int)graph_size.x, values_count) + ((plot_type == ImGuiPlotType_Lines) ? -1 : 0);
        int item_count = values_count + ((plot_type == ImGuiPlotType_Lines) ? -1 : 0);

        // Tooltip on hover
        int v_hovered = -1;
        if (hovered)
        {
            const float t = ImClamp((g.IO.MousePos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x), 0.0f, 0.9999f);
            const int v_idx = (int)(t * item_count);
            IM_ASSERT(v_idx >= 0 && v_idx < values_count);

            const float v0 = values_getter(data, (v_idx + values_offset) % values_count);
            const float v1 = values_getter(data, (v_idx + 1 + values_offset) % values_count);
            if (plot_type == ImGuiPlotType_Lines)
                SetTooltip("%d: %8.4g\n%d: %8.4g", v_idx, v0, v_idx+1, v1);
            else if (plot_type == ImGuiPlotType_Histogram)
                SetTooltip("%d: %8.4g", v_idx, v0);
            v_hovered = v_idx;
        }

        const float t_step = 1.0f / (float)res_w;
        const float inv_scale = (scale_min == scale_max) ? 0.0f : (1.0f / (scale_max - scale_min));

        float v0 = values_getter(data, (0 + values_offset) % values_count);
        float t0 = 0.0f;
        ImVec2 tp0 = ImVec2( t0, 1.0f - ImSaturate((v0 - scale_min) * inv_scale) );                       // Point in the normalized space of our target rectangle
        float histogram_zero_line_t = (scale_min * scale_max < 0.0f) ? (-scale_min * inv_scale) : (scale_min < 0.0f ? 0.0f : 1.0f);   // Where does the zero line stands

        const ImU32 col_base = GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLines : ImGuiCol_PlotHistogram);
        const ImU32 col_hovered = GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLinesHovered : ImGuiCol_PlotHistogramHovered);

        float gridAlphaMul = 0.45f;
        const ImU32 col_grid = GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLines : ImGuiCol_PlotHistogram, gridAlphaMul);

        int currentGrid=0;

        for (int n = 0; n < res_w; n++)
        {
            const float t1 = t0 + t_step;
            const int v1_idx = (int)(t0 * item_count + 0.5f);
            IM_ASSERT(v1_idx >= 0 && v1_idx < values_count);
            const float v1 = values_getter(data, (v1_idx + values_offset + 1) % values_count);
            const ImVec2 tp1 = ImVec2( t1, 1.0f - ImSaturate((v1 - scale_min) * inv_scale) );

            // NB: Draw calls are merged together by the DrawList system. Still, we should render our batch are lower level to save a bit of CPU.
            ImVec2 pos0 = ImLerp(inner_bb.Min, inner_bb.Max, tp0);
            ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, (plot_type == ImGuiPlotType_Lines) ? tp1 : ImVec2(tp1.x, histogram_zero_line_t));

            //Add grid lines first if needed
            if( currentGrid < grid_count )
            {
                const int gridIndex = grid[currentGrid];
                if(gridIndex == n)
                {
                    ++currentGrid;

                    ImVec2 posBot( pos0.x, frame_bb.Min.y );
                    ImVec2 posTop( pos0.x, frame_bb.Max.y );
                    window->DrawList->AddLine(posBot, posTop, col_grid);
                }
            }

            if (plot_type == ImGuiPlotType_Lines)
            {
                window->DrawList->AddLine(pos0, pos1, v_hovered == v1_idx ? col_hovered : col_base);
            }
            else if (plot_type == ImGuiPlotType_Histogram)
            {
                if (pos1.x >= pos0.x + 2.0f)
                    pos1.x -= 1.0f;
                window->DrawList->AddRectFilled(pos0, pos1, v_hovered == v1_idx ? col_hovered : col_base);
            }

            t0 = t1;
            tp0 = tp1;
        }
    }

    // Text overlay
    if (overlay_text)
        RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, overlay_text, NULL, NULL, ImVec2(0.5f,0.0f));

    if (label_size.x > 0.0f)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);
}

#endif // IMGUI_DISABLE_API
