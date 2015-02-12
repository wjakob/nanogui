#include <nanogui/screen.h>
#if defined(WIN32)
#include <windows.h>
#endif
#include <nanogui/opengl.h>
#include <map>
#include <thread>
#include <chrono>
#include <iostream>

#if !defined(WIN32)
    #include <locale.h>
    #include <signal.h>
    #include <sys/dir.h>
#endif

NANOGUI_NAMESPACE_BEGIN

static bool __mainloop_active = false;
extern std::map<GLFWwindow *, Screen *> __nanogui_screens;

void init() {
    #if !defined(WIN32)
        /* Avoid locale-related number parsing issues */
        setlocale(LC_NUMERIC, "C");
    #endif

    if (!glfwInit())
        throw std::runtime_error("Could not initialize GLFW!");

    glfwSetErrorCallback(
        [](int error, const char *desc) {
            std::cerr << "GLFW error " << error << ": " << desc << std::endl;
        }
    );

    glfwSetTime(0);
}

void mainloop() {
    __mainloop_active = true;

    /* If there are no mouse/keyboard events, try to refresh the
       view roughly every 50 ms; this is to support animations
       such as progress bards while keeping the system load
       reasonably low */
    std::thread refresh_thread = std::thread(
        [&]() {
            std::chrono::milliseconds time(50);
            while (__mainloop_active) {
                std::this_thread::sleep_for(time);
                glfwPostEmptyEvent();
            }
        }
    );

    while (__mainloop_active) {
        int numScreens = 0;
        for (auto kv : __nanogui_screens) {
            Screen *screen = kv.second;
            if (!screen->visible()) {
                continue;
            } else if (glfwWindowShouldClose(screen->glfwWindow())) {
                screen->setVisible(false);
                continue;
            }
            screen->drawAll();
            numScreens++;
        }

        if (numScreens == 0) {
            /* Give up if there was nothing to draw */
            __mainloop_active = false;
            break;
        }

        /* Wait for mouse/keyboard or empty refresh events */
        glfwWaitEvents();
    }

    refresh_thread.join();
}

void leave() {
    __mainloop_active = false;
}

void shutdown() {
    glfwTerminate();
}

std::array<char, 8> utf8(int c) {
    std::array<char, 8> seq;
    int n = 0;
    if (c < 0x80) n = 1;
    else if (c < 0x800) n = 2;
    else if (c < 0x10000) n = 3;
    else if (c < 0x200000) n = 4;
    else if (c < 0x4000000) n = 5;
    else if (c <= 0x7fffffff) n = 6;
    seq[n] = '\0';
    switch (n) {
        case 6: seq[5] = 0x80 | (c & 0x3f); c = c >> 6; c |= 0x4000000;
        case 5: seq[4] = 0x80 | (c & 0x3f); c = c >> 6; c |= 0x200000;
        case 4: seq[3] = 0x80 | (c & 0x3f); c = c >> 6; c |= 0x10000;
        case 3: seq[2] = 0x80 | (c & 0x3f); c = c >> 6; c |= 0x800;
        case 2: seq[1] = 0x80 | (c & 0x3f); c = c >> 6; c |= 0xc0;
        case 1: seq[0] = c;
    }
    return seq;
}

inline int __nanogui_get_image(NVGcontext *ctx, const std::string &name, uint8_t *data, uint32_t size) {
    static std::map<std::string, int> iconCache;
    auto it = iconCache.find(name);
    if (it != iconCache.end())
        return it->second;
    int iconID = nvgCreateImageMem(ctx, 0, data, size);
    if (iconID == 0)
        throw std::runtime_error("Unable to load resource data.");
    iconCache[name] = iconID;
    return iconID;
}

std::vector<std::pair<int, std::string>>
loadImageDirectory(NVGcontext *ctx, const std::string &path) {
    std::vector<std::pair<int, std::string> > result;
#if !defined(WIN32)
    DIR *dp = opendir(path.c_str());
    if (!dp)
        throw std::runtime_error("Could not open image directory!");
    struct dirent *ep;
    while ((ep = readdir(dp))) {
        const char *fname = ep->d_name;
#else
    WIN32_FIND_DATA ffd;
    std::string searchPath = path + "/*.*";
    HANDLE handle = FindFirstFileA(searchPath.c_str(), &ffd);
    if (handle == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Could not open image directory!");
    do {
        const char *fname = ffd.cFileName;
#endif
        if (strstr(fname, "png") == nullptr)
            continue;
        std::string fullName = path + "/" + std::string(fname);
        int img = nvgCreateImage(ctx, fullName.c_str(), 0);
        if (img == 0)
            throw std::runtime_error("Could not open image data!");
        result.push_back(
            std::make_pair(img, fullName.substr(0, fullName.length() - 4)));
#if !defined(WIN32)
    }
    closedir(dp);
#else
    } while (FindNextFileA(handle, &ffd) != 0);
    FindClose(handle);
#endif
    return result;
}

#if !defined(__APPLE__)
std::string file_dialog(const std::vector<std::pair<std::string, std::string>> &filetypes, bool save) {
#define FILE_DIALOG_MAX_BUFFER 1024
#if defined(WIN32)
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    char tmp = '\0';
    ofn.lpstrFile = &tmp;
    ofn.nMaxFile = FILE_DIALOG_MAX_BUFFER;
    ofn.nFilterIndex = 1;

    std::vector<char> filter;
    for (auto pair: filetypes) {
        for (char c : pair.second)
            filter.push_back(c);
        filter.push_back('\0');
        for (char c : pair.first)
            filter.push_back(c);
        filter.push_back('\0');
    }
    filter.push_back('\0');
    ofn.lpstrFilter = filter.data();

    if (save) {
        ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
        if (GetSaveFileNameA(&ofn) == FALSE)
            return "";
    } else {
        ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        if (GetOpenFileNameA(&ofn) == FALSE)
            return "";
    }
    return std::string(ofn.lpstrFile);
#else
    char buffer[FILE_DIALOG_MAX_BUFFER];
    std::string cmd = "/usr/bin/zenity --file-selection ";
    if (save)
        cmd += "--save";
    cmd += "--file-filter=\"";
    for (auto pair: filetypes)
        cmd += "\"*." + pair.first +  "\" ";
    cmd += "\"";
    FILE *output = popen(cmd.c_str(), "r");
    while (fgets(buffer, FILE_DIALOG_MAX_BUFFER, output) != NULL)
        ;
    pclose(output);
    std::string result(buffer);
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    return result;
#endif
}
#endif

NANOGUI_NAMESPACE_END
