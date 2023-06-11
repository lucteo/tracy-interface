#include <tracy_interface.hpp>

#include "common/TracySystem.hpp"
#include "common/TracyAlign.hpp"
#include "common/TracyAlloc.hpp"
#include "client/TracyProfiler.hpp"

using namespace tracy;

namespace tracy_interface {
inline namespace v1 {

// Most of the code here is directly copied from tracy's TracyScoped.hpp

void emit_zone_begin(const location* loc) {
    TracyLfqPrepare(QueueType::ZoneBegin);
    MemWrite(&item->zoneBegin.time, Profiler::GetTime());
    MemWrite(&item->zoneBegin.srcloc, (uint64_t)loc);
    TracyLfqCommit;
}
void emit_zone_end() {
    TracyLfqPrepare(QueueType::ZoneEnd);
    MemWrite(&item->zoneEnd.time, Profiler::GetTime());
    TracyLfqCommit;
}

void set_dyn_name(std::string_view name) {
    assert(name.size() < std::numeric_limits<uint16_t>::max());
    auto ptr = (char*)tracy_malloc(name.size());
    memcpy(ptr, name.data(), name.size());
    TracyLfqPrepare(QueueType::ZoneName);
    MemWrite(&item->zoneTextFat.text, (uint64_t)ptr);
    MemWrite(&item->zoneTextFat.size, (uint16_t)name.size());
    TracyLfqCommit;
}
void set_text(std::string_view text) {
    assert(text.size() < std::numeric_limits<uint16_t>::max());
    auto ptr = (char*)tracy_malloc(text.size());
    memcpy(ptr, text.data(), text.size());
    TracyLfqPrepare(QueueType::ZoneText);
    MemWrite(&item->zoneTextFat.text, (uint64_t)ptr);
    MemWrite(&item->zoneTextFat.size, (uint16_t)text.size());
    TracyLfqCommit;
}
void set_color(uint32_t color) {
    TracyLfqPrepare(QueueType::ZoneColor);
    MemWrite(&item->zoneColor.r, uint8_t((color)&0xFF));
    MemWrite(&item->zoneColor.g, uint8_t((color >> 8) & 0xFF));
    MemWrite(&item->zoneColor.b, uint8_t((color >> 16) & 0xFF));
    TracyLfqCommit;
}
void set_value(uint64_t value) {
    TracyLfqPrepare(QueueType::ZoneValue);
    MemWrite(&item->zoneValue.value, value);
    TracyLfqCommit;
}

void set_plot_type(const char* static_plot_name, plot_type t) {
    switch (t) {
    case plot_type::number:
        Profiler::ConfigurePlot(static_plot_name, PlotFormatType::Number);
        break;
    case plot_type::memory:
        Profiler::ConfigurePlot(static_plot_name, PlotFormatType::Memory);
        break;
    case plot_type::percentage:
        Profiler::ConfigurePlot(static_plot_name, PlotFormatType::Percentage);
        break;
    }
}

void set_plot_value_int(const char* static_plot_name, int64_t val) {
    Profiler::PlotData(static_plot_name, val);
}
void set_plot_value_float(const char* static_plot_name, float val) {
    Profiler::PlotData(static_plot_name, val);
}

void set_cur_thread_name(const char* static_name) { SetThreadName(static_name); }

} // namespace v1
} // namespace tracy_interface
