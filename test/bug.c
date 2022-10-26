#include <cairo/cairo.h>
#include <cairo/cairo-xcb.h>
#include <xcb/xcb.h>

void draw(xcb_connection_t* conn, xcb_screen_t* screen, xcb_visualtype_t* visual, xcb_window_t win);
int main() {
    xcb_connection_t* conn = xcb_connect(NULL, NULL);
    xcb_screen_t* screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;

    xcb_visualtype_t* visual = NULL;
    xcb_depth_iterator_t iter = xcb_screen_allowed_depths_iterator(screen);
    for (; iter.rem; xcb_depth_next(&iter)) {
        if (iter.data->depth != 32) continue;

        xcb_visualtype_iterator_t visual_iter = xcb_depth_visuals_iterator(iter.data);
        for (; visual_iter.rem; xcb_visualtype_next(&visual_iter)) {
            visual = visual_iter.data;
            goto found_visual;
        }
    }
    found_visual:;

    xcb_colormap_t colormap = xcb_generate_id(conn);
    xcb_create_colormap(conn, XCB_COLORMAP_ALLOC_NONE, colormap, screen->root, visual->visual_id);

    xcb_window_t win = xcb_generate_id(conn);
    xcb_create_window(conn, 32, win, screen->root, 0, 0, 500, 500, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, visual->visual_id,
        XCB_CW_BACK_PIXMAP | XCB_CW_BORDER_PIXEL | XCB_CW_EVENT_MASK | XCB_CW_COLORMAP, 
        (uint32_t[]) { XCB_NONE, 0, XCB_EVENT_MASK_EXPOSURE, colormap }
    );
    xcb_map_window(conn, win);
    xcb_flush(conn);

    while (1) {
        xcb_generic_event_t* evt = xcb_wait_for_event(conn);
        if ((evt->response_type & ~0x80) == XCB_EXPOSE) {
            draw(conn, screen, visual, win);
            xcb_flush(conn);
        }
    }

    xcb_disconnect(conn);

    return 0;
}


void draw(xcb_connection_t* conn, xcb_screen_t* screen, xcb_visualtype_t* visual, xcb_window_t win) {
    cairo_surface_t* surface = cairo_xcb_surface_create(conn, win, visual, 500, 500);
    cairo_t* ctx = cairo_create(surface);

    cairo_set_source_rgba(ctx, 0, 0, 0, 1.0);
    cairo_paint(ctx);

    for (int i = 0; i < 50; i++) {
        cairo_set_source_rgba(ctx, 0.2, i/50.0, 0.45, 1.0);
        cairo_rectangle(ctx, i*2, 10, 2, 200);
        cairo_fill(ctx);

    }

    cairo_destroy(ctx);
    cairo_surface_destroy(surface);
}