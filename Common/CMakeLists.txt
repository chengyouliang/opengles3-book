set ( common_src Source/esShader.c 
                 Source/esShapes.c
                 Source/esTransform.c
                 Source/esUtil.c )

add_definitions(-DWL_EGL_PLATFORM)

# Win32 Platform files
if(WIN32)
    set( common_platform_src Source/Win32/esUtil_win32.c )
    add_library( Common STATIC ${common_src} ${common_platform_src} )
    target_link_libraries( Common ${OPENGLES3_LIBRARY} ${EGL_LIBRARY} )
else()
    #find_package(X11)
    find_library(M_LIB m)
    find_library(WAYLAND wayland-egl)
    #set( common_platform_src Source/LinuxX11/esUtil_X11.c )
    set( common_platform_src Source/linux-wayland/esUtil_wayland.c  Source/linux-wayland/xdg-shell-protocol.c)
    add_library( Common STATIC ${common_src} ${common_platform_src} )
    target_link_libraries( Common -lmali -lGLESv2  -lEGL -lwayland-client -lwayland-cursor -lgbm  ${M_LIB} )
endif()

             


