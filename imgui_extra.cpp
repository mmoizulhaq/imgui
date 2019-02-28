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
// [SECTION] Types: ImGuiGraphData, ImGuiNextGraphData, ImGuiGraph
//-----------------------------------------------------------------------------

ImGuiGraphData::ImGuiGraphData()
{
    Pos = AxesPos = ImVec2(0.0f, 0.0f);
    GridStride = ImVec2(100.0f, 100.0f);
    ZoomScale = GridThickness = 1.0f;
    AxesThickness = 2.0f;
    AxesFormat = "%.0f";
    AxisXName = "x";
    AxisYName = "y";
}

void ImGuiNextGraphData::Clear()
{
    PosCond = ZoomScaleCond = GridStrideCond = GridThicknessCond = AxesPosCond = AxesThicknessCond = AxesNameCond = AxesFormatCond = 0; 
}

ImGuiGraph::ImGuiGraph()
{
    ID = Flags = 0;
    Window = nullptr;
    IsAppearing = false;
    LastZoomScale = Factor = Ratio = 1.0f;
    FactoredStride = OriginalStride = Stride = ImVec2(100.0f, 100.0f);
}


enum ImGuiGraphPlotLineType
{
    ImGuiGraphPlotLineType_LineFromArray,
    ImGuiGraphPlotLineType_LineFunction
};

void ImGuiGraphPlotLineEx(ImGuiGraphPlotLineType plot_type, const char* label, void* function, void* data, int values_count, float min_x, float max_x, float thickness)
{
    if(values_count < 2)
        return; 

    IM_ASSERT(function);
    IM_ASSERT(min_x < max_x);

    ImGuiGraph* graph = ImGui::GetCurrentGraph();
    ImGuiIO& io = ImGui::GetIO();

    auto GetYValue = [&](int index, float x) -> float
    {
        float y_value = 0.0f;
        switch(plot_type)
        {
        case ImGuiGraphPlotLineType_LineFromArray:
            y_value = ((float(*)(void*, int))function)(data, index);
            y_value = graph->ContentRect.Max.y - graph->Data.Pos.y - (y_value * graph->Data.ZoomScale);
            break;
        case ImGuiGraphPlotLineType_LineFunction:
            y_value = ((float(*)(void*, float))function)(data, x);
            y_value = graph->ContentRect.Max.y - graph->Data.Pos.y - (y_value * graph->Data.ZoomScale);
            break;
        default:
            IM_ASSERT(0);
            break;
        }
        return y_value;
    };

    if(plot_type == ImGuiGraphPlotLineType_LineFromArray)
    {
        values_count -= 1;
    }

    float x0 = min_x;
    float y0 = GetYValue(0, min_x);
    x0 = graph->ContentRect.Min.x + graph->Data.Pos.x + (x0 * graph->Data.ZoomScale);
    for(int i = 1; i <= values_count; ++i)
    {
        float x1 = ImLerp(min_x, max_x, static_cast<float>(i) / static_cast<float>(values_count));
        const float y1 = GetYValue(i, x1);
        x1 = graph->ContentRect.Min.x + graph->Data.Pos.x + (x1 * graph->Data.ZoomScale);

        if(x0 > graph->ContentRect.Max.x && x1 > graph->ContentRect.Max.x)
        {
            break;
        }

        const ImRect rect(x0, graph->ContentRect.Min.y, x1, graph->ContentRect.Max.y);
        ImColor col = ImGui::GetColorU32(ImGuiCol_PlotLines);
        if(rect.Contains(io.MousePos))
        {
            if(!(graph->Flags & ImGuiGraphFlags_NoToolTips))
            {
                const float x0_gs = (x0 - graph->Data.Pos.x - graph->ContentRect.Min.x) * (1.0f / graph->Data.ZoomScale);
                const float y0_gs = (y0 + graph->Data.Pos.y - graph->ContentRect.Max.y) * (1.0f / graph->Data.ZoomScale);
                const float x1_gs = (x1 - graph->Data.Pos.x - graph->ContentRect.Min.x) * (1.0f / graph->Data.ZoomScale);
                const float y1_gs = (y1 + graph->Data.Pos.y - graph->ContentRect.Max.y) * (1.0f / graph->Data.ZoomScale);
                ImGuiGraph::ToolTipData data = { label, ImVec2(x0_gs, -y0_gs), ImVec2(x1_gs, -y1_gs), col };
                graph->ToolTipDatas.push_back(data);
            }

            col = ImGui::GetColorU32(ImGuiCol_PlotLinesHovered);
        }

        if((y0 > graph->ContentRect.Min.y && y0 < graph->ContentRect.Max.y) || (y1 > graph->ContentRect.Min.y && y1 < graph->ContentRect.Max.y))
        {
            graph->Window->DrawList->AddLine(ImVec2(x0, y0), ImVec2(x1, y1), col, thickness);
        }

        x0 = x1;
        y0 = y1;
    }
}

float ImGuiGraphLineArrayGetter(void* data, int index)
{
    IM_ASSERT(data);
    return ((const float*)data)[index];
}

void ImGuiGraph::AddLineArray(const char* label, const float* values, int values_count, float min_x, float max_x, float thickness)
{
    ImGuiGraphPlotLineEx(ImGuiGraphPlotLineType_LineFromArray, label, (void*)ImGuiGraphLineArrayGetter, (void*)values, values_count, min_x, max_x, thickness);
}

void ImGuiGraph::AddLineArray(const char* label, float(*function)(void* data, int index), void* data, int values_count, float min_x, float max_x, float thickness)
{
    ImGuiGraphPlotLineEx(ImGuiGraphPlotLineType_LineFromArray, label, (void*)function, data, values_count, min_x, max_x, thickness);
}

float ImGuiGraphLineFunctionGetter(void* data, float x)
{
    IM_ASSERT(data);
    return ((float(*)(float))data)(x);
}

void ImGuiGraph::AddLineFunction(const char* label, float(*function)(float x), int values_count, float min_x, float max_x, float thickness)
{
    ImGuiGraphPlotLineEx(ImGuiGraphPlotLineType_LineFunction, label, (void*)ImGuiGraphLineFunctionGetter, (void*)function, values_count, min_x, max_x, thickness);
}

void ImGuiGraph::AddLineFunction(const char* label, float(*function)(void* data, float x), void* data, int values_count, float min_x, float max_x, float thickness)
{
    ImGuiGraphPlotLineEx(ImGuiGraphPlotLineType_LineFunction, label, (void*)function, data, values_count, min_x, max_x, thickness);
}

void ImGuiGraphPlotPointsEx(const char* label, void* function, void* data, int values_count, float radius)
{
    if(values_count < 1)
        return; 

    IM_ASSERT(function);

    ImGuiGraph* graph = ImGui::GetCurrentGraph();
    ImGuiIO& io = ImGui::GetIO();

    for(int i = 0; i < values_count; ++i)
    {
        ImVec2 point = ((ImVec2(*)(void*, int))function)(data, i);
        point.x = graph->ContentRect.Min.x + graph->Data.Pos.x + (point.x * graph->Data.ZoomScale);
        point.y = graph->ContentRect.Max.y - graph->Data.Pos.y - (point.y * graph->Data.ZoomScale);

        // TODO tooltips for points either:
        // - some configurable radius around mouse to grab
        // - exact picking with mouse

        if(graph->ContentRect.Contains(point))
        {
            // MAYBE configuration for hollow points
            graph->Window->DrawList->AddCircleFilled(point, radius, ImGui::GetColorU32(ImGuiCol_PlotLines));
        }
    }
}

ImVec2 ImGuiGraphPointsFunctionGetter(void* data, int index)
{
    IM_ASSERT(data);
    return ((const ImVec2*)data)[index];
}

void ImGuiGraph::AddPoints(const char* label, const ImVec2* values, int values_count, float radius)
{
    ImGuiGraphPlotPointsEx(label, (void*)ImGuiGraphPointsFunctionGetter, (void*)values, values_count, radius);
}

void ImGuiGraph::AddPoints(const char* label, ImVec2(*function)(void* data, int index), void* data, int values_count, float radius)
{
    ImGuiGraphPlotPointsEx(label, function, data, values_count, radius);
}

//-----------------------------------------------------------------------------
// [SECTION] Types: ImGuiExtraContext
//-----------------------------------------------------------------------------

ImGuiExtraContext::~ImGuiExtraContext()
{
    GraphsById.Clear();
    for (int i = 0; i < Graphs.Size; ++i)
    {
        IM_DELETE(Graphs[i]);
    }
    Graphs.clear();
}

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

ImGuiExtraContext& ImGui::GetExtraContext()
{
    static ImGuiExtraContext context;
    return context;
}

ImGuiGraph* ImGui::FindGraphByID(ImGuiID id)
{
    ImGuiExtraContext& g = ImGui::GetExtraContext();
    return (ImGuiGraph*)g.GraphsById.GetVoidPtr(id);
}

ImGuiGraph* ImGui::FindGraphByName(const char* name)
{
    ImGuiID id = ImHash(name, 0);
    return FindGraphByID(id);
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

// NOTE: This is an internal struct in imgui_widgets.cpp as ImGuiPlotArrayGetterData
struct ImGuiPlotArrayGetterDataEx
{
    const float* Values;
    int Stride;

    ImGuiPlotArrayGetterDataEx(const float* values, int stride) { Values = values; Stride = stride; }
};

// NOTE: This is internally defined in imgui_widgets.cpp as Plot_ArrayGetter
float Plot_ArrayGetterEx(void* data, int idx)
{
    ImGuiPlotArrayGetterDataEx* plot_data = (ImGuiPlotArrayGetterDataEx*)data;
    const float v = *(const float*)(const void*)((const unsigned char*)plot_data->Values + (size_t)idx * plot_data->Stride);
    return v;
}

void PlotEx(ImGuiPlotType plot_type, const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, const int* grid, int grid_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    if (graph_size.x == 0.0f)
        graph_size.x = ImGui::CalcItemWidth();
    if (graph_size.y == 0.0f)
        graph_size.y = label_size.y + (style.FramePadding.y * 2);

    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(graph_size.x, graph_size.y));
    const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0));
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, 0, &frame_bb))
        return;
    const bool hovered = ImGui::ItemHoverable(inner_bb, 0);

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

    ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

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
                ImGui::SetTooltip("%d: %8.4g\n%d: %8.4g", v_idx, v0, v_idx+1, v1);
            else if (plot_type == ImGuiPlotType_Histogram)
                ImGui::SetTooltip("%d: %8.4g", v_idx, v0);
            v_hovered = v_idx;
        }

        const float t_step = 1.0f / (float)res_w;
        const float inv_scale = (scale_min == scale_max) ? 0.0f : (1.0f / (scale_max - scale_min));

        float v0 = values_getter(data, (0 + values_offset) % values_count);
        float t0 = 0.0f;
        ImVec2 tp0 = ImVec2( t0, 1.0f - ImSaturate((v0 - scale_min) * inv_scale) );                       // Point in the normalized space of our target rectangle
        float histogram_zero_line_t = (scale_min * scale_max < 0.0f) ? (-scale_min * inv_scale) : (scale_min < 0.0f ? 0.0f : 1.0f);   // Where does the zero line stands

        const ImU32 col_base = ImGui::GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLines : ImGuiCol_PlotHistogram);
        const ImU32 col_hovered = ImGui::GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLinesHovered : ImGuiCol_PlotHistogramHovered);

        float gridAlphaMul = 0.45f;
        const ImU32 col_grid = ImGui::GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLines : ImGuiCol_PlotHistogram, gridAlphaMul);

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
        ImGui::RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, overlay_text, NULL, NULL, ImVec2(0.5f,0.0f));

    if (label_size.x > 0.0f)
        ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);
}

void ImGui::PlotLinesWithGrid(const char* label, const float* values, int values_count, const int* grid, int grid_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride)
{
    ImGuiPlotArrayGetterDataEx data(values, stride);
    PlotEx(ImGuiPlotType_Lines, label, &Plot_ArrayGetterEx, (void*)&data, values_count, grid, grid_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

//-----------------------------------------------------------------------------
// [SECTION] Internal: PlotMultiLines, PlotMultiHistograms, etc...
// This code was originally taken from a comment at https://github.com/ocornut/imgui/issues/632
//-----------------------------------------------------------------------------
// - PlotMultiLines()
// - PlotMultiHistograms()
// - SelectablePlotMultiLines()
// - SelectablePlotMultiHistograms()
// - SelectablePlotLines()
// - SelectablePlotHistograms()
//-----------------------------------------------------------------------------

ImU32 InvertColorU32(ImU32 in)
{
    ImVec4 in4 = ImGui::ColorConvertU32ToFloat4(in);
    in4.x = 1.f - in4.x;
    in4.y = 1.f - in4.y;
    in4.z = 1.f - in4.z;
    return ImGui::GetColorU32(in4);
}

///////////////////////////////////////////////////////////////////////////////
//  Multi data source plot
///////////////////////////////////////////////////////////////////////////////

int PlotMultiEx(ImGuiPlotType plot_type, const char* label, int num_datas, const char** names, const ImColor* colors, float(*getter)(const void* data, int idx), const void * const * datas, int values_count, bool select_enabled, int selected_index, float scale_min, float scale_max, ImVec2 graph_size)
{
    bool isNewSelection = false;
    int newSelectedItem = -1;
    int returnSelectedItem = -1;

    int currentSelectedItem = plot_type == ImGuiPlotType_Lines ? selected_index - 1 : selected_index;
    currentSelectedItem = currentSelectedItem >= -1 ? currentSelectedItem : -1;

    const int values_offset = 0;

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return currentSelectedItem;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    if (graph_size.x == 0.0f)
        graph_size.x = ImGui::CalcItemWidth();
    if (graph_size.y == 0.0f)
        graph_size.y = label_size.y + (style.FramePadding.y * 2);

    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(graph_size.x, graph_size.y));
    const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0));
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, 0))
        return currentSelectedItem;

    // Determine scale from values if not specified
    if (scale_min == FLT_MAX || scale_max == FLT_MAX)
    {
        float v_min = FLT_MAX;
        float v_max = -FLT_MAX;
        for (int data_idx = 0; data_idx < num_datas; ++data_idx)
        {
            for (int i = 0; i < values_count; i++)
            {
                const float v = getter(datas[data_idx], i);
                v_min = ImMin(v_min, v);
                v_max = ImMax(v_max, v);
            }
        }
        if (scale_min == FLT_MAX)
            scale_min = v_min;
        if (scale_max == FLT_MAX)
            scale_max = v_max;
    }

    ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

    int res_w = ImMin((int)graph_size.x, values_count) + ((plot_type == ImGuiPlotType_Lines) ? -1 : 0);
    int item_count = values_count + ((plot_type == ImGuiPlotType_Lines) ? -1 : 0);

    // Tooltip on hover
    int v_hovered = -1;
    if (ImGui::ItemHoverable(inner_bb, 0))
    {
        const float t = ImClamp((g.IO.MousePos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x), 0.0f, 0.9999f);
        const int v_idx = (int)(t * item_count);
        IM_ASSERT(v_idx >= 0 && v_idx < values_count);

        // std::string toolTip;
        ImGui::BeginTooltip();
        const int idx0 = (v_idx + values_offset) % values_count;
        if (plot_type == ImGuiPlotType_Lines)
        {
            const int idx1 = (v_idx + 1 + values_offset) % values_count;
            ImGui::Text("%8d %8d | Name", v_idx, v_idx + 1);
            for (int dataIdx = 0; dataIdx < num_datas; ++dataIdx)
            {
                const float v0 = getter(datas ? datas[dataIdx] : nullptr, idx0);
                const float v1 = getter(datas ? datas[dataIdx] : nullptr, idx1);
                ImGui::TextColored(colors[dataIdx], "%8.4g %8.4g | %s", v0, v1, names[dataIdx]);
            }
        }
        else if (plot_type == ImGuiPlotType_Histogram)
        {
            for (int dataIdx = 0; dataIdx < num_datas; ++dataIdx)
            {
                const float v0 = getter(datas ? datas[dataIdx] : nullptr, idx0);
                ImGui::TextColored(colors[dataIdx], "%d: %8.4g | %s", v_idx, v0, names[dataIdx]);
            }
        }
        ImGui::EndTooltip();
        v_hovered = v_idx;

        if (select_enabled && ImGui::IsMouseClicked(0))
        {
            isNewSelection = true;
            newSelectedItem = v_idx + ((plot_type == ImGuiPlotType_Lines) ? 1 : 0);

            // check for deselection
            if (newSelectedItem == selected_index)
                newSelectedItem = -1;
        }
    }

    returnSelectedItem = isNewSelection ? newSelectedItem : selected_index;

    for (int data_idx = 0; data_idx < num_datas; ++data_idx)
    {
        const float t_step = 1.0f / (float)res_w;

        float v0 = getter(datas ? datas[data_idx] : nullptr, (0 + values_offset) % values_count);
        float t0 = 0.0f;
        ImVec2 tp0 = ImVec2(t0, 1.0f - ImSaturate((v0 - scale_min) / (scale_max - scale_min)));    // Point in the normalized space of our target rectangle

        const ImU32 col_base = colors[data_idx];
        const ImU32 col_hovered = InvertColorU32(colors[data_idx]);
        const ImU32 col_selected = colors[num_datas - data_idx - 1];

        for (int n = 0; n < res_w; n++)
        {
            const float t1 = t0 + t_step;
            const int v1_idx = (int)(t0 * item_count + 0.5f);
            IM_ASSERT(v1_idx >= 0 && v1_idx < values_count);
            const float v1 = getter(datas ? datas[data_idx] : nullptr, (v1_idx + values_offset + 1) % values_count);
            const ImVec2 tp1 = ImVec2(t1, 1.0f - ImSaturate((v1 - scale_min) / (scale_max - scale_min)));

            // NB: Draw calls are merged together by the DrawList system. Still, we should render our batch are lower level to save a bit of CPU.
            ImVec2 pos0 = ImLerp(inner_bb.Min, inner_bb.Max, tp0);
            ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, (plot_type == ImGuiPlotType_Lines) ? tp1 : ImVec2(tp1.x, 1.0f));
            if (plot_type == ImGuiPlotType_Lines)
            {
                window->DrawList->AddLine(pos0, pos1, (returnSelectedItem - 1) == v1_idx ? col_selected : v_hovered == v1_idx ? col_hovered : col_base);
            }
            else if (plot_type == ImGuiPlotType_Histogram)
            {
                if (pos1.x >= pos0.x + 2.0f)
                    pos1.x -= 1.0f;
                window->DrawList->AddRectFilled(pos0, pos1, returnSelectedItem == v1_idx ? col_selected : v_hovered == v1_idx ? col_hovered : col_base);
            }

            t0 = t1;
            tp0 = tp1;
        }
    }

    ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);

    return returnSelectedItem;
}

void ImGui::PlotMultiLines(const char* label, int num_datas, const char** names, const ImColor* colors, float(*getter)(const void* data, int idx), void** datas, int values_count, float scale_min, float scale_max, ImVec2 graph_size)
{
    PlotMultiEx(ImGuiPlotType_Lines, label, num_datas, names, colors, getter, datas, values_count, false, -1, scale_min, scale_max, graph_size);
}

void ImGui::PlotMultiHistograms(const char* label, int num_datas, const char** names, const ImColor* colors, float(*getter)(const void* data, int idx), void** datas, int values_count, float scale_min, float scale_max, ImVec2 graph_size)
{
    PlotMultiEx(ImGuiPlotType_Histogram, label, num_datas, names, colors, getter, datas, values_count, false, -1, scale_min, scale_max, graph_size);
}	

int ImGui::SelectablePlotMultiLines(const char* label, int num_datas, const char** names, const ImColor* colors, float(*getter)(const void* data, int idx), void** datas, int values_count, int selected_index, float scale_min, float scale_max, ImVec2 graph_size)
{
    return PlotMultiEx(ImGuiPlotType_Lines, label, num_datas, names, colors, getter, datas, values_count, true, selected_index, scale_min, scale_max, graph_size);
}

int ImGui::SelectablePlotMultiHistograms(const char* label, int num_datas, const char** names, const ImColor* colors, float(*getter)(const void* data, int idx), void** datas, int values_count, int selected_index, float scale_min, float scale_max, ImVec2 graph_size)
{
    return PlotMultiEx(ImGuiPlotType_Histogram, label, num_datas, names, colors, getter, datas, values_count, true, selected_index, scale_min, scale_max, graph_size);
}

///////////////////////////////////////////////////////////////////////////////
//  Single data source plot
///////////////////////////////////////////////////////////////////////////////

int SelectablePlotEx(ImGuiPlotType plot_type, const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int selectedIdx, const int* grid, int grid_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
{
    bool isNewSelection = false;
    int newSelectedItem = -1;
    int returnSelectedItem = -1;

    int currentSelectedItem = plot_type == ImGuiPlotType_Lines ? selectedIdx - 1 : selectedIdx;
    currentSelectedItem = currentSelectedItem >= -1 ? currentSelectedItem : -1;

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return currentSelectedItem;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    if (graph_size.x == 0.0f)
        graph_size.x = ImGui::CalcItemWidth();
    if (graph_size.y == 0.0f)
        graph_size.y = label_size.y + (style.FramePadding.y * 2);

    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(graph_size.x, graph_size.y));
    const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0));
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, 0, &frame_bb))
        return currentSelectedItem;
    const bool hovered = ImGui::ItemHoverable(inner_bb, 0);

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

    ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

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
                ImGui::SetTooltip("%d: %8.4g\n%d: %8.4g", v_idx, v0, v_idx + 1, v1);
            else if (plot_type == ImGuiPlotType_Histogram)
                ImGui::SetTooltip("%d: %8.4g", v_idx, v0);
            v_hovered = v_idx;
                
            if (ImGui::IsMouseClicked(0))
            {
                isNewSelection = true;
                newSelectedItem = v_idx + ((plot_type == ImGuiPlotType_Lines) ? 1 : 0);
                    
                // check for deselection
                if (newSelectedItem == selectedIdx)
                    newSelectedItem = -1;
            }
        }

        returnSelectedItem = isNewSelection ? newSelectedItem : selectedIdx;

        const float t_step = 1.0f / (float)res_w;
        const float inv_scale = (scale_min == scale_max) ? 0.0f : (1.0f / (scale_max - scale_min));

        float v0 = values_getter(data, (0 + values_offset) % values_count);
        float t0 = 0.0f;
        ImVec2 tp0 = ImVec2(t0, 1.0f - ImSaturate((v0 - scale_min) * inv_scale));                       // Point in the normalized space of our target rectangle
        float histogram_zero_line_t = (scale_min * scale_max < 0.0f) ? (-scale_min * inv_scale) : (scale_min < 0.0f ? 0.0f : 1.0f);   // Where does the zero line stands

        const ImU32 col_base = ImGui::GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLines : ImGuiCol_PlotHistogram);
        const ImU32 col_hovered = ImGui::GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLinesHovered : ImGuiCol_PlotHistogramHovered);
        const ImU32 col_selected = ImGui::GetColorU32(ImGuiCol_NavHighlight); // need to figure out best choice.

        float gridAlphaMul = 0.45f;
        const ImU32 col_grid = ImGui::GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLines : ImGuiCol_PlotHistogram, gridAlphaMul);

        int currentGrid = 0;

        for (int n = 0; n < res_w; n++)
        {
            const float t1 = t0 + t_step;
            const int v1_idx = (int)(t0 * item_count + 0.5f);
            IM_ASSERT(v1_idx >= 0 && v1_idx < values_count);
            const float v1 = values_getter(data, (v1_idx + values_offset + 1) % values_count);
            const ImVec2 tp1 = ImVec2(t1, 1.0f - ImSaturate((v1 - scale_min) * inv_scale));

            // NB: Draw calls are merged together by the DrawList system. Still, we should render our batch are lower level to save a bit of CPU.
            ImVec2 pos0 = ImLerp(inner_bb.Min, inner_bb.Max, tp0);
            ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, (plot_type == ImGuiPlotType_Lines) ? tp1 : ImVec2(tp1.x, histogram_zero_line_t));

            //Add grid lines first if needed
            if (currentGrid < grid_count)
            {
                const int gridIndex = grid[currentGrid];
                if (gridIndex == n)
                {
                    ++currentGrid;

                    ImVec2 posBot(pos0.x, frame_bb.Min.y);
                    ImVec2 posTop(pos0.x, frame_bb.Max.y);
                    window->DrawList->AddLine(posBot, posTop, col_grid);
                }
            }

            if (plot_type == ImGuiPlotType_Lines)
            {
                window->DrawList->AddLine(pos0, pos1, (returnSelectedItem -1) == v1_idx ? col_selected : v_hovered == v1_idx ? col_hovered : col_base);
            }
            else if (plot_type == ImGuiPlotType_Histogram)
            {
                if (pos1.x >= pos0.x + 2.0f)
                    pos1.x -= 1.0f;
                window->DrawList->AddRectFilled(pos0, pos1, returnSelectedItem == v1_idx ? col_selected : v_hovered == v1_idx ? col_hovered : col_base);
            }

            t0 = t1;
            tp0 = tp1;
        }
    }

    // Text overlay
    if (overlay_text)
        ImGui::RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, overlay_text, NULL, NULL, ImVec2(0.5f, 0.0f));

    if (label_size.x > 0.0f)
        ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);

    return returnSelectedItem;
}

int ImGui::SelectablePlotLines(const char* label, const float* values, int values_count, int values_offset, int selectedIdx, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride)
{
    ImGuiPlotArrayGetterDataEx data(values, stride);
    return SelectablePlotEx(ImGuiPlotType_Lines, label, &Plot_ArrayGetterEx, (void*)&data, values_count, selectedIdx, NULL, 0, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

int ImGui::SelectablePlotHistograms(const char* label, const float* values, int values_count, int values_offset, int selectedIdx, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride)
{
    ImGuiPlotArrayGetterDataEx data(values, stride);
    return SelectablePlotEx(ImGuiPlotType_Histogram, label, &Plot_ArrayGetterEx, (void*)&data, values_count, selectedIdx, NULL, 0, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool CanSetValue(ImGuiGraph* graph, const char* str, ImGuiCond cond)
{
    ImGuiExtraContext& g = ImGui::GetExtraContext();
    switch(cond)
    {
        case ImGuiCond_Always:       // Set the variable
            return true;
        case ImGuiCond_Once:         // Set the variable once per runtime session (only the first call with succeed)
        case ImGuiCond_FirstUseEver: // Set the variable if the object/window has no persistently saved data (no entry in .ini file)
        {
            ImGuiStorage* storage = ImGui::GetStateStorage();
            IM_ASSERT(storage);
            if(storage->GetVoidPtr(graph->Window->GetID(str)) == nullptr)
            {
                storage->SetVoidPtr(graph->Window->GetID(str), graph);
                return true;
            }
            return false;
        }
        case ImGuiCond_Appearing:    // Set the variable if the object/window is appearing after being hidden/inactive (or the first time)
            return g.CurrentGraph->IsAppearing;
        default:
        {
            break;
        }
    }
    return false;
}

void HandleNextGraphData(ImGuiGraph* graph)
{
    static const char* IM_GRAPH_ZOOM_SCALE_ID     = "##IM_GRAPH_ZOOM_SCALE_ID";
    static const char* IM_GRAPH_POSITION_ID       = "##IM_GRAPH_POSITION_ID";
    static const char* IM_GRAPH_RANGE_ID          = "##IM_GRAPH_RANGE_ID";
    static const char* IM_GRAPH_GRID_STRIDE_ID    = "##IM_GRAPH_GRID_STRIDE_ID";
    static const char* IM_GRAPH_GRID_THICKNESS_ID = "##IM_GRAPH_GRID_THICKNESS_ID";
    static const char* IM_GRAPH_AXES_POS_ID       = "##IM_GRAPH_AXES_POS_ID";
    static const char* IM_GRAPH_AXES_THICKNESS_ID = "##IM_GRAPH_AXES_THICKNESS_ID";
    static const char* IM_GRAPH_AXES_NAME_ID      = "##IM_GRAPH_AXES_NAME_ID";
    static const char* IM_GRAPH_AXES_FORMAT_ID    = "##IM_GRAPH_AXES_FORMAT_ID";

    // update previous

    graph->LastZoomScale = graph->Data.ZoomScale;

    // set new

    ImGuiExtraContext& g = ImGui::GetExtraContext();
    if(CanSetValue(graph, IM_GRAPH_ZOOM_SCALE_ID, g.NextGraphData.ZoomScaleCond))
    {
        graph->Data.ZoomScale = g.NextGraphData.Data.ZoomScale;
    }

    if(CanSetValue(graph, IM_GRAPH_POSITION_ID, g.NextGraphData.PosCond))
    {
        graph->Data.Pos = g.NextGraphData.Data.Pos;
    }

    if(CanSetValue(graph, IM_GRAPH_GRID_STRIDE_ID, g.NextGraphData.GridStrideCond))
    {
        graph->Data.GridStride = g.NextGraphData.Data.GridStride;
        graph->FactoredStride = graph->Data.GridStride * graph->Factor;
    }

    if(CanSetValue(graph, IM_GRAPH_GRID_THICKNESS_ID, g.NextGraphData.GridThicknessCond))
    {
        graph->Data.GridThickness = g.NextGraphData.Data.GridThickness;
    }

    if(CanSetValue(graph, IM_GRAPH_AXES_POS_ID, g.NextGraphData.AxesPosCond))
    {
        graph->Data.AxesPos = g.NextGraphData.Data.AxesPos;
    }

    if(CanSetValue(graph, IM_GRAPH_AXES_THICKNESS_ID, g.NextGraphData.AxesThicknessCond))
    {
        graph->Data.AxesThickness = g.NextGraphData.Data.AxesThickness;
    }

    if(CanSetValue(graph, IM_GRAPH_AXES_NAME_ID, g.NextGraphData.AxesNameCond))
    {
        graph->Data.AxisXName = g.NextGraphData.Data.AxisXName;
        graph->Data.AxisYName = g.NextGraphData.Data.AxisYName;
    }

    if(CanSetValue(graph, IM_GRAPH_AXES_FORMAT_ID, g.NextGraphData.AxesFormatCond))
    {
        graph->Data.AxesFormat = g.NextGraphData.Data.AxesFormat;
    }
}

void CalcFrame(ImGuiGraph* graph, ImVec2 desired_size)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    IM_ASSERT(window);

    ImGuiContext& g = *ImGui::GetCurrentContext();

    graph->FramePadding = g.Style.FramePadding;

    ImVec2 full_size = ImGui::GetContentRegionAvail();
    ImVec2 size = ImFloor(desired_size);
    if (size.x <= 0.0f)
        size.x = ImMax(full_size.x + size.x, 4.0f); // Arbitrary minimum child size (0.0f causing too much issues)
    if (size.y <= 0.0f)
        size.y = ImMax(full_size.y + size.y, 4.0f);

    graph->ControlRect = ImRect(window->DC.CursorPos, window->DC.CursorPos + size);
    graph->ContentRect = ImRect(graph->ControlRect.Min + graph->FramePadding, graph->ControlRect.Max - graph->FramePadding);
}

void CalcGraph(ImGuiGraph* graph)
{
    // handle modification from mouse move 
    if(!(graph->Flags & ImGuiGraphFlags_NoMove) && ImGui::IsMouseDragging() && ImGui::IsMouseHoveringRect(graph->ContentRect.Min, graph->ContentRect.Max))
    {
        ImGuiIO& io = ImGui::GetIO();
        const ImVec2 delta = io.MouseDelta;

        if(!(graph->Flags & ImGuiGraphFlags_NoMoveX))
        {
            graph->Data.Pos.x += delta.x;
        }

        if(!(graph->Flags & ImGuiGraphFlags_NoMoveY))
        {
            graph->Data.Pos.y -= delta.y;
        }
    }

    // handle modification from zoom
    if(!(graph->Flags & ImGuiGraphFlags_NoZoom) && ImGui::IsMouseHoveringRect(graph->ControlRect.Min, graph->ControlRect.Max))
    {
        static const float IM_GRAPH_ZOOM_DELTA_FACTOR = 0.05f;
        static const float IM_GRAPH_ZOOM_SCALE_MIN = 0.01f; // out
        static const float IM_GRAPH_ZOOM_SCALE_MAX = 10.0f; // in

        ImGuiIO& io = ImGui::GetIO();
        graph->Data.ZoomScale += io.MouseWheel * IM_GRAPH_ZOOM_DELTA_FACTOR;
        graph->Data.ZoomScale = ImClamp(graph->Data.ZoomScale, IM_GRAPH_ZOOM_SCALE_MIN, IM_GRAPH_ZOOM_SCALE_MAX);

        const float factor = graph->Data.ZoomScale - graph->LastZoomScale;
        if(factor && io.MouseWheel)
        {
            // zoom to cursor

            ImRect rect(ImVec2(), graph->Stride);
            rect.Translate(ImVec2(graph->ContentRect.Min.x + graph->Data.Pos.x, graph->ContentRect.Max.y - graph->Data.Pos.y));

            ImVec2 d = rect.GetSize() - (rect.GetSize() * (1.0f + factor));
            ImVec2 p = (io.MousePos - rect.GetTL()) / rect.GetSize();
            ImVec2 tl = p * d;
            rect.Translate(tl);
            rect.Max -= d;

            graph->Data.Pos.x += tl.x;
            graph->Data.Pos.y -= tl.y;
            graph->Stride = rect.GetSize();

            graph->Data.ZoomScale = graph->Stride.x / graph->OriginalStride.x;

            // auto stride

            graph->Ratio = graph->Stride.x / graph->OriginalStride.x;
            float upper_bound = 1.0f;
            float lower_bound = 1.0f;
            if(graph->Ratio >= 1.0f)
            {
                // determine bounds
                upper_bound = 1.0f;
                while(upper_bound < graph->Ratio) { upper_bound *= 10.0f; }
                lower_bound = upper_bound / 10.0f;
            }
            else if(graph->Ratio < 1.0f)
            {
                // determine bounds
                lower_bound = 1.0f;
                while(lower_bound > graph->Ratio) { lower_bound /= 10.0f; }
                upper_bound = lower_bound * 10.0f;
            }
            else
            {
                lower_bound = IM_GRAPH_ZOOM_SCALE_MIN;
                upper_bound = lower_bound * 10.0f;
            }
            const float bounds[] = { lower_bound, lower_bound * 2.0f, lower_bound * 5.0f, upper_bound };

            // determine best fit
            float best = 0.0f;
            float new_ratio = graph->Ratio;
            for(int i = 0; i < 4; ++i)
            {
                const float remainder = ImFmod(graph->Ratio, bounds[i]);
                if(remainder > best)
                {
                    best = remainder;
                    new_ratio = bounds[i];
                }
            }
            graph->Ratio = new_ratio;

            if(!(graph->Flags & ImGuiGraphFlags_NoAutoStrideBasedOnZoom))
            {
                graph->Factor = 1.0f / graph->Ratio; 
                graph->FactoredStride = graph->Data.GridStride * graph->Factor;
            }
        }
    }
}

void PlotGraph(ImGuiGraph* graph)
{
    ImGuiContext& g = *ImGui::GetCurrentContext();
    ImGui::RenderFrame(graph->ContentRect.Min, graph->ContentRect.Max, ImGui::GetColorU32(ImGuiExtraCol_GraphBackground), true, g.Style.FrameRounding);

    const float offset_x = ImFmod(graph->Data.Pos.x, graph->Stride.x * graph->Factor);
    const float offset_y = ImFmod(graph->Data.Pos.y, graph->Stride.y * graph->Factor);
    ImVec2 value = ((graph->Data.Pos - ImVec2(offset_x, offset_y)) / (graph->Stride * graph->Factor)) * ImVec2(-graph->FactoredStride.x, -graph->FactoredStride.y);
    char buffer[256];

    if(!(graph->Flags & ImGuiGraphFlags_NoGridX))
    {
        for(float x = graph->ContentRect.Min.x + offset_x; x < graph->ContentRect.Max.x; x += graph->Stride.x * graph->Factor, value.x += graph->FactoredStride.x)
        {
            graph->Window->DrawList->AddLine(ImVec2(x, graph->ContentRect.Min.y), ImVec2(x, graph->ContentRect.Max.y), ImGui::GetColorU32(ImGuiExtraCol_GraphGrid), graph->Data.GridThickness);

            if(!(graph->Flags & ImGuiGraphFlags_NoGridLabels))
            {
                if (value.x == -0.0) { value.x = +0.0; } // minus zero
                ImFormatString(buffer, 256, graph->Data.AxesFormat, value.x); 
                const ImVec2 label_size = ImGui::CalcTextSize(buffer, NULL, true);
                graph->Window->DrawList->AddText(ImVec2(x, graph->ContentRect.Max.y - label_size.y), ImGui::GetColorU32(ImGuiExtraCol_GraphGridText), buffer);
            }
        }
    }

    if(!(graph->Flags & ImGuiGraphFlags_NoGridY))
    {
        // Due to inverted y we render from max - y to min instead of min to max
        for(float y = graph->ContentRect.Max.y - offset_y; y > graph->ContentRect.Min.y; y -= graph->Stride.y * graph->Factor, value.y += graph->FactoredStride.y)
        {
            graph->Window->DrawList->AddLine(ImVec2(graph->ContentRect.Min.x, y), ImVec2(graph->ContentRect.Max.x, y), ImGui::GetColorU32(ImGuiExtraCol_GraphGrid), graph->Data.GridThickness);

            if(!(graph->Flags & ImGuiGraphFlags_NoGridLabels))
            {
                if (value.y == -0.0) { value.y = +0.0; } // minus zero
                ImFormatString(buffer, 256, graph->Data.AxesFormat, value.y); 
                const ImVec2 label_size = ImGui::CalcTextSize(buffer, NULL, true);
                graph->Window->DrawList->AddText(ImVec2(graph->ContentRect.Min.x, y), ImGui::GetColorU32(ImGuiExtraCol_GraphGridText), buffer);
            }
        }
    }

    if(!(graph->Flags & ImGuiGraphFlags_NoAxisX))
    {
        const float y = graph->ContentRect.Max.y - graph->Data.Pos.y - (graph->Data.AxesPos.y * graph->Data.ZoomScale);
        const float x0 = graph->ContentRect.Min.x;
        const float x1 = graph->ContentRect.Max.x;
        graph->Window->DrawList->AddLine(ImVec2(x0, y), ImVec2(x1, y), ImGui::GetColorU32(ImGuiCol_Text), graph->Data.AxesThickness);

        if(!(graph->Flags & ImGuiGraphFlags_NoAxesLabels))
        {
            const ImVec2 label_size = ImGui::CalcTextSize(graph->Data.AxisYName, NULL, true);
            // MAYBE configurable axis label position
            const ImRect label_rect(x1 - label_size.x, y - (label_size.y * 0.5f), x1, y + (label_size.y * 0.5f)); // center
            //const ImRect label_rect(x1 - label_size.x, y, x1, y + label_size.y); // below
            graph->Window->DrawList->AddRectFilled(label_rect.Min - graph->FramePadding, label_rect.Max + graph->FramePadding, ImGui::GetColorU32(ImGuiExtraCol_GraphAxes));
            graph->Window->DrawList->AddText(label_rect.Min, ImGui::GetColorU32(ImGuiExtraCol_GraphAxesText), graph->Data.AxisXName);
        }
    }

    if(!(graph->Flags & ImGuiGraphFlags_NoAxisY))
    {
        const float x = graph->ContentRect.Min.x + graph->Data.Pos.x + (graph->Data.AxesPos.x * graph->Data.ZoomScale);
        const float y0 = graph->ContentRect.Max.y;
        const float y1 = graph->ContentRect.Min.y;
        graph->Window->DrawList->AddLine(ImVec2(x, y0), ImVec2(x, y1), ImGui::GetColorU32(ImGuiCol_Text), graph->Data.AxesThickness);

        if(!(graph->Flags & ImGuiGraphFlags_NoAxesLabels))
        {
            const ImVec2 label_size = ImGui::CalcTextSize(graph->Data.AxisXName, NULL, true);
            // MAYBE configurable axis label position
            const ImRect label_rect(x - (label_size.x * 0.5f), y1, x + (label_size.x * 0.5f), y1 + label_size.y); // center
            //const ImRect label_rect(x - label_size.x, y1, x, y1 + label_size.y); // left
            graph->Window->DrawList->AddRectFilled(label_rect.Min - graph->FramePadding, label_rect.Max + graph->FramePadding, ImGui::GetColorU32(ImGuiExtraCol_GraphAxes));
            graph->Window->DrawList->AddText(label_rect.Min, ImGui::GetColorU32(ImGuiExtraCol_GraphAxesText), graph->Data.AxisYName);
        }
    }
}

bool ImGui::BeginGraph(const char* str_id, ImVec2 size, ImGuiGraphFlags flags)
{
    if(!ImGui::BeginChild(str_id, size, false, ImGuiWindowFlags_NoMove))
    {
        ImGui::EndChild();
        return false;
    }

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return true;

    ImGuiExtraContext& extra_ctx = ImGui::GetExtraContext();
    ImGuiGraph* graph = FindGraphByID(window->GetID(str_id));
    if(graph == nullptr)
    {
        extra_ctx.Graphs.push_back(IM_NEW(ImGuiGraph));
        graph = extra_ctx.Graphs.back();
        graph->Name = str_id;
        graph->ID = window->GetID(str_id);
        graph->IsAppearing = true;
        extra_ctx.GraphsById.SetVoidPtr(window->GetID(str_id), (void*)graph);
    }
    else
    {
        graph->IsAppearing = window->Appearing;
    }

    graph->Window = window;
    graph->Flags = flags;    
    extra_ctx.CurrentGraph = graph;

    CalcFrame(graph, size);
    HandleNextGraphData(graph);
    CalcGraph(graph);
    PlotGraph(graph);

    return true;
}

void ImGui::EndGraph()
{
    ImGuiExtraContext& g = ImGui::GetExtraContext();
    IM_ASSERT(g.CurrentGraph);

    if(!g.CurrentGraph->ToolTipDatas.empty())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%8s | (%8s, %8s)", "name", g.CurrentGraph->Data.AxisXName, g.CurrentGraph->Data.AxisYName);
        for(int i = 0; i < g.CurrentGraph->ToolTipDatas.size(); ++i)
        {
            ImGuiGraph::ToolTipData& data = g.CurrentGraph->ToolTipDatas[i];
            ImGui::TextColored(ImColor(data.Color), "%8s : (%8.4f, %8.4f) | (%8.4f, %8.4f)", data.Label, data.PosBegin.x, data.PosBegin.y, data.PosEnd.x, data.PosEnd.y);
        }
        ImGui::EndTooltip();
        g.CurrentGraph->ToolTipDatas.clear();
    }

    g.CurrentGraph->IsAppearing = false;
    g.CurrentGraph = nullptr;
    g.NextGraphData.Clear();

    ImGui::EndChild();
}

ImGuiGraph* ImGui::GetCurrentGraph()
{
    ImGuiExtraContext& g = ImGui::GetExtraContext();
    return g.CurrentGraph;
}

void ImGui::SetNextGraphPosition(ImVec2 pos, ImGuiCond cond)
{
    ImGuiExtraContext& g = ImGui::GetExtraContext();
    g.NextGraphData.Data.Pos = pos;
    g.NextGraphData.PosCond = cond;
}

void ImGui::SetNextGraphPosition(float x, float y, ImGuiCond cond)
{
    SetNextGraphPosition(ImVec2(x, y), cond);
}

void ImGui::SetNextGraphZoomScale(float zoom, ImGuiCond cond)
{
    ImGuiExtraContext& g = ImGui::GetExtraContext();
    g.NextGraphData.Data.ZoomScale = zoom;
    g.NextGraphData.ZoomScaleCond = cond;
}

void ImGui::SetNextGraphGridStride(ImVec2 stride, ImGuiCond cond)
{
    ImGuiExtraContext& g = ImGui::GetExtraContext();
    g.NextGraphData.Data.GridStride = stride;
    g.NextGraphData.GridStrideCond = cond;
}

void ImGui::SetNextGraphGridStride(float stride_x, float stride_y, ImGuiCond cond)
{
    SetNextGraphGridStride(ImVec2(stride_x, stride_y), cond);
}

void ImGui::SetNextGraphGridLineThickness(float thickness, ImGuiCond cond)
{
    ImGuiExtraContext& g = ImGui::GetExtraContext();
    g.NextGraphData.Data.GridThickness = thickness;
    g.NextGraphData.GridThicknessCond = cond;
}

void ImGui::SetNextGraphAxesName(const char* name_x, const char* name_y, ImGuiCond cond)
{
    ImGuiExtraContext& g = ImGui::GetExtraContext();
    g.NextGraphData.Data.AxisXName = name_x;
    g.NextGraphData.Data.AxisYName = name_y;
    g.NextGraphData.AxesNameCond = cond;
}

void ImGui::SetNextGraphAxesPosition(ImVec2 pos, ImGuiCond cond)
{
    ImGuiExtraContext& g = ImGui::GetExtraContext();
    g.NextGraphData.Data.AxesPos = pos;
    g.NextGraphData.AxesPosCond = cond;
}

void ImGui::SetNextGraphAxesPosition(float x, float y, ImGuiCond cond)
{
    SetNextGraphAxesPosition(ImVec2(x, y), cond);
}

void ImGui::SetNextGraphAxesLineThickness(float thickness, ImGuiCond cond)
{
    ImGuiExtraContext& g = ImGui::GetExtraContext();
    g.NextGraphData.Data.AxesThickness = thickness;
    g.NextGraphData.AxesThicknessCond = cond;
}

void ImGui::SetNextGraphAxesLabelFormat(const char* format,  ImGuiCond cond)
{
    ImGuiExtraContext& g = ImGui::GetExtraContext();
    g.NextGraphData.Data.AxesFormat = format;
    g.NextGraphData.AxesFormatCond = cond;
}

#endif // IMGUI_DISABLE_API
