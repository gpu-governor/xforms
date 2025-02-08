#include "xform.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <string.h>

#define MAX_TEXT_LENGTH 256

// --------------------------- Text Entry Struct ---------------------------
typedef struct {
    int x, y, width, height;
    char text[MAX_TEXT_LENGTH];
    int cursor_position;
    int text_offset;
    bool active;
    int font_size;
    Color text_color;
    Color background_color;
} TextEntry;

// Initialize a single-line text entry box
TextEntry create_text_entry(int x, int y, int width, int height, int font_size, Color text_color, Color background_color) {
    TextEntry entry = {0};
    entry.x = x;
    entry.y = y;
    entry.width = width;
    entry.height = height;
    entry.font_size = font_size;
    entry.text_color = text_color;
    entry.background_color = background_color;
    entry.active = false;
    entry.cursor_position = 0;
    entry.text_offset = 0;
    memset(entry.text, 0, MAX_TEXT_LENGTH); // Initialize text with empty characters
    return entry;
}

// Render the text entry box with scrolling support
void render_text_entry(TextEntry *entry) {
    // Draw background
    xiDrawRect(NULL, entry->x, entry->y, entry->width, entry->height, entry->background_color, FILLED);

    // Draw border
    xiDrawRect(NULL, entry->x, entry->y, entry->width, entry->height, COLOR_BLUE, OUTLINE);

    // Determine max visible characters (adjust for padding)
    int max_visible_chars = (entry->width - 10) / 10;  // 10px padding on the left side
    int text_length = strlen(entry->text);

    // Adjust text offset (scrolling one char earlier)
    if (entry->cursor_position >= entry->text_offset + max_visible_chars - 1) {
        entry->text_offset = entry->cursor_position - (max_visible_chars - 1);
    } else if (entry->cursor_position < entry->text_offset) {
        entry->text_offset = entry->cursor_position;
    }

    // Get substring to display
    char visible_text[MAX_TEXT_LENGTH];
    strncpy(visible_text, entry->text + entry->text_offset, max_visible_chars);
    visible_text[max_visible_chars] = '\0';

    // Draw only the visible portion of text
    xiDrawText(NULL, entry->x + 5, entry->y + 5, visible_text, entry->text_color, entry->font_size);

    // Draw cursor
    if (entry->active) {
        int cursor_x = entry->x + 5 + ((entry->cursor_position - entry->text_offset) * 10);
        xiDrawRect(NULL, cursor_x, entry->y + 5, 2, entry->font_size, entry->text_color, FILLED);
    }
}

// Update text entry with user input and enable scrolling
void update_text_entry(TextEntry *entry, SDL_Event *event) {
    if (!entry->active) return;

    if (event->type == SDL_TEXTINPUT) {
        int text_length = strlen(entry->text);
        if (text_length < MAX_TEXT_LENGTH - 1) {
            memmove(&entry->text[entry->cursor_position + 1], &entry->text[entry->cursor_position], text_length - entry->cursor_position + 1);
            entry->text[entry->cursor_position] = event->text.text[0];
            entry->cursor_position++;
        }
    } else if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_BACKSPACE && entry->cursor_position > 0) {
            memmove(&entry->text[entry->cursor_position - 1], &entry->text[entry->cursor_position], strlen(entry->text) - entry->cursor_position + 1);
            entry->cursor_position--;
        } else if (event->key.keysym.sym == SDLK_LEFT && entry->cursor_position > 0) {
            entry->cursor_position--;
        } else if (event->key.keysym.sym == SDLK_RIGHT && entry->cursor_position < strlen(entry->text)) {
            entry->cursor_position++;
        }
    }
}

// Handle activation on mouse click
void handle_text_entry_click(TextEntry *entry, SDL_Event *event) {
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        int mx = event->button.x;
        int my = event->button.y;

        if (mx >= entry->x && mx <= entry->x + entry->width &&
            my >= entry->y && my <= entry->y + entry->height) {
            entry->active = true;
        } else {
            entry->active = false;
        }
    }
}

int main() {
    xiWindow window = xiCreateWindow("Text Entry Demo", 800, 600);
   
    SDL_StartTextInput();

    TextEntry textEntry = create_text_entry(100, 100, 300, 40, 16, COLOR_BLUE, COLOR_GREEN);

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            handle_text_entry_click(&textEntry, &event);
            update_text_entry(&textEntry, &event);
        }

        xiClearScreen(&window, (Color){50, 50, 50, 255});
        render_text_entry(&textEntry);
        SDL_RenderPresent(grenderer);
    }

    SDL_StopTextInput();
    xiDestroyWindow(&window);
    return 0;
}