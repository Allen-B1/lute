#include <rhmap.h>
#include <arrlist.h>

typedef struct {
    xcb_connection_t* conn;
    xcb_screen_t* screen;
    xcb_visualtype_t* visual;

    RhMap windows;
} LuteState;

extern LuteState lute_xcb_state;

struct LuteWindow {
    xcb_window_t id; // owned
    LuteWidget* root; // unowned ptr, owned object

    // cached width & height from resize (configureNotify) events
    uint16_t width;
    uint16_t height;

    ArrList /*<LuteRect>*/ dirty_areas; // owned

    /** Whether the entire window needs to be repainted */
    bool all_dirty;
};