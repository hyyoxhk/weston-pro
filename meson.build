project('weston-pro',
	'c',
	version: '1.0.0',
	default_options: [
		'warning_level=3',
		'c_std=gnu99',
		'b_lundef=true',
	],
	meson_version: '>= 0.52.1',
	license: 'MIT/Expat',
)

inc_public = include_directories('.')
inc_weston_pro = [ include_directories('include'), inc_public ]

config_h = configuration_data()

cc = meson.get_compiler('c')

global_args = []
global_args_maybe = [
	'-Wmissing-prototypes',
	'-Wno-unused-parameter',
	'-Wno-missing-field-initializers',
	'-Wno-pedantic',
	'-Wundef',
	'-fvisibility=hidden',
	'-DWLR_USE_UNSTABLE',
]
foreach a : global_args_maybe
	if cc.has_argument(a)
		global_args += a
	endif
endforeach
add_global_arguments(global_args, language: 'c')

config_h.set('_GNU_SOURCE', '1')
config_h.set('_ALL_SOURCE', '1')

dep_wlroots = dependency('wlroots', version: ['>= 0.15.1', '< 0.16.2'])
dep_wayland_server = dependency('wayland-server', version: '>= 1.20.0')

dep_xkbcommon = dependency('xkbcommon', version: '>= 0.3.0')
if dep_xkbcommon.version().version_compare('>= 0.5.0')
	config_h.set('HAVE_XKBCOMMON_COMPOSE', '1')
endif

dep_pixman = dependency('pixman-1', version: '>= 0.25.2')

subdir('protocol')
subdir('compositor')

configure_file(output: 'config.h', configuration: config_h)
