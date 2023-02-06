add_executable( Hello_Triangle Hello_Triangle.c )
target_link_libraries( Hello_Triangle Common   ${OPENGLES3_LIBRARY} ${EGL_LIBRARY} )
