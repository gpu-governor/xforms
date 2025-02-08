#include "xform.h"  // Header for the main functions
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>


int main(int argc, char *argv[]) {
    xiWindow xiWin = xiCreateWindow("GUI Container Example", 800, 600);
 
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
        xiDrawRect(&containerWithTitle, 50, 50, 200, 100, COLOR_GREEN, FILLED);
        drawContainer(&xiWin, &containerWithTitle);
        drawContainer(&xiWin, &containerWithoutTitle);

        // Present the renderer
        SDL_RenderPresent(grenderer);
    }

    xiDestroyWindow(&xiWin);
    return 0;
}
