#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <string>
#include <map>

#include "generic/refcount_manager.h"
#include "generic/protected_ptr.h"

#include "texture.h"
#include "shader.h"
#include "mesh.h"
#include "material.h"
#include "sound.h"

namespace kglt {

class ResourceManagerImpl;

typedef generic::RefCountedTemplatedManager<ResourceManagerImpl, Mesh, MeshID> MeshManager;
typedef generic::RefCountedTemplatedManager<ResourceManagerImpl, ShaderProgram, ShaderID> ShaderManager;
typedef generic::RefCountedTemplatedManager<ResourceManagerImpl, Material, MaterialID> MaterialManager;
typedef generic::RefCountedTemplatedManager<ResourceManagerImpl, Texture, TextureID> TextureManager;
typedef generic::RefCountedTemplatedManager<ResourceManagerImpl, Sound, SoundID> SoundManager;

enum TextureFlag {
    TEXTURE_OPTION_CLAMP_TO_EDGE = 1,
    TEXTURE_OPTION_FLIP_VERTICALLY = 2,
    TEXTURE_OPTION_DISABLE_MIPMAPS = 4,
    TEXTURE_OPTION_NEAREST_FILTER = 8
};

typedef uint32_t TextureFlags;

class ResourceManager {
public:
    virtual ~ResourceManager() {}

    //Mesh functions
    virtual MeshID new_mesh(bool garbage_collect=true) = 0;
    virtual MeshID new_mesh_from_file(const unicode& path, bool garbage_collect=true) = 0;
    virtual MeshID new_mesh_as_cube(float width, bool garbage_collect=true) = 0;
    virtual MeshID new_mesh_as_sphere(float diameter, bool garbage_collect=true) = 0;
    virtual MeshID new_mesh_as_rectangle(float width, float height, const Vec2& offset=Vec2(), bool garbage_collect=true) = 0;

    virtual MeshID new_mesh_with_alias(const unicode& alias, bool garbage_collect=true) = 0;
    virtual MeshID new_mesh_with_alias_from_file(const unicode& alias, const unicode &path, bool garbage_collect=true) = 0;
    virtual MeshID new_mesh_with_alias_as_cube(const unicode& alias, float width, bool garbage_collect=true) = 0;
    virtual MeshID new_mesh_with_alias_as_sphere(const unicode& alias, float diameter, bool garbage_collect=true) = 0;
    virtual MeshID new_mesh_with_alias_as_rectangle(const unicode& alias, float width, float height, const Vec2& offset=Vec2(), bool garbage_collect=true) = 0;
    virtual MeshID get_mesh_with_alias(const unicode& alias) = 0;

    virtual void delete_mesh(MeshID m) = 0;

    //FIXME: More factory methods

    virtual ProtectedPtr<Mesh> mesh(MeshID m) = 0;
    virtual const ProtectedPtr<Mesh> mesh(MeshID m) const = 0;

    virtual bool has_mesh(MeshID m) const = 0;
    virtual uint32_t mesh_count() const = 0;


    //Texture functions
    virtual TextureID new_texture(bool garbage_collect=true) = 0;
    virtual TextureID new_texture_from_file(const unicode& path, TextureFlags flags=0, bool garbage_collect=true) = 0;
    virtual TextureID new_texture_with_alias(const unicode& alias, bool garbage_collect=true) = 0;
    virtual TextureID new_texture_with_alias_from_file(const unicode& alias, const unicode& path, TextureFlags flags=0, bool garbage_collect=true) = 0;
    virtual TextureID get_texture_with_alias(const unicode& alias) = 0;

    virtual ProtectedPtr<Texture> texture(TextureID t) = 0;
    virtual const ProtectedPtr<Texture> texture(TextureID t) const = 0;

    virtual bool has_texture(TextureID m) const = 0;
    virtual uint32_t texture_count() const = 0;
    virtual void mark_texture_as_uncollected(TextureID t) = 0;
    virtual void delete_texture(TextureID t) = 0;

    //Shader functions
    virtual ShaderID new_shader(bool garbage_collect=true) = 0;
    virtual ShaderID new_shader_from_files(const unicode& vert_shader, const unicode& frag_shader, bool garbage_collect=true) = 0;

    virtual ShaderRef shader(ShaderID t) = 0;
    virtual const ShaderRef shader(ShaderID t) const = 0;

    virtual bool has_shader(ShaderID m) const = 0;
    virtual uint32_t shader_count() const = 0;


    //Sound functions
    virtual SoundID new_sound(bool garbage_collect=true) = 0;
    virtual SoundID new_sound_from_file(const unicode& path, bool garbage_collect=true) = 0;

    virtual SoundID new_sound_with_alias(const unicode& alias, bool garbage_collect=true) = 0;
    virtual SoundID new_sound_with_alias_from_file(const unicode& alias, const unicode& path, bool garbage_collect=true) = 0;
    virtual SoundID get_sound_with_alias(const unicode& alias) = 0;

    virtual ProtectedPtr<Sound> sound(SoundID t) = 0;
    virtual const ProtectedPtr<Sound> sound(SoundID t) const = 0;

    virtual bool has_sound(SoundID m) const = 0;
    virtual uint32_t sound_count() const = 0;

    virtual void delete_sound(SoundID t) = 0;

    //Material functions
    virtual MaterialID new_material(bool garbage_collect=true) = 0;
    virtual MaterialID new_material_from_file(const unicode& path, bool garbage_collect=true) = 0;
    virtual MaterialID new_material_with_alias(const unicode& alias, bool garbage_collect=true) = 0;
    virtual MaterialID new_material_with_alias_from_file(const unicode& alias, const unicode& path, bool garbage_collect=true) = 0;
    virtual MaterialID get_material_with_alias(const unicode& alias) = 0;
    virtual MaterialID new_material_from_texture(TextureID texture, bool garbage_collect=true) = 0;

    virtual ProtectedPtr<Material> material(MaterialID t) = 0;
    virtual const ProtectedPtr<Material> material(MaterialID t) const = 0;

    virtual bool has_material(MaterialID m) const = 0;
    virtual uint32_t material_count() const = 0;
    virtual void mark_material_as_uncollected(MaterialID m) = 0;
    virtual void delete_material(MaterialID m) = 0;

    virtual WindowBase& window() = 0;
    virtual const WindowBase& window() const = 0;

    virtual MaterialID clone_default_material() = 0;

    virtual TextureID default_texture_id() const = 0;
    virtual MaterialID default_material_id() const = 0;

    virtual unicode default_material_filename() const = 0;

};

class ResourceManagerImpl:
    public virtual ResourceManager,
    public MeshManager,
    public ShaderManager,
    public MaterialManager,
    public TextureManager,
    public SoundManager {

public:
    ResourceManagerImpl(WindowBase* window);

    MeshID new_mesh(bool garbage_collect=true) override;
    MeshID new_mesh_from_file(const unicode& path, bool garbage_collect=true) override;
    MeshID new_mesh_as_cube(float width, bool garbage_collect=true) override;
    MeshID new_mesh_as_sphere(float diameter, bool garbage_collect=true) override;
    MeshID new_mesh_as_rectangle(float width, float height, const Vec2& offset=Vec2(), bool garbage_collect=true) override;

    MeshID new_mesh_with_alias(const unicode& alias, bool garbage_collect=true) override;
    MeshID new_mesh_with_alias_from_file(const unicode& alias, const unicode &path, bool garbage_collect=true) override;
    MeshID new_mesh_with_alias_as_cube(const unicode& alias, float width, bool garbage_collect=true) override;
    MeshID new_mesh_with_alias_as_sphere(const unicode& alias, float diameter, bool garbage_collect=true) override;
    MeshID new_mesh_with_alias_as_rectangle(const unicode &name, float width, float height, const Vec2& offset=Vec2(), bool garbage_collect=true) override;
    MeshID get_mesh_with_alias(const unicode& alias) override;

    ProtectedPtr<Mesh> mesh(MeshID m);
    const ProtectedPtr<Mesh> mesh(MeshID m) const;

    bool has_mesh(MeshID m) const;
    uint32_t mesh_count() const;
    void delete_mesh(MeshID m) override;


    TextureID new_texture(bool garbage_collect=true) override;
    TextureID new_texture_from_file(const unicode& path, TextureFlags flags=0, bool garbage_collect=true) override;

    TextureID new_texture_with_alias(const unicode& alias, bool garbage_collect=true) override;
    TextureID new_texture_with_alias_from_file(const unicode& alias, const unicode& path, TextureFlags flags=0, bool garbage_collect=true) override;
    TextureID get_texture_with_alias(const unicode& alias) override;

    ProtectedPtr<Texture> texture(TextureID t);
    const ProtectedPtr<Texture> texture(TextureID t) const;
    bool has_texture(TextureID t) const;
    uint32_t texture_count() const;
    void mark_texture_as_uncollected(TextureID t) override;
    void delete_texture(TextureID t) override;

    ShaderID new_shader(bool garbage_collect=true) override;
    ShaderID new_shader_from_files(const unicode& vert_shader, const unicode& frag_shader, bool garbage_collect=true) override;

    ShaderRef shader(ShaderID s);
    const ShaderRef shader(ShaderID s) const;
    uint32_t shader_count() const;
    ShaderProgram* __shader(ShaderID s);    

    bool has_shader(ShaderID s) const;

    MaterialID new_material(bool garbage_collect=true) override;
    MaterialID new_material_from_file(const unicode& path, bool garbage_collect=true) override;
    MaterialID new_material_with_alias(const unicode& alias, bool garbage_collect=true) override;
    MaterialID new_material_with_alias_from_file(const unicode& alias, const unicode& path, bool garbage_collect=true) override;
    MaterialID new_material_from_texture(TextureID texture, bool garbage_collect=true) override;
    MaterialID get_material_with_alias(const unicode& alias) override;    

    MaterialID clone_material(MaterialID mat);

    ProtectedPtr<Material> material(MaterialID material);
    const ProtectedPtr<Material> material(MaterialID material) const;
    bool has_material(MaterialID m) const;
    uint32_t material_count() const;
    void mark_material_as_uncollected(MaterialID t) override;
    void delete_material(MaterialID m) override;

    SoundID new_sound(bool garbage_collect=true);
    SoundID new_sound_from_file(const unicode& path, bool garbage_collect=true);

    SoundID new_sound_with_alias(const unicode& alias, bool garbage_collect=true);
    SoundID new_sound_with_alias_from_file(const unicode& alias, const unicode& path, bool garbage_collect=true);
    SoundID get_sound_with_alias(const unicode& alias);

    void delete_sound(SoundID t);

    ProtectedPtr<Sound> sound(SoundID sound);
    const ProtectedPtr<Sound> sound(SoundID sound) const;
    bool has_sound(SoundID s) const;
    uint32_t sound_count() const;

    std::pair<ShaderID, bool> find_shader(const std::string& name);
    void post_create_shader_callback(ShaderProgram& obj, ShaderID id) {
        shader_lookup_[obj.name()] = id;
    }

    WindowBase& window() { assert(window_); return *window_; }
    const WindowBase& window() const { return *window_; }

    Scene& scene();
    const Scene& scene() const;

    void update();

    unicode default_material_filename() const;

    MaterialID clone_default_material() {
        return new_material_from_file(default_material_filename());
    }

    MaterialID default_material_id() const;
    TextureID default_texture_id() const;

private:
    WindowBase* window_;
    std::map<std::string, ShaderID> shader_lookup_;

    MaterialID default_material_id_;
    TextureID default_texture_id_;
};


}

#endif // RESOURCE_MANAGER_H
