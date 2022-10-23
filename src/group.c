#include <stdlib.h>
#include <stdio.h>
#include "lute.h"

static const LuteWidgetTable GROUP_TABLE = {
    .resize = lute_group_resize,
    .draw = lute_group_draw,
    .deinit = lute_group_deinit,

    .on_mousedown = lute_group_on_mousedown,
    .on_mouseup = lute_group_on_mouseup
};

void lute_group_init(LuteGroup* group, LuteLayout layout) {
    group->super.layout = layout;
    group->super.background = LUTE_GROUP_DEFAULT_BACKGROUND;
    group->super.vtable = &GROUP_TABLE;
    group->children = NULL;
    group->children_cap = 0;
    group->children_len = 0;
}

void lute_group_resize(LuteWidget* widget, LuteRect parent) {
    lute_widget_resize(widget, parent);

    LuteGroup* group = (LuteGroup*)widget;
    for (size_t i = 0; i < group->children_len; i++) {
        LuteWidget* child = group->children[i];
        child->vtable->resize(child, widget->_rect);
    }
}

void lute_group_draw(LuteWidget* widget, cairo_t* ctx, LuteRect rectToPaint) {
    lute_widget_draw(widget, ctx, rectToPaint);

    LuteGroup* group = (LuteGroup*)widget;
    for (size_t i = 0; i < group->children_len; i++) {
        LuteWidget* child = group->children[i];
        LuteRect intersectRect = lute_rect_intersect(rectToPaint, child->_rect);
        if (!lute_rect_empty(intersectRect)) {
//            printf("    -> drawing widget: %x\n", (int)((uintptr_t)child) & 0xfff);
            child->vtable->draw(child, ctx, intersectRect);
        }
    }
}

bool lute_group_add(LuteGroup* group, LuteWidget* child) {
    child->window = group->super.window;
    if (group->children_cap == group->children_len) {
        size_t new_cap = group->children_cap == 0 ? 4 : group->children_cap * 2;
        LuteWidget** new_children = realloc(group->children, sizeof(LuteWidget*) * new_cap);
        if (new_children == NULL) return false;
        new_children[group->children_len] = child;

        group->children = new_children;
        group->children_len += 1;
        group->children_cap = new_cap;

        return true;
    } else {
        group->children[group->children_len] = child;
        group->children_len += 1;
        return true;
    }
}

void lute_group_deinit(LuteGroup* group) {
    for (int i = 0; i < group->children_len; i++) {
        if (group->children[i]->vtable->deinit != NULL) 
            group->children[i]->vtable->deinit(group->children[i]);
    }

    free(group->children);
    group->children_cap = 0;
    group->children_len = 0;
}

void lute_group_on_mousedown(LuteWidget* widget, enum LuteMouseButton button, uint16_t x, uint16_t y) {
    LuteGroup* group = (LuteGroup*)widget;
    for (int i = 0; i < group->children_len; i++) {
//        printf("child rect: %d %d %d %d\n", group->children[i]->_rect.x, group->children[i]->_rect.y, group->children[i]->_rect.width, group->children[i]->_rect.height);
//        printf("intersects: %s\n", lute_rect_contains(group->children[i]->_rect, x, y) ? "yes": "no");
        if (group->children[i]->vtable->on_mousedown != NULL && lute_rect_contains(group->children[i]->_rect, x, y)) {
            puts("intersect");
            group->children[i]->vtable->on_mousedown(group->children[i], button, x, y);
        }
    }
}

void lute_group_on_mouseup(LuteWidget* widget, enum LuteMouseButton button, uint16_t x, uint16_t y) {
    LuteGroup* group = (LuteGroup*)widget;
    for (int i = 0; i < group->children_len; i++) {
        if (group->children[i]->vtable->on_mouseup != NULL && lute_rect_contains(group->children[i]->_rect, x, y)) {
            group->children[i]->vtable->on_mouseup(group->children[i], button, x, y);
        }
    }
}
