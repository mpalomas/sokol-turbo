#ifndef SOKOL_APP_TURBO_INCLUDED
#define SOKOL_APP_TURBO_INCLUDED

#if defined(SOKOL_TURBO_WAYLAND) && !defined(__linux__) && !defined(__unix__)
#error("sokol_app_turbo.h: SOKOL_TURBO_WAYLAND is only supported on Linux")
#endif

#if defined(SOKOL_TURBO_WAYLAND) && defined(SOKOL_APP_IMPL)
#include <stdbool.h>
#include <stdint.h>
struct wl_display;
struct wl_registry;
struct wl_compositor;
struct wl_surface;
struct wl_seat;
struct wl_pointer;
struct wl_keyboard;
struct wl_egl_window;
struct xdg_wm_base;
struct xdg_surface;
struct xdg_toplevel;
struct libdecor;
struct libdecor_frame;
struct libdecor_configuration;
struct libdecor_state;
struct xkb_context;
struct xkb_keymap;
struct xkb_state;
struct wl_shm;
struct wl_cursor_theme;
struct wl_cursor;
struct wl_cursor_image;
struct wl_buffer;
struct zwp_pointer_constraints_v1;
struct zwp_locked_pointer_v1;
struct zwp_confined_pointer_v1;
struct zwp_relative_pointer_manager_v1;
struct zwp_relative_pointer_v1;

typedef struct _sapp_wayland_t {
    struct wl_display* display;
    struct wl_registry* registry;
    struct wl_compositor* compositor;
    struct wl_shm* shm;
    struct wl_surface* surface;
    struct wl_surface* cursor_surface;
    struct wl_seat* seat;
    struct wl_pointer* pointer;
    struct wl_keyboard* keyboard;
    struct wl_egl_window* egl_window;
    struct xdg_wm_base* xdg_wm_base;
    struct xdg_surface* xdg_surface;
    struct xdg_toplevel* xdg_toplevel;
    struct libdecor* decor_context;
    struct libdecor_frame* decor_frame;
    void* libdecor_so;
    uint32_t wm_base_version;
    uint32_t compositor_version;
    uint32_t seat_version;
    uint32_t pointer_serial;
    uint32_t keyboard_serial;
    uint32_t mouse_buttons;
    uint32_t modifiers;
    uint32_t xkb_shift;
    uint32_t xkb_ctrl;
    uint32_t xkb_alt;
    uint32_t xkb_super;
    struct xkb_context* xkb_context;
    struct xkb_keymap* xkb_keymap;
    struct xkb_state* xkb_state;
    struct wl_cursor_theme* cursor_theme;
    struct wl_cursor* default_cursor;
    struct wl_buffer* hidden_cursor_buffer;
    struct zwp_pointer_constraints_v1* pointer_constraints;
    struct zwp_locked_pointer_v1* locked_pointer;
    struct zwp_relative_pointer_manager_v1* relative_pointer_manager;
    struct zwp_relative_pointer_v1* relative_pointer;
    void* wayland_cursor_so;
    bool pointer_serial_valid;
    bool pointer_focused;
    bool keyboard_focused;
    bool configured;
    bool close_requested;
    bool using_libdecor;
} _sapp_wayland_t;
#endif

#include "sokol_app.h"

SOKOL_APP_API_DECL const void* sapp_wayland_get_display(void);
SOKOL_APP_API_DECL const void* sapp_wayland_get_surface(void);

#endif /* SOKOL_APP_TURBO_INCLUDED */

#if defined(SOKOL_APP_IMPL) && defined(SOKOL_TURBO_WAYLAND)
#ifndef SOKOL_APP_TURBO_IMPL_INCLUDED
#define SOKOL_APP_TURBO_IMPL_INCLUDED

#include <errno.h>
#include <poll.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-cursor.h>
#include <stdlib.h>

enum {
    XDG_WM_BASE_DESTROY = 0,
    XDG_WM_BASE_CREATE_POSITIONER = 1,
    XDG_WM_BASE_GET_XDG_SURFACE = 2,
    XDG_WM_BASE_PONG = 3,
    XDG_SURFACE_DESTROY = 0,
    XDG_SURFACE_GET_TOPLEVEL = 1,
    XDG_SURFACE_ACK_CONFIGURE = 4,
    XDG_TOPLEVEL_DESTROY = 0,
    XDG_TOPLEVEL_SET_TITLE = 2,
    XDG_TOPLEVEL_SET_FULLSCREEN = 11,
    XDG_TOPLEVEL_UNSET_FULLSCREEN = 12,
    ZWP_POINTER_CONSTRAINTS_DESTROY = 0,
    ZWP_POINTER_CONSTRAINTS_LOCK_POINTER = 1,
    ZWP_LOCKED_POINTER_DESTROY = 0,
    ZWP_RELATIVE_POINTER_MANAGER_DESTROY = 0,
    ZWP_RELATIVE_POINTER_MANAGER_GET_RELATIVE_POINTER = 1,
    ZWP_RELATIVE_POINTER_DESTROY = 0,
};

enum {
    ZWP_POINTER_CONSTRAINTS_LIFETIME_ONESHOT = 1,
    ZWP_POINTER_CONSTRAINTS_LIFETIME_PERSISTENT = 2,
};

struct xdg_wm_base { struct wl_proxy* proxy; };
struct xdg_surface { struct wl_proxy* proxy; };
struct xdg_toplevel { struct wl_proxy* proxy; };
struct zwp_pointer_constraints_v1 { struct wl_proxy* proxy; };
struct zwp_locked_pointer_v1 { struct wl_proxy* proxy; };
struct zwp_confined_pointer_v1 { struct wl_proxy* proxy; };
struct zwp_relative_pointer_manager_v1 { struct wl_proxy* proxy; };
struct zwp_relative_pointer_v1 { struct wl_proxy* proxy; };
struct libdecor { int _unused; };
struct libdecor_frame { int _unused; };
struct libdecor_configuration { int _unused; };
struct libdecor_state { int _unused; };

typedef int _sapp_libdecor_error;
typedef int _sapp_libdecor_window_state;
enum {
    _SAPP_LIBDECOR_WINDOW_STATE_MAXIMIZED = 1 << 1,
    _SAPP_LIBDECOR_WINDOW_STATE_FULLSCREEN = 1 << 2,
};

typedef struct {
    void (*error)(struct libdecor* context, _sapp_libdecor_error error, const char* message);
    void (*reserved0)(void);
    void (*reserved1)(void);
    void (*reserved2)(void);
    void (*reserved3)(void);
    void (*reserved4)(void);
    void (*reserved5)(void);
    void (*reserved6)(void);
    void (*reserved7)(void);
    void (*reserved8)(void);
    void (*reserved9)(void);
} _sapp_libdecor_interface;

typedef struct {
    void (*configure)(struct libdecor_frame* frame, struct libdecor_configuration* configuration, void* user_data);
    void (*close)(struct libdecor_frame* frame, void* user_data);
    void (*commit)(struct libdecor_frame* frame, void* user_data);
    void (*dismiss_popup)(struct libdecor_frame* frame, const char* seat_name, void* user_data);
    void (*reserved0)(void);
    void (*reserved1)(void);
    void (*reserved2)(void);
    void (*reserved3)(void);
    void (*reserved4)(void);
    void (*reserved5)(void);
    void (*reserved6)(void);
    void (*reserved7)(void);
    void (*reserved8)(void);
    void (*reserved9)(void);
} _sapp_libdecor_frame_interface;

typedef struct {
    struct libdecor* (*new_)(struct wl_display* display, _sapp_libdecor_interface* iface);
    void (*unref)(struct libdecor* context);
    int (*dispatch)(struct libdecor* context, int timeout);
    struct libdecor_frame* (*decorate)(struct libdecor* context, struct wl_surface* surface, _sapp_libdecor_frame_interface* iface, void* user_data);
    void (*frame_unref)(struct libdecor_frame* frame);
    void (*frame_map)(struct libdecor_frame* frame);
    void (*frame_set_title)(struct libdecor_frame* frame, const char* title);
    void (*frame_set_app_id)(struct libdecor_frame* frame, const char* app_id);
    void (*frame_commit)(struct libdecor_frame* frame, struct libdecor_state* state, struct libdecor_configuration* configuration);
    void (*frame_set_fullscreen)(struct libdecor_frame* frame, struct wl_output* output);
    void (*frame_unset_fullscreen)(struct libdecor_frame* frame);
    struct libdecor_state* (*state_new)(int width, int height);
    void (*state_free)(struct libdecor_state* state);
    bool (*configuration_get_content_size)(struct libdecor_configuration* configuration, struct libdecor_frame* frame, int* width, int* height);
    bool (*configuration_get_window_state)(struct libdecor_configuration* configuration, _sapp_libdecor_window_state* window_state);
} _sapp_libdecor_api_t;

static _sapp_libdecor_api_t _sapp_libdecor;

typedef struct {
    struct wl_cursor_theme* (*theme_load)(const char* name, int size, struct wl_shm* shm);
    void (*theme_destroy)(struct wl_cursor_theme* theme);
    struct wl_cursor* (*theme_get_cursor)(struct wl_cursor_theme* theme, const char* name);
    struct wl_buffer* (*image_get_buffer)(struct wl_cursor_image* image);
} _sapp_wayland_cursor_api_t;

static _sapp_wayland_cursor_api_t _sapp_wayland_cursor;

static const struct wl_interface xdg_wm_base_interface;
static const struct wl_interface xdg_surface_interface;
static const struct wl_interface xdg_toplevel_interface;
static const struct wl_interface xdg_positioner_interface;
static const struct wl_interface xdg_popup_interface;
static const struct wl_interface zwp_pointer_constraints_v1_interface;
static const struct wl_interface zwp_locked_pointer_v1_interface;
static const struct wl_interface zwp_confined_pointer_v1_interface;
static const struct wl_interface zwp_relative_pointer_manager_v1_interface;
static const struct wl_interface zwp_relative_pointer_v1_interface;

static const struct wl_interface* _sapp_xdg_wm_base_request_types[] = {
    NULL, NULL, &xdg_positioner_interface, &wl_surface_interface, NULL,
};
static const struct wl_interface* _sapp_xdg_surface_request_types[] = {
    NULL, NULL, &xdg_toplevel_interface, &xdg_positioner_interface,
    &xdg_popup_interface, NULL, NULL,
};
static const struct wl_interface* _sapp_xdg_toplevel_request_types[] = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &wl_output_interface, NULL,
};
static const struct wl_interface* _sapp_zwp_pointer_constraints_request_types[] = {
    NULL,
    &zwp_locked_pointer_v1_interface, &wl_surface_interface, &wl_pointer_interface, &wl_region_interface, NULL,
    &zwp_confined_pointer_v1_interface, &wl_surface_interface, &wl_pointer_interface, &wl_region_interface, NULL,
};
static const struct wl_interface* _sapp_zwp_locked_pointer_request_types[] = {
    NULL, NULL, NULL,
};
static const struct wl_interface* _sapp_zwp_relative_pointer_manager_request_types[] = {
    NULL, &zwp_relative_pointer_v1_interface, &wl_pointer_interface,
};
static const struct wl_interface* _sapp_zwp_relative_pointer_request_types[] = {
    NULL,
};
static const struct wl_message _sapp_xdg_wm_base_requests[] = {
    { "destroy", "", _sapp_xdg_wm_base_request_types + 0 },
    { "create_positioner", "n", _sapp_xdg_wm_base_request_types + 2 },
    { "get_xdg_surface", "no", _sapp_xdg_wm_base_request_types + 3 },
    { "pong", "u", _sapp_xdg_wm_base_request_types + 0 },
};
static const struct wl_message _sapp_xdg_wm_base_events[] = {
    { "ping", "u", _sapp_xdg_wm_base_request_types + 0 },
};
static const struct wl_message _sapp_xdg_surface_requests[] = {
    { "destroy", "", _sapp_xdg_surface_request_types + 0 },
    { "get_toplevel", "n", _sapp_xdg_surface_request_types + 2 },
    { "get_popup", "n?oo", _sapp_xdg_surface_request_types + 3 },
    { "set_window_geometry", "iiii", _sapp_xdg_surface_request_types + 0 },
    { "ack_configure", "u", _sapp_xdg_surface_request_types + 0 },
};
static const struct wl_message _sapp_xdg_surface_events[] = {
    { "configure", "u", _sapp_xdg_surface_request_types + 0 },
};
static const struct wl_message _sapp_xdg_toplevel_requests[] = {
    { "destroy", "", _sapp_xdg_toplevel_request_types + 0 },
    { "set_parent", "?o", _sapp_xdg_toplevel_request_types + 0 },
    { "set_title", "s", _sapp_xdg_toplevel_request_types + 0 },
    { "set_app_id", "s", _sapp_xdg_toplevel_request_types + 0 },
    { "show_window_menu", "ouii", _sapp_xdg_toplevel_request_types + 0 },
    { "move", "ou", _sapp_xdg_toplevel_request_types + 0 },
    { "resize", "ouu", _sapp_xdg_toplevel_request_types + 0 },
    { "set_max_size", "ii", _sapp_xdg_toplevel_request_types + 0 },
    { "set_min_size", "ii", _sapp_xdg_toplevel_request_types + 0 },
    { "set_maximized", "", _sapp_xdg_toplevel_request_types + 0 },
    { "unset_maximized", "", _sapp_xdg_toplevel_request_types + 0 },
    { "set_fullscreen", "?o", _sapp_xdg_toplevel_request_types + 11 },
    { "unset_fullscreen", "", _sapp_xdg_toplevel_request_types + 0 },
    { "set_minimized", "", _sapp_xdg_toplevel_request_types + 0 },
};
static const struct wl_message _sapp_xdg_toplevel_events[] = {
    { "configure", "iia", _sapp_xdg_toplevel_request_types + 0 },
    { "close", "", _sapp_xdg_toplevel_request_types + 0 },
    { "configure_bounds", "ii", _sapp_xdg_toplevel_request_types + 0 },
    { "wm_capabilities", "a", _sapp_xdg_toplevel_request_types + 0 },
};
static const struct wl_message _sapp_zwp_pointer_constraints_requests[] = {
    { "destroy", "", _sapp_zwp_pointer_constraints_request_types + 0 },
    { "lock_pointer", "noo?ou", _sapp_zwp_pointer_constraints_request_types + 1 },
    { "confine_pointer", "noo?ou", _sapp_zwp_pointer_constraints_request_types + 6 },
};
static const struct wl_message _sapp_zwp_locked_pointer_requests[] = {
    { "destroy", "", _sapp_zwp_locked_pointer_request_types + 0 },
    { "set_cursor_position_hint", "ff", _sapp_zwp_locked_pointer_request_types + 0 },
    { "set_region", "?o", _sapp_zwp_locked_pointer_request_types + 0 },
};
static const struct wl_message _sapp_zwp_locked_pointer_events[] = {
    { "locked", "", _sapp_zwp_locked_pointer_request_types + 0 },
    { "unlocked", "", _sapp_zwp_locked_pointer_request_types + 0 },
};
static const struct wl_message _sapp_zwp_relative_pointer_manager_requests[] = {
    { "destroy", "", _sapp_zwp_relative_pointer_manager_request_types + 0 },
    { "get_relative_pointer", "no", _sapp_zwp_relative_pointer_manager_request_types + 1 },
};
static const struct wl_message _sapp_zwp_relative_pointer_requests[] = {
    { "destroy", "", _sapp_zwp_relative_pointer_request_types + 0 },
};
static const struct wl_message _sapp_zwp_relative_pointer_events[] = {
    { "relative_motion", "uuffff", _sapp_zwp_relative_pointer_request_types + 0 },
};

static const struct wl_interface xdg_wm_base_interface = {
    "xdg_wm_base", 1, 4, _sapp_xdg_wm_base_requests, 1, _sapp_xdg_wm_base_events
};
static const struct wl_interface xdg_positioner_interface = {
    "xdg_positioner", 1, 0, NULL, 0, NULL
};
static const struct wl_interface xdg_surface_interface = {
    "xdg_surface", 1, 5, _sapp_xdg_surface_requests, 1, _sapp_xdg_surface_events
};
static const struct wl_interface xdg_toplevel_interface = {
    "xdg_toplevel", 5, 14, _sapp_xdg_toplevel_requests, 4, _sapp_xdg_toplevel_events
};
static const struct wl_interface xdg_popup_interface = {
    "xdg_popup", 1, 0, NULL, 0, NULL
};
static const struct wl_interface zwp_pointer_constraints_v1_interface = {
    "zwp_pointer_constraints_v1", 1, 3, _sapp_zwp_pointer_constraints_requests, 0, NULL
};
static const struct wl_interface zwp_locked_pointer_v1_interface = {
    "zwp_locked_pointer_v1", 1, 3, _sapp_zwp_locked_pointer_requests, 2, _sapp_zwp_locked_pointer_events
};
static const struct wl_interface zwp_confined_pointer_v1_interface = {
    "zwp_confined_pointer_v1", 1, 0, NULL, 0, NULL
};
static const struct wl_interface zwp_relative_pointer_manager_v1_interface = {
    "zwp_relative_pointer_manager_v1", 1, 2, _sapp_zwp_relative_pointer_manager_requests, 0, NULL
};
static const struct wl_interface zwp_relative_pointer_v1_interface = {
    "zwp_relative_pointer_v1", 1, 1, _sapp_zwp_relative_pointer_requests, 1, _sapp_zwp_relative_pointer_events
};

typedef void (*_sapp_xdg_wm_base_ping_func)(void*, struct xdg_wm_base*, uint32_t);
typedef void (*_sapp_xdg_surface_configure_func)(void*, struct xdg_surface*, uint32_t);
typedef void (*_sapp_xdg_toplevel_configure_func)(void*, struct xdg_toplevel*, int32_t, int32_t, struct wl_array*);
typedef void (*_sapp_xdg_toplevel_close_func)(void*, struct xdg_toplevel*);
typedef void (*_sapp_xdg_toplevel_configure_bounds_func)(void*, struct xdg_toplevel*, int32_t, int32_t);
typedef void (*_sapp_xdg_toplevel_wm_capabilities_func)(void*, struct xdg_toplevel*, struct wl_array*);
struct _sapp_xdg_wm_base_listener { _sapp_xdg_wm_base_ping_func ping; };
struct _sapp_xdg_surface_listener { _sapp_xdg_surface_configure_func configure; };
struct _sapp_xdg_toplevel_listener {
    _sapp_xdg_toplevel_configure_func configure;
    _sapp_xdg_toplevel_close_func close;
    _sapp_xdg_toplevel_configure_bounds_func configure_bounds;
    _sapp_xdg_toplevel_wm_capabilities_func wm_capabilities;
};
typedef void (*_sapp_zwp_locked_pointer_locked_func)(void*, struct zwp_locked_pointer_v1*);
typedef void (*_sapp_zwp_locked_pointer_unlocked_func)(void*, struct zwp_locked_pointer_v1*);
typedef void (*_sapp_zwp_relative_pointer_motion_func)(void*, struct zwp_relative_pointer_v1*, uint32_t, uint32_t, wl_fixed_t, wl_fixed_t, wl_fixed_t, wl_fixed_t);
struct _sapp_zwp_locked_pointer_listener {
    _sapp_zwp_locked_pointer_locked_func locked;
    _sapp_zwp_locked_pointer_unlocked_func unlocked;
};
struct _sapp_zwp_relative_pointer_listener {
    _sapp_zwp_relative_pointer_motion_func relative_motion;
};

_SOKOL_PRIVATE void _sapp_xdg_wm_base_pong(struct xdg_wm_base* wm_base, uint32_t serial) {
    wl_proxy_marshal_flags((struct wl_proxy*) wm_base, XDG_WM_BASE_PONG, NULL, wl_proxy_get_version((struct wl_proxy*) wm_base), 0, serial);
}

_SOKOL_PRIVATE struct xdg_surface* _sapp_xdg_wm_base_get_xdg_surface(struct xdg_wm_base* wm_base, struct wl_surface* surface) {
    return (struct xdg_surface*) wl_proxy_marshal_flags((struct wl_proxy*) wm_base, XDG_WM_BASE_GET_XDG_SURFACE,
        &xdg_surface_interface, wl_proxy_get_version((struct wl_proxy*) wm_base), 0, NULL, surface);
}

_SOKOL_PRIVATE struct xdg_toplevel* _sapp_xdg_surface_get_toplevel(struct xdg_surface* surface) {
    return (struct xdg_toplevel*) wl_proxy_marshal_flags((struct wl_proxy*) surface, XDG_SURFACE_GET_TOPLEVEL,
        &xdg_toplevel_interface, wl_proxy_get_version((struct wl_proxy*) surface), 0, NULL);
}

_SOKOL_PRIVATE void _sapp_xdg_surface_ack_configure(struct xdg_surface* surface, uint32_t serial) {
    wl_proxy_marshal_flags((struct wl_proxy*) surface, XDG_SURFACE_ACK_CONFIGURE, NULL, wl_proxy_get_version((struct wl_proxy*) surface), 0, serial);
}

_SOKOL_PRIVATE void _sapp_xdg_toplevel_set_title(struct xdg_toplevel* toplevel, const char* title) {
    wl_proxy_marshal_flags((struct wl_proxy*) toplevel, XDG_TOPLEVEL_SET_TITLE, NULL, wl_proxy_get_version((struct wl_proxy*) toplevel), 0, title);
}

_SOKOL_PRIVATE void _sapp_xdg_toplevel_set_fullscreen(struct xdg_toplevel* toplevel) {
    wl_proxy_marshal_flags((struct wl_proxy*) toplevel, XDG_TOPLEVEL_SET_FULLSCREEN, NULL, wl_proxy_get_version((struct wl_proxy*) toplevel), 0, NULL);
}

_SOKOL_PRIVATE void _sapp_xdg_toplevel_unset_fullscreen(struct xdg_toplevel* toplevel) {
    wl_proxy_marshal_flags((struct wl_proxy*) toplevel, XDG_TOPLEVEL_UNSET_FULLSCREEN, NULL, wl_proxy_get_version((struct wl_proxy*) toplevel), 0);
}

_SOKOL_PRIVATE void _sapp_xdg_proxy_destroy(struct wl_proxy* proxy, uint32_t opcode) {
    if (proxy) {
        wl_proxy_marshal_flags(proxy, opcode, NULL, wl_proxy_get_version(proxy), WL_MARSHAL_FLAG_DESTROY);
    }
}

_SOKOL_PRIVATE struct zwp_locked_pointer_v1* _sapp_zwp_pointer_constraints_lock_pointer(struct zwp_pointer_constraints_v1* pointer_constraints, struct wl_surface* surface, struct wl_pointer* pointer) {
    return (struct zwp_locked_pointer_v1*) wl_proxy_marshal_flags((struct wl_proxy*) pointer_constraints, ZWP_POINTER_CONSTRAINTS_LOCK_POINTER,
        &zwp_locked_pointer_v1_interface, wl_proxy_get_version((struct wl_proxy*) pointer_constraints), 0,
        NULL, surface, pointer, NULL, ZWP_POINTER_CONSTRAINTS_LIFETIME_PERSISTENT);
}

_SOKOL_PRIVATE struct zwp_relative_pointer_v1* _sapp_zwp_relative_pointer_manager_get_relative_pointer(struct zwp_relative_pointer_manager_v1* manager, struct wl_pointer* pointer) {
    return (struct zwp_relative_pointer_v1*) wl_proxy_marshal_flags((struct wl_proxy*) manager, ZWP_RELATIVE_POINTER_MANAGER_GET_RELATIVE_POINTER,
        &zwp_relative_pointer_v1_interface, wl_proxy_get_version((struct wl_proxy*) manager), 0, NULL, pointer);
}

_SOKOL_PRIVATE bool _sapp_wayland_load_libdecor(void) {
    _sapp.wayland.libdecor_so = dlopen("libdecor-0.so.0", RTLD_LAZY | RTLD_LOCAL);
    if (!_sapp.wayland.libdecor_so) {
        return false;
    }
    #define _SAPP_WAYLAND_LOAD_LIBDECOR_SYM(field, sym) \
        do { \
            _sapp_libdecor.field = (void*) dlsym(_sapp.wayland.libdecor_so, sym); \
            if (!_sapp_libdecor.field) { \
                dlclose(_sapp.wayland.libdecor_so); \
                _sapp.wayland.libdecor_so = 0; \
                return false; \
            } \
        } while (0)
    _SAPP_WAYLAND_LOAD_LIBDECOR_SYM(new_, "libdecor_new");
    _SAPP_WAYLAND_LOAD_LIBDECOR_SYM(unref, "libdecor_unref");
    _SAPP_WAYLAND_LOAD_LIBDECOR_SYM(dispatch, "libdecor_dispatch");
    _SAPP_WAYLAND_LOAD_LIBDECOR_SYM(decorate, "libdecor_decorate");
    _SAPP_WAYLAND_LOAD_LIBDECOR_SYM(frame_unref, "libdecor_frame_unref");
    _SAPP_WAYLAND_LOAD_LIBDECOR_SYM(frame_map, "libdecor_frame_map");
    _SAPP_WAYLAND_LOAD_LIBDECOR_SYM(frame_set_title, "libdecor_frame_set_title");
    _SAPP_WAYLAND_LOAD_LIBDECOR_SYM(frame_set_app_id, "libdecor_frame_set_app_id");
    _SAPP_WAYLAND_LOAD_LIBDECOR_SYM(frame_commit, "libdecor_frame_commit");
    _SAPP_WAYLAND_LOAD_LIBDECOR_SYM(frame_set_fullscreen, "libdecor_frame_set_fullscreen");
    _SAPP_WAYLAND_LOAD_LIBDECOR_SYM(frame_unset_fullscreen, "libdecor_frame_unset_fullscreen");
    _SAPP_WAYLAND_LOAD_LIBDECOR_SYM(state_new, "libdecor_state_new");
    _SAPP_WAYLAND_LOAD_LIBDECOR_SYM(state_free, "libdecor_state_free");
    _SAPP_WAYLAND_LOAD_LIBDECOR_SYM(configuration_get_content_size, "libdecor_configuration_get_content_size");
    _SAPP_WAYLAND_LOAD_LIBDECOR_SYM(configuration_get_window_state, "libdecor_configuration_get_window_state");
    #undef _SAPP_WAYLAND_LOAD_LIBDECOR_SYM
    return true;
}

_SOKOL_PRIVATE bool _sapp_wayland_load_cursor_lib(void) {
    _sapp.wayland.wayland_cursor_so = dlopen("libwayland-cursor.so.0", RTLD_LAZY | RTLD_LOCAL);
    if (!_sapp.wayland.wayland_cursor_so) {
        return false;
    }
    #define _SAPP_WAYLAND_LOAD_CURSOR_SYM(field, sym) \
        do { \
            _sapp_wayland_cursor.field = (void*) dlsym(_sapp.wayland.wayland_cursor_so, sym); \
            if (!_sapp_wayland_cursor.field) { \
                dlclose(_sapp.wayland.wayland_cursor_so); \
                _sapp.wayland.wayland_cursor_so = 0; \
                return false; \
            } \
        } while (0)
    _SAPP_WAYLAND_LOAD_CURSOR_SYM(theme_load, "wl_cursor_theme_load");
    _SAPP_WAYLAND_LOAD_CURSOR_SYM(theme_destroy, "wl_cursor_theme_destroy");
    _SAPP_WAYLAND_LOAD_CURSOR_SYM(theme_get_cursor, "wl_cursor_theme_get_cursor");
    _SAPP_WAYLAND_LOAD_CURSOR_SYM(image_get_buffer, "wl_cursor_image_get_buffer");
    #undef _SAPP_WAYLAND_LOAD_CURSOR_SYM
    return true;
}

_SOKOL_PRIVATE void _sapp_wayland_init_cursor(void) {
    if (!_sapp.wayland.shm || !_sapp.wayland.compositor || !_sapp_wayland_load_cursor_lib()) {
        return;
    }
    int cursor_size = (int)(24.0f * _sapp.dpi_scale);
    if (cursor_size < 24) {
        cursor_size = 24;
    }
    _sapp.wayland.cursor_theme = _sapp_wayland_cursor.theme_load(0, cursor_size, _sapp.wayland.shm);
    if (!_sapp.wayland.cursor_theme) {
        return;
    }
    _sapp.wayland.default_cursor = _sapp_wayland_cursor.theme_get_cursor(_sapp.wayland.cursor_theme, "left_ptr");
    if (!_sapp.wayland.default_cursor) {
        _sapp.wayland.default_cursor = _sapp_wayland_cursor.theme_get_cursor(_sapp.wayland.cursor_theme, "default");
    }
    _sapp.wayland.cursor_surface = wl_compositor_create_surface(_sapp.wayland.compositor);
    char tmp[] = "/tmp/sokol-wayland-cursor-XXXXXX";
    int fd = mkstemp(tmp);
    if (fd >= 0) {
        unlink(tmp);
        const uint32_t transparent = 0;
        if ((ftruncate(fd, 4) == 0) && (write(fd, &transparent, 4) == 4)) {
            struct wl_shm_pool* pool = wl_shm_create_pool(_sapp.wayland.shm, fd, 4);
            if (pool) {
                _sapp.wayland.hidden_cursor_buffer = wl_shm_pool_create_buffer(pool, 0, 1, 1, 4, WL_SHM_FORMAT_ARGB8888);
                wl_shm_pool_destroy(pool);
            }
        }
        close(fd);
    }
}

_SOKOL_PRIVATE void _sapp_wayland_apply_cursor(bool shown) {
    if (!_sapp.wayland.pointer || !_sapp.wayland.pointer_serial_valid) {
        return;
    }
    if (!shown) {
        if (_sapp.wayland.hidden_cursor_buffer && _sapp.wayland.cursor_surface) {
            wl_pointer_set_cursor(_sapp.wayland.pointer, _sapp.wayland.pointer_serial, _sapp.wayland.cursor_surface, 0, 0);
            wl_surface_attach(_sapp.wayland.cursor_surface, _sapp.wayland.hidden_cursor_buffer, 0, 0);
            wl_surface_damage_buffer(_sapp.wayland.cursor_surface, 0, 0, 1, 1);
            wl_surface_commit(_sapp.wayland.cursor_surface);
        } else {
            wl_pointer_set_cursor(_sapp.wayland.pointer, _sapp.wayland.pointer_serial, 0, 0, 0);
        }
        wl_display_flush(_sapp.wayland.display);
        return;
    }
    if (!_sapp.wayland.default_cursor || !_sapp.wayland.cursor_surface || (_sapp.wayland.default_cursor->image_count == 0)) {
        return;
    }
    struct wl_cursor_image* image = _sapp.wayland.default_cursor->images[0];
    struct wl_buffer* buffer = _sapp_wayland_cursor.image_get_buffer(image);
    if (!buffer) {
        return;
    }
    wl_pointer_set_cursor(_sapp.wayland.pointer, _sapp.wayland.pointer_serial, _sapp.wayland.cursor_surface, (int32_t)image->hotspot_x, (int32_t)image->hotspot_y);
    wl_surface_attach(_sapp.wayland.cursor_surface, buffer, 0, 0);
    wl_surface_damage_buffer(_sapp.wayland.cursor_surface, 0, 0, (int32_t)image->width, (int32_t)image->height);
    wl_surface_commit(_sapp.wayland.cursor_surface);
    wl_display_flush(_sapp.wayland.display);
}

_SOKOL_PRIVATE void _sapp_wayland_app_event(sapp_event_type type) {
    _sapp_init_event(type);
    _sapp_call_event(&_sapp.event);
}

_SOKOL_PRIVATE void _sapp_wayland_resize(int32_t width, int32_t height) {
    if (width <= 0) {
        width = _sapp.window_width;
    }
    if (height <= 0) {
        height = _sapp.window_height;
    }
    if ((width != _sapp.window_width) || (height != _sapp.window_height)) {
        _sapp.window_width = width;
        _sapp.window_height = height;
        _sapp.framebuffer_width = width;
        _sapp.framebuffer_height = height;
        if (_sapp.wayland.egl_window) {
            wl_egl_window_resize(_sapp.wayland.egl_window, width, height, 0, 0);
        }
        if (_sapp.valid) {
            _sapp_wayland_app_event(SAPP_EVENTTYPE_RESIZED);
        }
    }
}

_SOKOL_PRIVATE void _sapp_wayland_xdg_ping(void* data, struct xdg_wm_base* wm_base, uint32_t serial) {
    _SOKOL_UNUSED(data);
    _sapp_xdg_wm_base_pong(wm_base, serial);
}

_SOKOL_PRIVATE void _sapp_wayland_xdg_surface_configure(void* data, struct xdg_surface* surface, uint32_t serial) {
    _SOKOL_UNUSED(data);
    _sapp_xdg_surface_ack_configure(surface, serial);
    _sapp.wayland.configured = true;
}

_SOKOL_PRIVATE void _sapp_wayland_xdg_toplevel_configure(void* data, struct xdg_toplevel* toplevel, int32_t width, int32_t height, struct wl_array* states) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(toplevel); _SOKOL_UNUSED(states);
    _sapp_wayland_resize(width, height);
}

_SOKOL_PRIVATE void _sapp_wayland_xdg_toplevel_close(void* data, struct xdg_toplevel* toplevel) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(toplevel);
    _sapp.quit_requested = true;
}

_SOKOL_PRIVATE void _sapp_wayland_xdg_toplevel_configure_bounds(void* data, struct xdg_toplevel* toplevel, int32_t width, int32_t height) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(toplevel); _SOKOL_UNUSED(width); _SOKOL_UNUSED(height);
}

_SOKOL_PRIVATE void _sapp_wayland_xdg_toplevel_wm_capabilities(void* data, struct xdg_toplevel* toplevel, struct wl_array* capabilities) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(toplevel); _SOKOL_UNUSED(capabilities);
}

_SOKOL_PRIVATE void _sapp_wayland_libdecor_error(struct libdecor* context, _sapp_libdecor_error error, const char* message) {
    _SOKOL_UNUSED(context); _SOKOL_UNUSED(error); _SOKOL_UNUSED(message);
}

_SOKOL_PRIVATE void _sapp_wayland_libdecor_configure(struct libdecor_frame* frame, struct libdecor_configuration* configuration, void* user_data) {
    _SOKOL_UNUSED(user_data);
    _sapp.wayland.configured = true;
    int width = _sapp.window_width;
    int height = _sapp.window_height;
    if (!_sapp_libdecor.configuration_get_content_size(configuration, frame, &width, &height)) {
        width = _sapp.window_width;
        height = _sapp.window_height;
    }
    _sapp_wayland_resize(width, height);
    _sapp_libdecor_window_state window_state = 0;
    if (_sapp_libdecor.configuration_get_window_state(configuration, &window_state)) {
        _sapp.fullscreen = (0 != (window_state & _SAPP_LIBDECOR_WINDOW_STATE_FULLSCREEN));
    }
    struct libdecor_state* state = _sapp_libdecor.state_new(_sapp.window_width, _sapp.window_height);
    if (state) {
        _sapp_libdecor.frame_commit(frame, state, configuration);
        _sapp_libdecor.state_free(state);
    }
}

_SOKOL_PRIVATE void _sapp_wayland_libdecor_close(struct libdecor_frame* frame, void* user_data) {
    _SOKOL_UNUSED(frame); _SOKOL_UNUSED(user_data);
    _sapp.quit_requested = true;
}

_SOKOL_PRIVATE void _sapp_wayland_libdecor_commit(struct libdecor_frame* frame, void* user_data) {
    _SOKOL_UNUSED(frame); _SOKOL_UNUSED(user_data);
    if (_sapp.wayland.surface) {
        wl_surface_commit(_sapp.wayland.surface);
    }
}

_SOKOL_PRIVATE void _sapp_wayland_libdecor_dismiss_popup(struct libdecor_frame* frame, const char* seat_name, void* user_data) {
    _SOKOL_UNUSED(frame); _SOKOL_UNUSED(seat_name); _SOKOL_UNUSED(user_data);
}

_SOKOL_PRIVATE _sapp_libdecor_interface _sapp_wayland_libdecor_interface = {
    _sapp_wayland_libdecor_error,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

_SOKOL_PRIVATE _sapp_libdecor_frame_interface _sapp_wayland_libdecor_frame_interface = {
    _sapp_wayland_libdecor_configure,
    _sapp_wayland_libdecor_close,
    _sapp_wayland_libdecor_commit,
    _sapp_wayland_libdecor_dismiss_popup,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

_SOKOL_PRIVATE const struct _sapp_xdg_wm_base_listener _sapp_wayland_wm_base_listener = {
    _sapp_wayland_xdg_ping
};
_SOKOL_PRIVATE const struct _sapp_xdg_surface_listener _sapp_wayland_xdg_surface_listener = {
    _sapp_wayland_xdg_surface_configure
};
_SOKOL_PRIVATE const struct _sapp_xdg_toplevel_listener _sapp_wayland_xdg_toplevel_listener = {
    _sapp_wayland_xdg_toplevel_configure,
    _sapp_wayland_xdg_toplevel_close,
    _sapp_wayland_xdg_toplevel_configure_bounds,
    _sapp_wayland_xdg_toplevel_wm_capabilities
};

_SOKOL_PRIVATE uint32_t _sapp_wayland_button_modifier_bit(sapp_mousebutton btn) {
    switch (btn) {
        case SAPP_MOUSEBUTTON_LEFT: return SAPP_MODIFIER_LMB;
        case SAPP_MOUSEBUTTON_RIGHT: return SAPP_MODIFIER_RMB;
        case SAPP_MOUSEBUTTON_MIDDLE: return SAPP_MODIFIER_MMB;
        default: return 0;
    }
}

_SOKOL_PRIVATE uint32_t _sapp_wayland_key_modifier_bit(sapp_keycode key) {
    switch (key) {
        case SAPP_KEYCODE_LEFT_SHIFT:
        case SAPP_KEYCODE_RIGHT_SHIFT:
            return SAPP_MODIFIER_SHIFT;
        case SAPP_KEYCODE_LEFT_CONTROL:
        case SAPP_KEYCODE_RIGHT_CONTROL:
            return SAPP_MODIFIER_CTRL;
        case SAPP_KEYCODE_LEFT_ALT:
        case SAPP_KEYCODE_RIGHT_ALT:
            return SAPP_MODIFIER_ALT;
        case SAPP_KEYCODE_LEFT_SUPER:
        case SAPP_KEYCODE_RIGHT_SUPER:
            return SAPP_MODIFIER_SUPER;
        default:
            return 0;
    }
}

_SOKOL_PRIVATE uint32_t _sapp_wayland_mods(void) {
    uint32_t mods = _sapp.wayland.modifiers;
    if (_sapp.wayland.mouse_buttons & (1u << SAPP_MOUSEBUTTON_LEFT)) {
        mods |= SAPP_MODIFIER_LMB;
    }
    if (_sapp.wayland.mouse_buttons & (1u << SAPP_MOUSEBUTTON_RIGHT)) {
        mods |= SAPP_MODIFIER_RMB;
    }
    if (_sapp.wayland.mouse_buttons & (1u << SAPP_MOUSEBUTTON_MIDDLE)) {
        mods |= SAPP_MODIFIER_MMB;
    }
    return mods;
}

_SOKOL_PRIVATE void _sapp_wayland_mouse_update(float x, float y, bool clear_dxdy) {
    if (!_sapp.mouse.locked) {
        if (clear_dxdy) {
            _sapp.mouse.dx = 0.0f;
            _sapp.mouse.dy = 0.0f;
        } else if (_sapp.mouse.pos_valid) {
            _sapp.mouse.dx = x - _sapp.mouse.x;
            _sapp.mouse.dy = y - _sapp.mouse.y;
        }
        _sapp.mouse.x = x;
        _sapp.mouse.y = y;
        _sapp.mouse.pos_valid = true;
    }
}

_SOKOL_PRIVATE void _sapp_wayland_mouse_event(sapp_event_type type, sapp_mousebutton btn) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        _sapp.event.mouse_button = btn;
        _sapp.event.modifiers = _sapp_wayland_mods();
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE void _sapp_wayland_scroll_event(float x, float y) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(SAPP_EVENTTYPE_MOUSE_SCROLL);
        _sapp.event.modifiers = _sapp_wayland_mods();
        _sapp.event.scroll_x = x;
        _sapp.event.scroll_y = y;
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE void _sapp_wayland_key_event(sapp_event_type type, sapp_keycode key, bool repeat) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        _sapp.event.key_code = key;
        _sapp.event.key_repeat = repeat;
        _sapp.event.modifiers = _sapp_wayland_mods();
        if (type == SAPP_EVENTTYPE_KEY_DOWN) {
            _sapp.event.modifiers |= _sapp_wayland_key_modifier_bit(key);
        } else if (type == SAPP_EVENTTYPE_KEY_UP) {
            _sapp.event.modifiers &= ~_sapp_wayland_key_modifier_bit(key);
        }
        _sapp_call_event(&_sapp.event);
        if (_sapp.clipboard.enabled &&
            (type == SAPP_EVENTTYPE_KEY_DOWN) &&
            (_sapp.event.modifiers == SAPP_MODIFIER_CTRL) &&
            (_sapp.event.key_code == SAPP_KEYCODE_V))
        {
            _sapp_init_event(SAPP_EVENTTYPE_CLIPBOARD_PASTED);
            _sapp_call_event(&_sapp.event);
        }
    }
}

_SOKOL_PRIVATE void _sapp_wayland_char_event(uint32_t chr, bool repeat) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(SAPP_EVENTTYPE_CHAR);
        _sapp.event.char_code = chr;
        _sapp.event.key_repeat = repeat;
        _sapp.event.modifiers = _sapp_wayland_mods();
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE sapp_keycode _sapp_wayland_translate_key(uint32_t key) {
    switch (key) {
        case 1: return SAPP_KEYCODE_ESCAPE;
        case 2: return SAPP_KEYCODE_1;
        case 3: return SAPP_KEYCODE_2;
        case 4: return SAPP_KEYCODE_3;
        case 5: return SAPP_KEYCODE_4;
        case 6: return SAPP_KEYCODE_5;
        case 7: return SAPP_KEYCODE_6;
        case 8: return SAPP_KEYCODE_7;
        case 9: return SAPP_KEYCODE_8;
        case 10: return SAPP_KEYCODE_9;
        case 11: return SAPP_KEYCODE_0;
        case 12: return SAPP_KEYCODE_MINUS;
        case 13: return SAPP_KEYCODE_EQUAL;
        case 14: return SAPP_KEYCODE_BACKSPACE;
        case 15: return SAPP_KEYCODE_TAB;
        case 16: return SAPP_KEYCODE_Q;
        case 17: return SAPP_KEYCODE_W;
        case 18: return SAPP_KEYCODE_E;
        case 19: return SAPP_KEYCODE_R;
        case 20: return SAPP_KEYCODE_T;
        case 21: return SAPP_KEYCODE_Y;
        case 22: return SAPP_KEYCODE_U;
        case 23: return SAPP_KEYCODE_I;
        case 24: return SAPP_KEYCODE_O;
        case 25: return SAPP_KEYCODE_P;
        case 26: return SAPP_KEYCODE_LEFT_BRACKET;
        case 27: return SAPP_KEYCODE_RIGHT_BRACKET;
        case 28: return SAPP_KEYCODE_ENTER;
        case 29: return SAPP_KEYCODE_LEFT_CONTROL;
        case 30: return SAPP_KEYCODE_A;
        case 31: return SAPP_KEYCODE_S;
        case 32: return SAPP_KEYCODE_D;
        case 33: return SAPP_KEYCODE_F;
        case 34: return SAPP_KEYCODE_G;
        case 35: return SAPP_KEYCODE_H;
        case 36: return SAPP_KEYCODE_J;
        case 37: return SAPP_KEYCODE_K;
        case 38: return SAPP_KEYCODE_L;
        case 39: return SAPP_KEYCODE_SEMICOLON;
        case 40: return SAPP_KEYCODE_APOSTROPHE;
        case 41: return SAPP_KEYCODE_GRAVE_ACCENT;
        case 42: return SAPP_KEYCODE_LEFT_SHIFT;
        case 43: return SAPP_KEYCODE_BACKSLASH;
        case 44: return SAPP_KEYCODE_Z;
        case 45: return SAPP_KEYCODE_X;
        case 46: return SAPP_KEYCODE_C;
        case 47: return SAPP_KEYCODE_V;
        case 48: return SAPP_KEYCODE_B;
        case 49: return SAPP_KEYCODE_N;
        case 50: return SAPP_KEYCODE_M;
        case 51: return SAPP_KEYCODE_COMMA;
        case 52: return SAPP_KEYCODE_PERIOD;
        case 53: return SAPP_KEYCODE_SLASH;
        case 54: return SAPP_KEYCODE_RIGHT_SHIFT;
        case 55: return SAPP_KEYCODE_KP_MULTIPLY;
        case 56: return SAPP_KEYCODE_LEFT_ALT;
        case 57: return SAPP_KEYCODE_SPACE;
        case 58: return SAPP_KEYCODE_CAPS_LOCK;
        case 59: return SAPP_KEYCODE_F1;
        case 60: return SAPP_KEYCODE_F2;
        case 61: return SAPP_KEYCODE_F3;
        case 62: return SAPP_KEYCODE_F4;
        case 63: return SAPP_KEYCODE_F5;
        case 64: return SAPP_KEYCODE_F6;
        case 65: return SAPP_KEYCODE_F7;
        case 66: return SAPP_KEYCODE_F8;
        case 67: return SAPP_KEYCODE_F9;
        case 68: return SAPP_KEYCODE_F10;
        case 69: return SAPP_KEYCODE_NUM_LOCK;
        case 70: return SAPP_KEYCODE_SCROLL_LOCK;
        case 71: return SAPP_KEYCODE_KP_7;
        case 72: return SAPP_KEYCODE_KP_8;
        case 73: return SAPP_KEYCODE_KP_9;
        case 74: return SAPP_KEYCODE_KP_SUBTRACT;
        case 75: return SAPP_KEYCODE_KP_4;
        case 76: return SAPP_KEYCODE_KP_5;
        case 77: return SAPP_KEYCODE_KP_6;
        case 78: return SAPP_KEYCODE_KP_ADD;
        case 79: return SAPP_KEYCODE_KP_1;
        case 80: return SAPP_KEYCODE_KP_2;
        case 81: return SAPP_KEYCODE_KP_3;
        case 82: return SAPP_KEYCODE_KP_0;
        case 83: return SAPP_KEYCODE_KP_DECIMAL;
        case 87: return SAPP_KEYCODE_F11;
        case 88: return SAPP_KEYCODE_F12;
        case 96: return SAPP_KEYCODE_KP_ENTER;
        case 97: return SAPP_KEYCODE_RIGHT_CONTROL;
        case 98: return SAPP_KEYCODE_KP_DIVIDE;
        case 99: return SAPP_KEYCODE_PRINT_SCREEN;
        case 100: return SAPP_KEYCODE_RIGHT_ALT;
        case 102: return SAPP_KEYCODE_HOME;
        case 103: return SAPP_KEYCODE_UP;
        case 104: return SAPP_KEYCODE_PAGE_UP;
        case 105: return SAPP_KEYCODE_LEFT;
        case 106: return SAPP_KEYCODE_RIGHT;
        case 107: return SAPP_KEYCODE_END;
        case 108: return SAPP_KEYCODE_DOWN;
        case 109: return SAPP_KEYCODE_PAGE_DOWN;
        case 110: return SAPP_KEYCODE_INSERT;
        case 111: return SAPP_KEYCODE_DELETE;
        case 119: return SAPP_KEYCODE_PAUSE;
        case 125: return SAPP_KEYCODE_LEFT_SUPER;
        case 126: return SAPP_KEYCODE_RIGHT_SUPER;
        case 127: return SAPP_KEYCODE_MENU;
        default: return SAPP_KEYCODE_INVALID;
    }
}

_SOKOL_PRIVATE sapp_mousebutton _sapp_wayland_translate_button(uint32_t button) {
    switch (button) {
        case 0x110: return SAPP_MOUSEBUTTON_LEFT;
        case 0x111: return SAPP_MOUSEBUTTON_RIGHT;
        case 0x112: return SAPP_MOUSEBUTTON_MIDDLE;
        default: return SAPP_MOUSEBUTTON_INVALID;
    }
}

_SOKOL_PRIVATE void _sapp_wayland_pointer_enter(void* data, struct wl_pointer* pointer, uint32_t serial, struct wl_surface* surface, wl_fixed_t sx, wl_fixed_t sy) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(pointer);
    _sapp.wayland.pointer_serial = serial;
    _sapp.wayland.pointer_serial_valid = true;
    _sapp.wayland.pointer_focused = (surface == _sapp.wayland.surface);
    if (_sapp.wayland.pointer_focused) {
        _sapp_wayland_mouse_update((float)wl_fixed_to_double(sx), (float)wl_fixed_to_double(sy), true);
        _sapp_wayland_apply_cursor(_sapp.mouse.shown);
        _sapp_wayland_mouse_event(SAPP_EVENTTYPE_MOUSE_ENTER, SAPP_MOUSEBUTTON_INVALID);
    }
}

_SOKOL_PRIVATE void _sapp_wayland_pointer_leave(void* data, struct wl_pointer* pointer, uint32_t serial, struct wl_surface* surface) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(pointer); _SOKOL_UNUSED(serial); _SOKOL_UNUSED(surface);
    if (_sapp.wayland.pointer_focused) {
        _sapp_wayland_mouse_event(SAPP_EVENTTYPE_MOUSE_LEAVE, SAPP_MOUSEBUTTON_INVALID);
    }
    _sapp.wayland.pointer_focused = false;
    _sapp.wayland.pointer_serial_valid = false;
    _sapp.mouse.pos_valid = false;
}

_SOKOL_PRIVATE void _sapp_wayland_pointer_motion(void* data, struct wl_pointer* pointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(pointer); _SOKOL_UNUSED(time);
    if (_sapp.wayland.pointer_focused && !_sapp.mouse.locked) {
        _sapp_wayland_mouse_update((float)wl_fixed_to_double(sx), (float)wl_fixed_to_double(sy), false);
        _sapp_wayland_mouse_event(SAPP_EVENTTYPE_MOUSE_MOVE, SAPP_MOUSEBUTTON_INVALID);
    }
}

_SOKOL_PRIVATE void _sapp_wayland_pointer_button(void* data, struct wl_pointer* pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(pointer); _SOKOL_UNUSED(serial); _SOKOL_UNUSED(time);
    if (_sapp.wayland.pointer_focused) {
        const sapp_mousebutton btn = _sapp_wayland_translate_button(button);
        if (btn != SAPP_MOUSEBUTTON_INVALID) {
            if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
                _sapp.wayland.mouse_buttons |= (1u << btn);
                _sapp_wayland_mouse_event(SAPP_EVENTTYPE_MOUSE_DOWN, btn);
            } else {
                _sapp.wayland.mouse_buttons &= ~(1u << btn);
                _sapp_wayland_mouse_event(SAPP_EVENTTYPE_MOUSE_UP, btn);
            }
        }
    }
}

_SOKOL_PRIVATE void _sapp_wayland_pointer_axis(void* data, struct wl_pointer* pointer, uint32_t time, uint32_t axis, wl_fixed_t value) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(pointer); _SOKOL_UNUSED(time);
    if (_sapp.wayland.pointer_focused) {
        const float scroll = (float)wl_fixed_to_double(value) / 10.0f;
        if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL) {
            _sapp_wayland_scroll_event(0.0f, -scroll);
        } else if (axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL) {
            _sapp_wayland_scroll_event(scroll, 0.0f);
        }
    }
}

_SOKOL_PRIVATE void _sapp_wayland_pointer_frame(void* data, struct wl_pointer* pointer) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(pointer);
}

_SOKOL_PRIVATE void _sapp_wayland_pointer_axis_source(void* data, struct wl_pointer* pointer, uint32_t axis_source) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(pointer); _SOKOL_UNUSED(axis_source);
}

_SOKOL_PRIVATE void _sapp_wayland_pointer_axis_stop(void* data, struct wl_pointer* pointer, uint32_t time, uint32_t axis) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(pointer); _SOKOL_UNUSED(time); _SOKOL_UNUSED(axis);
}

_SOKOL_PRIVATE void _sapp_wayland_pointer_axis_discrete(void* data, struct wl_pointer* pointer, uint32_t axis, int32_t discrete) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(pointer); _SOKOL_UNUSED(axis); _SOKOL_UNUSED(discrete);
}

_SOKOL_PRIVATE const struct wl_pointer_listener _sapp_wayland_pointer_listener = {
    _sapp_wayland_pointer_enter,
    _sapp_wayland_pointer_leave,
    _sapp_wayland_pointer_motion,
    _sapp_wayland_pointer_button,
    _sapp_wayland_pointer_axis,
    _sapp_wayland_pointer_frame,
    _sapp_wayland_pointer_axis_source,
    _sapp_wayland_pointer_axis_stop,
    _sapp_wayland_pointer_axis_discrete,
};

_SOKOL_PRIVATE void _sapp_wayland_locked_pointer_locked(void* data, struct zwp_locked_pointer_v1* locked_pointer) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(locked_pointer);
    _sapp.mouse.locked = true;
}

_SOKOL_PRIVATE void _sapp_wayland_locked_pointer_unlocked(void* data, struct zwp_locked_pointer_v1* locked_pointer) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(locked_pointer);
    if (_sapp.wayland.locked_pointer) {
        _sapp_xdg_proxy_destroy((struct wl_proxy*) _sapp.wayland.locked_pointer, ZWP_LOCKED_POINTER_DESTROY);
        _sapp.wayland.locked_pointer = 0;
    }
    _sapp.mouse.locked = false;
    _sapp.mouse.dx = 0.0f;
    _sapp.mouse.dy = 0.0f;
    _sapp_wayland_apply_cursor(_sapp.mouse.shown);
}

_SOKOL_PRIVATE const struct _sapp_zwp_locked_pointer_listener _sapp_wayland_locked_pointer_listener = {
    _sapp_wayland_locked_pointer_locked,
    _sapp_wayland_locked_pointer_unlocked,
};

_SOKOL_PRIVATE void _sapp_wayland_relative_pointer_motion(void* data, struct zwp_relative_pointer_v1* relative_pointer, uint32_t utime_hi, uint32_t utime_lo, wl_fixed_t dx, wl_fixed_t dy, wl_fixed_t dx_unaccel, wl_fixed_t dy_unaccel) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(relative_pointer); _SOKOL_UNUSED(utime_hi); _SOKOL_UNUSED(utime_lo); _SOKOL_UNUSED(dx); _SOKOL_UNUSED(dy);
    if (_sapp.mouse.locked && _sapp.wayland.pointer_focused) {
        _sapp.mouse.dx = (float)wl_fixed_to_double(dx_unaccel);
        _sapp.mouse.dy = (float)wl_fixed_to_double(dy_unaccel);
        _sapp_wayland_mouse_event(SAPP_EVENTTYPE_MOUSE_MOVE, SAPP_MOUSEBUTTON_INVALID);
    }
}

_SOKOL_PRIVATE const struct _sapp_zwp_relative_pointer_listener _sapp_wayland_relative_pointer_listener = {
    _sapp_wayland_relative_pointer_motion,
};

_SOKOL_PRIVATE void _sapp_wayland_keyboard_keymap(void* data, struct wl_keyboard* keyboard, uint32_t format, int fd, uint32_t size) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(keyboard);
    if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
        close(fd);
        return;
    }
    char* keymap_str = (char*)mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (keymap_str == MAP_FAILED) {
        return;
    }
    if (_sapp.wayland.xkb_state) {
        xkb_state_unref(_sapp.wayland.xkb_state);
        _sapp.wayland.xkb_state = 0;
    }
    if (_sapp.wayland.xkb_keymap) {
        xkb_keymap_unref(_sapp.wayland.xkb_keymap);
        _sapp.wayland.xkb_keymap = 0;
    }
    _sapp.wayland.xkb_keymap = xkb_keymap_new_from_string(_sapp.wayland.xkb_context, keymap_str, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
    munmap(keymap_str, size);
    if (_sapp.wayland.xkb_keymap) {
        _sapp.wayland.xkb_state = xkb_state_new(_sapp.wayland.xkb_keymap);
        _sapp.wayland.xkb_shift = xkb_keymap_mod_get_index(_sapp.wayland.xkb_keymap, XKB_MOD_NAME_SHIFT);
        _sapp.wayland.xkb_ctrl = xkb_keymap_mod_get_index(_sapp.wayland.xkb_keymap, XKB_MOD_NAME_CTRL);
        _sapp.wayland.xkb_alt = xkb_keymap_mod_get_index(_sapp.wayland.xkb_keymap, XKB_MOD_NAME_ALT);
        _sapp.wayland.xkb_super = xkb_keymap_mod_get_index(_sapp.wayland.xkb_keymap, XKB_MOD_NAME_LOGO);
    }
}

_SOKOL_PRIVATE void _sapp_wayland_keyboard_enter(void* data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface* surface, struct wl_array* keys) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(keyboard); _SOKOL_UNUSED(keys);
    _sapp.wayland.keyboard_serial = serial;
    _sapp.wayland.keyboard_focused = (surface == _sapp.wayland.surface);
    if (_sapp.wayland.keyboard_focused) {
        _sapp_wayland_app_event(SAPP_EVENTTYPE_FOCUSED);
    }
}

_SOKOL_PRIVATE void _sapp_wayland_keyboard_leave(void* data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface* surface) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(keyboard); _SOKOL_UNUSED(serial); _SOKOL_UNUSED(surface);
    if (_sapp.mouse.locked) {
        _sapp_wayland_lock_mouse(false);
    }
    if (_sapp.wayland.keyboard_focused) {
        _sapp_wayland_app_event(SAPP_EVENTTYPE_UNFOCUSED);
    }
    _sapp.wayland.keyboard_focused = false;
}

_SOKOL_PRIVATE void _sapp_wayland_keyboard_key(void* data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(keyboard); _SOKOL_UNUSED(time);
    _sapp.wayland.keyboard_serial = serial;
    if (!_sapp.wayland.keyboard_focused) {
        return;
    }
    const sapp_keycode keycode = _sapp_wayland_translate_key(key);
    if (keycode != SAPP_KEYCODE_INVALID) {
        _sapp_wayland_key_event((state == WL_KEYBOARD_KEY_STATE_PRESSED) ? SAPP_EVENTTYPE_KEY_DOWN : SAPP_EVENTTYPE_KEY_UP, keycode, false);
    }
    if ((state == WL_KEYBOARD_KEY_STATE_PRESSED) && _sapp.wayland.xkb_state) {
        const xkb_keysym_t sym = xkb_state_key_get_one_sym(_sapp.wayland.xkb_state, key + 8);
        const uint32_t chr = xkb_keysym_to_utf32(sym);
        if ((chr >= 32) && (chr != 127)) {
            _sapp_wayland_char_event(chr, false);
        }
    }
}

_SOKOL_PRIVATE uint32_t _sapp_wayland_mod_index_mask(uint32_t index) {
    return (index < 32) ? (1u << index) : 0;
}

_SOKOL_PRIVATE void _sapp_wayland_keyboard_modifiers(void* data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t depressed, uint32_t latched, uint32_t locked, uint32_t group) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(keyboard); _SOKOL_UNUSED(serial);
    const uint32_t active = depressed | latched | locked;
    uint32_t mods = 0;
    if (active & _sapp_wayland_mod_index_mask(_sapp.wayland.xkb_shift)) {
        mods |= SAPP_MODIFIER_SHIFT;
    }
    if (active & _sapp_wayland_mod_index_mask(_sapp.wayland.xkb_ctrl)) {
        mods |= SAPP_MODIFIER_CTRL;
    }
    if (active & _sapp_wayland_mod_index_mask(_sapp.wayland.xkb_alt)) {
        mods |= SAPP_MODIFIER_ALT;
    }
    if (active & _sapp_wayland_mod_index_mask(_sapp.wayland.xkb_super)) {
        mods |= SAPP_MODIFIER_SUPER;
    }
    _sapp.wayland.modifiers = mods;
    if (_sapp.wayland.xkb_state) {
        xkb_state_update_mask(_sapp.wayland.xkb_state, depressed, latched, locked, 0, 0, group);
    }
}

_SOKOL_PRIVATE void _sapp_wayland_keyboard_repeat_info(void* data, struct wl_keyboard* keyboard, int32_t rate, int32_t delay) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(keyboard); _SOKOL_UNUSED(rate); _SOKOL_UNUSED(delay);
}

_SOKOL_PRIVATE const struct wl_keyboard_listener _sapp_wayland_keyboard_listener = {
    _sapp_wayland_keyboard_keymap,
    _sapp_wayland_keyboard_enter,
    _sapp_wayland_keyboard_leave,
    _sapp_wayland_keyboard_key,
    _sapp_wayland_keyboard_modifiers,
    _sapp_wayland_keyboard_repeat_info,
};

_SOKOL_PRIVATE void _sapp_wayland_seat_capabilities(void* data, struct wl_seat* seat, uint32_t capabilities) {
    _SOKOL_UNUSED(data);
    if ((capabilities & WL_SEAT_CAPABILITY_POINTER) && !_sapp.wayland.pointer) {
        _sapp.wayland.pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(_sapp.wayland.pointer, &_sapp_wayland_pointer_listener, 0);
        if (_sapp.wayland.relative_pointer_manager && !_sapp.wayland.relative_pointer) {
            _sapp.wayland.relative_pointer = _sapp_zwp_relative_pointer_manager_get_relative_pointer(_sapp.wayland.relative_pointer_manager, _sapp.wayland.pointer);
            wl_proxy_add_listener((struct wl_proxy*) _sapp.wayland.relative_pointer, (void (**)(void)) &_sapp_wayland_relative_pointer_listener, NULL);
        }
    } else if (!(capabilities & WL_SEAT_CAPABILITY_POINTER) && _sapp.wayland.pointer) {
        if (_sapp.wayland.locked_pointer) {
            _sapp_xdg_proxy_destroy((struct wl_proxy*) _sapp.wayland.locked_pointer, ZWP_LOCKED_POINTER_DESTROY);
            _sapp.wayland.locked_pointer = 0;
        }
        if (_sapp.wayland.relative_pointer) {
            _sapp_xdg_proxy_destroy((struct wl_proxy*) _sapp.wayland.relative_pointer, ZWP_RELATIVE_POINTER_DESTROY);
            _sapp.wayland.relative_pointer = 0;
        }
        wl_pointer_release(_sapp.wayland.pointer);
        _sapp.wayland.pointer = 0;
        _sapp.wayland.pointer_focused = false;
        _sapp.wayland.pointer_serial_valid = false;
        _sapp.mouse.locked = false;
    }
    if ((capabilities & WL_SEAT_CAPABILITY_KEYBOARD) && !_sapp.wayland.keyboard) {
        _sapp.wayland.keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(_sapp.wayland.keyboard, &_sapp_wayland_keyboard_listener, 0);
    } else if (!(capabilities & WL_SEAT_CAPABILITY_KEYBOARD) && _sapp.wayland.keyboard) {
        wl_keyboard_release(_sapp.wayland.keyboard);
        _sapp.wayland.keyboard = 0;
        _sapp.wayland.keyboard_focused = false;
    }
}

_SOKOL_PRIVATE void _sapp_wayland_seat_name(void* data, struct wl_seat* seat, const char* name) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(seat); _SOKOL_UNUSED(name);
}

_SOKOL_PRIVATE const struct wl_seat_listener _sapp_wayland_seat_listener = {
    _sapp_wayland_seat_capabilities,
    _sapp_wayland_seat_name,
};

_SOKOL_PRIVATE void _sapp_wayland_registry_global(void* data, struct wl_registry* registry, uint32_t name, const char* interface, uint32_t version) {
    _SOKOL_UNUSED(data);
    if (0 == strcmp(interface, wl_compositor_interface.name)) {
        uint32_t bind_version = version < 4 ? version : 4;
        _sapp.wayland.compositor_version = bind_version;
        _sapp.wayland.compositor = (struct wl_compositor*) wl_registry_bind(registry, name, &wl_compositor_interface, bind_version);
    } else if (0 == strcmp(interface, wl_shm_interface.name)) {
        _sapp.wayland.shm = (struct wl_shm*) wl_registry_bind(registry, name, &wl_shm_interface, 1);
    } else if (0 == strcmp(interface, xdg_wm_base_interface.name)) {
        uint32_t bind_version = version < 4 ? version : 4;
        _sapp.wayland.wm_base_version = bind_version;
        _sapp.wayland.xdg_wm_base = (struct xdg_wm_base*) wl_registry_bind(registry, name, &xdg_wm_base_interface, bind_version);
        wl_proxy_add_listener((struct wl_proxy*) _sapp.wayland.xdg_wm_base, (void (**)(void)) &_sapp_wayland_wm_base_listener, NULL);
    } else if (0 == strcmp(interface, wl_seat_interface.name)) {
        uint32_t bind_version = version < 5 ? version : 5;
        _sapp.wayland.seat_version = bind_version;
        _sapp.wayland.seat = (struct wl_seat*) wl_registry_bind(registry, name, &wl_seat_interface, bind_version);
        wl_seat_add_listener(_sapp.wayland.seat, &_sapp_wayland_seat_listener, 0);
    } else if (0 == strcmp(interface, zwp_pointer_constraints_v1_interface.name)) {
        uint32_t bind_version = version < 1 ? version : 1;
        _sapp.wayland.pointer_constraints = (struct zwp_pointer_constraints_v1*) wl_registry_bind(registry, name, &zwp_pointer_constraints_v1_interface, bind_version);
    } else if (0 == strcmp(interface, zwp_relative_pointer_manager_v1_interface.name)) {
        uint32_t bind_version = version < 1 ? version : 1;
        _sapp.wayland.relative_pointer_manager = (struct zwp_relative_pointer_manager_v1*) wl_registry_bind(registry, name, &zwp_relative_pointer_manager_v1_interface, bind_version);
    }
}

_SOKOL_PRIVATE void _sapp_wayland_registry_global_remove(void* data, struct wl_registry* registry, uint32_t name) {
    _SOKOL_UNUSED(data); _SOKOL_UNUSED(registry); _SOKOL_UNUSED(name);
}

_SOKOL_PRIVATE const struct wl_registry_listener _sapp_wayland_registry_listener = {
    _sapp_wayland_registry_global,
    _sapp_wayland_registry_global_remove
};

_SOKOL_PRIVATE void _sapp_wayland_init_window(void) {
    _sapp.wayland.surface = wl_compositor_create_surface(_sapp.wayland.compositor);
    SOKOL_ASSERT(_sapp.wayland.surface);
    if (_sapp.wayland.using_libdecor) {
        _sapp.wayland.decor_frame = _sapp_libdecor.decorate(_sapp.wayland.decor_context, _sapp.wayland.surface, &_sapp_wayland_libdecor_frame_interface, 0);
        if (!_sapp.wayland.decor_frame) {
            _SAPP_PANIC(LINUX_X11_CREATE_WINDOW_FAILED);
        }
        _sapp_libdecor.frame_set_title(_sapp.wayland.decor_frame, _sapp.window_title);
        _sapp_libdecor.frame_set_app_id(_sapp.wayland.decor_frame, _sapp.window_title);
        wl_surface_commit(_sapp.wayland.surface);
        _sapp_libdecor.frame_map(_sapp.wayland.decor_frame);
        while (!_sapp.wayland.configured) {
            if (_sapp_libdecor.dispatch(_sapp.wayland.decor_context, -1) < 0) {
                _SAPP_PANIC(LINUX_X11_CREATE_WINDOW_FAILED);
            }
        }
    } else {
        _sapp.wayland.xdg_surface = _sapp_xdg_wm_base_get_xdg_surface(_sapp.wayland.xdg_wm_base, _sapp.wayland.surface);
        wl_proxy_add_listener((struct wl_proxy*) _sapp.wayland.xdg_surface, (void (**)(void)) &_sapp_wayland_xdg_surface_listener, NULL);
        _sapp.wayland.xdg_toplevel = _sapp_xdg_surface_get_toplevel(_sapp.wayland.xdg_surface);
        wl_proxy_add_listener((struct wl_proxy*) _sapp.wayland.xdg_toplevel, (void (**)(void)) &_sapp_wayland_xdg_toplevel_listener, NULL);
        _sapp_xdg_toplevel_set_title(_sapp.wayland.xdg_toplevel, _sapp.window_title);
        wl_surface_commit(_sapp.wayland.surface);
        while (!_sapp.wayland.configured) {
            wl_display_dispatch(_sapp.wayland.display);
        }
    }
}

#if defined(_SAPP_EGL)
_SOKOL_PRIVATE void _sapp_wayland_egl_init(void) {
    #if defined(SOKOL_GLCORE)
        if (!eglBindAPI(EGL_OPENGL_API)) {
            _SAPP_PANIC(LINUX_EGL_BIND_OPENGL_API_FAILED);
        }
    #else
        if (!eglBindAPI(EGL_OPENGL_ES_API)) {
            _SAPP_PANIC(LINUX_EGL_BIND_OPENGL_ES_API_FAILED);
        }
    #endif
    _sapp.egl.display = eglGetDisplay((EGLNativeDisplayType) _sapp.wayland.display);
    if (EGL_NO_DISPLAY == _sapp.egl.display) {
        _SAPP_PANIC(LINUX_EGL_GET_DISPLAY_FAILED);
    }
    EGLint major, minor;
    if (!eglInitialize(_sapp.egl.display, &major, &minor)) {
        _SAPP_PANIC(LINUX_EGL_INITIALIZE_FAILED);
    }
    EGLint sample_count = _sapp.desc.sample_count > 1 ? _sapp.desc.sample_count : 0;
    EGLint alpha_size = _sapp.desc.alpha ? 8 : 0;
    const EGLint config_attrs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        #if defined(SOKOL_GLCORE)
            EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        #elif defined(SOKOL_GLES3)
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        #endif
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, alpha_size,
        EGL_DEPTH_SIZE, 24,
        EGL_STENCIL_SIZE, 8,
        EGL_SAMPLE_BUFFERS, _sapp.desc.sample_count > 1 ? 1 : 0,
        EGL_SAMPLES, sample_count,
        EGL_NONE,
    };
    EGLConfig configs[32];
    EGLint config_count = 0;
    if (!eglChooseConfig(_sapp.egl.display, config_attrs, configs, 32, &config_count) || config_count == 0) {
        _SAPP_PANIC(LINUX_EGL_NO_CONFIGS);
    }
    EGLConfig config = configs[0];
    _sapp.wayland.egl_window = wl_egl_window_create(_sapp.wayland.surface, _sapp.window_width, _sapp.window_height);
    if (!_sapp.wayland.egl_window) {
        _SAPP_PANIC(LINUX_EGL_CREATE_WINDOW_SURFACE_FAILED);
    }
    _sapp.egl.surface = eglCreateWindowSurface(_sapp.egl.display, config, (EGLNativeWindowType) _sapp.wayland.egl_window, NULL);
    if (EGL_NO_SURFACE == _sapp.egl.surface) {
        _SAPP_PANIC(LINUX_EGL_CREATE_WINDOW_SURFACE_FAILED);
    }
    EGLint ctx_attrs[] = {
        EGL_CONTEXT_MAJOR_VERSION, _sapp.desc.gl.major_version,
        EGL_CONTEXT_MINOR_VERSION, _sapp.desc.gl.minor_version,
        #if defined(SOKOL_GLCORE)
            EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
        #endif
        EGL_NONE,
    };
    _sapp.egl.context = eglCreateContext(_sapp.egl.display, config, EGL_NO_CONTEXT, ctx_attrs);
    if (EGL_NO_CONTEXT == _sapp.egl.context) {
        _SAPP_PANIC(LINUX_EGL_CREATE_CONTEXT_FAILED);
    }
    if (!eglMakeCurrent(_sapp.egl.display, _sapp.egl.surface, _sapp.egl.surface, _sapp.egl.context)) {
        _SAPP_PANIC(LINUX_EGL_MAKE_CURRENT_FAILED);
    }
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*) &_sapp.gl.framebuffer);
    eglSwapInterval(_sapp.egl.display, _sapp.swap_interval);
}

_SOKOL_PRIVATE void _sapp_wayland_egl_destroy(void) {
    if (_sapp.egl.display != EGL_NO_DISPLAY) {
        eglMakeCurrent(_sapp.egl.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (_sapp.egl.context != EGL_NO_CONTEXT) {
            eglDestroyContext(_sapp.egl.display, _sapp.egl.context);
            _sapp.egl.context = EGL_NO_CONTEXT;
        }
        if (_sapp.egl.surface != EGL_NO_SURFACE) {
            eglDestroySurface(_sapp.egl.display, _sapp.egl.surface);
            _sapp.egl.surface = EGL_NO_SURFACE;
        }
        eglTerminate(_sapp.egl.display);
        _sapp.egl.display = EGL_NO_DISPLAY;
    }
    if (_sapp.wayland.egl_window) {
        wl_egl_window_destroy(_sapp.wayland.egl_window);
        _sapp.wayland.egl_window = 0;
    }
}
#endif

_SOKOL_PRIVATE void _sapp_wayland_create_vk_surface(void) {
    #if defined(SOKOL_VULKAN)
        _SAPP_STRUCT(VkWaylandSurfaceCreateInfoKHR, info);
        info.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
        info.display = _sapp.wayland.display;
        info.surface = _sapp.wayland.surface;
        VkResult res = vkCreateWaylandSurfaceKHR(_sapp.vk.instance, &info, 0, &_sapp.vk.surface);
        if (res != VK_SUCCESS) {
            _SAPP_PANIC(VULKAN_CREATE_SURFACE_FAILED);
        }
        SOKOL_ASSERT(_sapp.vk.surface);
    #endif
}

_SOKOL_PRIVATE void _sapp_wayland_frame(void) {
    if (_sapp.wayland.using_libdecor) {
        _sapp_libdecor.dispatch(_sapp.wayland.decor_context, 0);
    }
    while (wl_display_prepare_read(_sapp.wayland.display) != 0) {
        wl_display_dispatch_pending(_sapp.wayland.display);
    }
    wl_display_flush(_sapp.wayland.display);
    struct pollfd pfd;
    pfd.fd = wl_display_get_fd(_sapp.wayland.display);
    pfd.events = POLLIN;
    pfd.revents = 0;
    int poll_res = poll(&pfd, 1, 0);
    if (poll_res > 0) {
        wl_display_read_events(_sapp.wayland.display);
        wl_display_dispatch_pending(_sapp.wayland.display);
    } else {
        wl_display_cancel_read(_sapp.wayland.display);
    }
    #if defined(SOKOL_VULKAN)
        _sapp_vk_frame();
    #else
        _sapp_frame();
        #if defined(_SAPP_EGL)
            eglSwapBuffers(_sapp.egl.display, _sapp.egl.surface);
        #endif
    #endif
}

_SOKOL_PRIVATE void _sapp_wayland_run(const sapp_desc* desc) {
    pthread_attr_t pthread_attr;
    pthread_attr_init(&pthread_attr);
    pthread_attr_destroy(&pthread_attr);
    _sapp_init_state(desc);
    _sapp.dpi_scale = 1.0f;
    _sapp.wayland.display = wl_display_connect(NULL);
    if (!_sapp.wayland.display) {
        _SAPP_PANIC(LINUX_X11_OPEN_DISPLAY_FAILED);
    }
    _sapp.wayland.xkb_shift = XKB_MOD_INVALID;
    _sapp.wayland.xkb_ctrl = XKB_MOD_INVALID;
    _sapp.wayland.xkb_alt = XKB_MOD_INVALID;
    _sapp.wayland.xkb_super = XKB_MOD_INVALID;
    _sapp.wayland.xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    if (!_sapp.wayland.xkb_context) {
        _SAPP_PANIC(LINUX_X11_CREATE_WINDOW_FAILED);
    }
    _sapp.wayland.registry = wl_display_get_registry(_sapp.wayland.display);
    wl_registry_add_listener(_sapp.wayland.registry, &_sapp_wayland_registry_listener, NULL);
    wl_display_roundtrip(_sapp.wayland.display);
    if (!_sapp.wayland.compositor) {
        _SAPP_PANIC(LINUX_X11_CREATE_WINDOW_FAILED);
    }
    _sapp_wayland_init_cursor();
    if (!_sapp_wayland_load_libdecor()) {
        _SAPP_PANIC(LINUX_X11_CREATE_WINDOW_FAILED);
    }
    _sapp.wayland.decor_context = _sapp_libdecor.new_(_sapp.wayland.display, &_sapp_wayland_libdecor_interface);
    if (!_sapp.wayland.decor_context) {
        _SAPP_PANIC(LINUX_X11_CREATE_WINDOW_FAILED);
    }
    _sapp.wayland.using_libdecor = true;
    _sapp_wayland_init_window();
    #if defined(_SAPP_EGL)
        _sapp_wayland_egl_init();
    #elif defined(SOKOL_VULKAN)
        _sapp_vk_init();
    #endif
    _sapp.valid = true;
    if (_sapp.fullscreen) {
        if (_sapp.wayland.decor_frame) {
            _sapp_libdecor.frame_set_fullscreen(_sapp.wayland.decor_frame, 0);
        } else if (_sapp.wayland.xdg_toplevel) {
            _sapp_xdg_toplevel_set_fullscreen(_sapp.wayland.xdg_toplevel);
            wl_surface_commit(_sapp.wayland.surface);
        }
    }
    while (!_sapp.quit_ordered) {
        _sapp_timing_update(&_sapp.timing, 0.0);
        _sapp_wayland_frame();
        if (_sapp.quit_requested && !_sapp.quit_ordered) {
            _sapp_wayland_app_event(SAPP_EVENTTYPE_QUIT_REQUESTED);
            if (_sapp.quit_requested) {
                _sapp.quit_ordered = true;
            }
        }
    }
    _sapp_call_cleanup();
    #if defined(_SAPP_EGL)
        _sapp_wayland_egl_destroy();
    #elif defined(SOKOL_VULKAN)
        _sapp_vk_discard();
    #endif
    if (_sapp.wayland.keyboard) {
        wl_keyboard_release(_sapp.wayland.keyboard);
        _sapp.wayland.keyboard = 0;
    }
    if (_sapp.wayland.locked_pointer) {
        _sapp_xdg_proxy_destroy((struct wl_proxy*) _sapp.wayland.locked_pointer, ZWP_LOCKED_POINTER_DESTROY);
        _sapp.wayland.locked_pointer = 0;
    }
    if (_sapp.wayland.relative_pointer) {
        _sapp_xdg_proxy_destroy((struct wl_proxy*) _sapp.wayland.relative_pointer, ZWP_RELATIVE_POINTER_DESTROY);
        _sapp.wayland.relative_pointer = 0;
    }
    if (_sapp.wayland.pointer) {
        wl_pointer_release(_sapp.wayland.pointer);
        _sapp.wayland.pointer = 0;
    }
    if (_sapp.wayland.seat) {
        wl_seat_release(_sapp.wayland.seat);
        _sapp.wayland.seat = 0;
    }
    if (_sapp.wayland.cursor_surface) {
        wl_surface_destroy(_sapp.wayland.cursor_surface);
        _sapp.wayland.cursor_surface = 0;
    }
    if (_sapp.wayland.hidden_cursor_buffer) {
        wl_buffer_destroy(_sapp.wayland.hidden_cursor_buffer);
        _sapp.wayland.hidden_cursor_buffer = 0;
    }
    if (_sapp.wayland.cursor_theme) {
        _sapp_wayland_cursor.theme_destroy(_sapp.wayland.cursor_theme);
        _sapp.wayland.cursor_theme = 0;
        _sapp.wayland.default_cursor = 0;
    }
    if (_sapp.wayland.wayland_cursor_so) {
        dlclose(_sapp.wayland.wayland_cursor_so);
        _sapp.wayland.wayland_cursor_so = 0;
    }
    if (_sapp.wayland.xkb_state) {
        xkb_state_unref(_sapp.wayland.xkb_state);
        _sapp.wayland.xkb_state = 0;
    }
    if (_sapp.wayland.xkb_keymap) {
        xkb_keymap_unref(_sapp.wayland.xkb_keymap);
        _sapp.wayland.xkb_keymap = 0;
    }
    if (_sapp.wayland.xkb_context) {
        xkb_context_unref(_sapp.wayland.xkb_context);
        _sapp.wayland.xkb_context = 0;
    }
    if (_sapp.wayland.pointer_constraints) {
        _sapp_xdg_proxy_destroy((struct wl_proxy*) _sapp.wayland.pointer_constraints, ZWP_POINTER_CONSTRAINTS_DESTROY);
        _sapp.wayland.pointer_constraints = 0;
    }
    if (_sapp.wayland.relative_pointer_manager) {
        _sapp_xdg_proxy_destroy((struct wl_proxy*) _sapp.wayland.relative_pointer_manager, ZWP_RELATIVE_POINTER_MANAGER_DESTROY);
        _sapp.wayland.relative_pointer_manager = 0;
    }
    if (_sapp.wayland.decor_frame) {
        _sapp_libdecor.frame_unref(_sapp.wayland.decor_frame);
        _sapp.wayland.decor_frame = 0;
    }
    if (_sapp.wayland.decor_context) {
        _sapp_libdecor.unref(_sapp.wayland.decor_context);
        _sapp.wayland.decor_context = 0;
    }
    if (_sapp.wayland.libdecor_so) {
        dlclose(_sapp.wayland.libdecor_so);
        _sapp.wayland.libdecor_so = 0;
    }
    if (_sapp.wayland.xdg_toplevel) {
        _sapp_xdg_proxy_destroy((struct wl_proxy*) _sapp.wayland.xdg_toplevel, XDG_TOPLEVEL_DESTROY);
    }
    if (_sapp.wayland.xdg_surface) {
        _sapp_xdg_proxy_destroy((struct wl_proxy*) _sapp.wayland.xdg_surface, XDG_SURFACE_DESTROY);
    }
    _sapp_xdg_proxy_destroy((struct wl_proxy*) _sapp.wayland.xdg_wm_base, XDG_WM_BASE_DESTROY);
    if (_sapp.wayland.surface) {
        wl_surface_destroy(_sapp.wayland.surface);
    }
    if (_sapp.wayland.shm) {
        wl_shm_destroy(_sapp.wayland.shm);
        _sapp.wayland.shm = 0;
    }
    if (_sapp.wayland.registry) {
        wl_registry_destroy(_sapp.wayland.registry);
    }
    wl_display_disconnect(_sapp.wayland.display);
    _sapp_discard_state();
}

_SOKOL_PRIVATE void _sapp_wayland_toggle_fullscreen(void) {
    _sapp.fullscreen = !_sapp.fullscreen;
    if (_sapp.wayland.decor_frame) {
        if (_sapp.fullscreen) {
            _sapp_libdecor.frame_set_fullscreen(_sapp.wayland.decor_frame, 0);
        } else {
            _sapp_libdecor.frame_unset_fullscreen(_sapp.wayland.decor_frame);
        }
    } else if (_sapp.wayland.xdg_toplevel) {
        if (_sapp.fullscreen) {
            _sapp_xdg_toplevel_set_fullscreen(_sapp.wayland.xdg_toplevel);
        } else {
            _sapp_xdg_toplevel_unset_fullscreen(_sapp.wayland.xdg_toplevel);
        }
        wl_surface_commit(_sapp.wayland.surface);
    }
}

_SOKOL_PRIVATE void _sapp_wayland_update_cursor(sapp_mouse_cursor cursor, bool shown) {
    _SOKOL_UNUSED(cursor); _SOKOL_UNUSED(shown);
    _sapp_wayland_apply_cursor(shown);
}

_SOKOL_PRIVATE void _sapp_wayland_lock_mouse(bool lock) {
    if (lock == _sapp.mouse.locked) {
        return;
    }
    _sapp.mouse.dx = 0.0f;
    _sapp.mouse.dy = 0.0f;
    if (lock) {
        if (!_sapp.wayland.pointer_constraints || !_sapp.wayland.pointer || !_sapp.wayland.surface) {
            _sapp.mouse.locked = false;
            return;
        }
        if (!_sapp.wayland.relative_pointer && _sapp.wayland.relative_pointer_manager) {
            _sapp.wayland.relative_pointer = _sapp_zwp_relative_pointer_manager_get_relative_pointer(_sapp.wayland.relative_pointer_manager, _sapp.wayland.pointer);
            wl_proxy_add_listener((struct wl_proxy*) _sapp.wayland.relative_pointer, (void (**)(void)) &_sapp_wayland_relative_pointer_listener, NULL);
        }
        if (!_sapp.wayland.locked_pointer) {
            _sapp.wayland.locked_pointer = _sapp_zwp_pointer_constraints_lock_pointer(_sapp.wayland.pointer_constraints, _sapp.wayland.surface, _sapp.wayland.pointer);
            wl_proxy_add_listener((struct wl_proxy*) _sapp.wayland.locked_pointer, (void (**)(void)) &_sapp_wayland_locked_pointer_listener, NULL);
        }
        _sapp.mouse.locked = true;
        _sapp_wayland_apply_cursor(false);
    } else {
        if (_sapp.wayland.locked_pointer) {
            _sapp_xdg_proxy_destroy((struct wl_proxy*) _sapp.wayland.locked_pointer, ZWP_LOCKED_POINTER_DESTROY);
            _sapp.wayland.locked_pointer = 0;
        }
        _sapp.mouse.locked = false;
        _sapp_wayland_apply_cursor(_sapp.mouse.shown);
    }
    wl_display_flush(_sapp.wayland.display);
}

_SOKOL_PRIVATE bool _sapp_wayland_make_custom_mouse_cursor(sapp_mouse_cursor cursor, const sapp_image_desc* desc) {
    _SOKOL_UNUSED(cursor); _SOKOL_UNUSED(desc);
    return false;
}

_SOKOL_PRIVATE void _sapp_wayland_destroy_custom_mouse_cursor(sapp_mouse_cursor cursor) {
    _SOKOL_UNUSED(cursor);
}

_SOKOL_PRIVATE void _sapp_wayland_set_clipboard_string(const char* str) {
    _sapp_strcpy(str, _sapp.clipboard.buffer, (size_t) _sapp.clipboard.buf_size);
}

_SOKOL_PRIVATE const char* _sapp_wayland_get_clipboard_string(void) {
    return _sapp.clipboard.buffer;
}

_SOKOL_PRIVATE void _sapp_wayland_update_window_title(void) {
    if (_sapp.wayland.decor_frame) {
        _sapp_libdecor.frame_set_title(_sapp.wayland.decor_frame, _sapp.window_title);
    } else if (_sapp.wayland.xdg_toplevel) {
        _sapp_xdg_toplevel_set_title(_sapp.wayland.xdg_toplevel, _sapp.window_title);
        wl_surface_commit(_sapp.wayland.surface);
    }
}

_SOKOL_PRIVATE void _sapp_wayland_set_icon(const sapp_icon_desc* desc, int num_images) {
    _SOKOL_UNUSED(desc); _SOKOL_UNUSED(num_images);
}

SOKOL_API_IMPL const void* sapp_wayland_get_display(void) {
    #if defined(_SAPP_WAYLAND)
        return _sapp.wayland.display;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_wayland_get_surface(void) {
    #if defined(_SAPP_WAYLAND)
        return _sapp.wayland.surface;
    #else
        return 0;
    #endif
}

#endif /* SOKOL_APP_TURBO_IMPL_INCLUDED */
#endif /* SOKOL_APP_IMPL && SOKOL_TURBO_WAYLAND */
