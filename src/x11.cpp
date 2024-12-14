#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <iostream>

int main() {
  Display *display = XOpenDisplay(NULL);
  if (!display) {
    std::cerr << "Unable to open X Display [hint set DISPLAY env, and startx]" << std::endl;
    return 1;
  }

  Screen *screen = DefaultScreenOfDisplay(display);
  if (!screen) {
    std::cerr << "Unable to Get Screen of X Display: " << display << std::endl;
    XCloseDisplay(display);
    return 1;
  }

  const int screen_num = XScreenNumberOfScreen(screen);
  const Window root_window = XRootWindow(display, screen_num);
  const Window window = XCreateSimpleWindow(display, root_window, 20, 20, 100, 100, 0, 0, 0x000000);
  XMapWindow(display, window);

  XEvent event;
  XSelectInput(display, window, KeyPressMask);

  Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", false);
  XSetWMProtocols(display, window, &wm_delete_window, 1);

  while (true) {
    XNextEvent(display, &event);

    switch (event.type) {
    case KeyPress:
      switch (XLookupKeysym(&event.xkey, 0)) {
      case XK_Escape:
        goto CLEAN_UP;
      }
    case ClientMessage:
      if (event.xclient.data.l[0] == wm_delete_window) { goto CLEAN_UP; }
      break;
    }
  }

CLEAN_UP:
  XDestroyWindow(display, window);
  XCloseDisplay(display);
  return 0;
}
