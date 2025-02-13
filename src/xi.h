/// LIBS INCLUDED:
#include <SDL2/SDL.h> /// SDL2
#include <SDL2/SDL_ttf.h> ///SDL TTF
#include <stdbool.h> /// STDBOOL

static const char *xi_fontpath = "FreeMono.ttf";
/// ============================ COLOR STRUCT ============================
typedef struct {
    Uint8 r, g, b, a;
} Color;
/// ============================ WINDOW STRUCT ============================
typedef struct {
    TTF_Font *defaultFont;
    Color background_color;
} xi_Window;

SDL_Window *gwindow;
SDL_Renderer *grenderer;


/// ============================ ENUMS ============================
typedef enum { FILLED, OUTLINE } ShapeType;

/// ============================ COLOR CONSTANTS ============================
const Color COLOR_RED = {255, 0, 0, 255};
const Color COLOR_GREEN = {0, 255, 0, 255};
const Color COLOR_BLUE = {0, 0, 255, 255};
const Color COLOR_WHITE ={255, 255, 255,255};
const Color COLOR_YELLOW = { 255, 255, 0,255 };
const Color COLOR_DARK_BLUE = { 0,0, 139,255 };
const Color COLOR_GRAY = { 128,128, 128,255 };
const Color COLOR_BLACK = {0,0,0,255 };

/// =============================== REGISTERING WIDGETS ===================================

typedef enum {
    WIDGET_BUTTON,
    WIDGET_LABEL,
    WIDGET_TEXT,
    WIDGET_SLIDER,
    WIDGET_CONTAINER,
    WIDGET_ENTRY
} WidgetType;

typedef struct {
    WidgetType type;
    void* widget;  // Pointer to the actual widget (Button, Label, or Text)
} Widget;

#define MAX_WIDGETS 100
Widget widgets[MAX_WIDGETS];
int widget_count = 0;


void register_widget(WidgetType type, void* widget) {
// this function is called internally when a new widgets is created
/*
When users call button(), label(), or text(), these will register their widgets internally:

Button button(parameters) {
    Button new_button = create_button(text, x, y, font_size, fg, bg, style);
    register_widget(WIDGET_BUTTON, &new_button); <<<<<< registering
    return new_button;
}

*/
    if (widget_count < MAX_WIDGETS) {
        widgets[widget_count].type = type;
        widgets[widget_count].widget = widget;
        widget_count++;
    }
}

/// ============================ DRAW FUNCTIONS ============================
static void xi_DrawRect(SDL_Renderer *renderer, int x, int y, int width, int height, Color color, ShapeType type) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect rect = {x, y, width, height};
    if (type == FILLED) {
        SDL_RenderFillRect(renderer, &rect);
    } else {
        SDL_RenderDrawRect(renderer, &rect);
    }
}

void xi_DrawText(SDL_Renderer *renderer, const char *text, int x, int y, Color color, int fontSize) {
    if (!renderer) {
        SDL_Log("Renderer is NULL");
        return;
    }

    if (!xi_fontpath || xi_fontpath[0] == '\0') {
        SDL_Log("Font path is not set");
        return;
    }

    if (fontSize <= 0) {
        SDL_Log("Invalid font size: %d", fontSize);
        return;
    }

    if (!text) {
        SDL_Log("Text is NULL");
        return;
    }

    TTF_Font *font = TTF_OpenFont(xi_fontpath, fontSize);
    if (!font) {
        SDL_Log("Failed to load font '%s': %s", xi_fontpath, TTF_GetError());
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
    if (SDL_RenderCopy(renderer, textTexture, NULL, &destRect) != 0) {
        SDL_Log("Failed to render text: %s", SDL_GetError());
    }

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
}


static void xi_DrawCircle(SDL_Renderer *renderer, int x, int y, int radius, Color color, ShapeType type) {
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

static void xi_DrawTriangle(SDL_Renderer *renderer, int x1, int y1, int x2, int y2, int x3, int y3, Color color, ShapeType type) {
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

static void xi_ClearScreen(SDL_Renderer *renderer, Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

/// ============================ WINDOW FUNCTIONS ============================
// Create and initialize the SDL window and renderer
xi_Window xiCreateWindow(const char *title, int width, int height) {
    xi_Window xiWin = {NULL, COLOR_GRAY};

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
void xiDestroyWindow(xi_Window *xiWin) {
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


//============================= CONTAINER ===========================================
// Container widget structure
typedef struct {
    int x, y, width, height;
    const char *title;
    Color color;
    bool movable;
} xi_Container;

// Create a new container instance
xi_Container createContainer(int x, int y, int width, int height, Color color, const char *title, bool movable) {
    xi_Container container;
    container.x = x;
    container.y = y;
    container.width = width;
    container.height = height;
    container.color = color;
    container.title = title;
    container.movable = movable;
    
    register_widget(WIDGET_CONTAINER, &container);
    	
    return container;
}

// Render the container on the screen
void render_container(xi_Container *container) {
    int x = container->x;
    int y = container->y;
    int width = container->width;
    int height = container->height;

    // Draw the main container rectangle (filled)
    xi_DrawRect(grenderer, x, y, width, height, container->color, FILLED);

    // Handle title bar and title if a valid title is provided
    if (container->title && strlen(container->title) > 0) {
        int titleBarHeight = 30;  // Fixed height for the title bar
        Color titleBarColor = {200, 200, 200, 255};

        // Draw title bar
        xi_DrawRect(grenderer, x, y, width, titleBarHeight, titleBarColor, FILLED);

        // Draw title text centered vertically within the title bar
        int textX = x + 10;
        int textY = y + (titleBarHeight / 4);  // Simple vertical alignment
        xi_DrawText(grenderer, container->title, textX, textY, COLOR_BLUE, 16);

        // Adjust the container rectangle position if title bar exists
        y += titleBarHeight;
        height -= titleBarHeight;
    }

    // Redraw container outline after adjusting for title bar
    xi_DrawRect(grenderer, x, y, width, height, container->color, OUTLINE);
}

// Handle container movement if movable is true
void handleContainerMovement(xi_Container *container, SDL_Event *event) {
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
    xi_Container* parent;
} TextEntry;

// Initialize a single-line text entry box
TextEntry CreateTextEntry(int x, int y, int width, int height, int font_size, Color text_color, Color background_color) {
    TextEntry entry;
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
    entry.parent=NULL;
    memset(entry.text, 0, MAX_TEXT_LENGTH); // Initialize text with empty characters
    
    // registers widgets so sw_loop() can keep track of it, see render_widgets() for more details
    	register_widget(WIDGET_ENTRY, &entry);
    	
    return entry;
}

// Render the text entry box with scrolling support
void render_text_entry(TextEntry *entry) {
        //------------------ for containers ------------
        int x, y;

        if (entry->parent) { // Check if rect is inside a container
            x = entry->parent->x + entry->x; 
            y = entry->parent->y + entry->y; 
        } else {
            x = entry->x;
            y = entry->y;
        }
        //--------------------------
    // Draw background
    xi_DrawRect(grenderer, x, y, entry->width, entry->height, entry->background_color, FILLED);

    // Draw border
    xi_DrawRect(grenderer, x, y, entry->width, entry->height, COLOR_BLUE, OUTLINE);

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
    xi_DrawText(grenderer,  visible_text, x + 5, y + 5, entry->text_color, entry->font_size);

    // Draw cursor
    if (entry->active) {
        int cursor_x = entry->x + 5 + ((entry->cursor_position - entry->text_offset) * 10);
        xi_DrawRect(grenderer, cursor_x, y + 5, 2, entry->font_size, entry->text_color, FILLED);
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
    xi_Container* parent;
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
    xi_Container* parent;
} Button;

// ---------------- Text Structure ----------------
typedef struct {
    int x, y;
    const char *text;
    Color text_color;
    int font_size;
    xi_Container* parent;
} Text;

// ---------------- Label Functions ----------------
Label CreateLabel(int x, int y, int width, int height, const char *text, Color text_color, Color background_color) {
    Label label = {x, y, width, height, text, text_color, background_color, NULL};
    
    register_widget(WIDGET_LABEL, &label);
    	
    return label;
}

void render_label(Label *label) {
    int x, y;

    if (label->parent) { // Check if rect is inside a container
        x = label->parent->x + label->x; 
        y = label->parent->y + label->y; 
    } else {
        x = label->x;
        y = label->y;
    }
    if (label->background_color.a != 0) {  // If not transparent
        xi_DrawRect(grenderer, x, y, label->width, label->height, label->background_color, FILLED);
    }
    xi_DrawText(grenderer, label->text, x + 5,y + 5, label->text_color, 16);
}

// ---------------- Button Functions ----------------
Button CreateButton(int x, int y, int width, int height, const char *text, Color text_color, Color background_color, Color hover_color, Color click_color) {
    Button button = {x, y, width, height, text, text_color, background_color, hover_color, click_color, false, false, NULL};
    register_widget(WIDGET_TEXT, &button);
    return button;
}

void render_button(Button *button) {
    //------------------ for containers ------------
    int x, y;

    if (button->parent) { // Check if rect is inside a container
        x = button->parent->x + button->x; 
        y = button->parent->y + button->y; 
    } else {
        x = button->x;
        y = button->y;
    }
    //--------------------------
    Color current_color = button->background_color;
    if (button->clicked) {
        current_color = button->click_color;
    } else if (button->hovered) {
        current_color = button->hover_color;
    }

    xi_DrawRect(grenderer, x, y, button->width, button->height, current_color, FILLED);
    xi_DrawText(grenderer,  button->text,x + 10, y + 10, button->text_color, 16);
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
Text CreateText( const char *text,int x, int y, Color text_color, int font_size) {
    Text txt = {x, y, text, text_color, font_size,NULL};
    
    register_widget(WIDGET_TEXT, &txt);
    	
    return txt;
}

void render_text(Text *text) {
    int x, y;

    if (text->parent) { // Check if rect is inside a container
        x = text->parent->x + text->x; 
        y = text->parent->y + text->y; 
    } else {
        x = text->x;
        y = text->y;
    }
    xi_DrawText(grenderer, text->text,x,y, text->text_color,text->font_size);
}

// ----------- slider -----------------
typedef struct {
    int x, y, width, height;
    int min_value, max_value;
    int value;
    bool dragging;
    xi_Container* parent;
} Slider;

// Create a slider
Slider CreateSlider(int x, int y, int width, int height, int min_value, int max_value, int start_value) {
    Slider slider = {x, y, width, height, min_value, max_value, start_value, false,NULL};
    register_widget(WIDGET_SLIDER, &slider);
    	
    return slider;
}

// Render the slider with a centered value
void render_slider(Slider *slider) {
        //------------------ for containers ------------
        int x, y;

        if (slider->parent) { // Check if rect is inside a container
            x = slider->parent->x + slider->x; 
            y = slider->parent->y + slider->y; 
        } else {
            x = slider->x;
            y = slider->y;
        }
        //--------------------------
    // Draw the bar (track)
    xi_DrawRect(grenderer, x, y, slider->width, slider->height, COLOR_WHITE, FILLED);

    // Calculate the thumb (handle) position
    float percentage = (float)(slider->value - slider->min_value) / (slider->max_value - slider->min_value);
    int handle_x = x + (int)(percentage * (slider->width - slider->height)); // Keep thumb inside the track

    // Draw the thumb (handle) inside the bar
    xi_DrawRect(grenderer, handle_x, y, slider->height, slider->height, COLOR_BLUE, FILLED);

    // Render the value inside the thumb
    char value_text[16];
    snprintf(value_text, sizeof(value_text), "%d", slider->value);

    int text_x = handle_x + (slider->height / 4);  // Center inside the thumb
    int text_y = y + (slider->height / 4);

    xi_DrawText(grenderer, value_text, text_x, text_y,  COLOR_WHITE,slider->height / 2);
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


//=================== Main Loop ==================
//=====================RENDER ALL WIDGETS=============================
void render_widgets() {
/*
 Key Steps for render_widgets():
 
    *  Widget Registration: Whenever the user calls button(), label(), or text(), these widgets need to be registered in an internal data structure like a list or array.
    * Widget Rendering: In render_widgets(), the system will loop through all registered widgets and call the appropriate rendering function for each one, like render_button(), render_label(), or render_text().
    * Widget Type Checking: Each widget will have a type (e.g., button, label, text), and based on that type, the corresponding render function will be called.
*/
    for (int i = 0; i < widget_count; ++i) {
        switch (widgets[i].type) {
            case WIDGET_CONTAINER:
                render_container((xi_Container*)widgets[i].widget);
                break;     
            case WIDGET_BUTTON:
                render_button((Button*)widgets[i].widget);
                break;
            case WIDGET_LABEL:
                render_label((Label*)widgets[i].widget);
                break;
            case WIDGET_TEXT:
                render_text((Text*)widgets[i].widget);
                break;
         case WIDGET_SLIDER:
                render_slider((Slider*)widgets[i].widget);
                break; 
         case WIDGET_ENTRY:
             	render_text_entry((TextEntry*)widgets[i].widget);
                break;
            // Add cases for other widget types here as you implement them
            default:
                break;
        }
    }
}

//=====================================gui loop=================================================
bool program_active = true; 
void EventLoop() {
     while (program_active) {
         SDL_Event event;
         while (SDL_PollEvent(&event)) {
             switch (event.type) {
                 case SDL_QUIT:
                    program_active = false;  // User closed the window
                     break;
                 
                     break;
                 case SDL_WINDOWEVENT:
                     if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                         // Resize event triggers window size update
                     }
                     break;
                 default:
                     break;
             }
             //buttons
           //  sw_render_all_button_states(&event);
             //drop down
           //  sw_render_all_drop_down_states(&event);
			//slider
			//sw_render_all_slider_states(&event);
            //entry
        //   sw_render_all_entry_states(&event);
        
         }
          //clear_screen(xiWindow.background_color);
          xi_ClearScreen(grenderer, COLOR_GRAY);
         render_widgets();  // Render all widgets (handled by library)
         SDL_RenderPresent(grenderer);       // Present the rendered output
     }
 }
