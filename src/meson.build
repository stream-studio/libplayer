gstreamerdep = dependency('gstreamer-1.0')
gstreamervideodep = dependency('gstreamer-video-1.0')
gstreamerbasedep = dependency('gstreamer-base-1.0')

libplayerdeps = [gstreamerdep, gstreamerbasedep, gstreamervideodep]

player = library('player', ['libplayer.cpp'],
            dependencies: libplayerdeps 
)

libplayerdep = declare_dependency(dependencies: libplayerdeps, include_directories : include_directories('.'), link_with : player)