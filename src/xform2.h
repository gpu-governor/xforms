#ifndef UNIXFORM_H
#define UNIXFORM_H

#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//------------------------- GLOBAL VAR -------------------------
Display *xiDisplay;
int xiScreen;
Window xiRoot;

//------------------------- STRUCTURES -------------------------
typedef struct {
    Window win;
    GC gc;
} XiWindow;

typedef struct {
    unsigned char r, g, b;
} Color;

typedef enum {
    FILLED,
    OUTLINE
} DrawMode;

//------------------------- COLOR CONSTANTS -------------------------
#define RED   (Color){255, 0, 0}
#define BLACK (Color){0, 0, 0}
#define GREEN (Color){0, 255, 0}
#define BLUE  (Color){0, 0, 255}
#define GRAY  (Color){128, 128, 128}

// Converts RGB color to 16-bit format for Xlib
static unsigned long convert_color_to_pixel(Color color) {
    return (color.r << 16) | (color.g << 8) | color.b;
}

//------------------------- WINDOW MANAGEMENT -------------------------
XiWindow xiCreateWindow(int width, int height, char *title, int xpos, int ypos) {
    if ((xiDisplay = XOpenDisplay(NULL)) == NULL) {
        fprintf(stderr, "Unable to open X display\n");
        exit(1);
    }

    xiScreen = DefaultScreen(xiDisplay);
    xiRoot = RootWindow(xiDisplay, xiScreen);

    XSetWindowAttributes xva;
    xva.background_pixel = WhitePixel(xiDisplay, xiScreen);
    xva.border_pixel = BlackPixel(xiDisplay, xiScreen);
    xva.event_mask = ButtonPressMask | ExposureMask | KeyPressMask;

    Window win = XCreateWindow(
        xiDisplay, xiRoot, xpos, ypos, width, height, 1,
        DefaultDepth(xiDisplay, xiScreen), InputOutput,
        DefaultVisual(xiDisplay, xiScreen),
        CWBackPixel | CWBorderPixel | CWEventMask, &xva
    );

    XStoreName(xiDisplay, win, title);
    XMapWindow(xiDisplay, win);

    // Create and store a single GC per window
    GC gc = XCreateGC(xiDisplay, win, 0, NULL);

    return (XiWindow){win, gc};
}

void xiDestroyWindow(XiWindow *xw) {
    XFreeGC(xiDisplay, xw->gc);
    XDestroyWindow(xiDisplay, xw->win);
    XCloseDisplay(xiDisplay);
}

//------------------------- GRAPHICS CONTEXT MANAGEMENT -------------------------
void setGCColor(GC gc, Color color) {
    XSetForeground(xiDisplay, gc, convert_color_to_pixel(color));
}

//------------------------- DRAWING FUNCTIONS -------------------------
void xiDrawRectangle(XiWindow *xw, int x, int y, int w, int h, Color color, DrawMode mode) {
    setGCColor(xw->gc, color);

    if (mode == FILLED) {
        XFillRectangle(xiDisplay, xw->win, xw->gc, x, y, w, h);
    } else {
        XDrawRectangle(xiDisplay, xw->win, xw->gc, x, y, w, h);
    }
}

void xiDrawText(XiWindow *xw, int x, int y, const char *text, Color color) {
    setGCColor(xw->gc, color);

    XFontStruct *font = XLoadQueryFont(xiDisplay, "fixed");
    if (font) {
        XSetFont(xiDisplay, xw->gc, font->fid);
    } else {
        fprintf(stderr, "Failed to load font. Using default font.\n");
    }

    XDrawString(xiDisplay, xw->win, xw->gc, x, y, text, strlen(text));

    if (font) {
        XFreeFont(xiDisplay, font);
    }
}

//------------------------- CONTAINER WIDGET -------------------------
typedef struct {
    Window containerWin;
    int x, y, width, height;
    Color backgroundColor;
    bool fixed;
    char *title;
    bool dragging;
    int dragStartX, dragStartY;
    GC gc;
} Container;

Container xiCreateContainer(XiWindow *parent, int x, int y, int width, int height, Color bgColor, bool fixed, char *title) {
    XSetWindowAttributes xva;
    xva.background_pixel = convert_color_to_pixel(bgColor);
    xva.border_pixel = BlackPixel(xiDisplay, xiScreen);
    xva.event_mask = ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ExposureMask;

    Window containerWin = XCreateWindow(
        xiDisplay, parent->win, x, y, width, height,
        0, DefaultDepth(xiDisplay, xiScreen),
        InputOutput, DefaultVisual(xiDisplay, xiScreen),
        CWBackPixel | CWBorderPixel | CWEventMask, &xva
    );

    XMapWindow(xiDisplay, containerWin);

    GC gc = XCreateGC(xiDisplay, containerWin, 0, NULL);

    return (Container){containerWin, x, y, width, height, bgColor, fixed, title, false, 0, 0, gc};
}

void xiRenderContainer(Container *container) {
    setGCColor(container->gc, container->backgroundColor);
    XFillRectangle(xiDisplay, container->containerWin, container->gc, 0, 0, container->width, container->height);

    if (container->title) {
        xiDrawRectangle((XiWindow *)container, 0, 0, container->width, 20, BLACK, FILLED);
        xiDrawText((XiWindow *)container, 5, 15, container->title, GRAY);
    }
}

void xiHandleContainerEvents(Container *container, XEvent *event) {
    if (!container->fixed && event->type == ButtonPress && event->xbutton.y < 20) {
        container->dragging = true;
        container->dragStartX = event->xbutton.x_root;
        container->dragStartY = event->xbutton.y_root;
    } else if (event->type == ButtonRelease) {
        container->dragging = false;
    } else if (container->dragging && event->type == MotionNotify) {
        int dx = event->xmotion.x_root - container->dragStartX;
        int dy = event->xmotion.y_root - container->dragStartY;
        container->x += dx;
        container->y += dy;
        XMoveWindow(xiDisplay, container->containerWin, container->x, container->y);
        container->dragStartX = event->xmotion.x_root;
        container->dragStartY = event->xmotion.y_root;
    }
}

void xiDestroyContainer(Container *container) {
    XFreeGC(xiDisplay, container->gc);
    XDestroyWindow(xiDisplay, container->containerWin);
}

//------------------------- EVENT LOOP -------------------------
void xiUpdate() {
    XEvent xiEvent;
    while (1) {
        XNextEvent(xiDisplay, &xiEvent);
        // Handle events here
    }
}

#endif // UNIXFORM_H
