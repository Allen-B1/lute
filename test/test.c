#include <lute.h>

void main() {
    lute_init();

    LuteWindow* win = lute_window_new(384, 256, "Hello World");
    lute_window_show(win);

    LuteGroup* group = lute_group_new((LuteLayout){
        .x = {0.0, 0},
        .y = {0.0, 0},
        .width = {1, 0},
        .height = {1.0, 0}});
    ((LuteWidget*)group)->background = 0xff00ffff;

    LuteGroup* rect1 = lute_group_new((LuteLayout){
        .x = {0.0, 16},
        .y = {0.0, 16},
        .width = {0.0, 64},
        .height = {0.0, 64}
    });
    ((LuteWidget*)rect1)->background = 0x21f63dff;
    lute_group_add(group, rect1);

    LuteGroup* rect2 = lute_group_new((LuteLayout){
        .x = {0.0, 16},
        .y = {0.0, 64+32},
        .width = {0.0, 64},
        .height = {1.0, -(64+32+16)}
    });
    ((LuteWidget*)rect2)->background = 0x0000ffff;
    lute_group_add(group, rect2);

    LuteGroup* rect3 = lute_group_new((LuteLayout){
        .x = {0.0, 64+32},
        .y = {0.0, 16},
        .width = {1.0, -(64+32+16)},
        .height = {1.0, -(80+32+16)},
    });
    ((LuteWidget*)rect3)->background = 0xffaa00ff;
    lute_group_add(group, rect3);

    LuteGroup* rect4 = lute_group_new((LuteLayout){
        .x = {0.0, 64+32},
        .y = {1.0, -(80+16)},
        .width = {0.5, -(64+32+32)/2},
        .height = {0.0, 80},
    });
    ((LuteWidget*)rect4)->background = 0x00ffffff;
    lute_group_add(group, rect4);

    LuteGroup* rect5 = lute_group_new((LuteLayout){
        .x = {0.5, (64+32)/2},
        .y = {1.0, -(80+16)},
        .width = {0.5, -(64+32+32)/2},
        .height = {0.0, 80},
    });
    ((LuteWidget*)rect5)->background = 0xff0000ff;
    lute_group_add(group, rect5);

    lute_window_root(win, (LuteWidget*)group);
    lute_main();
    lute_deinit();
}