#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

/// ============================ WINDOW STRUCT ============================
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *defaultFont;
} xiWindow;

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

    xiWin.window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!xiWin.window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return xiWin;
    }

    xiWin.renderer = SDL_CreateRenderer(xiWin.window, -1, SDL_RENDERER_ACCELERATED);
    if (!xiWin.renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(xiWin.window);
        SDL_Quit();
        xiWin.window = NULL;
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
    if (xiWin->renderer) {
        SDL_DestroyRenderer(xiWin->renderer);
    }
    if (xiWin->window) {
        SDL_DestroyWindow(xiWin->window);
    }
    TTF_Quit();
    SDL_Quit();
}

/// ============================ DRAW FUNCTIONS ============================

// Draw a rectangle
void xiDrawRect(xiWindow *xiWin, int x, int y, int width, int height, Color color, ShapeType type) {
    draw_rect(xiWin->renderer, x, y, width, height, color, type);
}

// Draw text
void xiDrawText(xiWindow *xiWin, int x, int y, const char *text, Color color, int fontSize) {
    draw_text(xiWin->renderer, "FreeMono.ttf", fontSize, x, y, text, color);
}

// Draw a circle using midpoint algorithm
void xiDrawCircle(xiWindow *xiWin, int x, int y, int radius, Color color, ShapeType type) {
    draw_circle(xiWin->renderer, x, y, radius, color, type);
}

// Draw a triangle
void xiDrawTriangle(xiWindow *xiWin, int x1, int y1, int x2, int y2, int x3, int y3, Color color, ShapeType type) {
    draw_triangle(xiWin->renderer, x1, y1, x2, y2, x3, y3, color, type);
}

// Clear the screen with a specific color
void xiClearScreen(xiWindow *xiWin, Color color) {
    clear_screen(xiWin->renderer, color);
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