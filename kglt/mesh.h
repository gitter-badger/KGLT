#ifndef NEWMESH_H
#define NEWMESH_H

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <set>

#include "generic/managed.h"
#include "generic/identifiable.h"

#include <kazmath/kazmath.h>
#include <kazbase/list_utils.h>

#include "loadable.h"
#include "resource.h"
#include "vertex_data.h"
#include "types.h"
#include "interfaces.h"

namespace kglt {

class ResourceManager;

class MeshInterface:
    public Boundable {

public:
    virtual ~MeshInterface() {}
    virtual const VertexData& shared_data() const = 0;
};

class SubMeshInterface:
    public Boundable {

public:
    virtual ~SubMeshInterface() {}

    virtual const VertexData& vertex_data() const = 0;
    virtual const IndexData& index_data() const = 0;
    virtual const MaterialID material_id() const = 0;
    virtual const MeshArrangement arrangement() const = 0;

    virtual void _update_vertex_array_object() = 0;
    virtual void _bind_vertex_array_object() = 0;
};

enum VertexSharingMode {
    VERTEX_SHARING_MODE_SHARED,
    VERTEX_SHARING_MODE_INDEPENDENT
};

class SubMesh :
    public SubMeshInterface,
    public Managed<SubMesh>,
    public generic::Identifiable<SubMeshID> {

public:
    SubMesh(
        SubMeshID id,
        Mesh* parent,
        const std::string& name,
        MaterialID material,
        MeshArrangement arrangement=MESH_ARRANGEMENT_TRIANGLES,
        VertexSharingMode vertex_sharing=VERTEX_SHARING_MODE_SHARED
    );

    virtual ~SubMesh();

    VertexData& vertex_data();
    IndexData& index_data();

    const VertexData& vertex_data() const;
    const IndexData& index_data() const;

    const MaterialID material_id() const;
    void set_material_id(MaterialID mat);

    void set_diffuse(const kglt::Colour& colour);

    const MeshArrangement arrangement() const { return arrangement_; }

    const AABB aabb() const {
        return bounds_;
    }

    bool uses_shared_vertices() const { return uses_shared_data_; }
    void reverse_winding();
    void transform_vertices(const Mat4 &transformation);
    void set_texture_on_material(uint8_t unit, TextureID tex, uint8_t pass=0);

    void _recalc_bounds();
    void _update_vertex_array_object();
    void _bind_vertex_array_object();

    void generate_texture_coordinates_cube(uint32_t texture=0);
private:
    Mesh* parent_;

    MaterialPtr material_;
    MeshArrangement arrangement_;
    bool uses_shared_data_;

    VertexData vertex_data_;
    IndexData index_data_;
    VertexArrayObject::ptr vertex_array_object_;

    bool vertex_data_dirty_ = false;
    bool index_data_dirty_ = false;

    AABB bounds_;

    sig::connection vrecalc_;
    sig::connection irecalc_;
};

typedef generic::TemplatedManager<SubMesh, SubMeshID> TemplatedSubMeshManager;

class Mesh :
    public MeshInterface,
    public Resource,
    public Loadable,
    public Managed<Mesh>,
    public generic::Identifiable<MeshID>,
    public TemplatedSubMeshManager {

public:
    Mesh(MeshID id, ResourceManager* resource_manager);

    VertexData& shared_data() {
        return shared_data_;
    }

    const VertexData& shared_data() const {
        return shared_data_;
    }

    SubMeshID new_submesh_with_material(
        MaterialID material,
        MeshArrangement arrangement=MESH_ARRANGEMENT_TRIANGLES,
        VertexSharingMode vertex_sharing=VERTEX_SHARING_MODE_SHARED
    );

    SubMeshID new_submesh(
        MeshArrangement arrangement=MESH_ARRANGEMENT_TRIANGLES,
        VertexSharingMode vertex_sharing=VERTEX_SHARING_MODE_SHARED
    );

    SubMeshID new_submesh_as_rectangle(MaterialID material, float width, float height, const Vec3& offset=Vec3());
    SubMeshID new_submesh_as_box(MaterialID material, float width, float height, float depth, const Vec3& offset=Vec3());
    uint32_t submesh_count() const { return TemplatedSubMeshManager::manager_count(); }

    SubMesh* submesh(SubMeshID index);
    SubMesh* any_submesh() const;
    SubMesh* only_submesh() const;

    void delete_submesh(SubMeshID index);
    void clear();

    void enable_debug(bool value);

    void set_material_id(MaterialID material); ///< Apply material to all submeshes
    void set_diffuse(const kglt::Colour& colour, bool include_submeshes=true); ///< Override vertex colour on all vertices

    void reverse_winding(); ///< Reverse the winding of all submeshes
    void set_texture_on_material(uint8_t unit, TextureID tex, uint8_t pass=0); ///< Replace the texture unit on all submesh materials

    sig::signal<void ()>& signal_submeshes_changed() { return signal_submeshes_changed_; }

    const AABB aabb() const;
    void normalize(); //Scales the mesh so it has a radius of 1.0
    void transform_vertices(const kglt::Mat4& transform, bool include_submeshes=true);

private:
    friend class SubMesh;
    void _update_buffer_object();

    bool shared_data_dirty_ = false;
    VertexData shared_data_;
    BufferObject::ptr shared_data_buffer_object_;

    SubMeshID normal_debug_mesh_;

    sig::signal<void ()> signal_submeshes_changed_;
};

}

#endif // NEWMESH_H
