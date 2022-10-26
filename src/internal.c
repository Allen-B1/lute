#include "internal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <lute.h>
#include <lute/themes/clarion.h>
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <dlfcn.h>
#endif

const LuteButtonStyle* LUTE_BUTTON_DEFAULT_STYLE;
LuteColor LUTE_BUTTON_PRIMARY_BACKGROUND;
LuteColor LUTE_BUTTON_ACCENT_BACKGROUND;
LuteColor LUTE_BUTTON_DANGER_BACKGROUND;
LuteColor LUTE_GROUP_DEFAULT_BACKGROUND;

static void lute_internal_set_theme(const char* name) {
    #if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
    if (name[0] == '/') {
        void* handle = dlopen(name, RTLD_NOW);
        #define SET(name, type) name = *((type*)dlsym(handle, #name))

        SET(LUTE_GROUP_DEFAULT_BACKGROUND, LuteColor);

        SET(LUTE_BUTTON_DEFAULT_STYLE, LuteButtonStyle*);
        SET(LUTE_BUTTON_PRIMARY_BACKGROUND, LuteColor);
        SET(LUTE_BUTTON_ACCENT_BACKGROUND, LuteColor);
        SET(LUTE_BUTTON_DANGER_BACKGROUND, LuteColor);

        dlclose(handle);
        return;
    }
    #endif

    if (strcmp(name, "clarion") != 0) {
        fprintf(stderr, "lute warning: unknown theme '%s'. defaulting to clarion.\n", name);
    }
    LUTE_GROUP_DEFAULT_BACKGROUND = LUTE_CLARION_GROUP_DEFAULT_BACKGROUND;

    LUTE_BUTTON_DEFAULT_STYLE = &LUTE_CLARION_BUTTON_STYLE;
    LUTE_BUTTON_PRIMARY_BACKGROUND = LUTE_CLARION_BUTTON_PRIMARY_BACKGROUND;
    LUTE_BUTTON_ACCENT_BACKGROUND = LUTE_CLARION_BUTTON_ACCENT_BACKGROUND;
    LUTE_BUTTON_DANGER_BACKGROUND = LUTE_CLARION_BUTTON_DANGER_BACKGROUND;
}

void lute_internal_setup_theme(const char* name) {
    if (name == NULL) {
        const char* theme = getenv("LUTE_THEME");
        if (theme == NULL) {
            theme = "clarion";
        }
        lute_internal_set_theme(theme);
    } else {
        lute_internal_set_theme(name);
    }
}