#pragma once

#include <string_view>

namespace tracy_interface {

inline namespace v1 {

//! Structure defining a (static) location in the code
struct location {
    const char* name;
    const char* function;
    const char* file;
    uint32_t line;
    uint32_t color;
};

//! Emit a zone begin for the given location.
//! The location needs to have a static storage (same pointer for the program lifetime)
void emit_zone_begin(const location* loc);
//! Emit a zone end.
//! This needs to be called on the same thread as the corresponding emit_zone_begin()
void emit_zone_end();
//! Set the dynamic name for the zone.
//! This can be used when the location name is not enough (being static).
//! This needs to be called between emit_zone_begin() and emit_zone_end()
void set_dyn_name(std::string_view name);
//! Set associated text for the zone.
//! This needs to be called between emit_zone_begin() and emit_zone_end()
void set_text(std::string_view text);
//! Set color for the zone.
//! This needs to be called between emit_zone_begin() and emit_zone_end()
void set_color(uint32_t color);
//! Set extra value for the zone.
//! This needs to be called between emit_zone_begin() and emit_zone_end()
void set_value(uint64_t value);

//! The type of a plot
enum class plot_type {
    number,
    memory,
    percentage,
};
//! Sets the type of the plot
//! The plot is identified by name; this needs to be a static name.
void set_plot_type(const char* static_plot_name, plot_type t);
//! Sets a value for the plot
//! The plot is identified by name; this needs to be a static name.
void set_plot_value_int(const char* static_plot_name, int64_t val);
//! Sets a value for the plot
//! The plot is identified by name; this needs to be a static name.
void set_plot_value_float(const char* static_plot_name, float val);

//! Set the name of the current thread
void set_cur_thread_name(const char* static_name);

} // namespace v1
} // namespace tracy_interface
