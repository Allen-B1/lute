#include "../widgets/button.h"

void flat_button_draw_down(LuteButton* button, cairo_t* ctx) {
    cairo_set_source_rgba(ctx, 0.05, 0.24, 0.37, 0.8);
    cairo_rectangle(ctx, button->super._rect.x, button->super._rect.y, button->super._rect.width, button->super._rect.height);
    cairo_fill(ctx);

    cairo_set_source_rgba(ctx, lute_color_red(button->super.background), lute_color_green(button->super.background), lute_color_blue(button->super.background), 1.0);
    cairo_rectangle(ctx, button->super._rect.x - 2.0, button->super._rect.y - 2.0, button->super._rect.width, button->super._rect.height);
    cairo_fill(ctx);

    cairo_set_source_rgba(ctx, 1.0, 1.0, 1.0, 1.0);
    cairo_select_font_face(ctx, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_text_extents_t textents;
    cairo_text_extents(ctx, button->label, &textents);
    cairo_move_to(ctx, button->super._rect.x - 2.0 + (button->super._rect.width / 2) - (textents.width / 2),
         button->super._rect.y - 2.0 + (button->super._rect.height / 2) - (textents.height / 2));
    cairo_show_text(ctx, button->label);
}

void flat_button_draw_up(LuteButton* button, cairo_t* ctx) {
    cairo_set_source_rgba(ctx, 0.05, 0.24, 0.37, 0.8);
    cairo_rectangle(ctx, button->super._rect.x, button->super._rect.y, button->super._rect.width, button->super._rect.height);
    cairo_fill(ctx);

    cairo_set_source_rgba(ctx, lute_color_red(button->super.background), lute_color_green(button->super.background), lute_color_blue(button->super.background), 1.0);
    cairo_rectangle(ctx, button->super._rect.x - 4.0, button->super._rect.y - 4.0, button->super._rect.width, button->super._rect.height);
    cairo_fill(ctx);

    cairo_set_source_rgba(ctx, 1.0, 1.0, 1.0, 1.0);
    cairo_select_font_face(ctx, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_text_extents_t textents;
    cairo_text_extents(ctx, button->label, &textents);
    cairo_move_to(ctx, button->super._rect.x - 4.0 + (button->super._rect.width / 2) - (textents.width / 2),
         button->super._rect.y - 4.0 + (button->super._rect.height / 2) - (textents.height / 2));
    cairo_show_text(ctx, button->label);
}

const LuteButtonStyle FLAT_BUTTON_STYLE = {
    .draw_down = flat_button_draw_down,
    .draw_up = flat_button_draw_up    
};

const LuteColor FLAT_BUTTON_PRIMARY_BACKGROUND = 0x2196f3ff;

const LuteColor FLAT_GROUP_BACKGROUND = 0xffffffff;

#ifdef LUTE_FLAT_THEME
const LuteButtonStyle* const LUTE_BUTTON_DEFAULT_STYLE = &FLAT_BUTTON_STYLE;
const LuteColor LUTE_BUTTON_PRIMARY_BACKGROUND = FLAT_BUTTON_PRIMARY_BACKGROUND;
const LuteColor LUTE_GROUP_DEFAULT_BACKGROUND = FLAT_GROUP_BACKGROUND;
#endif