ypedef struct {
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