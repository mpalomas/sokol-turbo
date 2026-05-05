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

typedef struct _sapp_wayland_t {
    struct wl_display* display;
    struct wl_registry* registry;
    struct wl_compositor* compositor;
    struct wl_surface* surface;
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
};

struct xdg_wm_base { struct wl_proxy* proxy; };
struct xdg_surface { struct wl_proxy* proxy; };
struct xdg_toplevel { struct wl_proxy* proxy; };
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

static const struct wl_interface xdg_wm_base_interface;
static const struct wl_interface xdg_surface_interface;
static const struct wl_interface xdg_toplevel_interface;
static const struct wl_interface xdg_positioner_interface;
static const struct wl_interface xdg_popup_interface;

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

_SOKOL_PRIVATE void _sapp_wayland_registry_global(void* data, struct wl_registry* registry, uint32_t name, const char* interface, uint32_t version) {
    _SOKOL_UNUSED(data);
    if (0 == strcmp(interface, wl_compositor_interface.name)) {
        uint32_t bind_version = version < 4 ? version : 4;
        _sapp.wayland.compositor_version = bind_version;
        _sapp.wayland.compositor = (struct wl_compositor*) wl_registry_bind(registry, name, &wl_compositor_interface, bind_version);
    } else if (0 == strcmp(interface, xdg_wm_base_interface.name)) {
        uint32_t bind_version = version < 4 ? version : 4;
        _sapp.wayland.wm_base_version = bind_version;
        _sapp.wayland.xdg_wm_base = (struct xdg_wm_base*) wl_registry_bind(registry, name, &xdg_wm_base_interface, bind_version);
        wl_proxy_add_listener((struct wl_proxy*) _sapp.wayland.xdg_wm_base, (void (**)(void)) &_sapp_wayland_wm_base_listener, NULL);
    } else if (0 == strcmp(interface, wl_seat_interface.name)) {
        uint32_t bind_version = version < 5 ? version : 5;
        _sapp.wayland.seat_version = bind_version;
        _sapp.wayland.seat = (struct wl_seat*) wl_registry_bind(registry, name, &wl_seat_interface, bind_version);
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
    _sapp.wayland.registry = wl_display_get_registry(_sapp.wayland.display);
    wl_registry_add_listener(_sapp.wayland.registry, &_sapp_wayland_registry_listener, NULL);
    wl_display_roundtrip(_sapp.wayland.display);
    if (!_sapp.wayland.compositor) {
        _SAPP_PANIC(LINUX_X11_CREATE_WINDOW_FAILED);
    }
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
}

_SOKOL_PRIVATE void _sapp_wayland_lock_mouse(bool lock) {
    _sapp.mouse.locked = lock;
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
