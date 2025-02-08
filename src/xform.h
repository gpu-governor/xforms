#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

/// ============================ WINDOW STRUCT ============================
typedef struct {
    TTF_Font *defaultFont;
} xiWindow;

SDL_Window *gwindow;
SDL_Renderer *grenderer;

/// ============================ COLOR STRUCT ============================
typedef struct {
    Uint8 r, g, b, a;
} Color;

/// ============================ ENUMS ============================
typedef enum { FILLED, OUTLINE } ShapeType;

/// ============================ COLOR CONSTANTS ============================
const Color COLOR_RED = {255, 0, 0, 255};
const Color COLOR_GREEN = {0, 255, 0, 255};
const Color COLOR_BLUE = {0, 0, 255, 255};
const Color COLOR_WHITE ={255, 255, 255};
const Color COLOR_YELLOW = { 255, 255, 0 };
const Color COLOR_DARK_BLUE = { 0,0, 139 };


/// ===============================


/// ============================ LOCAL FUNCTIONS ============================
static void draw_rect(SDL_Renderer *renderer, int x, int y, int width, int height, Color color, ShapeType type) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect rect = {x, y, width, height};
    if (type == FILLED) {
        SDL_RenderFillRect(renderer, &rect);
    } else {
        SDL_RenderDrawRect(renderer, &rect);
    }
}

static void draw_text(SDL_Renderer *renderer, const char *fontPath, int fontSize, int x, int y, const char *text, Color color) {
    TTF_Font *font = TTF_OpenFont(fontPath, fontSize);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return;
    }

    SDL_Color sdlColor = {color.r, color.g, color.b, color.a};
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, text, sdlColor);
    if (!textSurface) {
        SDL_Log("Failed to create text surface: %s", TTF_GetError());
        TTF_CloseFont(font);
        return;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        SDL_Log("Failed to create text texture: %s", SDL_GetError());
        SDL_FreeSurface(textSurface);
        TTF_CloseFont(font);
        return;
    }

    SDL_Rect destRect = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &destRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
}

static void draw_circle(SDL_Renderer *renderer, int x, int y, int radius, Color color, ShapeType type) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    int offsetX = 0, offsetY = radius;
    int d = 1 - radius;

    while (offsetX <= offsetY) {
        if (type == FILLED) {
            SDL_RenderDrawLine(renderer, x - offsetX, y + offsetY, x + offsetX, y + offsetY);
            SDL_RenderDrawLine(renderer, x - offsetX, y - offsetY, x + offsetX, y - offsetY);
            SDL_RenderDrawLine(renderer, x - offsetY, y + offsetX, x + offsetY, y + offsetX);
            SDL_RenderDrawLine(renderer, x - offsetY, y - offsetX, x + offsetY, y - offsetX);
        } else {
            SDL_RenderDrawPoint(renderer, x + offsetX, y + offsetY);
            SDL_RenderDrawPoint(renderer, x + offsetX, y - offsetY);
            SDL_RenderDrawPoint(renderer, x - offsetX, y + offsetY);
            SDL_RenderDrawPoint(renderer, x - offsetX, y - offsetY);
            SDL_RenderDrawPoint(renderer, x + offsetY, y + offsetX);
            SDL_RenderDrawPoint(renderer, x + offsetY, y - offsetX);
            SDL_RenderDrawPoint(renderer, x - offsetY, y + offsetX);
            SDL_RenderDrawPoint(renderer, x - offsetY, y - offsetX);
        }

        if (d < 0) {
            d += 2 * offsetX + 3;
        } else {
            d += 2 * (offsetX - offsetY) + 5;
            offsetY--;
        }
        offsetX++;
    }
}

static void draw_triangle(SDL_Renderer *renderer, int x1, int y1, int x2, int y2, int x3, int y3, Color color, ShapeType type) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    if (type == FILLED) {
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
        SDL_RenderDrawLine(renderer, x3, y3, x1, y1);
    } else {
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
        SDL_RenderDrawLine(renderer, x3, y3, x1, y1);
    }
}

static void clear_screen(SDL_Renderer *renderer, Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

/// ============================ WINDOW FUNCTIONS ============================
// Create and initialize the SDL window and renderer
xiWindow xiCreateWindow(const char *title, int width, int height) {
    xiWindow xiWin = {NULL, NULL, NULL};

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return xiWin;
    }

    if (TTF_Init() == -1) {
        SDL_Log("Unable to initialize SDL_ttf: %s", TTF_GetError());
        SDL_Quit();
        return xiWin;
    }

    gwindow = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!gwindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return xiWin;
    }

    grenderer = SDL_CreateRenderer(gwindow, -1, SDL_RENDERER_ACCELERATED);
    if (!grenderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(gwindow);
        SDL_Quit();
        gwindow = NULL;
        return xiWin;
    }

    xiWin.defaultFont = NULL;  // No default font to preload
    return xiWin;
}

// Destroy the SDL window and renderer
void xiDestroyWindow(xiWindow *xiWin) {
    if (xiWin->defaultFont) {
        TTF_CloseFont(xiWin->defaultFont);
    }
    if (grenderer) {
        SDL_DestroyRenderer(grenderer);
    }
    if (gwindow) {
        SDL_DestroyWindow(gwindow);
    }
    TTF_Quit();
    SDL_Quit();
}

/// ============================ DRAW FUNCTIONS ============================

// Draw a rectangle
void xiDrawRect(void *parent, int x, int y, int width, int height, Color color, ShapeType type) {
   // apply_offset(parent, &x, &y);
    draw_rect(grenderer, x, y, width, height, color, type);
}

// Draw text
void xiDrawText(void *parent, int x, int y, const char *text, Color color, int fontSize) {
    draw_text(grenderer, "FreeMono.ttf", fontSize, x, y, text, color);
}

// Draw a circle using midpoint algorithm
void xiDrawCircle(void *parent, int x, int y, int radius, Color color, ShapeType type) {
    draw_circle(grenderer, x, y, radius, color, type);
}

// Draw a triangle
void xiDrawTriangle(void *parent, int x1, int y1, int x2, int y2, int x3, int y3, Color color, ShapeType type) {
    draw_triangle(grenderer, x1, y1, x2, y2, x3, y3, color, type);
}

// Clear the screen with a specific color
void xiClearScreen(xiWindow *xiWin, Color color) {
    clear_screen(grenderer, color);
}


//============================= CONTAINER ===========================================
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

//==================== WIDGETS ==================

///--------------- Rect ------
typedef struct {
    int x;
    int y;
    int width;
    int height;
    Color color;
    ShapeType type;
    xiContainer* parent;
} Rect;

Rect CreateRectangle(int x, int y, int width, int height, Color color, ShapeType type) {
    Rect rect;
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    rect.color = color;
    rect.type = type;
    rect.parent = NULL; // Initialize parent to NULL
    return rect;
}

// Draw a rectangle
void render_rect(Rect rect) {
    int x, y;

    if (rect.parent) { // Check if rect is inside a container
        x = rect.parent->x + rect.x; 
        y = rect.parent->y + rect.y; 
    } else {
        x = rect.x;
        y = rect.y;
    }

    draw_rect(grenderer, x, y, rect.width, rect.height, rect.color, rect.type);
}

//-------------------
// --------------------------- Text Entry Struct ---------------------------

#define MAX_TEXT_LENGTH 256
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
TextEntry CreateTextEntry(int x, int y, int width, int height, int font_size, Color text_color, Color background_color) {
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
    int font_size;
} Text;

// ---------------- Label Functions ----------------
Label CreateLabel(int x, int y, int width, int height, const char *text, Color text_color, Color background_color) {
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
Button CreateButton(int x, int y, int width, int height, const char *text, Color text_color, Color background_color, Color hover_color, Color click_color) {
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
Text create_text(int x, int y, const char *text, Color text_color, int font_size) {
    Text txt = {x, y, text, text_color, font_size};
    return txt;
}

void render_text(Text *text) {
    xiDrawText(NULL, text->x, text->y, text->text, text->text_color, text->font_size);
}

// ----------- slider -----------------
typedef struct {
    int x, y, width, height;
    int min_value, max_value;
    int value;
    bool dragging;
} Slider;

// Create a slider
Slider CreateSlider(int x, int y, int width, int height, int min_value, int max_value, int start_value) {
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