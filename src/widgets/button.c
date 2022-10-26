#include <lute/widgets/button.h>
#include <string.h>

const LuteWidgetTable BUTTON_TABLE = {
    .draw = lute_button_draw,
    .resize = lute_widget_resize,
    .deinit = NULL,
    .on_mousedown = lute_button_on_mousedown,
    .on_mouseup = lute_button_on_mouseup,
};

void lute_button_draw(LuteWidget* widget, cairo_t* ctx, LuteRect rectToPaint) {
    LuteButton* button = (LuteButton*)widget;
    if (button->down) {
        button->style->draw_down(button, ctx);
    } else {
        button->style->draw_up(button, ctx);
    }
}

void lute_button_init(LuteButton* button, LuteLayout layout, const char* label) {
    button->super.vtable = &BUTTON_TABLE;
    button->super.background = LUTE_BUTTON_PRIMARY_BACKGROUND;
    button->super.layout = layout;
    button->style = LUTE_BUTTON_DEFAULT_STYLE;
    button->label = label;

    button->down = false;
    button->hover = false;

    button->click = NULL;
    button->click_data = NULL;
}

void lute_button_on_mousedown(LuteWidget* widget, enum LuteMouseButton button, uint16_t x, uint16_t y) {
    if (button == LUTE_MOUSEBUTTON_LEFT) {
        LuteButton* btn = (LuteButton*)widget;
        btn->down = true;
    }

    lute_window_mark_dirty_rect(widget->window, widget->_rect);
}

void lute_button_on_mouseup(LuteWidget* widget, enum LuteMouseButton button, uint16_t x, uint16_t y) {
    LuteButton* btn = (LuteButton*)widget;
    if (btn->down && btn->click != NULL) {
        btn->click(btn, btn->click_data);
    }
    btn->down = false;

    lute_window_mark_dirty_rect(widget->window, widget->_rect);
}