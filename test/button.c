#include <lute.h>

#define LUTE_FLAT_THEME
#include "../src/themes/flat.c"

static LuteWindow* window = NULL;
static int count = 0;

void clear_handler(LuteButton* button, void* data) {
    LuteButton* add_button = data;

    if (count > 0)
        count = 0;
    else
        count -= 1;

    static char buf[32] = {0};
    snprintf(buf, 31, "Add to %d", count);
    add_button->label = buf;

    lute_window_mark_dirty(window, (LuteWidget*)add_button);
}

void add_handler(LuteButton* button, void* data) {
    if (count >= 0)
        count += 1;
    else
        count = 1;
    
    static char buf[32] = {0};
    snprintf(buf, 31, "Add to %d", count);
    button->label = buf;

    lute_window_mark_dirty(window, (LuteWidget*)button);
}

int main() {
    lute_init();

    window = lute_window_new(256, 192, "Counter");
    lute_window_show(window);

    LuteGroup group;
    lute_group_init(&group, LUTE_LAYOUT_FULL);
    lute_window_root(window, (LuteWidget*)&group);

    LuteButton add_button;
    lute_button_init(&add_button, (LuteLayout){
        .x = {0.5, -(128 + 80 + 16) / 2},
        .y = {0.5, -20},
        .width = {0, 128},
        .height = {0, 40}
    }, "Add to 0");
    lute_button_set_click(&add_button, add_handler, NULL);
    lute_group_add(&group, (LuteWidget*)&add_button);

    LuteButton clear_button;
    lute_button_init(&clear_button, (LuteLayout) {
        .x = {0.5, -(128 + 80 + 16) / 2 + 128 + 16},
        .y = {0.5, -20},
        .width = {0, 80},
        .height = {0, 40}
    }, "Clear");
    ((LuteWidget*)&clear_button)->background = 0xa4df44ff;
    lute_button_set_click(&clear_button, clear_handler, &add_button);
    lute_group_add(&group, (LuteWidget*)&clear_button);


    lute_main();
    lute_deinit();

    return 0;
}