#pragma once

#include "../controller.h"
#include "../../event_listener.h"

namespace smlt {
namespace controllers {

class Airplane:
    public ControllerWithInput,
    public Managed<Airplane> {

public:
    Airplane(Controllable* owner, Window* window);

    void set_turn_speed(float x) { turn_speed_ = x; }

    void fixed_update(float step);

    const std::string name() const { return "Airplane"; }

private:
    void on_controller_added(Controllable *controllable);

    Controllable* owner_;
    Window* window_;

    float turn_speed_ = 10.0f;
    float throttle_speed_ = 200.0f;
    float throttle_ = 0.0f;
};


}
}