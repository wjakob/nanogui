#ifdef NANOGUI_PYTHON

#include <thread>
#include <mutex>
#include <condition_variable>
#include "python.h"
#include "nanogui/layout.h"

#if defined(__APPLE__) || defined(__linux__)
#  include <coro.h>
#  include <signal.h>
#endif

#if defined(__APPLE__) || defined(__linux__)
namespace {
    class semaphore {
    public:
        semaphore(int count = 0) : count(count) { }

        void notify() {
            std::unique_lock<std::mutex> lck(mtx);
            ++count;
            cv.notify_one();
        }

        void wait() {
            std::unique_lock<std::mutex> lck(mtx);
            while (count == 0)
                cv.wait(lck);
            --count;
        }

    private:
        std::mutex mtx;
        std::condition_variable cv;
        int count;
    };
}
#endif

extern void register_constants_ui(py::module &m);
extern void register_constants_entypo(py::module &m);
extern void register_widget(py::module &m);
extern void register_layout(py::module &m);
extern void register_basics(py::module &m);
extern void register_button(py::module &m);
extern void register_tabs(py::module &m);
extern void register_textbox(py::module &m);
extern void register_theme(py::module &m);
extern void register_glcanvas(py::module &m);
extern void register_formhelper(py::module &m);
extern void register_misc(py::module &m);
extern void register_glutil(py::module &m);
extern void register_nanovg(py::module &m);

class MainloopHandle;
static MainloopHandle *handle = nullptr;

class MainloopHandle {
public:
    bool active = false;
    bool detached = false;
    int refresh = 0;
    std::thread thread;

    #if defined(__APPLE__) || defined(__linux__)
        coro_context ctx_helper, ctx_main, ctx_thread;
        coro_stack stack;
        semaphore sema;
    #endif

    ~MainloopHandle() {
        join();
        handle = nullptr;
    }

    void join() {
        if (!detached)
            return;

        #if defined(__APPLE__) || defined(__linux__)
            /* Release GIL and disassociate from thread state (which was originally
               associated with the main Python thread) */
            py::gil_scoped_release thread_state(true);

            coro_transfer(&ctx_main, &ctx_thread);
            coro_stack_free(&stack);

            /* Destroy the thread state that was created in mainloop() */
            {
                py::gil_scoped_acquire acquire;
                acquire.dec_ref();
            }
        #endif

        thread.join();
        detached = false;

        #if defined(__APPLE__) || defined(__linux__)
            /* Reacquire GIL and reassociate with thread state
               [via RAII destructor in 'thread_state'] */
        #endif
    }
};

#if defined(__APPLE__) || defined(__linux__)
static void (*sigint_handler_prev)(int) = nullptr;
static void sigint_handler(int sig) {
    nanogui::sample::stop_frame_loop();
    signal(sig, sigint_handler_prev);
    raise(sig);
}
#endif

PYBIND11_MODULE(nanogui, m) {
    m.attr("__doc__") = "NanoGUI plugin";

    py::class_<MainloopHandle>(m, "MainloopHandle")
        .def("join", &MainloopHandle::join);

    m.def("init", &nanogui::init, D(init));
    m.def("shutdown", &nanogui::shutdown, D(shutdown));
    m.def("mainloop", [](int refresh, py::object detach) -> MainloopHandle* {
        if (!detach.is(py::none())) {
            if (handle)
                throw std::runtime_error("Main loop is already running!");

            handle = new MainloopHandle();
            handle->detached = true;
            handle->refresh = refresh;

                handle->thread = std::thread([]{
                    auto window = nanogui::sample::create_window(1600, 900, "NanoGUI Python", true, false);
                    nanogui::sample::create_context();

                    Screen screen({ 1600, 900 }, "NanoGUI Python", false);
                    nanogui::sample::setup_window_params(window, &screen);
                    
                    nanogui::sample::run([&] {
                      nanogui::sample::clear_frame(screen.background());

                      screen.drawAll();

                      nanogui::sample::present_frame(window);

                      /* Wait for mouse/keyboard or empty refresh events */
                      nanogui::sample::wait_events();
                    }, handle->refresh);
                });
            
            return handle;
        } else {
            py::gil_scoped_release release;

            #if defined(__APPLE__) || defined(__linux__)
                sigint_handler_prev = signal(SIGINT, sigint_handler);
            #endif

            auto window = nanogui::sample::create_window(1600, 900, "NanoGUI Python", true, false);
            nanogui::sample::create_context();

            Screen screen({ 1600, 900 }, "NanoGUI Python", false);
            nanogui::sample::setup_window_params(window, &screen);

            nanogui::sample::run([&] {
                nanogui::sample::clear_frame(screen.background());

                screen.drawAll();

                nanogui::sample::present_frame(window);

                /* Wait for mouse/keyboard or empty refresh events */
                nanogui::sample::wait_events();
            }, handle->refresh);

            #if defined(__APPLE__) || defined(__linux__)
                signal(SIGINT, sigint_handler_prev);
            #endif

            return nullptr;
        }
    }, py::arg("refresh") = 50, py::arg("detach") = py::none(),
       D(mainloop), py::keep_alive<0, 2>());

    m.def("stop_frame_loop", &nanogui::sample::stop_frame_loop, D(leave));
    m.def("is_main_loop_active", &nanogui::sample::is_main_loop_active, D(active));
    m.def("file_dialog", (std::string(*)(const std::vector<std::pair<std::string, std::string>> &, bool)) &nanogui::file_dialog, D(file_dialog));
    m.def("file_dialog", (std::vector<std::string>(*)(const std::vector<std::pair<std::string, std::string>> &, bool, bool)) &nanogui::file_dialog, D(file_dialog, 2));
    #if defined(__APPLE__)
        m.def("chdir_to_bundle_parent", &nanogui::chdir_to_bundle_parent);
    #endif
    m.def("utf8", [](int c) { return std::string(utf8(c).data()); }, D(utf8));
    m.def("loadImageDirectory", &nanogui::loadImageDirectory, D(loadImageDirectory));

    py::enum_<IconAlign>(m, "IconAlign", D(IconAlign))
      .value("Left", IconAlign::Left)
      .value("LeftCentered", IconAlign::LeftCentered)
      .value("RightCentered", IconAlign::RightCentered)
      .value("Right", IconAlign::Right);

    py::enum_<Cursor>(m, "Cursor", D(Cursor))
        .value("Arrow", Cursor::Arrow)
        .value("IBeam", Cursor::IBeam)
        .value("Crosshair", Cursor::Crosshair)
        .value("Hand", Cursor::Hand)
        .value("HResize", Cursor::HResize)
        .value("VResize", Cursor::VResize);

    py::enum_<Alignment>(m, "Alignment", D(Alignment))
        .value("Minimum", Alignment::Minimum)
        .value("Middle", Alignment::Middle)
        .value("Maximum", Alignment::Maximum)
        .value("Fill", Alignment::Fill);
    
    py::enum_<TextAlignment>(m, "TextAlignment", D(TextAlignment))
      .value("Auto", TextAlignment::Auto)
      .value("Left", TextAlignment::Left)
      .value("Center", TextAlignment::Center)
      .value("Right", TextAlignment::Right);

    py::enum_<Orientation>(m, "Orientation", D(Orientation))
        .value("Horizontal", Orientation::Horizontal)
        .value("Vertical", Orientation::Vertical);

    register_constants_ui(m);
    register_constants_entypo(m);
    register_widget(m);
    register_layout(m);
    register_basics(m);
    register_button(m);
    register_tabs(m);
    register_textbox(m);
    register_theme(m);
#if defined(NANOVG_OPENGL)
    register_glcanvas(m);
    register_glutil(m);
#endif
    register_formhelper(m);
    register_misc(m);
    register_nanovg(m);
}

#endif
