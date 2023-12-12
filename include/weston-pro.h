// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2023 He Yong <hyyoxhk@163.com>
 */

#ifndef WESTON_SERVER_H
#define WESTON_SERVER_H

#include "config.h"
#include <wayland-server-core.h>
#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_keyboard.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_pointer.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_xcursor_manager.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/util/box.h>

/* For brevity's sake, struct members are annotated where they are used. */
enum wet_cursor_mode {
	CURSOR_PASSTHROUGH,
	CURSOR_MOVE,
	CURSOR_RESIZE,
};

struct wet_server {
	struct wl_display *wl_display;
	struct wlr_backend *backend;
	struct wlr_renderer *renderer;
	struct wlr_allocator *allocator;
	struct wlr_scene *scene;

	struct wlr_xdg_shell *xdg_shell;
	struct wl_listener new_xdg_surface;
	struct wl_list views;

	struct wlr_cursor *cursor;
	struct wlr_xcursor_manager *cursor_mgr;
	struct wl_listener cursor_motion;
	struct wl_listener cursor_motion_absolute;
	struct wl_listener cursor_button;
	struct wl_listener cursor_axis;
	struct wl_listener cursor_frame;

	struct wlr_seat *seat;
	struct wl_listener new_input;
	struct wl_listener request_cursor;
	struct wl_listener request_set_selection;
	struct wl_list keyboards;
	enum wet_cursor_mode cursor_mode;
	struct wet_view *grabbed_view;
	double grab_x, grab_y;
	struct wlr_box grab_geobox;
	uint32_t resize_edges;

	struct wlr_output_layout *output_layout;
	struct wl_list outputs;
	struct wl_listener new_output;
};

struct wet_output {
	struct wl_list link;
	struct wet_server *server;
	struct wlr_output *wlr_output;
	struct wl_listener frame;
};

struct wet_view {
	struct wl_list link;
	struct wet_server *server;
	struct wlr_xdg_surface *xdg_surface;
	struct wlr_scene_node *scene_node;
	struct wl_listener map;
	struct wl_listener unmap;
	struct wl_listener destroy;
	struct wl_listener request_move;
	struct wl_listener request_resize;
	int x, y;
};

struct wet_keyboard {
	struct wl_list link;
	struct wet_server *server;
	struct wlr_input_device *device;

	struct wl_listener modifiers;
	struct wl_listener key;
};

bool server_init(struct wet_server *server);

bool server_start(struct wet_server *server);

bool output_init(struct wet_server *server);

void seat_init(struct wet_server *server);

void cursor_init(struct wet_server *server);

void keyboard_init(struct wet_server *server);

void focus_view(struct wet_view *view, struct wlr_surface *surface);

void server_new_xdg_surface(struct wl_listener *listener, void *data);

#endif
