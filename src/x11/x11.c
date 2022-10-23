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

    lute_xcb_state.screen = xcb_setup_roots_iterator (xcb_get_setup (lute_xcb_state.conn)).data;

    lute_xcb_state.visual = NULL;
    xcb_depth_iterator_t iter = xcb_screen_allowed_depths_iterator(lute_xcb_state.screen);
    for (; iter.rem; xcb_depth_next(&iter)) {
        xcb_visualtype_iterator_t visual_iter = xcb_depth_visuals_iterator(iter.data);
        for (; visual_iter.rem; xcb_visualtype_next(&visual_iter)) {
            if (visual_iter.data->visual_id == lute_xcb_state.screen->root_visual) {
                lute_xcb_state.visual = visual_iter.data;
                goto found_visual;
            }
        }
    }
    
    found_visual:;
}

void lute_deinit() {
    xcb_disconnect(lute_xcb_state.conn);
    rhmap_deinit(&lute_xcb_state.windows);
}

/** Handles incoming resize events & adds other events to the given arraylist. */
static void get_events(ArrList* events) {
    while (true) {
        xcb_generic_event_t* event = xcb_poll_for_event(lute_xcb_state.conn);
        if (event == NULL) break;

        switch (event->response_type & ~0x80) {
        // 2. Layout
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
        default:
            arrlist_add(events, event);
        }
    }
}

static enum LuteMouseButton button_xcb_to_mouse (int button) {
    switch (button) {
        case 1: return LUTE_MOUSEBUTTON_LEFT;
        case 2: return LUTE_MOUSEBUTTON_MIDDLE;
        case 3: return LUTE_MOUSEBUTTON_RIGHT;
        case 4: return LUTE_MOUSEBUTTON_SCROLL_UP;
        case 5: return LUTE_MOUSEBUTTON_SCROLL_DOWN;
        default: return LUTE_MOUSEBUTTON_UNKNOWN;
    }
}

/** Handles all events in the array list */
static void handle_events(ArrList* events) {
    for (int i = 0; i < events->len; i++) {
        xcb_generic_event_t* event = arrlist_get(events, i);
        switch (event->response_type & ~0x80) {
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
            case XCB_BUTTON_PRESS: {
                xcb_button_press_event_t* btn = (xcb_button_press_event_t*)event;
                xcb_window_t id = btn->event;
                LuteWindow* win = rhmap_get(&lute_xcb_state.windows, (void*)id);
                if (win == NULL) {
                    fprintf(stderr, "Lute error: window in button press event does not exist");
                    continue;
                }
                enum LuteMouseButton button = button_xcb_to_mouse(btn->detail);
                if (win->root != NULL && win->root->vtable->on_mousedown != NULL) {
                    win->root->vtable->on_mousedown(win->root, button, btn->event_x, btn->event_y);
                }

                printf("button press event: %d (%d, %d)\n", btn->detail, btn->event_x, btn->event_y);

                break; }
            case XCB_BUTTON_RELEASE: {
                xcb_button_press_event_t* btn = (xcb_button_press_event_t*)event;
                xcb_window_t id = btn->event;
                LuteWindow* win = rhmap_get(&lute_xcb_state.windows, (void*)id);
                if (win == NULL) {
                    fprintf(stderr, "Lute error: window in button press event does not exist");
                    continue;
                }
                enum LuteMouseButton button = button_xcb_to_mouse(btn->detail);
                if (win->root != NULL && win->root->vtable->on_mouseup != NULL) {
                    win->root->vtable->on_mouseup(win->root, button, btn->event_x, btn->event_y);
                }
                break; }
        }
    }
}

static void draw() {
    xcb_window_t id;
    LuteWindow* win;
    rhmap_iter(&lute_xcb_state.windows, &id, &win, {
        if (win->root == NULL) continue;
        if (!win->all_dirty && win->dirty_areas.len == 0) {
            continue;
        }
  
        cairo_surface_t* surface = cairo_xcb_surface_create(lute_xcb_state.conn, id, lute_xcb_state.visual, win->width, win->height);
        cairo_t* ctx = cairo_create(surface);

        if (win->all_dirty) {
//            puts("all dirty...!");
            win->root->vtable->draw(win->root, ctx, win->root->_rect);
            win->all_dirty = false;
            arrlist_clear(&win->dirty_areas);
            continue;
        }

//        printf("dirty areas: %d\n", (int)win->dirty_areas.len);
        for (size_t i = 0; i < win->dirty_areas.len; i++) {
            LuteRect* rect = win->dirty_areas.data[i];
//            printf(" => dirty area: (%d, %d) %dx%d\n", rect->x, rect->y, rect->width, rect->height);
            win->root->vtable->draw(win->root, ctx, lute_rect_intersect(*rect, win->root->_rect));
            continue;
        }
        arrlist_clear(&win->dirty_areas);

        cairo_destroy(ctx);
        cairo_surface_destroy(surface);
    });
}

void lute_main() {
    xcb_flush (lute_xcb_state.conn);
    ArrList events;
    arrlist_init(&events, 4);
    while (true) {
        get_events(&events); // 1 & 2: Events + Layout
        handle_events(&events); // 3: Update
        arrlist_clear(&events);
        xcb_flush (lute_xcb_state.conn);
        draw(); // 4: Draw
        xcb_flush (lute_xcb_state.conn);
    }
}