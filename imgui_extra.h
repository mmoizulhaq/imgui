// dear imgui
// (extra headers)

#pragma once

#ifndef IMGUI_VERSION
#error Must include imgui.h before imgui_extra.h
#endif

#ifndef IMGUI_DISABLE_API

//-----------------------------------------------------------------------------
// Extra forward declarations and basic types
//-----------------------------------------------------------------------------

typedef int ImGuiGraphFlags; 
typedef int ImGuiExtraCol; 

enum ImGuiGraphFlags_
{
    ImGuiGraphFlags_None                    = 0,
    ImGuiGraphFlags_NoZoom                  = 1 << 0,
    ImGuiGraphFlags_NoMoveX                 = 1 << 1,
    ImGuiGraphFlags_NoMoveY                 = 1 << 2,
    ImGuiGraphFlags_NoAxesLabels            = 1 << 3,
    ImGuiGraphFlags_NoAxisX                 = 1 << 4,
    ImGuiGraphFlags_NoAxisY                 = 1 << 5,
    ImGuiGraphFlags_NoGridX                 = 1 << 6,
    ImGuiGraphFlags_NoGridY                 = 1 << 7,
    ImGuiGraphFlags_NoGridLabels            = 1 << 8,
    ImGuiGraphFlags_NoGridBorder            = 1 << 9,
    ImGuiGraphFlags_NoToolTips              = 1 << 10,
    ImGuiGraphFlags_NoAutoStrideBasedOnZoom = 1 << 11,

    // TODO: auto scroll x etc...

    ImGuiGraphFlags_NoMove = ImGuiGraphFlags_NoMoveX | ImGuiGraphFlags_NoMoveY,
    ImGuiGraphFlags_NoAxes = ImGuiGraphFlags_NoAxisX | ImGuiGraphFlags_NoAxisY,
    ImGuiGraphFlags_NoGrid = ImGuiGraphFlags_NoGridX | ImGuiGraphFlags_NoGridY | ImGuiGraphFlags_NoGridBorder,
};

enum ImGuiExtraCol_
{
    ImGuiExtraCol_GraphBackground = ImGuiCol_FrameBg,
    ImGuiExtraCol_GraphGrid       = ImGuiCol_Border,
    ImGuiExtraCol_GraphGridText   = ImGuiCol_Text,
    ImGuiExtraCol_GraphAxes       = ImGuiCol_PopupBg,
    ImGuiExtraCol_GraphAxesText   = ImGuiCol_Header
};

//-----------------------------------------------------------------------------
// Extra types: Graph
//-----------------------------------------------------------------------------

// WIP
struct ImGuiGraphData
{
    ImVec2      Pos;
    float       ZoomScale;
    ImVec2      GridStride;     // stride value
    float       GridThickness;
    ImVec2      AxesPos;
    float       AxesThickness;
    const char* AxesFormat;
    const char* AxisXName;
    const char* AxisYName;

    ImGuiGraphData();
};

// WIP
// Storage for SetNextGraph** functions
struct ImGuiNextGraphData
{
    ImGuiCond      PosCond;           // For Pos
    ImGuiCond      ZoomScaleCond;     // For ZoomScale
    ImGuiCond      GridStrideCond;    // For GridStride
    ImGuiCond      GridThicknessCond; // For GridThickness
    ImGuiCond      AxesPosCond;       // For AxesPos
    ImGuiCond      AxesThicknessCond; // For AxesThickness
    ImGuiCond      AxesFormatCond;    // For AxesFormat
    ImGuiCond      AxesNameCond;      // For AxisXName, AxisYName
    ImGuiGraphData Data;

    void Clear();
};

// WIP
// Storage for one graph
struct IMGUI_API ImGuiGraph
{
    const char*     Name;
    ImGuiID         ID;
    ImGuiGraphFlags Flags;
    ImGuiWindow*    Window;
    bool            IsAppearing;
    ImGuiGraphData  Data;            // User Specified Data, note can be modified during runtime
    float           LastZoomScale;
    ImVec2          Stride;          // Grid stride in screen space
    ImVec2          OriginalStride;  // Original stride in screen space
    ImVec2          FactoredStride;  // Gactored stride value
    float           Ratio;           // Auto stride ratio (Stride / OriginalStride) adjusted by best fitting bound
    float           Factor;          // 1 / Ratio
    ImRect          ControlRect;     // Full rect without frame padding
    ImRect          ContentRect;     // Content rect with frame padding
    ImVec2          FramePadding;    // Frame padding from style

    struct ToolTipData
    {
        // TODO: chart types
        const char* Label;
        ImVec2 PosBegin;
        ImVec2 PosEnd;
        ImU32 Color;
    };
    ImVector<ToolTipData> ToolTipDatas;

    ImGuiGraph();

    // Add line data set from array `y = values[index]`
    IMGUI_API void AddLineArray(const char* label, const float* values, int values_count, float min_x = 0.0f, float max_x = 100.0f, float thickness = 2.0f);
    // Add line data set from function using index with data source `y = function(data, index)`
    IMGUI_API void AddLineArray(const char* label, float(*function)(void* data, int index), void* data, int values_count, float min_x = 0.0f, float max_x = 100.0f, float thickness = 2.0f);
    // Add line data set from function `y = function(x)`
    IMGUI_API void AddLineFunction(const char* label, float(*function)(float x), int values_count, float min_x = 0.0f, float max_x = 100.0f, float thickness = 2.0f);
    // Add line data set from function with data source `y = function(data, x)`
    IMGUI_API void AddLineFunction(const char* label, float(*function)(void* data, float x), void* data, int values_count, float min_x = 0.0f, float max_x = 100.0f, float thickness = 2.0f);
    // MAYBE Bezier, curves, splines

    // Add points from array `(x, y) = values[index]`
    IMGUI_API void AddPoints(const char* label, const ImVec2* values, int values_count, float radius = 2.0f);
    // Add points from function using index with data source `(x, y) = function(data, index)`
    IMGUI_API void AddPoints(const char* label, ImVec2(*function)(void* data, int index), void* data, int values_count, float radius = 2.0f);
    // MAYBE Bubble chart

    // MAYBE Histogram
    // MAYBE Box plot
};

//-----------------------------------------------------------------------------
// Extra types: Context
//-----------------------------------------------------------------------------

struct IMGUI_API ImGuiExtraContext
{
    ImGuiNextGraphData    NextGraphData; // Storage for SetNextGraph** functions
    ImVector<ImGuiGraph*> Graphs;
    ImGuiStorage          GraphsById;
    ImGuiGraph*           CurrentGraph;

    ~ImGuiExtraContext();
};

//-----------------------------------------------------------------------------
// Extra ImGui end-user API
//-----------------------------------------------------------------------------
namespace ImGui
{
    // Context
    // Extra context data seperate from ImGuiContext* in imgui_internal.h to avoid modification to main libary.
    IMGUI_API ImGuiExtraContext& GetExtraContext();

    // Internal API
    // No guarantee of forward compatibility here.
    IMGUI_API ImGuiGraph* FindGraphByID(ImGuiID id);
    IMGUI_API ImGuiGraph* FindGraphByName(const char* name);

    // Utilities
    IMGUI_API void AddInputString(const ImWchar* str);
    IMGUI_API float GetTimeFloat(); 

    // Widgets: Sliders (tip: ctrl+click on a slider to input with keyboard. manually input values aren't clamped, can go off-bounds)
    // Adjust format string to decorate the value with a prefix, a suffix, or adapt the editing and display precision e.g. "%.3f" -> 1.234; "%5.2f secs" -> 01.23 secs; "Biscuit: %.0f" -> Biscuit: 1; etc.
    IMGUI_API bool SliderUnsignedInt(const char* label, unsigned int* v, unsigned int v_min, unsigned int v_max, const char* format = "%u");
    IMGUI_API bool SliderUnsignedInt2(const char* label, unsigned int v[2], unsigned int v_min, unsigned int v_max, const char* format = "%u");
    IMGUI_API bool SliderUnsignedInt3(const char* label, unsigned int v[3], unsigned int v_min, unsigned int v_max, const char* format = "%u");
    IMGUI_API bool SliderUnsignedInt4(const char* label, unsigned int v[4], unsigned int v_min, unsigned int v_max, const char* format = "%u");
    IMGUI_API bool SliderUnsignedIntN(const char* label, unsigned int* v, int components, unsigned int v_min, unsigned int v_max, const char* format = "%u");
    IMGUI_API bool SliderLongInt(const char* label, long long* v, long long v_min, long long v_max, const char* format = "%lld");
    IMGUI_API bool SliderLongInt2(const char* label, long long v[2], long long v_min, long long v_max, const char* format = "%lld");
    IMGUI_API bool SliderLongInt3(const char* label, long long v[3], long long v_min, long long v_max, const char* format = "%lld");
    IMGUI_API bool SliderLongInt4(const char* label, long long v[4], long long v_min, long long v_max, const char* format = "%lld");
    IMGUI_API bool SliderLongIntN(const char* label, long long* v, int components, long long v_min, long long v_max, const char* format = "%lld");
    IMGUI_API bool SliderUnsignedLongInt(const char* label, unsigned long long* v, unsigned long long v_min, unsigned long long v_max, const char* format = "%llu");
    IMGUI_API bool SliderUnsignedLongInt2(const char* label, unsigned long long v[2], unsigned long long v_min, unsigned long long v_max, const char* format = "%llu");
    IMGUI_API bool SliderUnsignedLongInt3(const char* label, unsigned long long v[3], unsigned long long v_min, unsigned long long v_max, const char* format = "%llu");
    IMGUI_API bool SliderUnsignedLongInt4(const char* label, unsigned long long v[4], unsigned long long v_min, unsigned long long v_max, const char* format = "%llu");
    IMGUI_API bool SliderUnsignedLongIntN(const char* label, unsigned long long* v, int components, unsigned long long v_min, unsigned long long v_max, const char* format = "%llu");
    IMGUI_API bool VSliderUnsignedInt(const char* label, const ImVec2& size, unsigned int* v, unsigned int v_min, unsigned int v_max, const char* format = "%u");
    IMGUI_API bool VSliderLongInt(const char* label, const ImVec2& size, long long* v, long long v_min, long long v_max, const char* format = "%lld");
    IMGUI_API bool VSliderUnsignedLongInt(const char* label, const ImVec2& size, unsigned long long* v, unsigned long long v_min, unsigned long long v_max, const char* format = "%llu");

    // Widgets: Input with Keyboard
    // If you want to use InputText() with a dynamic string type such as std::string or your own, see misc/cpp/imgui_stdlib.h
    IMGUI_API bool InputUnsignedInt(const char* label, unsigned int* v, unsigned int step = 1, unsigned int step_fast = 100, ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool InputUnsignedInt2(const char* label, unsigned int v[2], ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool InputUnsignedInt3(const char* label, unsigned int v[3], ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool InputUnsignedInt4(const char* label, unsigned int v[4], ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool InputUnsignedIntN(const char* label, unsigned int* v, int components, ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool InputLongInt(const char* label, long long* v, long long step = 1, long long step_fast = 100, ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool InputLongInt2(const char* label, long long v[2], ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool InputLongInt3(const char* label, long long v[3], ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool InputLongInt4(const char* label, long long v[4], ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool InputLongIntN(const char* label, long long* v, int components, ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool InputUnsignedLongInt(const char* label, unsigned long long* v, unsigned long long step = 1, unsigned long long step_fast = 100, ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool InputUnsignedLongInt2(const char* label, unsigned long long v[2], ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool InputUnsignedLongInt3(const char* label, unsigned long long v[3], ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool InputUnsignedLongInt4(const char* label, unsigned long long v[4], ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool InputUnsignedLongIntN(const char* label, unsigned long long* v, int components, ImGuiInputTextFlags extra_flags = 0);

    // Widgets: Plot
    IMGUI_API void PlotLinesWithGrid(const char* label, const float* values, int values_count, const int* grid, int grid_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0,0), int stride = sizeof(float));
    // Plot lines from an array of data sources
    IMGUI_API void PlotMultiLines(const char* label, int num_datas, const char** names, const ImColor* colors, float(*getter)(const void* data, int idx), void** datas, int values_count, float scale_min, float scale_max, ImVec2 graph_size);
    // Plot histograms from an array of data sources
    IMGUI_API void PlotMultiHistograms(const char* label, int num_datas, const char** names, const ImColor* colors, float(*getter)(const void* data, int idx), void** datas, int values_count, float scale_min, float scale_max, ImVec2 graph_size);
    // Plot selectable line segments from an array of data source
    // return : Returns the selected segment index value
    IMGUI_API int SelectablePlotMultiLines(const char* label, int num_datas, const char** names, const ImColor* colors, float(*getter)(const void* data, int idx), void** datas, int values_count, int selectedIdx, float scale_min, float scale_max, ImVec2 graph_size);
    // Plot selectable histograms from an array of data sources
    // return : Returns the selected segment index value
    IMGUI_API int SelectablePlotMultiHistograms(const char* label, int num_datas, const char** names, const ImColor* colors, float(*getter)(const void* data, int idx), void** datas, int values_count, int selectedIdx, float scale_min, float scale_max, ImVec2 graph_size);
    // Plot selectable line segments from a single data source
    // return : Returns the selected segment index value
    IMGUI_API int SelectablePlotLines(const char* label, const float* values, int values_count, int values_offset = 0, int selectedIdx = -1, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0), int stride = sizeof(float));
    // Plot selectable histograms from a single data source
    // return : Returns the selected segment index value
    IMGUI_API int SelectablePlotHistograms(const char* label, const float* values, int values_count, int values_offset = 0, int selectedIdx = -1, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0), int stride = sizeof(float));

    // Widgets: Graph
    // - The BeginGraph()/EndGraph() api allows you to style and plot graphs.
    // - Only call a matching EndGraph() if BeginGraph() returns true
    IMGUI_API bool BeginGraph(const char* str_id, ImVec2 size = ImVec2(0,0), ImGuiGraphFlags flags = 0);
    IMGUI_API void EndGraph();
    IMGUI_API ImGuiGraph* GetCurrentGraph();

    // WIP
    // Widgets: Graph Utilities
    // - ImGuiCond_Once and ImGuiCond_FirstUseEver treated the same since saving/loading is no supported yet with graphs.
    IMGUI_API void SetNextGraphPosition(ImVec2 pos, ImGuiCond cond = 0);
    IMGUI_API void SetNextGraphPosition(float x, float y, ImGuiCond cond = 0);
    IMGUI_API void SetNextGraphZoomScale(float zoom, ImGuiCond cond = 0);
    IMGUI_API void SetNextGraphGridLineThickness(float thickness, ImGuiCond cond = 0);
    IMGUI_API void SetNextGraphGridStride(ImVec2 stride, ImGuiCond cond = 0);
    IMGUI_API void SetNextGraphGridStride(float stride_x, float stride_y, ImGuiCond cond = 0);
    IMGUI_API void SetNextGraphAxesName(const char* name_x, const char* name_y, ImGuiCond cond = 0);
    IMGUI_API void SetNextGraphAxesPosition(ImVec2 pos, ImGuiCond cond = 0);
    IMGUI_API void SetNextGraphAxesPosition(float x, float y, ImGuiCond cond = 0);
    IMGUI_API void SetNextGraphAxesLineThickness(float thickness, ImGuiCond cond = 0);
    IMGUI_API void SetNextGraphAxesLabelFormat(const char* format,  ImGuiCond cond = 0);
} // namespace ImGui


#endif // IMGUI_DISABLE_API
