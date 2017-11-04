/* *   Copyright (c) 2011-2017 Luke Benstead https://simulant-engine.appspot.com
 *
 *     This file is part of Simulant.
 *
 *     Simulant is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     Simulant is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Simulant.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <vector>
#include <set>
#include <memory>
#include <thread>
#include <mutex>
#include <type_traits>
#include <thread>

#include "../deps/kazlog/kazlog.h"

#include "../generic/property.h"
#include "../generic/managed.h"
#include "../interfaces/updateable.h"
#include "../input/input_manager.h"

namespace smlt {

class InputManager;
class Material;
class Controller;
class Controllable;

typedef std::shared_ptr<Controller> ControllerPtr;

class Controller:
    public Updateable {
public:
    virtual ~Controller() {}

    virtual const std::string name() const = 0;

    void enable();
    void disable();

    void _update_thunk(float dt) override;
    void _late_update_thunk(float dt) override;
    void _fixed_update_thunk(float step) override;

private:
    friend class Controllable;

    virtual void on_controller_added(Controllable* controllable) {}
    virtual void on_controller_removed(Controllable* controllable) {}

    /* Called on the first update after being enabled */
    virtual void on_controller_first_update(Controllable* controllable) {}

    bool is_enabled_ = true;
    bool first_update_done_ = false;
};

class ControllerWithInput : public Controller {
public:
    ControllerWithInput(InputManager* input):
        input_(input) {}

protected:
    Property<ControllerWithInput, InputManager> input = {this, &ControllerWithInput::input_};

private:
    InputManager* input_;
};

class MaterialController : public Controller {
public:
    MaterialController(Material* material):
        Controller(),
        material_(material) {
    }

protected:
    Property<MaterialController, Material> material = { this, &MaterialController::material_ };
    Material* get_material() const { return material_; }

private:
    Material* material_;
};

class Controllable {
public:
    virtual ~Controllable() {
        controllers_.clear();
        controller_names_.clear();
        controller_types_.clear();
    }

    template<typename T>
    bool has_controller() const {
        return controller_types_.count(typeid(T).hash_code()) > 0;
    }

    template<typename T>
    T* controller() const {
        auto hash_code = typeid(T).hash_code();
        if(!controller_types_.count(hash_code)) {
            return nullptr;
        }

        return dynamic_cast<T*>(controller_types_.at(hash_code).get());
    }

    void add_controller(ControllerPtr controller) {
        if(!controller) {
            L_WARN("Tried to add a null controller to the controllable");
            return;
        }

        std::lock_guard<std::mutex> lock(container_lock_);

        if(controller_names_.count(controller->name())) {
            L_WARN(_F("Tried to add a duplicate controller: {0}").format((std::string)controller->name()));
            return;
        }

        controller_types_.insert(std::make_pair(typeid(*controller).hash_code(), controller));
        controller_names_.insert(controller->name());
        controllers_.push_back(controller);

        controller->on_controller_added(this);
    }

    template<typename T>
    T* new_controller() {
        static_assert(std::is_base_of<Controller, T>::value, "Controllers must derive smlt::Controller");
        static_assert(std::is_base_of<Managed<T>, T>::value, "Controllers must derive Managed<T>");
        std::shared_ptr<T> ret = T::create(this);
        add_controller(ret);
        return ret.get();
    }

    template<typename T, typename ...Params>
    T* new_controller(Params&&... params) {
        static_assert(std::is_base_of<Controller, T>::value, "Controllers must derive smlt::Controller");
        static_assert(std::is_base_of<Managed<T>, T>::value, "Controllers must derive Managed<T>");
        std::shared_ptr<T> ret = T::create(this, std::forward<Params>(params)...);
        add_controller(ret);
        return ret.get();
    }

    void fixed_update_controllers(float step) {
        for(auto& controller: controllers_) {
            controller->_fixed_update_thunk(step);
        }
    }

    void update_controllers(float dt) {
        for(auto& controller: controllers_) {

            // Call any overridden functions looking for first update
            if(!controller->first_update_done_) {
                controller->on_controller_first_update(this);
                controller->first_update_done_ = true;
            }

            controller->_update_thunk(dt);
        }
    }

    void late_update_controllers(float dt) {
        for(auto& controller: controllers_) {
            controller->_late_update_thunk(dt);
        }
    }

private:
    std::mutex container_lock_;

    std::vector<ControllerPtr> controllers_;
    std::unordered_set<std::string> controller_names_;
    std::unordered_map<std::size_t, ControllerPtr> controller_types_;

};


}
#endif // CONTROLLER_H