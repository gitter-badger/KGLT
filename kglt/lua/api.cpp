#include "interpreter.h"

#include "../kglt.h"

namespace kglt {

void export_lua_api(lua_State* state) {
    luabind::module(state) [
        luabind::class_<StageID>("StageID")
            .def(luabind::constructor<int>())
            .property("value", &StageID::value)
    ];

    luabind::module(state) [
        luabind::class_<MaterialID>("MaterialID")
            .def(luabind::constructor<int>())
            .property("value", &MaterialID::value)
    ];

    luabind::module(state) [
        luabind::class_<ActorID>("ActorID")
            .def(luabind::constructor<int>())
            .property("value", &ActorID::value)
    ];

    luabind::module(state) [
        luabind::class_<SpriteID>("SpriteID")
            .def(luabind::constructor<int>())
            .property("value", &SpriteID::value)
    ];

    luabind::module(state) [
        luabind::class_<MeshID>("MeshID")
            .def(luabind::constructor<int>())
            .property("value", &MeshID::value)
    ];

    luabind::module(state) [
        luabind::class_<kmVec3>("kmVec3")
            .property("x", &Vec3::x)
            .property("y", &Vec3::y)
            .property("z", &Vec3::z),

        luabind::class_<Vec3, kmVec3>("Vec3")
            .def(luabind::constructor<float, float, float>()),

        luabind::class_<kmQuaternion>("kmQuaternion")
            .property("x", &kmQuaternion::x)
            .property("y", &kmQuaternion::y)
            .property("z", &kmQuaternion::z)
            .property("w", &kmQuaternion::w)
    ];

    luabind::module(state) [
        luabind::class_<ResourceManager>("ResourceManager")
//            .def("mesh", (Mesh&(ResourceManager::*)(MeshID))&ResourceManager::mesh)
            .property("mesh_count", &ResourceManager::mesh_count)
    ];

    luabind::module(state) [
        luabind::class_<WindowBase, luabind::bases<ResourceManager> >("WindowBase")
            .def("set_title", &WindowBase::set_title)
            .def("quit", &WindowBase::stop_running)
            .def("new_stage", &WindowBase::new_stage)
            .def("stage", (StagePtr(WindowBase::*)(StageID))&WindowBase::stage)
            .def("delete_stage", &WindowBase::delete_stage)
            .def("show_stats", &WindowBase::show_stats)
            .def("hide_stats", &WindowBase::hide_stats)
            .property("stage_count", &WindowBase::stage_count)
            .property("width", &WindowBase::width)
            .property("height", &WindowBase::height)
            .property("total_time", &WindowBase::total_time)
            .property("delta_time", &WindowBase::delta_time)
            .def("print_tree", (void(WindowBase::*)(void))&WindowBase::print_tree)
    ];

    luabind::module(state) [
        luabind::class_<Stage, luabind::bases<ResourceManager> >("Stage")
            .def("new_actor", (ActorID(Stage::*)())&Stage::new_actor)
            .def("new_actor_from_mesh", (ActorID(Stage::*)(MeshID))&Stage::new_actor)
            .def("actor", (ProtectedPtr<Actor>(Stage::*)(ActorID))&Stage::actor)
            .def("delete_actor", &Stage::delete_actor)
            .def("sprite", (ProtectedPtr<Sprite>(Stage::*)(SpriteID))&Stage::sprite)
            .def("delete_sprite", &Stage::delete_sprite)
            .def("tostring", &Stage::__unicode__)
            .property("window", (WindowBase&(Stage::*)())&Stage::window)
            .property("actor_count", &Stage::actor_count)
            .property("sprite_count", &Stage::sprite_count)
            .property("light_count", &Stage::light_count)
            .property("id", &Stage::id)            
    ];

    /*
        kmMat4 absolute_transformation();
    */

    luabind::module(state) [
        luabind::class_<Object>("Object")
            .def("move_to", (void(Object::*)(float, float, float))&Object::set_absolute_position)
//            .def("move_forward", &Object::move_forward)
            .def("rotate_x", &Object::rotate_x)
            .def("rotate_y", &Object::rotate_y)
            .def("rotate_z", &Object::rotate_z)
            .def("rotate_to", (void(Object::*)(const Degrees&, float, float, float))&Object::set_absolute_rotation)
            .def("lock_rotation", &Object::lock_rotation)
            .def("unlock_rotation", &Object::unlock_rotation)
            .def("lock_position", &Object::lock_position)
            .def("unlock_position", &Object::unlock_position)
            .property("uuid", &Object::uuid)
            .property("relative_position", &Object::relative_position)
            .property("absolute_position", &Object::absolute_position)
            .property("relative_rotation", &Object::relative_rotation)
            .property("absolute_rotation", &Object::absolute_rotation)
    ];

    luabind::module(state) [
        luabind::class_<Actor, Object>("Actor")
            .property("id", &Actor::id)
    ];

    luabind::module(state) [
        luabind::class_<Sprite, Object>("Sprite")
            .property("id", &Sprite::id)
    ];

    luabind::module(state)[
        luabind::class_<Mesh>("Mesh")
            .property("id", &Mesh::id)
    ];

    luabind::module(state) [
        luabind::class_<GeomFactory>("GeomFactory")
            .def("new_line", &GeomFactory::new_line)
    ];
}

}
