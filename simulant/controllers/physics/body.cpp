
#include "body.h"
#include "simulation.h"
#include "collision_listener.h"

#include "../../nodes/stage_node.h"
#include "../../nodes/actor.h"
#include "../../time_keeper.h"
#include "../../deps/bounce/bounce.h"
#include "../../stage.h"

namespace smlt {
namespace controllers {
namespace impl {

Body::Body(Controllable* object, RigidBodySimulation* simulation):
    Controller(),
    simulation_(simulation->shared_from_this()) {

    object_ = dynamic_cast<StageNode*>(object);
    if(!object_) {
        throw std::runtime_error("Tried to attach a rigid body controller to something that isn't moveable");
    }

    // Keep each body's last_state in sync when the simulation is stepped
    simulation_stepped_connection_ = simulation->signal_simulation_pre_step().connect([this]() {
        if(auto sim = simulation_.lock()) {
            last_state_ = sim->body_transform(this);
        }
    });
}

Body::~Body() {
    simulation_stepped_connection_.disconnect();
}

bool Body::init() {
    auto sim = simulation_.lock();
    if(!sim) {
        return false;
    }

    body_ = sim->acquire_body(this);

    return true;
}

void Body::cleanup() {
    for(auto listener: listeners_) {
        unregister_collision_listener(listener);
    }

    auto sim = simulation_.lock();
    if(sim) {
        sim->release_body(this);
    }
}

void Body::move_to(const Vec3& position) {
    auto sim = simulation_.lock();
    if(!sim) {
        return;
    }

    auto xform = sim->body_transform(this);
    sim->set_body_transform(
        this,
        position,
        xform.second
    );

    object_->move_to_absolute(position);
}

void Body::rotate_to(const Quaternion& rotation) {
    auto sim = simulation_.lock();
    if(!sim) {
        return;
    }

    auto xform = sim->body_transform(this);
    sim->set_body_transform(
        this,
        xform.first,
        rotation
    );

    object_->rotate_to_absolute(rotation);
}

void Body::update(float dt) {
    const bool INTERPOLATION_ENABLED = true;

    auto sim = simulation_.lock();
    if(!sim) {
        return;
    }

    if(INTERPOLATION_ENABLED) {
        auto prev_state = last_state_; // This is set by the signal connected in Body::Body()
        auto next_state = sim->body_transform(this);

        float t = sim->time_keeper_->fixed_step_remainder() / dt;

        auto new_pos = prev_state.first.lerp(next_state.first, t);
        auto new_rot = prev_state.second.slerp(next_state.second, t);

        object_->move_to_absolute(new_pos);
        object_->rotate_to_absolute(new_rot);
    } else {
        auto state = sim->body_transform(this);
        object_->move_to_absolute(state.first);
        object_->rotate_to_absolute(state.second);
    }
}

void Body::store_collider(b3Shape *shape, const PhysicsMaterial &material) {
    // Store details about the collider so that when contacts
    // arise we can provide more detailed information to the user
    ColliderDetails details;
    details.material = material;

    // Make sure the b3Shape has this body as its userData!
    shape->SetUserData(this);

    collider_details_.insert(std::make_pair(shape, details));
}

void Body::contact_started(const Collision &collision) {
    for(auto listener: listeners_) {
        listener->on_collision_enter(collision);
    }
}

void Body::contact_finished(const Collision& collision) {
    for(auto listener: listeners_) {
        listener->on_collision_exit(collision);
    }
}

void Body::add_box_collider(const Vec3 &size, const PhysicsMaterial &properties, const Vec3 &offset, const Quaternion &rotation) {
    auto sim = simulation_.lock();
    if(!sim) {
        return;
    }

    b3Vec3 p;
    b3Quat q;
    to_b3vec3(offset, p);
    to_b3quat(rotation, q);
    b3Transform tx(p, q);

    // Apply scaling
    tx.rotation[0][0] = size.x * 0.5;
    tx.rotation[1][1] = size.y * 0.5;
    tx.rotation[2][2] = size.z * 0.5;

    auto def = std::make_shared<b3BoxHull>();
    def->Set(size.x * 0.5, size.y * 0.5, size.z * 0.5);
    def->SetTransform(tx);
    hulls_.push_back(def);

    b3HullShape hsdef;
    hsdef.m_hull = def.get();

    b3ShapeDef sdef;
    sdef.shape = &hsdef;
    sdef.userData = this;
    sdef.density = properties.density;
    sdef.friction = properties.friction;
    sdef.restitution = properties.bounciness;

    store_collider(sim->bodies_.at(this)->CreateShape(sdef), properties);

}

void Body::add_sphere_collider(const float diameter, const PhysicsMaterial& properties, const Vec3& offset) {
    auto sim = simulation_.lock();
    if(!sim) {
        return;
    }

    b3SphereShape sphere;
    to_b3vec3(offset, sphere.m_center);
    sphere.m_radius = diameter * 0.5;

    b3ShapeDef sdef;
    sdef.shape = &sphere;
    sdef.density = properties.density;
    sdef.friction = properties.friction;
    sdef.restitution = properties.bounciness;

    store_collider(sim->bodies_.at(this)->CreateShape(sdef), properties);
}

void Body::register_collision_listener(CollisionListener *listener) {
    listeners_.insert(listener);
    listener->watching_.insert(this);
}

void Body::unregister_collision_listener(CollisionListener *listener) {
    listener->watching_.erase(this);
    listeners_.erase(listener);
}

}
}

}
