#include "xform.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
    int x, y, width, height;
    int min_value, max_value;
    int value;
    bool dragging;
} Slider;

// Create a slider
Slider create_slider(int x, int y, int width, int height, int min_value, int max_value, int start_value) {
    Slider slider = {x, y, width, height, min_value, max_value, start_value, false};
    return slider;
}

// Render the slider with a centered value
void render_slider(SDL_Renderer *renderer, Slider *slider) {
    // Draw the bar (track)
    xiDrawRect(NULL, slider->x, slider->y, slider->width, slider->height, COLOR_WHITE, FILLED);

    // Calculate the thumb (handle) position
    float percentage = (float)(slider->value - slider->min_value) / (slider->max_value - slider->min_value);
    int handle_x = slider->x + (int)(percentage * (slider->width - slider->height)); // Keep thumb inside the track

    // Draw the thumb (handle) inside the bar
    xiDrawRect(NULL, handle_x, slider->y, slider->height, slider->height, COLOR_BLUE, FILLED);

    // Render the value inside the thumb
    char value_text[16];
    snprintf(value_text, sizeof(value_text), "%d", slider->value);

    int text_x = handle_x + (slider->height / 4);  // Center inside the thumb
    int text_y = slider->y + (slider->height / 4);

    draw_text(renderer, "FreeMono.ttf", slider->height / 2, text_x, text_y, value_text, COLOR_WHITE);
}

// Update the slider based on mouse input
void update_slider(Slider *slider, SDL_Event *event) {
    int mx = event->motion.x;
    int my = event->motion.y;

    if (event->type == SDL_MOUSEBUTTONDOWN) {
        float percentage = (float)(slider->value - slider->min_value) / (slider->max_value - slider->min_value);
        int handle_x = slider->x + (int)(percentage * (slider->width - slider->height));

        if (mx >= handle_x && mx <= handle_x + slider->height &&
            my >= slider->y && my <= slider->y + slider->height) {
            slider->dragging = true;
        }
    }

    if (event->type == SDL_MOUSEMOTION && slider->dragging) {
        int new_value = slider->min_value + ((mx - slider->x) * (slider->max_value - slider->min_value)) / (slider->width - slider->height);
        if (new_value < slider->min_value) new_value = slider->min_value;
        if (new_value > slider->max_value) new_value = slider->max_value;
        slider->value = new_value;
    }

    if (event->type == SDL_MOUSEBUTTONUP) {
        slider->dragging = false;
    }
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) {
        return 1;
    }

    xiWindow window = xiCreateWindow("Slider Widget", 800, 600);
    SDL_Renderer *renderer = grenderer;  // Assuming xi provides this renderer

    Slider slider = create_slider(100, 200, 300, 30, 0, 100, 50);
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            update_slider(&slider, &event);
        }

        xiClearScreen(&window, (Color){30, 30, 30, 255});
        render_slider(renderer, &slider);
        SDL_RenderPresent(renderer);
    }

    xiDestroyWindow(&window);
    SDL_Quit();
    return 0;
}