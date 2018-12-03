// dear imgui
// (extra headers)

#pragma once

// Configuration file (edit imconfig.h or define IMGUI_USER_CONFIG to your own filename)
#ifdef IMGUI_USER_CONFIG
#include IMGUI_USER_CONFIG
#endif
#if !defined(IMGUI_DISABLE_INCLUDE_IMCONFIG_H) || defined(IMGUI_INCLUDE_IMCONFIG_H)
#include "imconfig.h"
#endif

#ifndef IMGUI_DISABLE_API

//-----------------------------------------------------------------------------
// Extra ImGui
//-----------------------------------------------------------------------------

namespace ImGui
{
    //-------------------------------------------------------------------------
    // Miscellaneous Utilities
    //-------------------------------------------------------------------------

    // Input:
    IMGUI_API void          AddInputString(const ImWchar* str);

    // Time:
    IMGUI_API float         GetTimeFloat(); 

    //-------------------------------------------------------------------------
    // Miscellaneous Widgets
    //-------------------------------------------------------------------------

    // Widgets: Sliders (tip: ctrl+click on a slider to input with keyboard. manually input values aren't clamped, can go off-bounds)
    // Adjust format string to decorate the value with a prefix, a suffix, or adapt the editing and display precision e.g. "%.3f" -> 1.234; "%5.2f secs" -> 01.23 secs; "Biscuit: %.0f" -> Biscuit: 1; etc.
    IMGUI_API bool          SliderUnsignedInt(const char* label, unsigned int* v, unsigned int v_min, unsigned int v_max, const char* format = "%u");
    IMGUI_API bool          SliderUnsignedInt2(const char* label, unsigned int v[2], unsigned int v_min, unsigned int v_max, const char* format = "%u");
    IMGUI_API bool          SliderUnsignedInt3(const char* label, unsigned int v[3], unsigned int v_min, unsigned int v_max, const char* format = "%u");
    IMGUI_API bool          SliderUnsignedInt4(const char* label, unsigned int v[4], unsigned int v_min, unsigned int v_max, const char* format = "%u");
    IMGUI_API bool          SliderUnsignedIntN(const char* label, unsigned int* v, int components, unsigned int v_min, unsigned int v_max, const char* format = "%u");
    IMGUI_API bool          SliderLongInt(const char* label, long long* v, long long v_min, long long v_max, const char* format = "%lld");
    IMGUI_API bool          SliderLongInt2(const char* label, long long v[2], long long v_min, long long v_max, const char* format = "%lld");
    IMGUI_API bool          SliderLongInt3(const char* label, long long v[3], long long v_min, long long v_max, const char* format = "%lld");
    IMGUI_API bool          SliderLongInt4(const char* label, long long v[4], long long v_min, long long v_max, const char* format = "%lld");
    IMGUI_API bool          SliderLongIntN(const char* label, long long* v, int components, long long v_min, long long v_max, const char* format = "%lld");
    IMGUI_API bool          SliderUnsignedLongInt(const char* label, unsigned long long* v, unsigned long long v_min, unsigned long long v_max, const char* format = "%llu");
    IMGUI_API bool          SliderUnsignedLongInt2(const char* label, unsigned long long v[2], unsigned long long v_min, unsigned long long v_max, const char* format = "%llu");
    IMGUI_API bool          SliderUnsignedLongInt3(const char* label, unsigned long long v[3], unsigned long long v_min, unsigned long long v_max, const char* format = "%llu");
    IMGUI_API bool          SliderUnsignedLongInt4(const char* label, unsigned long long v[4], unsigned long long v_min, unsigned long long v_max, const char* format = "%llu");
    IMGUI_API bool          SliderUnsignedLongIntN(const char* label, unsigned long long* v, int components, unsigned long long v_min, unsigned long long v_max, const char* format = "%llu");
    IMGUI_API bool          VSliderUnsignedInt(const char* label, const ImVec2& size, unsigned int* v, unsigned int v_min, unsigned int v_max, const char* format = "%u");
    IMGUI_API bool          VSliderLongInt(const char* label, const ImVec2& size, long long* v, long long v_min, long long v_max, const char* format = "%lld");
    IMGUI_API bool          VSliderUnsignedLongInt(const char* label, const ImVec2& size, unsigned long long* v, unsigned long long v_min, unsigned long long v_max, const char* format = "%llu");

    // Widgets: Input with Keyboard
    // If you want to use InputText() with a dynamic string type such as std::string or your own, see misc/cpp/imgui_stdlib.h
    IMGUI_API bool          InputUnsignedInt(const char* label, unsigned int* v, unsigned int step = 1, unsigned int step_fast = 100, ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool          InputUnsignedInt2(const char* label, unsigned int v[2], ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool          InputUnsignedInt3(const char* label, unsigned int v[3], ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool          InputUnsignedInt4(const char* label, unsigned int v[4], ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool          InputUnsignedIntN(const char* label, unsigned int* v, int components, ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool          InputLongInt(const char* label, long long* v, long long step = 1, long long step_fast = 100, ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool          InputLongInt2(const char* label, long long v[2], ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool          InputLongInt3(const char* label, long long v[3], ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool          InputLongInt4(const char* label, long long v[4], ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool          InputLongIntN(const char* label, long long* v, int components, ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool          InputUnsignedLongInt(const char* label, unsigned long long* v, unsigned long long step = 1, unsigned long long step_fast = 100, ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool          InputUnsignedLongInt2(const char* label, unsigned long long v[2], ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool          InputUnsignedLongInt3(const char* label, unsigned long long v[3], ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool          InputUnsignedLongInt4(const char* label, unsigned long long v[4], ImGuiInputTextFlags extra_flags = 0);
    IMGUI_API bool          InputUnsignedLongIntN(const char* label, unsigned long long* v, int components, ImGuiInputTextFlags extra_flags = 0);

    //-------------------------------------------------------------------------
    // Miscellaneous Internal
    //-------------------------------------------------------------------------

    // Plot:
    IMGUI_API void          PlotLinesWithGrid(const char* label, const float* values, int values_count, const int* grid, int grid_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0,0), int stride = sizeof(float));
    IMGUI_API void          PlotEx(ImGuiPlotType plot_type, const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, const int* grid, int grid_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size);

} // namespace ImGui


#endif // IMGUI_DISABLE_API
