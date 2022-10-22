#include <lute.h>
#include <widgets/button.h>

#define LUTE_FLAT_THEME
#include <themes/flat.c>

static LuteWindow* window = NULL;
static int count = 0;

void add_handler(LuteButton* button, void* data) {
    count += 1;
    
    static char buf[16] = {0};
    snprintf(buf, 15, "%d", count);
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
        .x = {0.5, -32},
        .y = {0.5, -16},
        .width = {0, 64},
        .height = {0, 32}
    }, "0");
    lute_button_set_click(&add_button, add_handler, NULL);
    lute_group_add(&group, (LuteWidget*)&add_button);

    lute_main();
    lute_deinit();

    return 0;
}