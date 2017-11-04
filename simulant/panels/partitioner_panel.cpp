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


#include "partitioner_panel.h"
#include "../window.h"
#include "../stage.h"
#include "../nodes/actor.h"
#include "../partitioner.h"

namespace smlt {

PartitionerPanel::PartitionerPanel(Window* window):
    window_(window) {

    window_->signal_stage_removed().connect([=](StageID stage_id) {
        debug_actors_.erase(stage_id);
    });
}

void PartitionerPanel::do_activate() {
    initialize();

    for(auto& pair: debug_actors_) {
        window_->stage(pair.first)->actor(pair.second)->set_visible(true);
    }
}

void PartitionerPanel::do_deactivate() {
    for(auto& pair: debug_actors_) {
        window_->stage(pair.first)->actor(pair.second)->set_visible(false);
    }
}

void PartitionerPanel::initialize() {
    if(initialized_) return;

    /* for stage in stages: stage->new_actor(stage->partitioner->debug_mesh_id()) */
    window_->each_stage([=](uint32_t i, Stage* stage) {
        debug_actors_[stage->id()] = stage->new_actor_with_mesh(
            stage->partitioner->debug_mesh_id(),
            RENDERABLE_CULLING_MODE_NEVER //Important, we never want this to use the partitioner itself
        );
    });

    initialized_ = true;
}


}