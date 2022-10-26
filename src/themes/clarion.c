#include <lute.h>

/** Converts a `LuteColor` into arguments for cairo */
#define COLOR2ARGS(color) (lute_color_red(color) / 255.0), (lute_color_green(color) / 255.0), (lute_color_blue(color) / 255.0), (lute_color_alpha(color) / 255.0)

static void draw_top(cairo_t* ctx, LuteRect* rect) {
    cairo_move_to(ctx, rect->x, rect->y);
    cairo_line_to(ctx, rect->x + rect->width, rect->y);
    cairo_line_to(ctx, rect->x + rect->width - 2, rect->y + 2);
    cairo_line_to(ctx, rect->x + 2, rect->y + 2);
    cairo_close_path(ctx);
    cairo_fill(ctx);
}

static void draw_left(cairo_t* ctx, LuteRect* rect) {
    cairo_move_to(ctx, rect->x, rect->y);
    cairo_line_to(ctx, rect->x, rect->y + rect->height);
    cairo_line_to(ctx, rect->x + 2, rect->y + rect->height - 2);
    cairo_line_to(ctx, rect->x + 2, rect->y + 2);
    cairo_close_path(ctx);
    cairo_fill(ctx);
}

static void draw_right(cairo_t* ctx, LuteRect* rect) {
    cairo_move_to(ctx, rect->x + rect->width, rect->y);
    cairo_line_to(ctx, rect->x + rect->width, rect->y + rect->height);
    cairo_line_to(ctx, rect->x + rect->width - 2, rect->y + rect->height - 2);
    cairo_line_to(ctx, rect->x + rect->width - 2, rect->y + 2);
    cairo_close_path(ctx);
    cairo_fill(ctx);
}

static void draw_bottom(cairo_t* ctx, LuteRect* rect) {
    cairo_move_to(ctx, rect->x, rect->y + rect->height);
    cairo_line_to(ctx, rect->x + rect->width, rect->y + rect->height);
    cairo_line_to(ctx, rect->x + rect->width - 2, rect->y + rect->height - 2);
    cairo_line_to(ctx, rect->x + 2, rect->y + rect->height - 2);
    cairo_close_path(ctx);
    cairo_fill(ctx);
}

static void calculate_shades(LuteColor color, LuteColor shades[4]) {
    shades[0] = lute_color_rgba(
        lute_color_red(color) * 2/4,
        lute_color_green(color) * 2/4,
        lute_color_blue(color) * 2/4,
        lute_color_alpha(color)
    );
    shades[1] = lute_color_rgba(
        lute_color_red(color) * 3/4,
        lute_color_green(color) * 3/4,
        lute_color_blue(color) * 3/4,
        lute_color_alpha(color)
    );
    shades[2] = lute_color_rgba(
        255 - (255 - lute_color_red(color)) * 3/4,
        255 - (255 - lute_color_green(color)) * 3/4,
        255 - (255 - lute_color_blue(color)) * 3/4,
        lute_color_alpha(color)
    );
    shades[3] = lute_color_rgba(
        255 - (255 - lute_color_red(color)) * 2/4,
        255 - (255 - lute_color_green(color)) * 2/4,
        255 - (255 - lute_color_blue(color)) * 2/4,
        lute_color_alpha(color)
    );
}

void lute_clarion_button_draw_down(LuteButton* button, cairo_t* ctx) {
    LuteWidget* widget = (LuteWidget*)button;
    cairo_set_source_rgba(ctx, COLOR2ARGS(widget->background));
    cairo_rectangle(ctx, widget->_rect.x + 1, widget->_rect.y + 1, widget->_rect.width - 2, widget->_rect.height - 2);
    cairo_fill(ctx);

    LuteColor shades[4];
    calculate_shades(widget->background, shades);

    LuteRect* rect = &widget->_rect;
    cairo_set_source_rgba(ctx, COLOR2ARGS(shades[0]));
    draw_top(ctx, rect);

    cairo_set_source_rgba(ctx, COLOR2ARGS(shades[1]));
    draw_left(ctx, rect);

    cairo_set_source_rgba(ctx, COLOR2ARGS(shades[2]));
    draw_right(ctx, rect);

    cairo_set_source_rgba(ctx, COLOR2ARGS(shades[3]));
    draw_bottom(ctx, rect);

    cairo_set_source_rgba(ctx, 1.0, 1.0, 1.0, lute_color_alpha(widget->background) / 255);
    cairo_select_font_face(ctx, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(ctx, 12);
    cairo_text_extents_t textents;
    cairo_text_extents(ctx, button->label, &textents);
    cairo_move_to(ctx, rect->x + rect->width / 2 - (textents.width / 2),
         rect->y + rect->height / 2 + (textents.height / 2));
    cairo_show_text(ctx, button->label);
}

void lute_clarion_button_draw_up(LuteButton* button, cairo_t* ctx) {
    LuteWidget* widget = (LuteWidget*)button;
    cairo_set_source_rgba(ctx, COLOR2ARGS(widget->background));
    cairo_rectangle(ctx, widget->_rect.x + 1, widget->_rect.y + 1, widget->_rect.width - 2, widget->_rect.height - 2);
    cairo_fill(ctx);

    LuteColor shades[4];
    calculate_shades(widget->background, shades);

    LuteRect* rect = &widget->_rect;
    cairo_set_source_rgba(ctx, COLOR2ARGS(shades[3]));
    draw_top(ctx, rect);

    cairo_set_source_rgba(ctx, COLOR2ARGS(shades[2]));
    draw_left(ctx, rect);

    cairo_set_source_rgba(ctx, COLOR2ARGS(shades[1]));
    draw_right(ctx, rect);

    cairo_set_source_rgba(ctx, COLOR2ARGS(shades[0]));
    draw_bottom(ctx, rect);

    cairo_set_source_rgba(ctx, 1.0, 1.0, 1.0, lute_color_alpha(widget->background) / 255);
    cairo_select_font_face(ctx, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(ctx, 12);
    cairo_text_extents_t textents;
    cairo_text_extents(ctx, button->label, &textents);
    cairo_move_to(ctx, rect->x + rect->width / 2 - (textents.width / 2),
         rect->y + rect->height / 2 + (textents.height / 2));
    cairo_show_text(ctx, button->label);
}

const LuteButtonStyle LUTE_CLARION_BUTTON_STYLE = {
    .draw_down = lute_clarion_button_draw_down,
    .draw_up = lute_clarion_button_draw_up
};

const LuteColor LUTE_CLARION_BUTTON_PRIMARY_BACKGROUND = 0x3F7FBFff;
const LuteColor LUTE_CLARION_BUTTON_ACCENT_BACKGROUND = 0xCC3399ff;
const LuteColor LUTE_CLARION_BUTTON_DANGER_BACKGROUND = 0xCC4C33ff;

const LuteColor LUTE_CLARION_GROUP_DEFAULT_BACKGROUND = 0x28313dff;