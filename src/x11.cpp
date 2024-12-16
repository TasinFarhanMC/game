#include "glad/gl.h"
#include "shader_src.hpp"
#include "types.hpp"

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <X11/Xlib.h>
#include <cstddef>
#include <iostream>

Display *display = nullptr;
Window window = -1;
GLXContext context = NULL;
Flag running = true;
Signal<bool> v_sync = false;

// {{{ Render
PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB;
PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT;

void render() {
  if (!glXMakeCurrent(display, window, context)) {
    std::cerr << "Failed to make GL context current in render thread." << std::endl;
    return;
  }

  if (!gladLoadGL((GLADloadfunc)glXGetProcAddress)) {
    std::cerr << "Unable to init GLAD" << std::endl;
    return;
  }
  glXSwapIntervalEXT(display, window, 0);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  Buffer base_vbo(GL_ARRAY_BUFFER, {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f}, GL_STATIC_DRAW);
  glBindVertexBuffer(0, base_vbo, 0, sizeof(Vec2));

  Buffer vbo(GL_ARRAY_BUFFER, InitList<Vertex> {{0.0f, 0.0f, 255, 255, 0, 0}, {0.0f, 0.0f, 0, 0, 0, 0}}, GL_STATIC_DRAW);
  glBindVertexBuffer(1, vbo, 0, sizeof(Vertex));
  glVertexBindingDivisor(1, 1);

  glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);
  glEnableVertexAttribArray(0);
  glVertexAttribBinding(0, 0);

  glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
  glEnableVertexAttribArray(1);
  glVertexAttribBinding(1, 1);

  glVertexAttribIFormat(2, 2, GL_UNSIGNED_BYTE, offsetof(Vertex, scale));
  glEnableVertexAttribArray(2);
  glVertexAttribBinding(2, 1);

  Program vert_program(GL_VERTEX_SHADER, vert_src);
  Program frag_program(GL_FRAGMENT_SHADER, frag_src);

  GLuint pipeline;
  glGenProgramPipelines(1, &pipeline);
  glBindProgramPipeline(pipeline);

  glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vert_program);
  glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, frag_program);

  while (running) {
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArraysInstancedBaseInstance(GL_TRIANGLE_FAN, 0, 4, 2, 0);
    glXSwapBuffers(display, window);

    v_sync.handle([](const bool state) { glXSwapIntervalEXT(display, window, state); });
  }

  glDeleteVertexArrays(1, &vao);
  glDeleteProgramPipelines(1, &pipeline);
  glXDestroyContext(display, context);
}
// }}}

int main() {
  // {{{ Init
  display = XOpenDisplay(NULL);
  if (!display) {
    std::cerr << "Unable to open X Display" << std::endl;
    return 1;
  }

  const int screen = DefaultScreen(display);
  const Window root = XRootWindow(display, screen);

  // {{{ Framebuffer Config
  GLXFBConfig frame_buf_config;
  {
    const int visual_attribs[] = {// RGBA Render
                                  GLX_RENDER_TYPE, GLX_RGBA_BIT,
                                  // Drawable Window
                                  GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
                                  // Double Buffer
                                  GLX_DOUBLEBUFFER, True,
                                  // Red Channel
                                  GLX_RED_SIZE, 8,
                                  // Green Channel
                                  GLX_GREEN_SIZE, 8,
                                  // Blue Channel
                                  GLX_BLUE_SIZE, 8,
                                  // Alpha Channel
                                  GLX_ALPHA_SIZE, 8, None
    };

    int frame_buf_count;
    GLXFBConfig *frame_buf_configs = glXChooseFBConfig(display, screen, visual_attribs, &frame_buf_count);
    if (!frame_buf_configs) {
      std::cerr << "Failed to get a framebuffer config" << std::endl;
      XCloseDisplay(display);
      return 1;
    }

    frame_buf_config = frame_buf_configs[0];
    XFree(frame_buf_configs);
  }

  XVisualInfo *visual_info = glXGetVisualFromFBConfig(display, frame_buf_config);
  if (!visual_info) {
    std::cerr << "No visual info found" << std::endl;
    XCloseDisplay(display);
    return 1;
  }

  // }}}
  // {{{ Window
  XSetWindowAttributes window_attribs;
  window_attribs.colormap = XCreateColormap(display, root, visual_info->visual, AllocNone);
  window_attribs.event_mask = ExposureMask | KeyPressMask;
  window_attribs.background_pixel = BlackPixel(display, screen);

  const unsigned int window_size = std::min(DisplayWidth(display, screen), DisplayHeight(display, screen)) * 2 / 3;
  window = XCreateWindow(
      display, root, 0, 0, window_size, window_size, 0, visual_info->depth, InputOutput, visual_info->visual,
      CWColormap | CWBackPixel | CWEventMask, &window_attribs
  );
  XMapWindow(display, window);
  // }}}
  // {{{ Create Context
  glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress((const GLubyte *)"glXCreateContextAttribsARB");
  glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddress((const GLubyte *)("glXSwapIntervalEXT"));

  const int context_attribs[] = {// Major Version
                                 GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
                                 // Minor Version
                                 GLX_CONTEXT_MINOR_VERSION_ARB, 3,
                                 // Profile
                                 GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB, None
  };

  context = glXCreateContextAttribsARB(display, frame_buf_config, nullptr, true, context_attribs);
  if (!context) {
    fprintf(stderr, "Failed to create OpenGL context\n");
    return -1;
  }
  // }}}
  // }}}
  // {{{ Loop
  Thread render_thread(render);

  Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", false);
  XSetWMProtocols(display, window, &wm_delete_window, 1);

  const auto clean = [&render_thread]() {
    running = false;
    render_thread.join();

    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
  };

  XEvent event;
  while (true) {
    XNextEvent(display, &event);

    switch (event.type) {
    case KeyPress:
      switch (XLookupKeysym(&event.xkey, 0)) {
      case XK_F1:
        v_sync.set([](auto value) { return !value; });
        break;
      case XK_Escape:
        return clean();
      }
    case ClientMessage:
      if (event.xclient.data.l[0] == wm_delete_window) { return clean(); }
    }
  }
  // }}}
}
