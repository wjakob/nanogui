#ifdef NANOGUI_PYTHON

#include "python.h"

void register_constants_glfw(py::module &m) {
    /* GLFW constants */
    {
        #define C(name) g.attr(#name) = py::int_(GLFW_##name);
        py::module g = m.def_submodule("glfw");
        C(KEY_UNKNOWN); C(KEY_SPACE); C(KEY_APOSTROPHE); C(KEY_COMMA);
        C(KEY_MINUS); C(KEY_PERIOD); C(KEY_SLASH); C(KEY_0); C(KEY_1);
        C(KEY_2); C(KEY_3); C(KEY_4); C(KEY_5); C(KEY_6); C(KEY_7); C(KEY_8);
        C(KEY_9); C(KEY_SEMICOLON); C(KEY_EQUAL); C(KEY_A); C(KEY_B); C(KEY_C);
        C(KEY_D); C(KEY_E); C(KEY_F); C(KEY_G); C(KEY_H); C(KEY_I); C(KEY_J);
        C(KEY_K); C(KEY_L); C(KEY_M); C(KEY_N); C(KEY_O); C(KEY_P); C(KEY_Q);
        C(KEY_R); C(KEY_S); C(KEY_T); C(KEY_U); C(KEY_V); C(KEY_W); C(KEY_X);
        C(KEY_Y); C(KEY_Z); C(KEY_LEFT_BRACKET); C(KEY_BACKSLASH);
        C(KEY_RIGHT_BRACKET); C(KEY_GRAVE_ACCENT); C(KEY_WORLD_1);
        C(KEY_WORLD_2); C(KEY_ESCAPE); C(KEY_ENTER); C(KEY_TAB);
        C(KEY_BACKSPACE); C(KEY_INSERT); C(KEY_DELETE); C(KEY_RIGHT);
        C(KEY_LEFT); C(KEY_DOWN); C(KEY_UP); C(KEY_PAGE_UP); C(KEY_PAGE_DOWN);
        C(KEY_HOME); C(KEY_END); C(KEY_CAPS_LOCK); C(KEY_SCROLL_LOCK);
        C(KEY_NUM_LOCK); C(KEY_PRINT_SCREEN); C(KEY_PAUSE); C(KEY_F1);
        C(KEY_F2); C(KEY_F3); C(KEY_F4); C(KEY_F5); C(KEY_F6); C(KEY_F7);
        C(KEY_F8); C(KEY_F9); C(KEY_F10); C(KEY_F11); C(KEY_F12); C(KEY_F13);
        C(KEY_F14); C(KEY_F15); C(KEY_F16); C(KEY_F17); C(KEY_F18); C(KEY_F19);
        C(KEY_F20); C(KEY_F21); C(KEY_F22); C(KEY_F23); C(KEY_F24); C(KEY_F25);
        C(KEY_KP_0); C(KEY_KP_1); C(KEY_KP_2); C(KEY_KP_3); C(KEY_KP_4);
        C(KEY_KP_5); C(KEY_KP_6); C(KEY_KP_7); C(KEY_KP_8); C(KEY_KP_9);
        C(KEY_KP_DECIMAL); C(KEY_KP_DIVIDE); C(KEY_KP_MULTIPLY);
        C(KEY_KP_SUBTRACT); C(KEY_KP_ADD); C(KEY_KP_ENTER); C(KEY_KP_EQUAL);
        C(KEY_LEFT_SHIFT); C(KEY_LEFT_CONTROL); C(KEY_LEFT_ALT);
        C(KEY_LEFT_SUPER); C(KEY_RIGHT_SHIFT); C(KEY_RIGHT_CONTROL);
        C(KEY_RIGHT_ALT); C(KEY_RIGHT_SUPER); C(KEY_MENU); C(KEY_LAST);
        C(MOD_SHIFT); C(MOD_CONTROL); C(MOD_ALT); C(MOD_SUPER);
        C(MOUSE_BUTTON_1); C(MOUSE_BUTTON_2); C(MOUSE_BUTTON_3);
        C(MOUSE_BUTTON_4); C(MOUSE_BUTTON_5); C(MOUSE_BUTTON_6);
        C(MOUSE_BUTTON_7); C(MOUSE_BUTTON_8); C(MOUSE_BUTTON_LAST);
        C(MOUSE_BUTTON_LEFT); C(MOUSE_BUTTON_RIGHT); C(MOUSE_BUTTON_MIDDLE);
        C(RELEASE); C(PRESS); C(REPEAT);
        #undef C
    }
}

#endif
