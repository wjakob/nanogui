#ifdef NANOGUI_PYTHON

#include <thread>
#include <mutex>
#include <condition_variable>
#include "python.h"

#if defined(__APPLE__) || defined(__linux__)
#  include <coro.h>
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

extern void register_constants(py::module &m);
extern void register_eigen(py::module &m);
extern void register_widget(py::module &m);
extern void register_layout(py::module &m);
extern void register_basics(py::module &m);
extern void register_button(py::module &m);
extern void register_tabs(py::module &m);
extern void register_textbox(py::module &m);
extern void register_formhelper(py::module &m);
extern void register_misc(py::module &m);

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

PYBIND11_PLUGIN(nanogui) {
    py::module m("nanogui", "NanoGUI plugin");

    py::class_<MainloopHandle>(m, "MainloopHandle")
        .def("join", &MainloopHandle::join);

    m.def("init", &nanogui::init, D(init));
    m.def("shutdown", &nanogui::shutdown, D(shutdown));
    m.def("mainloop", [](int refresh, py::object detach) -> MainloopHandle* {
        if (detach != py::none()) {
            if (handle)
                throw std::runtime_error("Main loop is already running!");

            handle = new MainloopHandle();
            handle->detached = true;
            handle->refresh = refresh;

            #if defined(__APPLE__) || defined(__linux__)
                /* Release GIL and completely disassociate the calling thread
                   from its associated Python thread state data structure */
                py::gil_scoped_release thread_state(true);

                /* Create a new thread state for the nanogui main loop
                   and reference it once (to keep it from being constructed and
                   destructed at every callback invocation) */
                {
                    py::gil_scoped_acquire acquire;
                    acquire.inc_ref();
                }

                handle->thread = std::thread([]{
                    /* Handshake 1: wait for signal from detach_helper */
                    handle->sema.wait();

                    /* Swap context with main thread */
                    coro_transfer(&handle->ctx_thread, &handle->ctx_main);

                    /* Handshake 2: wait for signal from detach_helper */
                    handle->sema.notify();
                });

                void (*detach_helper)(void *) = [](void *ptr) -> void {
                    MainloopHandle *handle = (MainloopHandle *) ptr;

                    /* Handshake 1: Send signal to new thread  */
                    handle->sema.notify();

                    /* Enter main loop */
                    mainloop(handle->refresh);

                    /* Handshake 2: Wait for signal from new thread */
                    handle->sema.wait();

                    /* Return back to Python */
                    coro_transfer(&handle->ctx_helper, &handle->ctx_main);
                };

                /* Allocate an 8MB stack and transfer context to the
                   detach_helper function */
                coro_stack_alloc(&handle->stack, 8 * 1024 * 1024);
                coro_create(&handle->ctx_helper, detach_helper, handle,
                            handle->stack.sptr, handle->stack.ssze);
                coro_transfer(&handle->ctx_main, &handle->ctx_helper);
            #else
                handle->thread = std::thread([]{
                    mainloop(handle->refresh);
                });
            #endif

            #if defined(__APPLE__) || defined(__linux__)
                /* Reacquire GIL and reassociate with thread state on newly
                   created thread [via RAII destructor in 'thread_state'] */
            #endif

            return handle;
        } else {
            mainloop(refresh);
            return nullptr;
        }
    }, py::arg("refresh") = 50, py::arg("detach") = py::none(),
       D(mainloop), py::keep_alive<0, 2>());

    m.def("leave", &nanogui::leave, D(leave));
    m.def("file_dialog", &nanogui::file_dialog, D(file_dialog));
    #if defined(__APPLE__)
        m.def("chdir_to_bundle_parent", &nanogui::chdir_to_bundle_parent);
    #endif
    m.def("utf8", [](int c) { return std::string(utf8(c).data()); }, D(utf8));
    m.def("loadImageDirectory", &nanogui::loadImageDirectory, D(loadImageDirectory));

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

    py::enum_<Orientation>(m, "Orientation", D(Orientation))
        .value("Horizontal", Orientation::Horizontal)
        .value("Vertical", Orientation::Vertical);

    py::class_<NVGcontext> context(m, "NVGcontext");

    register_constants(m);
    register_eigen(m);
    register_widget(m);
    register_layout(m);
    register_basics(m);
    register_button(m);
    register_tabs(m);
    register_textbox(m);
    register_formhelper(m);
    register_misc(m);

    return m.ptr();
}

#endif
