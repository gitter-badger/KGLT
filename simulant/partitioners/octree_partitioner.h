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

#ifndef OCTREE_PARTITIONER_H
#define OCTREE_PARTITIONER_H

#include "../deps/kazsignal/kazsignal.h"

#include "../partitioner.h"
#include "../interfaces.h"
#include "../mesh.h"
#include "impl/octree.h"

namespace smlt {

class Renderable;

typedef std::shared_ptr<Renderable> RenderablePtr;

bool should_split_predicate(const octree_impl::OctreeNode *node);
bool should_merge_predicate(const octree_impl::NodeList& nodes);


class OctreePartitioner :
    public Partitioner {

public:
    OctreePartitioner(Stage* ss):
        Partitioner(ss),
        tree_(ss, &should_split_predicate, &should_merge_predicate) {}

    void add_actor(ActorID obj);
    void remove_actor(ActorID obj);

    void add_geom(GeomID geom_id);
    void remove_geom(GeomID geom_id);

    void add_light(LightID obj);
    void remove_light(LightID obj);

    void add_particle_system(ParticleSystemID ps);
    void remove_particle_system(ParticleSystemID ps);

    std::vector<LightID> lights_visible_from(CameraID camera_id);
    std::vector<RenderablePtr> geometry_visible_from(CameraID camera_id);

    void event_actor_changed(ActorID ent);

    MeshID debug_mesh_id() override { return tree_.debug_mesh_id(); }
private:
    octree_impl::Octree tree_;

    std::map<ActorID, std::vector<BoundableEntity*> > actor_to_registered_subactors_;

    std::map<ActorID, sig::connection> actor_changed_connections_;
    std::map<const BoundableEntity*, RenderablePtr> boundable_to_renderable_;
    std::map<const BoundableEntity*, LightID> boundable_to_light_;

    std::set<ActorID> actors_always_visible_;
    std::set<LightID> lights_always_visible_;
};


}

#endif // OCTREE_PARTITIONER_H
