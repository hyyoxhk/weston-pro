// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2023 He Yong <hyyoxhk@163.com>
 */

#include <stdio.h>
#include <stdlib.h>

#include <wlr/backend.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/render/allocator.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/types/wlr_output_layout.h>

#include "weston-pro.h"

bool server_init(struct wet_server *server)
{
	struct wlr_compositor *compositor;
	struct wlr_data_device_manager *device_manager;

	/*
	 * The backend is a feature which abstracts the underlying input and
	 * output hardware. The autocreate option will choose the most suitable
	 * backend based on the current environment, such as opening an x11
	 * window if an x11 server is running.
	 */
	server->backend = wlr_backend_autocreate(server->wl_display);
	if (!server->backend) {
		printf("failed to create backend\n");
		goto failed;
	}

	/*
	 * Autocreates a renderer, either Pixman, GLES2 or Vulkan for us. The
	 * user can also specify a renderer using the WLR_RENDERER env var.
	 * The renderer is responsible for defining the various pixel formats it
	 * supports for shared memory, this configures that for clients.
	 */
	server->renderer = wlr_renderer_autocreate(server->backend);
	if (!server->renderer) {
		printf("failed to create renderer\n");
		goto failed;
	}

	wlr_renderer_init_wl_display(server->renderer, server->wl_display);

	/*
	 * Autocreates an allocator for us. The allocator is the bridge between
	 * the renderer and the backend. It handles the buffer creation,
	 * allowing wlroots to render onto the screen
	 */
	server->allocator = wlr_allocator_autocreate(server->backend, server->renderer);
	if (!server->allocator) {
		printf("failed to create allocator\n");
		goto failed;
	}

	wl_list_init(&server->views);

	server->scene = wlr_scene_create();
	if (!server->scene) {
		printf("failed to create scene");
		goto failed;
	}

	if (!output_init(server))
		goto failed;

	/*
	 * Create some hands-off wlroots interfaces. The compositor is
	 * necessary for clients to allocate surfaces and the data device
	 * manager handles the clipboard. Each of these wlroots interfaces has
	 * room for you to dig your fingers in and play with their behavior if
	 * you want.
	 */
	compositor = wlr_compositor_create(server->wl_display, server->renderer);
	if (!compositor) {
		printf("failed to create the wlroots compositor\n");
		goto failed;
	}

	device_manager = wlr_data_device_manager_create(server->wl_display);
	if (!device_manager) {
		printf("failed to create data device manager\n");
		goto failed;
	}

	seat_init(server);

	server->xdg_shell = wlr_xdg_shell_create(server->wl_display);
	if (!server->xdg_shell) {
		printf("failed to create the XDG shell interface\n");
		goto failed;
	}

	server->new_xdg_surface.notify = server_new_xdg_surface;
	wl_signal_add(&server->xdg_shell->events.new_surface, &server->new_xdg_surface);

	return true;
failed:
	return false;
}

bool server_start(struct wet_server *server)
{
	/* Add a Unix socket to the Wayland display. */
	const char *socket = wl_display_add_socket_auto(server->wl_display);
	if (!socket) {
		wlr_backend_destroy(server->backend);
		return false;
	}

	/* Start the backend. This will enumerate outputs and inputs, become the DRM
	 * master, etc */
	if (!wlr_backend_start(server->backend)) {
		wlr_backend_destroy(server->backend);
		wl_display_destroy(server->wl_display);
		return false;
	}

	/* Set the WAYLAND_DISPLAY environment variable to our socket and run the
	 * startup command if requested. */
	setenv("WAYLAND_DISPLAY", socket, true);

	/* Run the Wayland event loop. This does not return until you exit the
	 * compositor. Starting the backend rigged up all of the necessary event
	 * loop configuration to listen to libinput events, DRM events, generate
	 * frame events at the refresh rate, and so on. */
	printf("Running Wayland compositor on WAYLAND_DISPLAY=%s", socket);

	return true;
}
