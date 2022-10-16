#include <lute.h>
#include <stdlib.h>
#include <stdio.h>
#include <xcb/xcb.h>
#include <cairo/cairo-xcb.h>
#include <arrlist.h>
#include "x11.h"

LuteState lute_xcb_state;

void lute_init() {
    lute_xcb_state.conn = xcb_connect(NULL, NULL);
    rhmap_init(&lute_xcb_state.windows, 0, (void*)rhmap_eq_int, (void*)rhmap_djb2_int);
}

void lute_deinit() {
    xcb_disconnect(lute_xcb_state.conn);
    rhmap_deinit(&lute_xcb_state.windows);
}

/** Handles incoming XCB events. If the mainloop should be exited, returns true. */
static bool handle_events() {
    while (true) {
        xcb_generic_event_t* event = xcb_poll_for_event(lute_xcb_state.conn);
        if (event == NULL) break;

        switch (event->response_type & ~0x80) {
        // 1a. LAYOUT
        case XCB_CONFIGURE_NOTIFY: {
            xcb_configure_notify_event_t* resize = (xcb_configure_notify_event_t*)event;
            xcb_window_t id = resize->window;
            LuteWindow* win = rhmap_get(&lute_xcb_state.windows, (void*)id);
            if (win == NULL) {
                fprintf(stderr, "Lute error: window does not exist");
                continue;
            }
            
            win->width = resize->width;
            win->height = resize->height;
            win->all_dirty = true;

            if (win->root != NULL)
                (win->root->vtable->resize)(win->root, (LuteRect) {
                    .x = 0,
                    .y = 0,
                    .width = resize->width,
                    .height = resize->height
                });
            break; }
        // 1b. EXPOSE
        case XCB_EXPOSE: {
            xcb_expose_event_t* expose = (xcb_expose_event_t*)event;
            xcb_window_t id = expose->window;
            LuteWindow* win = rhmap_get(&lute_xcb_state.windows, (void*)id);
            if (win == NULL) {
                fprintf(stderr, "Lute error: window in expose does not exist");
                continue;
            }

            LuteRect* rect = malloc(sizeof(LuteRect));
            rect->x = expose->x;
            rect->y = expose->y;
            rect->width = expose->width;
            rect->height = expose->height;
            
            arrlist_add(&win->dirty_areas, rect);

            break; }
        }
    }
    return false;
}

static xcb_visualtype_t* get_root_visual() {
    xcb_screen_t* screen = xcb_setup_roots_iterator (xcb_get_setup (lute_xcb_state.conn)).data;
    if (screen == NULL) return NULL;
    xcb_depth_iterator_t iter = xcb_screen_allowed_depths_iterator(screen);
    for (; iter.rem; xcb_depth_next(&iter)) {
        xcb_visualtype_iterator_t visual_iter = xcb_depth_visuals_iterator(iter.data);
        for (; visual_iter.rem; xcb_visualtype_next(&visual_iter)) {
            if (screen->root_visual == visual_iter.data->visual_id) {
                return visual_iter.data;
            }
        }
    }

    return NULL;
}

static void draw() {
    xcb_window_t id;
    LuteWindow* win;
    rhmap_iter(&lute_xcb_state.windows, &id, &win, {
        if (win->root == NULL) continue;
        if (!win->all_dirty && win->dirty_areas.len == 0 && win->dirty.len == 0) {
            continue;
        }
  
        cairo_surface_t* surface = cairo_xcb_surface_create(lute_xcb_state.conn, id, get_root_visual(), win->width, win->height);
        cairo_t* ctx = cairo_create(surface);

        if (win->all_dirty) {
            puts("all dirty...!");
            win->root->vtable->draw(win->root, ctx, win->root->_rect);
            win->all_dirty = false;
            continue;
        }

        for (size_t i = 0; i < win->dirty.len; i++) {
            LuteWidget* widget = win->dirty.data[i];
            arrlist_add(&win->dirty_areas, &widget->_rect);
        }

        arrlist_clear(&win->dirty);

        for (size_t i = 0; i < win->dirty_areas.len; i++) {
            LuteRect* rect = win->dirty_areas.data[i];
            win->root->vtable->draw(win->root, ctx, lute_rect_intersect(*rect, win->root->_rect));
            continue;
        }

        arrlist_clear(&win->dirty_areas);
//        cairo_paint(ctx);
        cairo_destroy(ctx);
        cairo_surface_destroy(surface);
    });
}

void lute_main() {
    xcb_flush (lute_xcb_state.conn);
    while (true) {
        if (handle_events()) {
            break;
        }
        xcb_flush (lute_xcb_state.conn);
        draw();
        xcb_flush (lute_xcb_state.conn);
    }
}