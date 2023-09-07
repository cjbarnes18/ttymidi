project(
  'ttymidi',
  'c',
  version : '0.0.3',
  license : 'GPLv2',
  meson_version : '>= 0.47',
  default_options : ['c_std=gnu99', 'warning_level=2']
)

if get_option('c_std') != 'gnu99'
  error('c_std must be gnu99')
endif

sources = files(
  'src/ttymidi.c',
  'src/term_posix.c',
  'src/termios2.c',
)

incdir = include_directories('inc')

cc = meson.get_compiler('c')
dep_threads = dependency('threads')
dep_alsa = dependency('alsa')

executable('ttymidi', sources,
           include_directories : incdir,
           dependencies : [ dep_threads, dep_alsa ],
           install : true)
