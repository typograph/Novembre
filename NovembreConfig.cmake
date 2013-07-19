include(CMakeDependentOption)

option(
   NOVEMBRE_LOG
   "Compile support for logging operations. (default=yes)"
   ON
)

CMAKE_DEPENDENT_OPTION(
   NOVEMBRE_VERBOSE_LOG
   "Enable verbose log messages. (default=no)"
   OFF
   "NOVEMBRE_LOG"
   OFF
)

option(
   NOVEMBRE_2DVIEW
   "Compile support for 2D image-like views. (default=yes)"
   ON
)

option(
   NOVEMBRE_GRAPHVIEW
   "Compile support for graph views. (default=yes)"
   ON
)

option(
   NOVEMBRE_3DVIEW
   "Compile support for 3D views. (default=no)"
   OFF
)

if (WIN32)
  option(
   NOVEMBRE_STATIC_PLUGINS
   "Compile plugins statically and link them to the main executable. (defaults to yes on Windows and no otherwise)"
   ON
 )
else(WIN32)
  option(
   NOVEMBRE_STATIC_PLUGINS
   "Compile plugins statically and link them to the main executable. (defaults to yes on Windows and no otherwise)"
   OFF
  )
endif(WIN32)
