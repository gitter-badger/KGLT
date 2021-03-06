#include <kazbase/exceptions.h>
#include <kazbase/logging.h>
#include "utils/glcompat.h"
#include "utils/gl_error.h"
#include "kazbase/unicode.h"
#include "input_controller.h"
#include "window.h"

namespace kglt {

Window::Window() {

}

Window::~Window() {
    SDL_GL_DeleteContext(context_);
}

void Window::set_title(const std::string& title) {
    if(screen_) {
        SDL_SetWindowTitle(screen_, title.c_str());
    }
}

void Window::show_cursor(bool value) {
	SDL_ShowCursor(value);
}

void Window::cursor_position(int32_t& mouse_x, int32_t& mouse_y) {
	SDL_GetMouseState(&mouse_x, &mouse_y);
}

int event_filter(void* user_data, SDL_Event* event) {
    /*
     *  This event filter is for Android, e.g. when someone switches task
     */
    Window* _this = (Window*) user_data;

    switch(event->type) {
        case SDL_APP_TERMINATING:
            _this->stop_running();
        break;
        case SDL_APP_WILLENTERBACKGROUND: {
            unicode sdl_err = SDL_GetError();
            if(!sdl_err.empty()) {
                L_ERROR(_u("Something went wrong with SDL: ") + sdl_err);
            }

            L_INFO("Application is entering the background, disabling rendering");


            _this->set_paused(true);
            {
                //See WindowBase::context_lock_ for details
                std::lock_guard<std::mutex> context_lock(_this->context_lock());
                _this->set_has_context(false);
            }
        } break;
        case SDL_APP_DIDENTERFOREGROUND: {
            unicode sdl_err = SDL_GetError();
            if(!sdl_err.empty()) {
                L_ERROR(_u("Something went wrong with SDL: ") + sdl_err);
            }

            L_INFO("Application is entering the foreground, enabling rendering");
            {
                //See WindowBase::context_lock_ for details
                std::lock_guard<std::mutex> context_lock(_this->context_lock());
                _this->set_has_context(true);
            }
            //FIXME: Reload textures and shaders
            _this->set_paused(false);
        } break;
        default:
            break;
    }

    return 1;
}

void Window::check_events() {
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        input_controller().handle_event(event);

        switch(event.type) {
            case SDL_QUIT:
                stop_running();
                break;
            case SDL_WINDOWEVENT: {
                /* Make sure we pause/unpause when the window is minimized and restored.
                 * We also unpause on maximize just in case (although I imagine that we shouldn't
                 * see a maximize without a restore */
                switch(event.window.event) {
                    case SDL_WINDOWEVENT_MINIMIZED:
                        set_paused(true);
                    break;
                    case SDL_WINDOWEVENT_RESTORED:
                    case SDL_WINDOWEVENT_MAXIMIZED:
                        set_paused(false);
                    break;
                    default:
                        break;
                }
            } break;
            case SDL_MOUSEMOTION: {
                handle_mouse_motion(event.motion.x, event.motion.y);
            } break;
            case SDL_MOUSEBUTTONDOWN: {
                L_DEBUG(_u("MOUSEDOWN received: {0}").format(event.button.button));
                handle_mouse_button_down(event.button.button);
            } break;
            case SDL_MOUSEBUTTONUP: {
                L_DEBUG(_u("MOUSEUP received: {0}").format(event.button.button));
                handle_mouse_button_up(event.button.button);
            } break;
            case SDL_FINGERDOWN: {
                L_DEBUG(_u("FINGERDOWN received: {0} - {1}, {2}").format(event.tfinger.fingerId, event.tfinger.x, event.tfinger.y));
                int x, y;
                denormalize(event.tfinger.x, event.tfinger.y, x, y);
                handle_touch_down(event.tfinger.fingerId, x, y);
            } break;
            case SDL_FINGERMOTION: {
                L_DEBUG(_u("FINGERMOTION received: {0}, {1}").format(event.tfinger.x, event.tfinger.y));
                int x, y;
                denormalize(event.tfinger.x, event.tfinger.y, x, y);
                handle_touch_motion(event.tfinger.fingerId, x, y);
            } break;
            case SDL_FINGERUP: {
                L_DEBUG(_u("FINGERUP received: {0} - {1}, {2}").format(event.tfinger.fingerId, event.tfinger.x, event.tfinger.y));
                int x, y;
                denormalize(event.tfinger.x, event.tfinger.y, x, y);
                handle_touch_up(event.tfinger.fingerId, x, y);
            } break;

            default:
                L_WARN_ONCE(_u("Unhandled event {0}").format(event.type));
                break;
        }
    }
}

void Window::denormalize(float x, float y, int& xout, int& yout) {
    /**
        Given normalized screen coordinates, outputs the absolute position
    */

    //FIXME: This should use SDL_GetTouch and a touchID to get the device dimensions
    xout = (int) (x * float(width()));
    yout = (int) (y * float(height()));
}

bool Window::create_window(int width, int height, int bpp, bool fullscreen) {
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        L_ERROR(_u("Unable to initialize SDL {0}").format(SDL_GetError()));
        return false;
    }

    int32_t flags = SDL_WINDOW_OPENGL;
    if(fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    screen_ = SDL_CreateWindow(
        "",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width, height,
        flags
    );
    assert(screen_);

#ifndef __ANDROID__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 5);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#else
    SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#endif

    context_ = SDL_GL_CreateContext(screen_);

    SDL_SetEventFilter(event_filter, this);
    set_has_context(true); //Mark that we have a valid GL context

#ifndef __ANDROID__
    glewExperimental = true;
    GLenum err = glewInit();
    glGetError(); //Sigh, sets this regardless
    assert(err == GLEW_OK);
    assert(GLEW_VERSION_3_1);
#endif
    //Reset the width and height to whatever was actually created
    SDL_GetWindowSize(screen_, &width, &height);

    set_width(width);
    set_height(height);

    SDL_ShowCursor(0);

    L_DEBUG(unicode("{0} joysicks found").format(SDL_NumJoysticks()).encode());
    for(uint16_t i = 0; i < SDL_NumJoysticks(); i++) {
        if(SDL_IsGameController(i)) {
            SDL_GameController* controller = SDL_GameControllerOpen(i);
            L_DEBUG(SDL_GameControllerName(controller));
        }
    }

    return true;
}

void Window::swap_buffers() {
    SDL_GL_SwapWindow(screen_);
}

}
