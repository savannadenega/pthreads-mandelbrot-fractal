//
//  x11.h
//  mandelbrot-fractal


#ifndef x11_h
#define x11_h

#import "X11/Xlib.h"
#import "X11/Xutil.h"

static Display *display;
static Window window;
static GC gc;
static XImage *x_image;

static void x11_init_image(int image_size) {
  int ii, jj;
  int depth = DefaultDepth(display, DefaultScreen(display));
  Visual *visual = DefaultVisual(display, DefaultScreen(display));
  int total;

  /* Determine total bytes needed for image */
  ii = 1;
  jj = (depth - 1) >> 2;
  while (jj >>= 1) ii <<= 1;
  /* Pad the scanline to a multiple of 4 bytes */
  total = image_size * ii;
  total = (total + 3) & ~3;
  total *= image_size;

  x_image = XCreateImage(display, visual, depth, ZPixmap, 0, malloc(total), image_size, image_size, 32, 0);
}

static void x11_init(int image_size) {
  display = XOpenDisplay(NULL);
  window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, image_size, image_size, 0, BlackPixel(display, DefaultScreen(display)),
    WhitePixel(display, DefaultScreen(display)));

  XSelectInput(display, window, StructureNotifyMask);
  XMapWindow(display, window);

  // sem isso alguns quadrados ficam brancos até que o flush aconteça
  while (1) {
    XEvent e;
    XNextEvent(display, &e);
    if (e.type == MapNotify) {
      break;
    }
  }

  char name[128] = "Mandelbrot set with pthreads";
  char *name_pointer = name;
  XTextProperty text_property;
  Status status = XStringListToTextProperty(&name_pointer, 1, &text_property);
  if (status) {
    XSetWMName(display, window, &text_property);
  }

  XFlush(display);
  x11_init_image(image_size);
  gc = XCreateGC(display, window, 0, NULL);
  XSelectInput(display, window, ExposureMask | KeyPressMask);
}

static void x11_destroy() {
  XDestroyImage(x_image);
  XDestroyWindow(display, window);
  XCloseDisplay(display);
}

static void x11_put_image(int src_x, int src_y, int dest_x, int dest_y, int width, int height) {
  XPutImage(display, window, gc, x_image, src_x, src_y, dest_x, dest_y, width, height);
}

static void x11_flush() {
  XFlush(display);
}

static void x11_handle_events(int image_size, void (*transform_coordinates_cb)(int, int, int, int)) {
  while(1) {
    XEvent event;
    KeySym key;

    XNextEvent(display, &event);

    // redraw on expose (resize etc)
    if ((event.type == Expose) && !event.xexpose.count) {
      x11_put_image(0, 0, 0, 0, image_size, image_size);
    }

    // esc to close
    char key_buffer[128];
    if (event.type == KeyPress) {
      XLookupString(&event.xkey, key_buffer, sizeof key_buffer, &key, NULL);

      if (key == XK_Escape) {
        break;
      } else if (key == XK_Up) {
        transform_coordinates_cb(0, 0, -1, -1);
      } else if (key == XK_Right) {
        transform_coordinates_cb(1, 1, 0, 0);
      } else if (key == XK_Down) {
        transform_coordinates_cb(0, 0, 1, 1);
      } else if (key == XK_Left) {
        transform_coordinates_cb(-1, -1, 0, 0);
      } else if (key == XK_m || key == XK_M) {
        transform_coordinates_cb(1, -1, 1, -1);
      } else if (key == XK_n || key == XK_N) {
        transform_coordinates_cb(-1, 1, -1, 1);
      }
    }
  }
}

#endif /* x11_h */
