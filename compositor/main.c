// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2023 He Yong <hyyoxhk@163.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>

#include <weston-pro.h>
#include "shared/helpers.h"

static int
on_term_signal(int signal_number, void *data)
{
	struct wl_display *display = data;

	//printf("caught signal %d\n", signal_number);
	wl_display_terminate(display);

	return 1;
}

int main(int argc, char *argv[]) {
	char *startup_cmd = NULL;
	int ret = EXIT_FAILURE;
	struct wl_display *display;
	struct wl_event_source *signals[3];
	struct wl_event_loop *loop;
	int i;
	struct wet_server server = { 0 };
	sigset_t mask;

	int c;
	while ((c = getopt(argc, argv, "s:h")) != -1) {
		switch (c) {
		case 's':
			startup_cmd = optarg;
			break;
		default:
			printf("Usage: %s [-s startup command]\n", argv[0]);
			return 0;
		}
	}
	if (optind < argc) {
		printf("Usage: %s [-s startup command]\n", argv[0]);
		return 0;
	}


	server.wl_display = display = wl_display_create();
	if (display == NULL) {
		printf("fatal: failed to create display\n");
		goto out_display;
	}

	loop = wl_display_get_event_loop(display);
	signals[0] = wl_event_loop_add_signal(loop, SIGTERM, on_term_signal,
					      display);
	signals[1] = wl_event_loop_add_signal(loop, SIGINT, on_term_signal,
					      display);
	signals[2] = wl_event_loop_add_signal(loop, SIGQUIT, on_term_signal,
					      display);

	if (!signals[0] || !signals[1] || !signals[2])
		goto out_signals;

	/* Xwayland uses SIGUSR1 for communicating with weston. Since some
	   weston plugins may create additional threads, set up any necessary
	   signal blocking early so that these threads can inherit the settings
	   when created. */
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);
	pthread_sigmask(SIG_BLOCK, &mask, NULL);

	if (!server_init(&server))
		return -1;

	if (!server_start(&server))
		return -1;

	if (startup_cmd) {
		if (fork() == 0) {
			execl("/bin/sh", "/bin/sh", "-c", startup_cmd, (void *)NULL);
		}
	}

	wl_display_run(server.wl_display);

	/* Once wl_display_run returns, we shut down the server. */
	wl_display_destroy_clients(server.wl_display);
	wl_display_destroy(server.wl_display);

out_signals:
	for (i = ARRAY_LENGTH(signals) - 1; i >= 0; i--)
		if (signals[i])
			wl_event_source_remove(signals[i]);
out_display:

	return ret;
}
