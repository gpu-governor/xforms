#include "xform.h"
#include <SDL2/SDL.h>

#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <string.h>

#define MAX_TEXT_LENGTH 256

// ---------------- Label Structure ----------------
typedef struct {
    int x, y, width, height;
    const char *text;
    Color text_color;
    Color background_color; // Can be transparent
} Label;

// ---------------- Button Structure ----------------
typedef struct {
    int x, y, width, height;
    const char *text;
    Color text_color;
    Color background_color;
    Color hover_color;
    Color click_color;
    bool hovered;
    bool clicked;
} Button;

// ---------------- Text Structure ----------------
typedef struct {
    int x, y;
    const char *text;
    Color text_color;
} Text;

// ---------------- Label Functions ----------------
Label create_label(int x, int y, int width, int height, const char *text, Color text_color, Color background_color) {
    Label label = {x, y, width, height, text, text_color, background_color};
    return label;
}

void render_label(Label *label) {
    if (label->background_color.a != 0) {  // If not transparent
        xiDrawRect(NULL, label->x, label->y, label->width, label->height, label->background_color, FILLED);
    }
    xiDrawText(NULL, label->x + 5, label->y + 5, label->text, label->text_color, 16);
}

// ---------------- Button Functions ----------------
Button create_button(int x, int y, int width, int height, const char *text, Color text_color, Color background_color, Color hover_color, Color click_color) {
    Button button = {x, y, width, height, text, text_color, background_color, hover_color, click_color, false, false};
    return button;
}

void render_button(Button *button) {
    Color current_color = button->background_color;
    if (button->clicked) {
        current_color = button->click_color;
    } else if (button->hovered) {
        current_color = button->hover_color;
    }

    xiDrawRect(NULL, button->x, button->y, button->width, button->height, current_color, FILLED);
    xiDrawText(NULL, button->x + 10, button->y + 10, button->text, button->text_color, 16);
}

void update_button(Button *button, SDL_Event *event) {
    int mx = event->motion.x;
    int my = event->motion.y;

    if (event->type == SDL_MOUSEMOTION) {
        button->hovered = (mx >= button->x && mx <= button->x + button->width &&
                           my >= button->y && my <= button->y + button->height);
    }
    if (event->type == SDL_MOUSEBUTTONDOWN && button->hovered) {
        button->clicked = true;
    }
    if (event->type == SDL_MOUSEBUTTONUP) {
        button->clicked = false;
    }
}

// ---------------- Text Functions ----------------
Text create_text(int x, int y, const char *text, Color text_color) {
    Text txt = {x, y, text, text_color};
    return txt;
}

void render_text(Text *text) {
    xiDrawText(NULL, text->x, text->y, text->text, text->text_color, 16);
}

// ---------------- Main Program ----------------
int main() {
    xiWindow window = xiCreateWindow("UI Widgets", 800, 600);

    Label label = create_label(50, 50, 200, 40, "Hello, Label!", COLOR_WHITE, COLOR_RED); 
   // (Color){0, 0, 0, 0}// Transparent background
    Button button = create_button(50, 120, 200, 50, "Click Me", COLOR_WHITE, COLOR_BLUE, COLOR_DARK_BLUE, COLOR_RED);
    Text text = create_text(50, 200, "This is just text.", COLOR_YELLOW);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            update_button(&button, &event);
        }

        xiClearScreen(&window, (Color){30, 30, 30, 255});

        render_label(&label);
        render_button(&button);
        render_text(&text);

        SDL_RenderPresent(grenderer);
    }

    xiDestroyWindow(&window);
    return 0;
}