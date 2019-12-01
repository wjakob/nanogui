#include <nanogui/common.h>

#if NANOGUI_OPENGL_BACKEND

#include <nanogui/screen.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <iostream>
#include <map>

NAMESPACE_BEGIN(nanogui)

bool isMouseButtonLeft(int button) { return button == GLFW_MOUSE_BUTTON_1; }
bool isMouseButtonLeftMod(int button) { return button == (1 << GLFW_MOUSE_BUTTON_1); }
bool isMouseButtonRight(int button) { return button == GLFW_MOUSE_BUTTON_RIGHT; }
bool isMouseActionRelease(int action) { return action == GLFW_RELEASE; }
bool isMouseActionPress(int action) { return action == GLFW_PRESS; }

#if defined(__APPLE__)
  extern void disable_saved_application_state_osx();
#endif

void init() {
#if !defined(_WIN32)
  /* Avoid locale-related number parsing issues */
  setlocale(LC_NUMERIC, "C");
#endif

#if defined(__APPLE__)
  disable_saved_application_state_osx();
#endif

  glfwSetErrorCallback(
    [](int error, const char *descr) {
    if (error == GLFW_NOT_INITIALIZED)
      return; /* Ignore */
    std::cerr << "GLFW error " << error << ": " << descr << std::endl;
  }
  );

  if (!glfwInit())
    throw std::runtime_error("Could not initialize GLFW!");

  glfwSetTime(0);
}

void shutdown() {
  glfwTerminate();
}

float getTimeFromStart() { return glfwGetTime(); }

bool appPostEmptyEvent() { glfwPostEmptyEvent(); return true; }

bool appIsShouldCloseScreen(Screen* screen)
{
  return glfwWindowShouldClose((GLFWwindow*)screen->hwWindow());
}

bool appWaitEvents() { glfwWaitEvents(); return true; }
bool appPollEvents() { glfwPollEvents(); return true; }

bool isKeyboardActionRelease(int action) { return action == GLFW_RELEASE; }
bool isKeyboardModifierCtrl(int modifier) { return modifier & SYSTEM_COMMAND_MOD; }
bool isKeyboardModifierShift(int modifier) { return modifier & GLFW_MOD_SHIFT; }
bool isKeyboardActionPress(int action) { return action == GLFW_PRESS; }
bool isKeyboardActionRepeat(int action) { return action == GLFW_REPEAT; }
bool isKeyboardKeyEscape(int key) { return key == GLFW_KEY_ESCAPE; }

uint32_t key2fourcc(int key)
{
  switch (key) {
#define RET_KEYCODE(k,c) case k: return FOURCCS(c);
  RET_KEYCODE(GLFW_KEY_DELETE, "KDEL")
  RET_KEYCODE(GLFW_KEY_A, "KEYA")
  RET_KEYCODE(GLFW_KEY_X, "KEYX")
  RET_KEYCODE(GLFW_KEY_N, "KEYN")
  RET_KEYCODE(GLFW_KEY_B, "KEYB")
  RET_KEYCODE(GLFW_KEY_C, "KEYC")
  RET_KEYCODE(GLFW_KEY_R, "KEYR")
  RET_KEYCODE(GLFW_KEY_P, "KEYP")
  RET_KEYCODE(GLFW_KEY_V, "KEYV")
  RET_KEYCODE(GLFW_KEY_Z, "KEYZ")
  RET_KEYCODE(GLFW_KEY_LEFT, "LEFT")
  RET_KEYCODE(GLFW_KEY_RIGHT, "RGHT")
  RET_KEYCODE(GLFW_KEY_DOWN, "DOWN")
  RET_KEYCODE(GLFW_KEY_UP, "KBUP")
  RET_KEYCODE(GLFW_KEY_HOME, "HOME")
  RET_KEYCODE(GLFW_KEY_END, "KEND")
  RET_KEYCODE(GLFW_KEY_BACKSPACE, "BACK")
  RET_KEYCODE(GLFW_KEY_ENTER, "ENTR")
#undef RET_KEYCODE
  default: return FOURCCS("UNKN");
  }
}

NAMESPACE_END(nanogui)

#endif