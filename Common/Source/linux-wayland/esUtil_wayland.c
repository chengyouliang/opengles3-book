
///
// Includes
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
#include <assert.h>
#include "esUtil.h"

#include <wayland-client.h>
#include <wayland-egl.h>
#include <wayland-cursor.h>
#include "xdg-shell-client-protocol.h"

#ifndef MIN
#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif

struct display {
	struct wl_display *display;
	struct wl_registry *registry;
	struct wl_surface *surface;
	struct wl_compositor *compositor;
	struct xdg_wm_base *wm_base;
	struct xdg_surface *xdg_surface;
	struct xdg_toplevel *xdg_toplevel;
};
static void xdg_wm_base_ping(void *data, struct xdg_wm_base *shell, uint32_t serial)
{
	xdg_wm_base_pong(shell, serial);
}

static const struct xdg_wm_base_listener wm_base_listener = {
	xdg_wm_base_ping,
};

static struct display gdisplay;
#if 0

void output_handle_geometry(void * /*data*/, struct wl_output * /*wl_output*/,
                                           int32_t /*x*/, int32_t /*y*/, int32_t /*physical_width*/,
                                           int32_t /*physical_height*/, int32_t /*subpixel*/,
                                           const char * /*make*/, const char * /*model*/,
                                           int32_t /*transform*/)
{
}

void output_handle_mode(void *data, struct wl_output * /*wl_output*/,
                                       uint32_t flags, int32_t width, int32_t height,
                                       int32_t refresh)
{
    /* Only handle output mode events for the shell's "current" mode */
    if (flags & WL_OUTPUT_MODE_CURRENT) {
        struct my_output *my_output = static_cast<struct my_output *>(data);

        my_output->width = width;
        my_output->height = height;
        my_output->refresh = refresh;
    }
}

void output_handle_done(void * /*data*/, struct wl_output * /*wl_output*/)
{
}

void output_handle_scale(void *data, struct wl_output * /*wl_output*/,int32_t factor)
{
    struct my_output *my_output = static_cast<struct my_output *>(data);
    my_output->scale = factor;
}
const struct wl_output_listener output_listener_ = {
   output_handle_geometry,
   output_handle_mode,
   output_handle_done,
   output_handle_scale
};
#endif
static void registry_handle_global(void *data, struct wl_registry *registry,
		       uint32_t name, const char *interface, uint32_t version)
{
	struct display *d = (struct display *)data;

	if (strcmp(interface, "wl_compositor") == 0) {
		printf("%s %d\n",__FUNCTION__,__LINE__);
		d->compositor =
			wl_registry_bind(registry, name,
					 &wl_compositor_interface,
					 MIN(version, 4));
	} else if (strcmp(interface, "xdg_wm_base") == 0) {
		printf("%s %d\n",__FUNCTION__,__LINE__);
#if 1
		d->wm_base = wl_registry_bind(registry, name,
					      &xdg_wm_base_interface, 1);
		xdg_wm_base_add_listener(d->wm_base, &wm_base_listener, d);
#endif
	}
	else if (strcmp(interface, "wl_output") == 0) {
#if 0
        struct my_output *my_output = new struct my_output();
        memset(my_output, 0, sizeof(*my_output));
        my_output->scale = 1;
        my_output->output =
                static_cast<struct wl_output *>(
                    wl_registry_bind(registry,
                                     id, &wl_output_interface, std::min(version, 2U)));
        that->display_->outputs.push_back(my_output);

        wl_output_add_listener(my_output->output, &output_listener_, my_output);
        wl_display_roundtrip(that->display_->display);
#endif
    }
}

static void
registry_handle_global_remove(void *data, struct wl_registry *registry,
			      uint32_t name)
{
}

static const struct wl_registry_listener registry_listener = {
	registry_handle_global,
	registry_handle_global_remove
};



static void
handle_surface_configure(void *data, struct xdg_surface *surface,
			 uint32_t serial)
{
	struct display *window = data;

	xdg_surface_ack_configure(surface, serial);

}

static const struct xdg_surface_listener xdg_surface_listener = {
	handle_surface_configure
};


static void
handle_toplevel_configure(void *data, struct xdg_toplevel *toplevel,
			  int32_t width, int32_t height,
			  struct wl_array *states)
{
#if 0
	struct window *window = data;
	uint32_t *p;

	window->fullscreen = 0;
	window->maximized = 0;
	wl_array_for_each(p, states) {
		uint32_t state = *p;
		switch (state) {
		case XDG_TOPLEVEL_STATE_FULLSCREEN:
			window->fullscreen = 1;
			break;
		case XDG_TOPLEVEL_STATE_MAXIMIZED:
			window->maximized = 1;
			break;
		}
	}

	if (width > 0 && height > 0) {
		if (!window->fullscreen && !window->maximized) {
			window->window_size.width = width;
			window->window_size.height = height;
		}
		window->geometry.width = width;
		window->geometry.height = height;
	} else if (!window->fullscreen && !window->maximized) {
		window->geometry = window->window_size;
	}

	if (window->native)
		wl_egl_window_resize(window->native,
				     window->geometry.width,
				     window->geometry.height, 0, 0);
#endif
}

static void
handle_toplevel_close(void *data, struct xdg_toplevel *xdg_toplevel)
{
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
	handle_toplevel_configure,
	handle_toplevel_close,
};
///
//  WinCreate()
//
//      This function initialized the native X11 display and window for EGL
//
EGLBoolean WinCreate(ESContext *esContext, const char *title)
{
   
	struct wl_registry *registry;
    struct wl_egl_window *native;
    gdisplay.display = wl_display_connect(NULL);
	assert(gdisplay.display);
	gdisplay.registry = wl_display_get_registry(gdisplay.display);
	wl_registry_add_listener(gdisplay.registry ,&registry_listener, &gdisplay);
    wl_display_roundtrip(gdisplay.display);

	gdisplay.surface = wl_compositor_create_surface(gdisplay.compositor);

	native = wl_egl_window_create(gdisplay.surface,
				     esContext->width,
				     esContext->height);
#if  0
    #esContext->eglDisplay = weston_platform_get_egl_display(EGL_PLATFORM_WAYLAND_KHR,display, NULL);
    #esContext->eglSurface =  weston_platform_create_egl_surface(esContext->eglDisplay,
						   display->egl.conf,
						   native, NULL);
#endif
	gdisplay.xdg_surface = xdg_wm_base_get_xdg_surface(gdisplay.wm_base,
							  gdisplay.surface);
	xdg_surface_add_listener(gdisplay.xdg_surface,
				 &xdg_surface_listener, &gdisplay);

	gdisplay.xdg_toplevel =
		xdg_surface_get_toplevel(gdisplay.xdg_surface);
	xdg_toplevel_add_listener(gdisplay.xdg_toplevel,
				  &xdg_toplevel_listener, &gdisplay);

	xdg_toplevel_set_title(gdisplay.xdg_toplevel, "simple-egl");
    esContext->eglNativeWindow = (EGLNativeWindowType)native;
    esContext->eglNativeDisplay = (EGLNativeDisplayType)gdisplay.display;
    wl_surface_commit(gdisplay.surface);
    return EGL_TRUE;
}

///
//  WinLoop()
//
//      Start main windows loop
//
void WinLoop ( ESContext *esContext )
{
    struct timeval t1, t2;
    struct timezone tz;
    float deltatime;

    gettimeofday ( &t1 , &tz );

    while(1)
    {
        gettimeofday(&t2, &tz);
        deltatime = (float)(t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 1e-6);
        t1 = t2;
        if (esContext->updateFunc != NULL)
            esContext->updateFunc(esContext, deltatime);
        if (esContext->drawFunc != NULL)
            esContext->drawFunc(esContext);

        eglSwapBuffers(esContext->eglDisplay, esContext->eglSurface); 
		wl_display_dispatch_pending(gdisplay.display);  
    }
}

///
//  Global extern.  The application must declsare this function
//  that runs the application.
//
extern int esMain( ESContext *esContext );

///
//  main()
//
//      Main entrypoint for application
//
int main ( int argc, char *argv[] )
{
   ESContext esContext;
   memset ( &esContext, 0, sizeof( esContext ) );

   if ( esMain ( &esContext ) != GL_TRUE )
      return 1;   
   WinLoop ( &esContext );

   if ( esContext.shutdownFunc != NULL )
	   esContext.shutdownFunc ( &esContext );
   if ( esContext.userData != NULL )
	   free ( esContext.userData );

   return 0;
}
///egl', 'wayland-egl', 'glesv2', 'wayland-cursor'