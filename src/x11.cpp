// {{{ Includes
#include "glad/gl.h"
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <X11/Xlib.h>

#include <algorithm>
#include <atomic>
#include <glm/glm.hpp>

#include <iostream>
#include <thread>

using IVec2 = glm::ivec2;
template <typename T> using Atomic = std::atomic<T>;
using Thread = std::thread;
// }}}
// {{{ Variables
Display *display = nullptr;
Window window = -1;
GLXContext context = NULL;
Atomic<bool> running = false;
// }}}

GLuint compileShader(GLenum type, const char *source) {
  GLuint shader = glCreateShader(type);     // Create the shader object
  glShaderSource(shader, 1, &source, NULL); // Attach source code
  glCompileShader(shader);                  // Compile the shader

  // Check for compilation errors
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    return 0;
  }
  return shader;
}

int main() {
  // {{{ Create Window
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
    const int visual_attribs[] = {
        GLX_RENDER_TYPE,
        GLX_RGBA_BIT,
        GLX_DRAWABLE_TYPE,
        GLX_WINDOW_BIT,
        GLX_DOUBLEBUFFER,
        True,
        GLX_RED_SIZE,
        8,
        GLX_GREEN_SIZE,
        8,
        GLX_BLUE_SIZE,
        8,
        GLX_ALPHA_SIZE,
        8,
        None
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
  // }}}

  XVisualInfo *visual_info = glXGetVisualFromFBConfig(display, frame_buf_config);
  if (!visual_info) {
    std::cerr << "No visual info found" << std::endl;
    XCloseDisplay(display);
    return 1;
  }

  // {{{ Window Attribs
  XSetWindowAttributes window_attribs;
  window_attribs.colormap = XCreateColormap(display, root, visual_info->visual, AllocNone);
  window_attribs.event_mask = ExposureMask | KeyPressMask;
  window_attribs.background_pixel = BlackPixel(display, screen);

  const unsigned int window_size =
      std::min(DisplayWidth(display, screen), DisplayHeight(display, screen)) * 2 / 3;
  window = XCreateWindow(
      display, root, 0, 0, window_size, window_size, 0, visual_info->depth, InputOutput, visual_info->visual,
      CWColormap | CWBackPixel | CWEventMask, &window_attribs
  );
  XMapWindow(display, window);
  // }}}

  // }}}
  // {{{ Create Context
  auto glXCreateContextAttribsARB =
      (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress((const GLubyte *)"glXCreateContextAttribsARB");
  auto glXSwapIntervalEXT =
      (PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddress((const GLubyte *)("glXSwapIntervalEXT"));

  const int context_attribs[] = {
      GLX_CONTEXT_MAJOR_VERSION_ARB,    3,   GLX_CONTEXT_MINOR_VERSION_ARB, 3, GLX_CONTEXT_PROFILE_MASK_ARB,
      GLX_CONTEXT_CORE_PROFILE_BIT_ARB, None
  };

  context = glXCreateContextAttribsARB(display, frame_buf_config, nullptr, true, context_attribs);
  if (!context) {
    fprintf(stderr, "Failed to create OpenGL context\n");
    return -1;
  }
  // }}}
  // {{{ Render
  if (!glXMakeCurrent(display, window, context)) {
    std::cerr << "Failed to make GL context current in render thread." << std::endl;
  }

  if (!gladLoadGL((GLADloadfunc)glXGetProcAddress)) { std::cerr << "Unable to init GLAD" << std::endl; }

  const char *const vert_src = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;  // Change to vec2 to match your data

        void main() {
            gl_Position = vec4(aPos, 0.0f, 1.0f);  // Add Z and W components for vec4
        }
    )";

  const char *const frag_src = R"(
        #version 330 core

        out vec4 color;

        void main() {
            color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
    )";

  const GLuint vert_shader = compileShader(GL_VERTEX_SHADER, vert_src);
  const GLuint frag_shader = compileShader(GL_FRAGMENT_SHADER, frag_src);
  const GLuint program = glCreateProgram();

  glAttachShader(program, vert_shader);
  glAttachShader(program, frag_shader);

  glLinkProgram(program);

  GLint success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    std::cerr << "ERROR::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
  }

  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);

  GLuint vao, vbo;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);

  const float data[] = {// Triangle 1
                        -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,

                        // Triangle 2
                        -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f
  };

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

  // Enable vertex attribute array at location 0
  glEnableVertexAttribArray(0);

  // Correct the pointer to the vertex data
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);

  glUseProgram(program);

  // }}}
  // {{{ Cleanup
  auto clean = [=](int code = 0) {
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
    glXDestroyContext(display, context);

    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return code;
  };
  // }}}
  // {{{ Loop
  Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", false);
  XSetWMProtocols(display, window, &wm_delete_window, 1);

  XEvent event;
  while (true) {
    XNextEvent(display, &event);

    glClear(GL_COLOR_BUFFER_BIT);     // Step 1: Clear the screen
    glDrawArrays(GL_TRIANGLES, 0, 6); // Step 2: Render your content
    glXSwapBuffers(display, window);  // Step 3: Present the rendered frame

    switch (event.type) {
    case KeyPress:
      switch (XLookupKeysym(&event.xkey, 0)) {
      case XK_Escape:
        return clean();
      }
    case ClientMessage:
      if (event.xclient.data.l[0] == wm_delete_window) { return clean(); }
    }
  }
  // }}}
}
