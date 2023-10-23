srcs_tinywl = [
	'tinywl.c',
	xdg_shell_protocol_h,
	xdg_shell_protocol_c,
]

deps_tinywl = [
	dep_wayland_server,
	dep_wlroots,
	dep_xkbcommon,
	dep_pixman,
]

executable(
	'tinywl',
	sources: srcs_tinywl,
	include_directories: public_inc,
	dependencies: deps_tinywl,
)
