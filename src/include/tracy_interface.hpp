#pragma once

#include <string_view>

namespace tracy_interface {

inline namespace v1 {

#if TRACY_INTERFACE_ENABLE_PROFILING

//! Structure defining a (static) location in the code
struct location {
    const char* name;
    const char* function;
    const char* file;
    uint32_t line;
    uint32_t color;
};

#define TRACY_INTERFACE_GET_LOCATION()                                                             \
    ([] {                                                                                          \
        static location loc{nullptr, __FUNCTION__, __FILE__, __LINE__, 0};                         \
        return loc;                                                                                \
    })
#define TRACY_INTERFACE_GET_LOCATION_N(static_name)                                                \
    ([] {                                                                                          \
        static location loc{(static_name), __FUNCTION__, __FILE__, __LINE__, 0};                   \
        return loc;                                                                                \
    })
#define TRACY_INTERFACE_GET_LOCATION_C(color)                                                      \
    ([] {                                                                                          \
        static location loc{nullptr, __FUNCTION__, __FILE__, __LINE__, color};                     \
        return loc;                                                                                \
    })
#define TRACY_INTERFACE_GET_LOCATION_NC(static_name, color)                                        \
    ([] {                                                                                          \
        static location loc{(static_name), __FUNCTION__, __FILE__, __LINE__, (color)};             \
        return loc;                                                                                \
    })

#else

struct location {};

#define TRACY_INTERFACE_GET_LOCATION() location{};
#define TRACY_INTERFACE_GET_LOCATION_N(static_name) location{};
#define TRACY_INTERFACE_GET_LOCATION_C(color) location{};
#define TRACY_INTERFACE_GET_LOCATION_NC(static_name, color) location{};

#endif

//! A profiling (scoped) zone. This needs to nest well with the other zones on the same thread.
struct profiling_zone {
    profiling_zone(const profiling_zone&) = delete;
    profiling_zone(profiling_zone&&) = delete;
    profiling_zone& operator=(const profiling_zone&) = delete;
    profiling_zone& operator=(profiling_zone&&) = delete;

    explicit profiling_zone(const location& l, bool is_active = true);
    ~profiling_zone();

    void set_dyn_name(std::string_view name);
    void set_text(std::string_view text);
    void set_color(uint32_t color);
    void set_value(uint64_t value);

    bool is_active() const;

#if TRACY_INTERFACE_ENABLE_PROFILING
private:
    const bool is_active_;
#endif
};

//! A profiling plot. This is cheap to create from a static string
struct profiling_plot {
    enum class type {
        number,
        memory,
        percentage,
    };
    explicit profiling_plot(const char* static_name);
    explicit profiling_plot(const char* static_name, type t);

    void set_value_int(int64_t val);
    void set_value_float(float val);

#if TRACY_INTERFACE_ENABLE_PROFILING
private:
    const char* plot_name_;
#endif
};

//! Set the name of the current thread
void set_cur_thread_name(const char* static_name);

#if !TRACY_INTERFACE_ENABLE_PROFILING

inline profiling_zone::profiling_zone(const location&, bool) {}
inline profiling_zone::~profiling_zone() {}

inline void profiling_zone::set_dyn_name(std::string_view) {}
inline void profiling_zone::set_text(std::string_view) {}
inline void profiling_zone::set_color(uint32_t) {}
inline void profiling_zone::set_value(uint64_t) {}

inline bool profiling_zone::is_active() const { return false; }

inline profiling_plot::profiling_plot(const char*) {}
inline profiling_plot::profiling_plot(const char*, type) {}

inline void profiling_plot::set_value_int(int64_t) {}
inline void profiling_plot::set_value_float(float) {}

#endif

} // namespace v1
} // namespace tracy_interface
