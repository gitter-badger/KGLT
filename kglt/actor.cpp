#include "stage.h"
#include "actor.h"

namespace kglt {

Actor::Actor(Stage* stage, ActorID id):
    generic::Identifiable<ActorID>(id),
    ParentSetterMixin<Object>(stage),
    Source(stage),
    render_priority_(RENDER_PRIORITY_MAIN) {

}

Actor::Actor(Stage* stage, ActorID id, MeshID mesh):
    generic::Identifiable<ActorID>(id),
    ParentSetterMixin<Object>(stage),
    Source(stage),
    render_priority_(RENDER_PRIORITY_MAIN) {

    set_mesh(mesh);
}

void Actor::override_material_id(MaterialID mat) {
    for(SubActor::ptr se: subactors_) {
        se->override_material_id(mat);
    }
}

const VertexData& Actor::shared_data() const {
    return mesh_->shared_data();
}


void Actor::rebuild_subactors() {
    subactors_.clear();

    for(SubMeshIndex idx: mesh_->submesh_ids()) {
        subactors_.push_back(SubActor::create(*this, &mesh_->submesh(idx)));
    }
}

void Actor::set_mesh(MeshID mesh) {
    if(!mesh) {
        submeshes_changed_connection_.disconnect();
        subactors_.clear();
        mesh_.reset();
        return;
    }

    //Increment the ref-count on this mesh
    mesh_ = stage()->mesh(mesh).__object;

    //Rebuild the subactors to match the meshes submeshes
    rebuild_subactors();

    //Watch the mesh for changes to its submeshes so we can adapt to it
    submeshes_changed_connection_ = mesh_->signal_submeshes_changed().connect(std::bind(&Actor::rebuild_subactors, this));

    signal_mesh_changed_(id());
}

const AABB Actor::aabb() const {
    if(has_mesh()) {
        return mesh()->aabb();
    }

    return AABB();
}

const AABB Actor::transformed_aabb() const {
    AABB box = aabb(); //Get the untransformed one

    auto pos = absolute_position();
    kmVec3Add(&box.min, &box.min, &pos);
    kmVec3Add(&box.max, &box.max, &pos);
    return box;
}

void Actor::ask_owner_for_destruction() {
    stage()->delete_actor(id());
}

const MaterialID SubActor::material_id() const {
    if(material_) {
        return material_->id();
    }

    return submesh().material_id();
}

void SubActor::override_material_id(MaterialID material) {
    //Store the pointer to maintain the ref-count
    material_ = parent_.stage()->material(material).__object;
}

ProtectedPtr<Mesh> Actor::mesh() const {
    return stage()->mesh(mesh_id());
}

const SubMeshIndex SubActor::submesh_id() const {
    if(!submesh_) {
        throw ValueError("Submesh was not initialized");
    }

    return submesh_->id();
}

SubMesh& SubActor::submesh() {
    if(!submesh_) {
        throw ValueError("Submesh was not initialized");
    }

    return *submesh_;
}

const SubMesh& SubActor::submesh() const {
    if(!submesh_) {
        throw ValueError("Submesh was not initialized");
    }

    return *submesh_;
}

}
