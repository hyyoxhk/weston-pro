srcs_weston_pro = [
	'main.c',
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
	include_directories: public_inc,
	dependencies: deps_weston_pro,
)
