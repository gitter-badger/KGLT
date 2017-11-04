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

#include "generic/simple_tostring.h"
#include "stage.h"
#include "debug.h"
#include "nodes/actor.h"
#include "utils/random.h"

namespace smlt {

Debug::Debug(Stage &stage):
    stage_(stage) {

    update_connection_ = stage_.window->signal_frame_finished().connect(
        std::bind(&Debug::update, this)
    );
}

Debug::~Debug() {
    // Make sure we disconnect otherwise crashes happen
    update_connection_.disconnect();
}

void Debug::update() {
    float dt = stage_.window->time_keeper->delta_time();

    std::vector<DebugElement> to_keep;

    for(auto elem: elements_) {
        elem.time_since_created += dt;
        if(elem.time_since_created >= elem.duration) {
            auto mesh = stage_.assets->mesh(mesh_);
            mesh->delete_submesh(elem.submesh->name());
        } else {
            to_keep.push_back(elem);
        }
    }

    elements_ = to_keep;
}

bool Debug::init() {
    mesh_ = stage_.assets->new_mesh(VertexSpecification::POSITION_AND_DIFFUSE);
    actor_ = stage_.new_actor_with_mesh(
        mesh_,
        RENDERABLE_CULLING_MODE_NEVER // Important!
    );

    //Don't GC the material, if there are no debug lines then it won't be attached to the mesh
    material_ = stage_.assets->new_material_from_file(Material::BuiltIns::DIFFUSE_ONLY, GARBAGE_COLLECT_NEVER);

    //Connect regular updates so we can remove debug lines after their duration
    /*stage_.window().signal_frame_finished().connect(
        std::bind(&Debug::update, this)
    );*/

    return true;
}

void Debug::draw_line(const Vec3 &start, const Vec3 &end, const Colour &colour, double duration, bool depth_test) {
    static int32_t line_counter = 0;


    auto mesh = stage_.assets->mesh(mesh_);

    DebugElement element;
    element.submesh = mesh->new_submesh_with_material(std::to_string(++line_counter), material_, MESH_ARRANGEMENT_LINE_STRIP);
    element.colour = colour;
    element.duration = duration;
    element.depth_test = depth_test;
    element.type = DebugElementType::DET_LINE;

    auto& submesh = *element.submesh;

    auto initial = submesh.vertex_data->count();

    submesh.vertex_data->position(start);
    submesh.vertex_data->diffuse(colour);
    submesh.vertex_data->move_next();

    submesh.vertex_data->position(end);
    submesh.vertex_data->diffuse(colour);
    submesh.vertex_data->done();

    submesh.index_data->index(initial);
    submesh.index_data->index(initial + 1);
    submesh.index_data->done();

    elements_.push_back(element);
}

void Debug::draw_ray(const Vec3 &start, const Vec3 &dir, const Colour &colour, double duration, bool depth_test) {
    draw_line(start, start+dir, colour, duration, depth_test);
}

void Debug::draw_point(const Vec3 &position, const Colour &colour, double duration, bool depth_test) {

}

}