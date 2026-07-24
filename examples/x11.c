#define GLCC_IMPLEMENTATION
#include "glcc.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

#define GL_MAJOR_VERSION 0x821B
#define GL_MINOR_VERSION 0x821C

#define GL_COLOR_BUFFER_BIT 0x00004000

void(*glGetIntegerv)(unsigned int, int*);
void(*glClear)(unsigned int);
void(*glClearColor)(float, float, float, float);

int main() {
  Display *dpy = XOpenDisplay(NULL);
  Window root_window = XDefaultRootWindow(dpy);

  XSetWindowAttributes window_attribs = {};
  window_attribs.event_mask = KeyPressMask | KeyReleaseMask;
  Window window = XCreateWindow(dpy, root_window,
      0, 0, 800, 600,
      0, CopyFromParent, CopyFromParent, CopyFromParent,
      CWBackPixel | CWEventMask, &window_attribs);
  XMapWindow(dpy, window);

  Atom wm_delete_message = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(dpy, window, &wm_delete_message, 1);

  assert(glcc_init(GLCC_PLATFORM_X11, dpy) && "failed to initialize glcc");

  glcc_Context ctx = glcc_create_context((void*)window);
  assert(ctx && "failed to create context");
  glcc_make_current(ctx);
  printf("%p\n", ctx);
  glGetIntegerv = glcc_get_proc_address("glGetIntegerv");
  glClearColor = glcc_get_proc_address("glClearColor");
  glClear = glcc_get_proc_address("glClear");
  int major, minor;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);
  printf("loaded OpenGL %d.%d\n", major, minor);

  glClearColor(0.1, 0.3, 0.3, 1.0);
  int running = 1;
  while (running) {
    XEvent e = {};
    XNextEvent(dpy, &e);
    switch (e.type) {
      case KeyPress:
        if (e.xkey.keycode == XKeysymToKeycode(dpy, XK_Escape)) {
          running = 0;
        }
        break;
      case KeyRelease:
        break;
      case ClientMessage:
        if (e.xclient.data.l[0] == wm_delete_message) running = 0;
        break;
    }

    glClear(GL_COLOR_BUFFER_BIT);

    glcc_swap_buffers(ctx);
  }

  return 0;
}
