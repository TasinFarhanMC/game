#include "window.hpp"
#include <render.hpp>

#include <betr/namespace.hpp>

#include <betr/chrono.hpp>
#include <betr/thread.hpp>
#include <glm/vec2.hpp>

#include <iostream>

#include <glad/gl.h>
#include <glad/glx.h>
#include <unistd.h>

constexpr NanoSeconds tick = duration_cast<NanoSeconds<>>(Seconds<float>(1.0 / 30.0f));

AtomicFlag running;
Display *display;
Window window;
GLXContext context;

int load_context() { return glXMakeCurrent(display, window, context); }
void unload_context() { glXMakeCurrent(display, None, None); }
void swap_buffers() { glXSwapBuffers(display, window); }

int main() {
  display = XOpenDisplay(NULL);
  if (!display) {
    std::cerr << "Unable to connect to X Server" << std::endl;
    return 1;
  }

  const int screen = DefaultScreen(display);
  const Window root = RootWindow(display, screen);
  const IVec2 display_size = {DisplayWidth(display, screen), DisplayHeight(display, screen)};
  const IVec2 window_size = display_size * 2 / 3;
  const IVec2 window_pos = (display_size - window_size) / 2;

  Visual *visual = DefaultVisual(display, screen);
  const Colormap colormap = XCreateColormap(display, root, visual, AllocNone);

  XSetWindowAttributes window_attributes;
  window_attributes.event_mask = ExposureMask | KeyPressMask;
  window_attributes.colormap = colormap;
  window_attributes.background_pixel = BlackPixel(display, screen);

  window = XCreateWindow(
      display, root, window_pos.x, window_pos.y, window_size.x, window_size.y, 0, DefaultDepth(display, screen), InputOutput, visual,
      CWBackPixel | CWColormap | CWEventMask, &window_attributes
  );
  if (!window) {
    std::cerr << "Unable to create X Window" << std::endl;
    return 1;
  }

  XMapWindow(display, window);
  XStoreName(display, window, "Game");

  if (!gladLoaderLoadGLX(display, screen)) {
    std::cerr << "Unable to load GLX" << std::endl;
    return 1;
  }

  const GLint visual_attributes[] = {GLX_RENDER_TYPE, GLX_RGBA_BIT, GLX_DOUBLEBUFFER, 1, None};

  int num_fbc = 0;
  GLXFBConfig *fbc = glXChooseFBConfig(display, screen, visual_attributes, &num_fbc);

  const GLint context_attributes[] = {GLX_CONTEXT_MAJOR_VERSION_ARB,    3,   GLX_CONTEXT_MINOR_VERSION_ARB, 3, GLX_CONTEXT_PROFILE_MASK_ARB,
                                      GLX_CONTEXT_CORE_PROFILE_BIT_ARB, None};

  context = glXCreateContextAttribsARB(display, fbc[0], NULL, 1, context_attributes);
  if (!context) {
    std::cerr << "Unable to create GL Context" << std::endl;
    return 1;
  }

  running.test_and_set();
  Thread render_thread(render);

  TimePoint start = SteadyClock::now();
  while (running.test()) {
    while (XPending(display)) {
      XEvent event;
      XNextEvent(display, &event);

      switch (event.type) {
      case KeyPress:
        switch (XLookupKeysym(&event.xkey, 0)) {
        case XK_Escape:
          running.clear();
          break;
        }
        break;
      }
    }

    this_thread::sleep_until(start + tick);
    start = SteadyClock::now();
  }

  render_thread.join();
  glXDestroyContext(display, context);

  XDestroyWindow(display, window);
  XFreeColormap(display, colormap);
  XCloseDisplay(display);

  gladLoaderUnloadGLX();
  gladLoaderUnloadGL();
  return 0;
}
