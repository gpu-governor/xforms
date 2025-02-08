#include"xform.h"
// Example usage
int main() {
    xiWindow xiWin = xiCreateWindow("xi SDL Window", 800, 600);

    if (!xiWin.window || !xiWin.renderer) return 1;

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Clear screen
        xiClearScreen(&xiWin, (Color){0, 0, 0, 255});

        // Draw shapes
        xiDrawRect(&xiWin, 50, 50, 200, 100, COLOR_RED, FILLED);
        xiDrawCircle(&xiWin, 400, 300, 50, COLOR_GREEN, OUTLINE);
        xiDrawTriangle(&xiWin, 500, 100, 600, 300, 700, 100, COLOR_BLUE, FILLED);

        // Draw text
        xiDrawText(&xiWin, 50, 200, "Hello, xi Graphics!", COLOR_GREEN, 24);

        SDL_RenderPresent(xiWin.renderer);
    }

    xiDestroyWindow(&xiWin);
    return 0;
}
