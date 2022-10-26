#ifndef _LUTE_H
#define _LUTE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <cairo.h>
#include <stdlib.h>
#include <stdio.h>

typedef uint32_t LuteColor; 
static inline uint8_t lute_color_red(LuteColor color) {
    return (color & 0xff000000) >> 24;
}
static inline uint8_t lute_color_green(LuteColor color) {
    return (color & 0xff0000) >> 16;
}
static inline uint8_t lute_color_blue(LuteColor color) {
    return (color & 0xff00) >> 8;
}
static inline uint8_t lute_color_alpha(LuteColor color) {
    return (color & 0xff);
}

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
} LuteRect;

/** Represents a value of a child relative to a parent */
typedef struct {
    double scale;
    int16_t off;
} LuteLayoutValue;

static inline uint16_t lute_layout_value_calc(LuteLayoutValue child, uint16_t parent) {
    int32_t val = ((int32_t)(parent * child.scale) + (int32_t)child.off);
    if (val < 0) return 0;
    return val;
}

/** Represents the size of a child, relative to a parent */
typedef struct {
    LuteLayoutValue x;
    LuteLayoutValue y;
    LuteLayoutValue width;
    LuteLayoutValue height;
} LuteLayout;

static const LuteLayout LUTE_LAYOUT_FULL = {
    .x = {0.0, 0},
    .y = {0.0, 0},
    .width = {1.0, 0},
    .height = {1.0, 0}
};

static inline LuteRect lute_layout_calc(LuteLayout child, LuteRect parent) {
    LuteRect rect;
    rect.x = lute_layout_value_calc(child.x, parent.width) + parent.x;
    rect.y = lute_layout_value_calc(child.y, parent.height) + parent.y;
    rect.width = lute_layout_value_calc(child.width, parent.width);
    rect.height = lute_layout_value_calc(child.height, parent.height);
    return rect;
}

static inline bool lute_rect_contains(LuteRect rect, uint16_t x, uint16_t y) {
    return rect.x <= x && x <= rect.x + rect.width &&
        rect.y <= y && y <= rect.y + rect.height;
}

static inline LuteRect lute_rect_intersect(LuteRect r1, LuteRect r2) {
    LuteRect rect;

    LuteRect rleft, rright;
    if (r1.x < r2.x) {
        rleft = r1; rright = r2;
    } else {
        rleft = r2; rright = r1;
    }

    rect.x = rright.x;
    if (rleft.x + rleft.width > rright.x) {
        if (rleft.x + rleft.width > rright.x + rright.width) {
            rect.width = rright.width;
        } else {
            rect.width = rleft.x + rleft.width - rright.x;
        }
    } else {
        rect.width = 0;
    }

    LuteRect rtop, rbottom;
    if (r1.y < r2.y) {
        rtop = r1; rbottom = r2;
    } else {
        rbottom = r1; rtop = r2;
    }

    rect.y = rbottom.y;
    if (rtop.y + rtop.height > rbottom.y) {
        if (rtop.y + rtop.height > rbottom.y + rbottom.height) {
            rect.height = rbottom.height;
        } else {
            rect.height = rtop.y + rtop.height - rbottom.y;
        }
    } else {
        rect.height = 0;
    }

    return rect;
}

static inline bool lute_rect_empty(LuteRect rect) {
    return rect.width == 0 || rect.height == 0;
}

enum LuteMouseButton {
    LUTE_MOUSEBUTTON_LEFT,
    LUTE_MOUSEBUTTON_RIGHT,
    LUTE_MOUSEBUTTON_MIDDLE,
    LUTE_MOUSEBUTTON_SCROLL_UP,
    LUTE_MOUSEBUTTON_SCROLL_DOWN,
    LUTE_MOUSEBUTTON_UNKNOWN
};

struct LuteWidget;
typedef struct {
    /** Draws the area in `rectToPaint`, assuming the dimensions of the widget are given by `newSize`. 
     * This method should not be called directly; it should be called only from within the Draw phase of the main loop.
     * This method should only be called after checking that the widget should be drawn.
     * `rectToPaint` should be entirely contained within `_rect` (this is to avoid calculating the intersection in both the parent and the child). */
    void (*draw)(struct LuteWidget* widget, cairo_t* ctx, LuteRect rectToPaint);

    /** Called during the Layout phase. Updates the internal _rect field, as well as any child widges. */
    void (*resize)(struct LuteWidget* widget, LuteRect parent);

    /** Optional.
     * Called during deinitialization of the parent widget, or of the window for the root widget.
     */
    void (*deinit)(struct LuteWidget* widget);

    /** Optional. Called on a mouse down event */
    void (*on_mousedown)(struct LuteWidget* widget, enum LuteMouseButton button, uint16_t x, uint16_t y);
    /** Optional. Called on a mouse up event */
    void (*on_mouseup)(struct LuteWidget* widget, enum LuteMouseButton button, uint16_t x, uint16_t y);
} LuteWidgetTable;

typedef struct LuteWidget {
    const LuteWidgetTable* vtable;
    LuteLayout layout;
    LuteColor background;
    struct LuteWindow* window;

    /** This field may only be written to during the Layout phase, and read from during the Draw phase */
    LuteRect _rect;
} LuteWidget;

static inline void lute_widget_draw(LuteWidget* widget, cairo_t* ctx, LuteRect rectToPaint) {
    LuteColor color = widget->background;
//    printf("%p dimensions: %hu %hu\n", widget, widget->_rect.width, widget->_rect.height);
//    printf("painting %p rect: %hu %hu\n", widget, rectToPaint.width, rectToPaint.height);

    bool isTransparent = lute_color_alpha(color) == 0;
    if (!isTransparent) {
        cairo_set_source_rgba(ctx, lute_color_red(color)/255.0, lute_color_green(color)/255.0, lute_color_blue(color)/255.0, lute_color_alpha(color)/255.0);
        cairo_rectangle(ctx, rectToPaint.x, rectToPaint.y, rectToPaint.width, rectToPaint.height);
        cairo_fill(ctx);
    }
}

static inline void lute_widget_resize(LuteWidget* widget, LuteRect parent) {
    widget->_rect = lute_layout_calc(widget->layout, parent);
}

typedef struct {
    LuteWidget super;

    LuteWidget** children; // owned
    size_t children_len;
    size_t children_cap;
} LuteGroup;

extern LuteColor LUTE_GROUP_DEFAULT_BACKGROUND;

void lute_group_init(LuteGroup* group, LuteLayout layout);
bool lute_group_add(LuteGroup* group, LuteWidget* child);
void lute_group_deinit(LuteGroup* group);

void lute_group_draw(LuteWidget* widget, cairo_t* ctx, LuteRect rectToPaint);
void lute_group_resize(LuteWidget* widget, LuteRect parent);
void lute_group_on_mousedown(LuteWidget* widget, enum LuteMouseButton button, uint16_t x, uint16_t y);
void lute_group_on_mouseup(LuteWidget* widget, enum LuteMouseButton button, uint16_t x, uint16_t y);

static inline LuteGroup* lute_group_new(LuteLayout layout) {
    LuteGroup* group = malloc(sizeof(LuteGroup));
    if (group == NULL) return NULL;
    lute_group_init(group, layout);
    return group;
}

static inline void lute_group_destroy(LuteGroup* group) {
    lute_group_deinit(group);
    free(group);
}

typedef struct LuteWindow LuteWindow;

/** Creates, but does not show, a window */
LuteWindow* lute_window_new(uint16_t width, uint16_t height, const char* title);
void lute_window_show(LuteWindow* win);
void lute_window_hide(LuteWindow* win);
void lute_window_destroy(LuteWindow* win);

/** Sets the root widget of the window */
void lute_window_root(LuteWindow* win, LuteWidget* child);

/** Mark a rectangle as dirty. Causes that rectangle to be re-drawn at the next Draw phase.
 * Should only be called from the Update phase.
 */
void lute_window_mark_dirty_rect(LuteWindow* win, LuteRect rect);

static inline void lute_window_mark_dirty(LuteWindow* win, LuteWidget* widget) {
    lute_window_mark_dirty_rect(win, widget->_rect);
}

/** Initialize the library. */
void lute_init();
void lute_deinit();
void lute_main();

#endif