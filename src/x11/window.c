#include <lute.h>
#include <stdlib.h>
#include <xcb/xcb.h>
#include <stdio.h>
#include "x11.h"

LuteWindow* lute_window_new(uint16_t width, uint16_t height, const char* title) {
    xcb_window_t win = xcb_generate_id(lute_xcb_state.conn);
    xcb_void_cookie_t cookie = xcb_create_window(
        lute_xcb_state.conn, XCB_COPY_FROM_PARENT, win, lute_xcb_state.screen->root, 0, 0, width, height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, 
        lute_xcb_state.visual->visual_id, 
        XCB_CW_EVENT_MASK,
        (uint32_t[]){XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_BUTTON_MOTION | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE}
        );
    LuteWindow* obj = malloc(sizeof(LuteWindow));
    if (obj == NULL) return NULL;
    obj->id = win;
    obj->root = NULL;
    obj->width = width;
    obj->height = height;

    rhmap_set(&lute_xcb_state.windows, (void*)win, obj);

    arrlist_init(&obj->dirty_areas, 0);
    obj->all_dirty = true;

    return obj;
}

void lute_window_show(LuteWindow* win) {
    xcb_map_window(lute_xcb_state.conn, win->id);
}

void lute_window_hide(LuteWindow* win) {
    xcb_unmap_window(lute_xcb_state.conn, win->id);
}

void lute_window_destroy(LuteWindow* win) {
    if (win->root->vtable->deinit != NULL) {
        win->root->vtable->deinit(win->root);
    }

    for (size_t i = 0; i < win->dirty_areas.len; i++) {
        free(win->dirty_areas.data[i]);
    }
    arrlist_deinit(&win->dirty_areas);

    rhmap_del(&lute_xcb_state.windows, (void*)win->id);
    xcb_destroy_window(lute_xcb_state.conn, win->id);
    free(win);
}

void lute_window_root(LuteWindow* win, LuteWidget* child) {
    child->window = win;
    win->root = child;
}

void lute_window_mark_dirty_rect(LuteWindow* win, LuteRect rect) {
    LuteRect* ptr = malloc(sizeof(LuteRect));
    if (ptr == NULL) return;
    memcpy(ptr, &rect, sizeof(LuteRect));
    arrlist_add(&win->dirty_areas, ptr);
}