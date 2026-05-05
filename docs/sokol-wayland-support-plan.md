# Wayland Support For `sokol_app_turbo.h`

This fork adds Wayland as a Linux window-system backend through a new
`sokol_app_turbo.h` wrapper. Upstream `sokol_app.h` remains the primary API
surface, with only small conditional hook points for Linux backend selection.

## Current Status

Initial decorated-window scaffold implemented. The current tree can compile the
fork-local Wayland backend for GL/EGL, GLES3/EGL, and Vulkan, and keeps the
default X11 path building unchanged.

This is not yet feature-parity Wayland support. The implemented backend creates
a `wl_surface`, decorates it through runtime-loaded `libdecor`, creates EGL or
Vulkan surfaces, runs the frame loop, reports the Wayland display/surface
through the new public getters, handles close requests, resize configure events,
window title updates, fullscreen requests, and a first slice of pointer and
keyboard input. Richer desktop integration remains pending.

Verified locally:

- X11 `sokol_app` component builds.
- Wayland `SOKOL_GLCORE + SOKOL_FORCE_EGL` `sokol_app` component builds.
- Wayland `SOKOL_GLES3` `sokol_app` component builds.
- Wayland `SOKOL_VULKAN` `sokol_app` component builds with the Vulkan SDK at
  `/home/michael/dev/vulkan/1.4.335.0/x86_64`.
- Direct single-translation-unit compile/link smoke tests pass for Wayland
  GLCORE/EGL and Wayland Vulkan.
- `sample_triangle_sapp` builds in the local `build` tree after libdecor
  integration.
- First Wayland input slice compiles in the local `build` tree and in the
  GLCORE/EGL, GLES3, and Vulkan component build dirs.
- `sample_events_sapp` builds in a filtered Wayland/ImGui CMake tree using a
  local dcimgui checkout.

Not yet verified:

- Decorated runtime behavior under a real Wayland compositor after the libdecor
  change. Before this change, `sample_triangle_sapp` rendered but had no window
  decorations.
- Broader `sokol_gfx` sample execution through `sglue_environment()` and
  `sglue_swapchain()`.
- Runtime behavior of pointer and keyboard events in `sample_events_sapp`.
- Full sample configure/build, because the local configure tried to fetch
  `sokol-tools-bin` from GitHub and network access was unavailable.

## Backend Selection

`SOKOL_TURBO_WAYLAND` selects Wayland instead of the upstream X11 backend on
Linux. It is independent of the rendering backend. The initially supported
combinations are:

- `SOKOL_TURBO_WAYLAND + SOKOL_GLCORE + SOKOL_FORCE_EGL`
- `SOKOL_TURBO_WAYLAND + SOKOL_GLES3`
- `SOKOL_TURBO_WAYLAND + SOKOL_VULKAN`

`SOKOL_GLCORE` on Wayland requires EGL. GLX remains X11-only.

## Header Shape

Applications that want turbo features include `sokol_app_turbo.h` instead of
`sokol_app.h`. The turbo header forwards the public upstream API and adds
Wayland-specific getters:

- `sapp_wayland_get_display()`
- `sapp_wayland_get_surface()`

Implementation mode keeps Wayland state, event handlers, libdecor loader
tables, and helper APIs in `sokol_app_turbo.h`. `sokol_app.h` only dispatches
to those helpers when `SOKOL_TURBO_WAYLAND` is defined. Runtime loader tables
for the remaining Wayland/EGL/xkbcommon calls are still planned follow-up work.

## Implementation Notes

The target design is to load Wayland-related shared libraries at runtime:

- `libwayland-client.so.0`
- `libxkbcommon.so.0`
- `libdecor-0.so.0`
- `libwayland-egl.so.1`
- `libEGL.so.1` for GL/EGL builds

The first implementation currently links the Wayland client libraries through
`pkg-config` while preserving the fork-local backend boundary; replacing direct
Wayland/EGL/xkbcommon calls with loader tables is a follow-up. It runtime-loads
`libdecor-0.so.0` with `dlopen` and uses `libdecor_decorate()` as the primary
top-level window path. The embedded `xdg-shell` binding remains as a narrow
fallback/debug path and for protocol types, but libdecor is the intended path
for usable decorated windows. EGL builds create a `wl_egl_window`,
`EGLDisplay`, `EGLSurface`, and `EGLContext`; Vulkan builds request
`VK_KHR_wayland_surface` and create a Wayland Vulkan surface.

Fractional scale, viewporter, cursor-shape, pointer constraints, relative
pointer, clipboard, and drag/drop are planned follow-up extensions behind the
same private Wayland state and public API.

## Findings And Decisions

- Raw `xdg_toplevel` windows can render correctly but may appear without title
  bars, resize borders, or close controls. This was observed with
  `sample_triangle_sapp`.
- The compositor sent `xdg_toplevel` event opcode 2 (`configure_bounds`), which
  exposed that the embedded protocol table was too small. The table now includes
  `configure_bounds` and `wm_capabilities`.
- `libdecor` is required for the first usable Wayland backend. It handles the
  practical decoration path across compositors and owns the correct map/configure
  lifecycle.
- Follow the working `wio-extra` design: runtime-load optional desktop
  integration libraries, keep Wayland state private, and let libdecor configure
  callbacks drive content-size commits.
- For input, start with direct `wl_seat`, `wl_pointer`, `wl_keyboard`, and
  `xkbcommon` calls because the current CMake path already links those
  libraries. Move them behind loader tables after behavior is validated.
- The original `sokol-samples` build does not use system Dear ImGui packages.
  Its `fibs` scripts import `dcimgui` and the `events` sample depends on the
  `imgui` target from that import. The CMake path now mirrors this with
  `SOKOL_DCIMGUI_DIR`, local `../dcimgui` auto-detection, or a fallback clone.
- `sample_events_sapp` initially aborted when the pointer entered the window.
  The cause was a listener ABI mismatch: the backend bound `wl_seat` version 5
  but provided only the first five `wl_pointer_listener` callbacks. The listener
  now includes the version-5 callbacks `frame`, `axis_source`, `axis_stop`, and
  `axis_discrete`.
- The cursor was invisible on initial pointer entry into the content surface.
  Wayland clients own cursor images for their surfaces; the backend now binds
  `wl_shm`, runtime-loads `libwayland-cursor.so.0`, creates a default cursor
  theme, and sets the `left_ptr` cursor on pointer enter.
- `sapp_show_mouse()` initially did not hide the cursor because sokol calls the
  platform cursor hook before updating `_sapp.mouse.shown`. The Wayland cursor
  apply path now uses the requested visibility argument directly.
- Some compositors still kept the cursor visible after `wl_pointer_set_cursor`
  with a null surface. The hide path now uses a 1x1 transparent `wl_shm` cursor
  buffer instead, while the show path restores the theme cursor image.
- Wayland cursor updates must use the serial from `wl_pointer.enter`. The
  backend now preserves that enter serial for cursor changes instead of
  replacing it with button-event serials, and invalidates it on pointer leave.
- X11 implements `sapp_lock_mouse()` with `XGrabPointer`, window confinement,
  a hidden cursor, and XI raw motion when available. Wayland has no equivalent
  in core `wl_pointer`; the backend now uses
  `zwp_pointer_constraints_v1.lock_pointer` for confinement and
  `zwp_relative_pointer_v1.relative_motion` for locked relative deltas.
- Fractional high-DPI support on Wayland requires separating logical content
  size from buffer size. The backend follows the wio-extra approach: use
  `wp_fractional_scale_v1.preferred_scale / 120.0f` as the scale, render to a
  scaled framebuffer when `sapp_desc.high_dpi` is true, and use `wp_viewport` to
  keep the surface destination at the logical window size.
- When `wp_fractional_scale_manager_v1` is unavailable, high-DPI falls back to
  integer output scale. The backend tracks `wl_surface.enter/leave`, binds
  `wl_output` scale events, uses the maximum scale of entered outputs, and uses
  `wl_surface.preferred_buffer_scale` as another integer fallback on newer
  compositors.

## Completed

- Added `sokol_app_turbo.h` as the fork-local public include for turbo app
  features.
- Added `SOKOL_TURBO_WAYLAND` as the Linux window-system selector.
- Added public Wayland getters:
  `sapp_wayland_get_display()` and `sapp_wayland_get_surface()`.
- Added small `sokol_app.h` hook points for Linux platform selection, X11 state
  isolation, Linux run/frame dispatch, public operation dispatch, and Vulkan
  platform extension/surface creation.
- Kept X11 as the default Linux backend.
- Added embedded minimal `xdg-shell` protocol binding in `sokol_app_turbo.h`.
- Added Wayland EGL window/context/surface creation.
- Added Wayland Vulkan surface creation through `VK_KHR_wayland_surface`.
- Added runtime loading of `libdecor-0.so.0`.
- Added libdecor context/frame creation, frame mapping, configure callbacks,
  close callbacks, title updates, fullscreen updates, and cleanup.
- Fixed Wayland startup fullscreen handling so `sapp_desc.fullscreen` is applied
  instead of toggled off.
- Added `xdg_toplevel.configure_bounds` and `xdg_toplevel.wm_capabilities`
  handling to the embedded protocol table.
- Added first-pass Wayland input:
  `wl_seat` capability handling, pointer enter/leave/motion/buttons/scroll,
  keyboard enter/leave/keymap/key/modifier callbacks, xkbcommon key translation
  for UTF-32 character events, and a Linux evdev-to-`sapp_keycode` mapping.
- Fixed the `wl_pointer_listener` callback table for `wl_pointer` version 5.
- Added minimal cursor support: `wl_shm` binding, runtime loading of
  `libwayland-cursor.so.0`, default cursor theme/image setup, pointer-enter
  cursor assignment, and show/hide handling.
- Fixed `sapp_show_mouse()` on Wayland so the SPACE toggle in
  `sample_events_sapp` can hide and show the cursor immediately. The final hide
  implementation uses a transparent 1x1 cursor buffer because a null Wayland
  cursor surface was not reliable on the tested compositor.
- Added first-pass Wayland mouse lock support through embedded
  `zwp_pointer_constraints_v1` and `zwp_relative_pointer_manager_v1` protocol
  bindings. `sapp_lock_mouse(true)` now requests a persistent locked pointer,
  hides the cursor while locked, and reports relative motion from
  `zwp_relative_pointer_v1`.
- Runtime validation confirms locked mouse behavior matches sokol semantics:
  the cursor disappears, absolute mouse position stops changing, and
  `mouse_dx/mouse_dy` continue updating from relative pointer events.
- Added first-pass Wayland high-DPI support through embedded `wp_viewporter` and
  `wp_fractional_scale_manager_v1` protocol bindings. Logical window size,
  framebuffer size, `sapp_dpi_scale()`, EGL window size, resize events, pointer
  coordinates, and relative mouse deltas now account for fractional scale when
  `sapp_desc.high_dpi` is enabled.
- Runtime validation on KDE Wayland with fractional scaling around 1.15 passed
  with `sample_imgui_highdpi_sapp`: rendering looked correct, ImGui interaction
  worked, and the sample reported a fractional DPI scale.
- Added integer high-DPI fallback for compositors without fractional-scale:
  `wl_output` scale tracking, surface enter/leave handling, optional
  `wl_surface.preferred_buffer_scale`, `wl_surface.set_buffer_scale` when no
  viewport is available, and viewport-based logical destination when it is.
- Added top-level CMake support for ImGui samples through dcimgui:
  `SOKOL_DCIMGUI_DIR`, automatic `../dcimgui` detection, fallback clone, and an
  `imgui` target.
- Added `SOKOL_SAMPLE_FILTER` so a single sample such as `events` can be
  configured without pulling shader-tooling dependencies for unrelated samples.
- Fixed sample dependency linking so dependencies can refer to exact CMake
  target names such as `imgui`, not only `sample_*` targets.
- Added `cmake/impl/sokol_app_turbo_impl.c`.
- Added `SOKOL_LINUX_WINDOW_SYSTEM=X11|WAYLAND`, defaulting to `X11`.
- Removed X11/Xcursor/Xi link dependencies from the Wayland CMake path.

## Not Completed

- Runtime-loaded Wayland, xkbcommon, wayland-egl, and EGL symbol tables.
- Runtime verification of libdecor decorations under the user's compositor.
- Runtime verification of keyboard input through `wl_keyboard` and `xkbcommon`.
- Runtime verification of pointer input through `wl_pointer`, including motion,
  buttons, scroll, enter, and leave.
- Key repeat handling.
- Touch input.
- Full cursor image/theme handling on Wayland. A default visible arrow cursor
  and show/hide are implemented, but custom cursor images and non-arrow
  standard shapes remain pending.
- Clipboard protocol support. Current clipboard functions only mirror the local
  sokol buffer.
- Drag/drop protocol support.
- Runtime verification of integer-scale fallback on a compositor without
  `wp_fractional_scale_manager_v1`.
- Full mouse-lock polish across compositors. Pointer constraints and relative
  pointer support are implemented and validated on the user's compositor, but
  fallback behavior for compositors without these protocols is still minimal.
- Wayland data-device, primary selection, and text-input protocols.
- Runtime smoke tests under a compositor.
- Sample-level `sokol_gfx` verification.

## CMake

`SOKOL_LINUX_WINDOW_SYSTEM` selects `X11` or `WAYLAND`, defaulting to `X11`.
The Wayland option defines `SOKOL_TURBO_WAYLAND`, uses
`sokol_app_turbo.h` for the `sokol_app` implementation unit, removes X11 link
dependencies, and keeps `dl`, `m`, and `Threads::Threads`.

Current implementation note: the Wayland path also requires `pkg-config`
packages for `wayland-client`, `wayland-egl`, and `xkbcommon`. This is an
intermediate build dependency until runtime loading is implemented.

For input validation without fetching shader tooling:

```sh
cmake -S . -B /tmp/sokol-turbo-events \
  -DSOKOL_LINUX_WINDOW_SYSTEM=WAYLAND \
  -DSOKOL_BACKEND=SOKOL_GLCORE \
  -DSOKOL_SAMPLES_WITH_IMGUI=ON \
  -DSOKOL_DCIMGUI_DIR=/home/michael/dev/c++/dcimgui \
  -DSOKOL_SAMPLE_FILTER=events

cmake --build /tmp/sokol-turbo-events --target sample_events_sapp -j2
```

For local Vulkan builds on this machine, pass the SDK include/library paths:

```sh
cmake -S . -B /tmp/sokol-turbo-wayland-vk \
  -DSOKOL_LINUX_WINDOW_SYSTEM=WAYLAND \
  -DSOKOL_BACKEND=SOKOL_VULKAN \
  -DCMAKE_C_FLAGS=-I/home/michael/dev/vulkan/1.4.335.0/x86_64/include \
  -DCMAKE_CXX_FLAGS=-I/home/michael/dev/vulkan/1.4.335.0/x86_64/include \
  -DCMAKE_EXE_LINKER_FLAGS=-L/home/michael/dev/vulkan/1.4.335.0/x86_64/lib \
  -DCMAKE_SHARED_LINKER_FLAGS=-L/home/michael/dev/vulkan/1.4.335.0/x86_64/lib
```

## Next Steps

1. Run `sample_clear_sapp` and `sample_triangle_sapp` under a real Wayland
   compositor for GLCORE/EGL and Vulkan.
2. Run `/tmp/sokol-turbo-events/bin/sample_events_sapp` and verify pointer
   motion, buttons, scroll, focus, key down/up, modifiers, and char events under
   Wayland.
3. Fix any event issues found by `sample_events_sapp`, then add key repeat
   handling for Wayland.
4. Runtime-test integer-scale fallback on a compositor without
   `wp_fractional_scale_manager_v1`.
5. Replace direct Wayland/EGL/xkbcommon calls with runtime loader tables.
6. Implement clipboard through Wayland data-device protocols.
7. Implement cursor theme/images beyond the default arrow and transparent hide
   cursor.
8. Improve fallback/error behavior for compositors that do not expose pointer
   constraints or relative pointer protocols.
9. Add drag/drop support.
10. Add compile presets or documented CMake invocations for the three Wayland
    backend combinations.
11. Build and run the `sokol_gfx` sample matrix once `sokol-tools-bin` is
    available locally or network fetch is allowed.

## Test Plan

- Build X11 presets unchanged. Completed for the `sokol_app` component.
- Build Wayland presets for GLCORE/EGL, GLES3, and Vulkan. Completed for the
  `sokol_app` component.
- Run `sample_clear_sapp`, `sample_triangle_sapp`, `sample_cube_sapp`, and
  `sample_events_sapp` in a real Wayland session. `sample_triangle_sapp` was
  observed rendering. Libdecor decoration behavior was verified by the user
  through resize, minimize/hide, and restore. `sample_events_sapp` has been used
  for first-pass keyboard, pointer, cursor visibility, and mouse-lock testing.
  Mouse lock was verified by observing hidden cursor, frozen absolute position,
  and changing relative deltas.
- `sample_imgui_highdpi_sapp` was validated on KDE Wayland at fractional scale
  around 1.15. The UI rendered and interacted correctly, and the sample reported
  a fractional DPI scale. Integer fallback compile coverage is complete, but it
  still needs runtime validation on a compositor without fractional-scale.
- Verify resize, high-DPI scale, key down/up/repeat, UTF-8 text, mouse motion,
  buttons, scroll, cursor visibility, quit, fullscreen, title changes, and
  `sokol_gfx` glue through `sglue_environment()` and `sglue_swapchain()`.
  Pending for the full matrix, except compile/link smoke coverage for surface
  creation paths, user runtime checks for triangle rendering/decorations, and
  first-pass input/cursor/mouse-lock runtime checks in `sample_events_sapp`.
