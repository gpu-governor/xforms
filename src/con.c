#include "xform.h"  // Header for the main functions
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

// Container widget structure
typedef struct {
    int x, y, width, height;
    const char *title;
    Color color;
    bool movable;
} xiContainer;

// Create a new container instance
xiContainer createContainer(int x, int y, int width, int height, Color color, const char *title, bool movable) {
    xiContainer container;
    container.x = x;
    container.y = y;
    container.width = width;
    container.height = height;
    container.color = color;
    container.title = title;
    container.movable = movable;
    return container;
}

// Render the container on the screen
void drawContainer(xiWindow *xiWin, xiContainer *container) {
    int x = container->x;
    int y = container->y;
    int width = container->width;
    int height = container->height;

    // Draw the main container rectangle (filled)
    xiDrawRect(xiWin, x, y, width, height, container->color, FILLED);

    // Handle title bar and title if a valid title is provided
    if (container->title && strlen(container->title) > 0) {
        int titleBarHeight = 30;  // Fixed height for the title bar
        Color titleBarColor = {200, 200, 200, 255};

        // Draw title bar
        xiDrawRect(xiWin, x, y, width, titleBarHeight, titleBarColor, FILLED);

        // Draw title text centered vertically within the title bar
        int textX = x + 10;
        int textY = y + (titleBarHeight / 4);  // Simple vertical alignment
        xiDrawText(xiWin, textX, textY, container->title, COLOR_BLUE, 16);

        // Adjust the container rectangle position if title bar exists
        y += titleBarHeight;
        height -= titleBarHeight;
    }

    // Redraw container outline after adjusting for title bar
    xiDrawRect(xiWin, x, y, width, height, container->color, OUTLINE);
}

// Handle container movement if movable is true
void handleContainerMovement(xiContainer *container, SDL_Event *event) {
    static bool dragging = false;
    static int offsetX = 0, offsetY = 0;

    if (!container->movable) return;

    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        int mouseX = event->button.x;
        int mouseY = event->button.y;

        // Check if click is within the title bar
        if (mouseX >= container->x && mouseX <= container->x + container->width &&
            mouseY >= container->y && mouseY <= container->y + 30) {
            dragging = true;
            offsetX = mouseX - container->x;
            offsetY = mouseY - container->y;
        }
    } else if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) {
        dragging = false;
    } else if (event->type == SDL_MOUSEMOTION && dragging) {
        container->x = event->motion.x - offsetX;
        container->y = event->motion.y - offsetY;
    }
}

int main(int argc, char *argv[]) {
    xiWindow xiWin = xiCreateWindow("GUI Container Example", 800, 600);
    if (!xiWin.window || !xiWin.renderer) {
        printf("Failed to create window or renderer.\n");
        return 1;
    }

    bool running = true;
    SDL_Event event;

    // Create a container with a title and movable
    xiContainer containerWithTitle = createContainer(100, 100, 400, 300, COLOR_RED, "My Container", true);

    // Create a container without a title and not movable
    xiContainer containerWithoutTitle = createContainer(150, 450, 300, 200, COLOR_GREEN, NULL, false);

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            // Handle container movement
            handleContainerMovement(&containerWithTitle, &event);
        }

        // Clear the screen
        xiClearScreen(&xiWin, COLOR_BLUE);

        // Draw the containers
        drawContainer(&xiWin, &containerWithTitle);
        drawContainer(&xiWin, &containerWithoutTitle);

        // Present the renderer
        SDL_RenderPresent(xiWin.renderer);
    }

    xiDestroyWindow(&xiWin);
    return 0;
}
