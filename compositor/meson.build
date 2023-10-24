srcs_weston_pro = [
	'main.c',
	'server.c',
	'output.c',
	'seat.c',
	'cursor.c',
	'xdg.c',
	'view.c',
	xdg_shell_protocol_h,
	xdg_shell_protocol_c,
]

deps_weston_pro = [
	dep_wayland_server,
	dep_wlroots,
	dep_xkbcommon,
	dep_pixman,
]

executable(
	'weston-pro',
	sources: srcs_weston_pro,
	include_directories: inc_weston_pro,
	dependencies: deps_weston_pro,
)
