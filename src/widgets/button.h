#ifndef _LUTE_BUTTON_H
#define _LUTE_BUTTON_H

#include "../lute.h"

struct LuteButton;

typedef struct {
    void (*draw_down)(struct LuteButton* button, cairo_t* ctx);
    void (*draw_up)(struct LuteButton* button, cairo_t* ctx);
} LuteButtonStyle;

typedef struct LuteButton {
    LuteWidget super;
    const LuteButtonStyle* style;
    const char* label;

    bool down;
    bool hover;

    void (*click)(struct LuteButton* button, void* data);
    void* click_data;
} LuteButton;

extern const LuteButtonStyle* const LUTE_BUTTON_DEFAULT_STYLE;
extern const LuteColor LUTE_BUTTON_PRIMARY_BACKGROUND;

void lute_button_init(LuteButton* button, LuteLayout layout, const char* label);
void lute_button_draw(LuteWidget* widget, cairo_t* ctx, LuteRect rectToPaint);
void lute_button_on_mousedown(LuteWidget* widget, enum LuteMouseButton button, uint16_t x, uint16_t y);
void lute_button_on_mouseup(LuteWidget* widget, enum LuteMouseButton button, uint16_t x, uint16_t y);

static inline void lute_button_set_click(LuteButton* button, void (*handler)(struct LuteButton* button, void* data), void* data) {
    button->click = handler;
    button->click_data = data;
}

#endif