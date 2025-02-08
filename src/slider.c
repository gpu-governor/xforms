#include "xform.h"
#include <SDL2/SDL.h>

#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

typedef struct {
    int x, y, width;
    int min_value, max_value;
    int value;
    bool dragging;
} Slider;

// Create a slider
Slider create_slider(int x, int y, int width, int min_value, int max_value, int start_value) {
    Slider slider = {x, y, width, min_value, max_value, start_value, false};
    return slider;
}

// Render the slider
void render_slider(Slider *slider) {
    // Draw the track
    xiDrawRect(NULL, slider->x, slider->y + 5, slider->width, 4, COLOR_WHITE, FILLED);

    // Calculate the handle position
    float percentage = (float)(slider->value - slider->min_value) / (slider->max_value - slider->min_value);
    int handle_x = slider->x + (int)(percentage * slider->width) - 5;

    // Draw the handle
    xiDrawRect(NULL, handle_x, slider->y, 10, 14, COLOR_BLUE, FILLED);
}

// Update the slider based on mouse input
void update_slider(Slider *slider, SDL_Event *event) {
    int mx = event->motion.x;
    int my = event->motion.y;

    // Handle mouse click on the slider handle
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        float percentage = (float)(slider->value - slider->min_value) / (slider->max_value - slider->min_value);
        int handle_x = slider->x + (int)(percentage * slider->width) - 5;

        if (mx >= handle_x && mx <= handle_x + 10 && my >= slider->y && my <= slider->y + 14) {
            slider->dragging = true;
        }
    }

    // Handle mouse movement while dragging
    if (event->type == SDL_MOUSEMOTION && slider->dragging) {
        int new_value = slider->min_value + ((mx - slider->x) * (slider->max_value - slider->min_value)) / slider->width;
        if (new_value < slider->min_value) new_value = slider->min_value;
        if (new_value > slider->max_value) new_value = slider->max_value;
        slider->value = new_value;
    }

    // Stop dragging when mouse is released
    if (event->type == SDL_MOUSEBUTTONUP) {
        slider->dragging = false;
    }
}

int main() {
    xiWindow window = xiCreateWindow("Slider Widget", 800, 600);

    Slider slider = create_slider(100, 150, 300, 0, 100, 50);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            update_slider(&slider, &event);
        }

        xiClearScreen(&window, (Color){30, 30, 30, 255});

        render_slider(&slider);

        SDL_RenderPresent(grenderer);
    }

    xiDestroyWindow(&window);
    return 0;
}