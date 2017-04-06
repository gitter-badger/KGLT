//
//   Copyright (c) 2011-2017 Luke Benstead https://simulant-engine.appspot.com
//
//     This file is part of Simulant.
//
//     Simulant is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
//
//     Simulant is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License
//     along with Simulant.  If not, see <http://www.gnu.org/licenses/>.
//

#include "rigid_body.h"
#include "simulation.h"
#include "../../nodes/actor.h"
#include "../../stage.h"
#include "../../deps/bounce/bounce.h"

// These are just to keep Bounce happy
bool b3PushProfileScope(char const*) { return false; }
void b3PopProfileScope() {}


namespace smlt {
namespace controllers {


RigidBody::RigidBody(Controllable* object, RigidBodySimulation* simulation, ColliderType collider):
    Body(object, simulation, collider) {

}

RigidBody::~RigidBody() {

}

void RigidBody::add_force(const Vec3 &force) {
    auto sim = simulation_.lock();
    if(!sim) {
        return;
    }

    b3Body* b = sim->bodies_.at(this);

    b3Vec3 v;
    to_b3vec3(force, v);
    b->ApplyForceToCenter(v, true);
}

void RigidBody::add_impulse(const Vec3& impulse) {
    auto sim = simulation_.lock();
    if(!sim) {
        return;
    }

    b3Body* b = sim->bodies_.at(this);

    b3Vec3 v;
    to_b3vec3(impulse, v);
    b->ApplyLinearImpulse(v, b->GetPosition(), true);
}

void RigidBody::add_impulse_at_position(const Vec3& impulse, const Vec3& position) {
    auto sim = simulation_.lock();
    if(!sim) {
        return;
    }

    b3Body* b = sim->bodies_.at(this);

    b3Vec3 i, p;
    to_b3vec3(impulse, i);
    to_b3vec3(position, p);
    b->ApplyLinearImpulse(i, p, true);
}

float RigidBody::mass() const {
    auto sim = simulation_.lock();
    if(!sim) {
        return 0;
    }

    const b3Body* b = sim->bodies_.at(this);
    return b->GetMass();
}

Vec3 RigidBody::linear_velocity() const {
    auto sim = simulation_.lock();
    if(!sim) {
        return Vec3();
    }

    const b3Body* b = sim->bodies_.at(this);

    Vec3 v;
    to_vec3(b->GetLinearVelocity(), v);
    return v;
}

Vec3 RigidBody::linear_velocity_at(const Vec3& position) const {
    auto sim = simulation_.lock();
    if(!sim) {
        return Vec3();
    }

    const b3Body* b = sim->bodies_.at(this);

    b3Vec3 bv;
    to_b3vec3(position, bv);
    auto direction_to_point = bv - b->GetPosition();
    auto relative_torque = b3Cross(b->GetAngularVelocity(), direction_to_point);

    Vec3 v;
    to_vec3(b->GetLinearVelocity() + relative_torque, v);
    return v;
}

Vec3 RigidBody::position() const {
    auto sim = simulation_.lock();
    if(!sim) {
        return Vec3();
    }

    return sim->body_transform(this).first;
}

Quaternion RigidBody::rotation() const {
    auto sim = simulation_.lock();
    if(!sim) {
        return Quaternion();
    }

    return sim->body_transform(this).second;
}

void RigidBody::add_force_at_position(const Vec3& force, const Vec3& position) {
    auto sim = simulation_.lock();
    if(!sim) {
        return;
    }

    b3Body* b = sim->bodies_.at(this);

    b3Vec3 f, p;
    to_b3vec3(force, f);
    to_b3vec3(position, p);

    b->ApplyForce(f, p, true);
}

void RigidBody::add_torque(const Vec3& torque) {
    auto sim = simulation_.lock();
    if(!sim) {
        return;
    }

    b3Body* b = sim->bodies_.at(this);
    b3Vec3 t;
    to_b3vec3(torque, t);
    b->ApplyTorque(t, true);
}

}
}
