#include "xform2.h"

int main() {
    // Create the main application window
    XiWindow mainWindow = xiCreateWindow(800, 600, "XiForm - Simple Container", 100, 100);

    // Create a container inside the main window
    Container myContainer = xiCreateContainer(&mainWindow, 50, 50, 300, 200, GRAY, false, "My Container");

    // Render the container (background and title bar)
    xiRenderContainer(&myContainer);

    // Draw a rectangle inside the container
    xiDrawRectangle((XiWindow *)&myContainer, 20, 40, 100, 50, BLUE, FILLED);

    // Event loop
    bool running = true;
    while (running) {
        XEvent event;
        XNextEvent(xiDisplay, &event);

        switch (event.type) {
            case Expose:
                // Redraw the container and its contents on expose event
                xiRenderContainer(&myContainer);
                xiDrawRectangle((XiWindow *)&myContainer, 20, 40, 100, 50, BLUE, FILLED);
                break;
            
            case KeyPress:
                // Close the window if the user presses 'q'
                if (XLookupKeysym(&event.xkey, 0) == 'q') {
                    running = false;
                }
                break;
            
            case ClientMessage:
                // Handle window close event
                running = false;
                break;
        }
    }

    // Cleanup
    xiDestroyContainer(&myContainer);
    xiDestroyWindow(&mainWindow);

    return 0;
}
