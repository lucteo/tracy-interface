#include <tracy_interface.hpp>

#if TRACY_INTERFACE_ENABLE_PROFILING

#include "common/TracySystem.hpp"
#include "common/TracyAlign.hpp"
#include "common/TracyAlloc.hpp"
#include "client/TracyProfiler.hpp"

using namespace tracy;

namespace tracy_interface {
inline namespace v1 {

// Most of the code here is directly copied from tracy's TracyScoped.hpp

profiling_zone::profiling_zone(const location& l, bool is_active)
    : is_active_(is_active) {
    if (!is_active_)
        return;
    TracyLfqPrepare(QueueType::ZoneBegin);
    MemWrite(&item->zoneBegin.time, Profiler::GetTime());
    MemWrite(&item->zoneBegin.srcloc, (uint64_t)&l);
    TracyLfqCommit;
}
profiling_zone::~profiling_zone() {
    if (!is_active_)
        return;
    TracyLfqPrepare(QueueType::ZoneEnd);
    MemWrite(&item->zoneEnd.time, Profiler::GetTime());
    TracyLfqCommit;
}

void profiling_zone::set_dyn_name(std::string_view name) {
    assert(name.size() < std::numeric_limits<uint16_t>::max());
    if (!is_active_)
        return;
    auto ptr = (char*)tracy_malloc(name.size());
    memcpy(ptr, name.data(), name.size());
    TracyLfqPrepare(QueueType::ZoneName);
    MemWrite(&item->zoneTextFat.text, (uint64_t)ptr);
    MemWrite(&item->zoneTextFat.size, (uint16_t)name.size());
    TracyLfqCommit;
}
void profiling_zone::set_text(std::string_view text) {
    assert(text.size() < std::numeric_limits<uint16_t>::max());
    if (!is_active_)
        return;
    auto ptr = (char*)tracy_malloc(text.size());
    memcpy(ptr, text.data(), text.size());
    TracyLfqPrepare(QueueType::ZoneText);
    MemWrite(&item->zoneTextFat.text, (uint64_t)ptr);
    MemWrite(&item->zoneTextFat.size, (uint16_t)text.size());
    TracyLfqCommit;
}
void profiling_zone::set_color(uint32_t color) {
    if (!is_active_)
        return;
    TracyLfqPrepare(QueueType::ZoneColor);
    MemWrite(&item->zoneColor.r, uint8_t((color)&0xFF));
    MemWrite(&item->zoneColor.g, uint8_t((color >> 8) & 0xFF));
    MemWrite(&item->zoneColor.b, uint8_t((color >> 16) & 0xFF));
    TracyLfqCommit;
}
void profiling_zone::set_value(uint64_t value) {
    if (!is_active_)
        return;
    TracyLfqPrepare(QueueType::ZoneValue);
    MemWrite(&item->zoneValue.value, value);
    TracyLfqCommit;
}

bool profiling_zone::is_active() const { return is_active_; }

profiling_plot::profiling_plot(const char* static_name)
    : plot_name_(static_name) {}
profiling_plot::profiling_plot(const char* static_name, type t)
    : plot_name_(static_name) {
    switch (t) {
    case type::number:
        Profiler::ConfigurePlot(static_name, PlotFormatType::Number);
        break;
    case type::memory:
        Profiler::ConfigurePlot(static_name, PlotFormatType::Memory);
        break;
    case type::percentage:
        Profiler::ConfigurePlot(static_name, PlotFormatType::Percentage);
        break;
    }
}

void profiling_plot::set_value_int(int64_t val) { Profiler::PlotData(plot_name_, val); }
void profiling_plot::set_value_float(float val) { Profiler::PlotData(plot_name_, val); }

void set_cur_thread_name(const char* static_name) { SetThreadName(static_name); }

} // namespace v1
} // namespace tracy_interface

#endif
