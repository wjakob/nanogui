/*
    nanogui/nanogui.cpp -- Basic initialization and utility routines

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/screen.h>

#if defined(_WIN32)
#  include <windows.h>
#endif

#include <nanovg.h>
#include <map>
#include <thread>
#include <chrono>
#include <algorithm>
#include <iostream>

#if !defined(_WIN32)
#  include <locale.h>
#  include <signal.h>
#  include <dirent.h>
#endif

#if !NANOGUI_CUSTOM_FONT_FUNCTION
#include "nanogui_resources.h"
#endif

extern int nvgCreateImageMem(NVGcontext*, int, unsigned char*, int);
extern int nvgCreateImage(NVGcontext*, const char*, int);

NAMESPACE_BEGIN(nanogui)

const struct RttiClass Object::rttiInfoObject = { "Object", sizeof(Object), nullptr, 0 };
RttiClass* Object::rttiClass() const { return RTTI_CLASS_INFO(Object); }

NVGcontext* __nanogui_context = nullptr;

#if !NANOGUI_CUSTOM_FONT_FUNCTION
void __nanogui_get_fontdata(const char* name, void*& data, uint32_t &datasize)
{
  if (!strcmp(name, "sans"))
  {
    data = roboto_regular_ttf;
    datasize = roboto_regular_ttf_size;
  }
  else if (!strcmp(name, "sans-bold"))
  {
    data = roboto_bold_ttf;
    datasize = roboto_bold_ttf_size;
  }
  else if (!strcmp(name, "icons"))
  {
    data = entypo_ttf;
    datasize = entypo_ttf_size;
  }
}
#endif

bool isPointInsideRect(const Vector2i& p, const Vector4i& r)
{
  return (p.x() >= r.x() && p.y() >= r.y() && p.x() <= r.z() && p.y() <= r.w());
}

void nvgRect(NVGcontext* ctx, const Vector2i& pos, const Vector2i& size) { nvgRect(ctx, pos.x(), pos.y(), size.x(), size.y()); }
Vector2f nvgTextBounds(NVGcontext* ctx, float x, float y, const char* string, const char* end)
{
  float bounds[4];
  nvgTextBounds(ctx, x, y, string, end, bounds);
  return {bounds[2]-bounds[0], bounds[3] - bounds[1]};
}

void nvgRect(NVGcontext* ctx, const Vector2f& pos, const Vector2f& size) { nvgRect(ctx, pos.x(), pos.y(), size.x(), size.y()); }
void nvgTranslate(NVGcontext* ctx, const Vector2i& pos) { nvgTranslate(ctx, pos.x(), pos.y() ); }
void nvgTranslate(NVGcontext* ctx, const Vector2f& pos) { nvgTranslate(ctx, pos.x(), pos.y() ); }
void nvgRect(NVGcontext* ctx, const Vector4i& r) { nvgRect(ctx, r.x(), r.y(), r.width(), r.height()); }
void nvgRoundedRect(NVGcontext* ctx, const Vector2i& p, const Vector2i& s, float r) { nvgRoundedRect(ctx, p.x(), p.y(), s.x(), s.y(), r); }
void nvgRoundedRect(NVGcontext* ctx, const Vector2f& p, const Vector2f& s, float r) { nvgRoundedRect(ctx, p.x(), p.y(), s.x(), s.y(), r); }
void nvgText(NVGcontext* ctx, const Vector2i& p, const std::string& text) { nvgText(ctx, p.x(), p.y(), text.c_str(), nullptr); }
void nvgText(NVGcontext* ctx, const Vector2f& p, const std::string& text) { nvgText(ctx, p.x(), p.y(), text.c_str(), nullptr); }
void nvgArc(NVGcontext* ctx, const Vector2f& c, float r, float a0, float a1, int dir) { nvgArc(ctx, c.x(), c.y(), r, a0, a1, dir); }
void nvgCircle(NVGcontext* ctx, const Vector2f& c, float r) { nvgCircle(ctx, c.x(), c.y(), r); }
void nvgFontFaceSize(NVGcontext* ctx, const char* font, float size) { nvgFontFace(ctx, font); nvgFontSize(ctx, size); }

std::string file_dialog(const std::vector<std::pair<std::string, std::string>> &filetypes, bool save) {
  auto result = file_dialog(filetypes, save, false);
  return result.empty() ? std::string() : result.front();
}

namespace sample
{
  bool mainloop_active = false;
  void stop_frame_loop() { mainloop_active = false; }
  bool is_main_loop_active() { return mainloop_active; }

  void run(std::function<void()> frame_func, int refresh)
  {
    if (mainloop_active)
      throw std::runtime_error("Main loop is already running!");

    mainloop_active = true;

    std::thread refresh_thread;
    if (refresh > 0) {
      /* If there are no mouse/keyboard events, try to refresh the
          view roughly every 50 ms (default); this is to support animations
          such as progress bars while keeping the system load
          reasonably low */
      refresh_thread = std::thread([refresh]() {
        std::chrono::milliseconds time(refresh);
        while (is_main_loop_active()) {
          std::this_thread::sleep_for(time);
          post_empty_event();
        }
      });
    }

    frame_loop(frame_func);

    if (refresh > 0)
      refresh_thread.join();
  }

} //end namespace sample

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
        case 1: seq[0] = (char)c ;
    }
    return seq;
}

int __nanogui_get_image(NVGcontext *ctx, const std::string &name, uint8_t *data, uint32_t size) {
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
loadImageDirectory(NVGcontext *ctx, const std::string &path,
                   std::function<bool (const std::string&)> filter)
{
    std::vector<std::pair<int, std::string> > result;
#if !defined(_WIN32)
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
    do
    {
        const char *fname = ffd.cFileName;
#endif
        if (filter != nullptr)
        {
          if (!filter(fname))
            continue;
        }
        if (strstr(fname, "png") == nullptr && strstr(fname, "jpg") == nullptr)
            continue;
        std::string fullName = path + "/" + std::string(fname);
        int img = nvgCreateImage(ctx, fullName.c_str(), 0);
        if (img == 0)
            throw std::runtime_error("Could not open image data!");
        result.push_back(std::make_pair(img, fullName));
#if !defined(_WIN32)
    }
    closedir(dp);
#else
    } while (FindNextFileA(handle, &ffd) != 0);
    FindClose(handle);
#endif
    return result;
}

void logic_error(const char* err, const char* file, int line)
{
  std::cout << err << " FILE:" << file << "  LINE:" << line;
}

#if !defined(__APPLE__)
std::vector<std::string> file_dialog(const std::vector<std::pair<std::string, std::string>> &filetypes, bool save, bool multiple) {
    static const int FILE_DIALOG_MAX_BUFFER = 16384;
    if (save && multiple) {
      logic_error("save and multiple must not both be true.", __FILE__, __LINE__);
      return{};
    }

#if defined(_WIN32)
    OPENFILENAMEW ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAMEW));
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    wchar_t tmp[FILE_DIALOG_MAX_BUFFER];
    ofn.lpstrFile = tmp;
    ZeroMemory(tmp, sizeof(tmp));
    ofn.nMaxFile = FILE_DIALOG_MAX_BUFFER;
    ofn.nFilterIndex = 1;

    std::string filter;

    if (!save && filetypes.size() > 1) {
        filter.append("Supported file types (");
        for (size_t i = 0; i < filetypes.size(); ++i) {
            filter.append("*.");
            filter.append(filetypes[i].first);
            if (i + 1 < filetypes.size())
                filter.append(";");
        }
        filter.append(")");
        filter.push_back('\0');
        for (size_t i = 0; i < filetypes.size(); ++i) {
            filter.append("*.");
            filter.append(filetypes[i].first);
            if (i + 1 < filetypes.size())
                filter.append(";");
        }
        filter.push_back('\0');
    }
    for (auto pair : filetypes) {
        filter.append(pair.second);
        filter.append(" (*.");
        filter.append(pair.first);
        filter.append(")");
        filter.push_back('\0');
        filter.append("*.");
        filter.append(pair.first);
        filter.push_back('\0');
    }
    filter.push_back('\0');

    int size = MultiByteToWideChar(CP_UTF8, 0, &filter[0], (int)filter.size(), NULL, 0);
    std::wstring wfilter(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, &filter[0], (int)filter.size(), &wfilter[0], size);

    ofn.lpstrFilter = wfilter.data();

    if (save) {
        ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
        if (GetSaveFileNameW(&ofn) == FALSE)
            return {};
    } else {
        ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        if (multiple)
            ofn.Flags |= OFN_ALLOWMULTISELECT;
        if (GetOpenFileNameW(&ofn) == FALSE)
            return {};
    }

    size_t i = 0;
    std::vector<std::string> result;
    while (tmp[i] != '\0') {
        std::string filename;
        int tmpSize = (int)wcslen(&tmp[i]);
        if (tmpSize > 0) {
            int filenameSize = WideCharToMultiByte(CP_UTF8, 0, &tmp[i], tmpSize, NULL, 0, NULL, NULL);
            filename.resize(filenameSize, 0);
            WideCharToMultiByte(CP_UTF8, 0, &tmp[i], tmpSize, &filename[0], filenameSize, NULL, NULL);
        }

        result.emplace_back(filename);
        i += tmpSize + 1;
    }

    if (result.size() > 1) {
        for (i = 1; i < result.size(); ++i) {
            result[i] = result[0] + "\\" + result[i];
        }
        result.erase(begin(result));
    }

    return result;
#else
    char buffer[FILE_DIALOG_MAX_BUFFER];
    buffer[0] = '\0';

    std::string cmd = "zenity --file-selection ";
    // The safest separator for multiple selected paths is /, since / can never occur
    // in file names. Only where two paths are concatenated will there be two / following
    // each other.
    if (multiple)
        cmd += "--multiple --separator=\"/\" ";
    if (save)
        cmd += "--save ";
    cmd += "--file-filter=\"";
    for (auto pair : filetypes)
        cmd += "\"*." + pair.first + "\" ";
    cmd += "\"";
    FILE *output = popen(cmd.c_str(), "r");
    if (output == nullptr)
    {
      logic_error("popen() failed -- could not launch zenity!", __FILE__, __LINE__);
      return {};
    }
    while (fgets(buffer, FILE_DIALOG_MAX_BUFFER, output) != NULL)
        ;
    pclose(output);
    std::string paths(buffer);
    paths.erase(std::remove(paths.begin(), paths.end(), '\n'), paths.end());

    std::vector<std::string> result;
    while (!paths.empty()) {
        size_t end = paths.find("//");
        if (end == std::string::npos) {
            result.emplace_back(paths);
            paths = "";
        } else {
            result.emplace_back(paths.substr(0, end));
            paths = paths.substr(end + 1);
        }
    }

    return result;
#endif
}
#endif

void Object::decRef(bool dealloc) const {
    --m_refCount;
    if (m_refCount == 0 && dealloc) {
        delete this;
    } else if (m_refCount < 0) {
        fprintf(stderr, "Internal error: Object reference count < 0!\n");
        abort();
    }
}

Object::~Object() { }

float nvgTextHeight(NVGcontext* ctx, float x, float y, const char* string, const char* end, float* bounds)
{
  float _bounds[4];
  if (!bounds)
    bounds = _bounds;

  nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
  nvgTextBounds(ctx, x, y, string, end, bounds);
  return bounds[3] - bounds[1];
}

NAMESPACE_END(nanogui)

