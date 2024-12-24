#include "render.hpp"
#include <window.hpp>

AtomicFlag running = true;

void render() {
  if (!load_context()) {
    std::cerr << "Failed to load GL context" << std::endl;
    return;
  }

  if (!gladLoaderLoadGL()) {
    std::cerr << "Unable to init GLAD" << std::endl;
    return;
  }

  while (running.test_and_set()) {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.5f, 4.0f, 3.0f, 1.0f);
    swap_buffers();
  }

  unload_context();
}
