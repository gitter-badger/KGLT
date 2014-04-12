#ifndef TEST_MATERIAL_H
#define TEST_MATERIAL_H

#include <kaztest/kaztest.h>

#include "kglt/kglt.h"
#include "global.h"

class MaterialTest : public TestCase {
public:
    void set_up() {
        if(!window) {
            window = kglt::Window::create();
            window->set_logging_level(kglt::LOG_LEVEL_NONE);
        }

        //window->reset();
    }

    void test_material_initialization() {
        auto mat = window->material(window->new_material());

        this->assert_equal((uint32_t)1, mat->technique_count()); //Should return the default technique
        this->assert_equal(kglt::DEFAULT_MATERIAL_SCHEME, mat->technique().scheme());
        mat->technique().new_pass(kglt::ShaderID()); //Create a pass
        this->assert_equal((uint32_t)1, mat->technique().pass_count()); //Should return the default pass
        this->assert_true(kglt::Colour::WHITE == mat->technique().pass(0).diffuse()); //this->assert_true the default pass sets white as the default
        this->assert_true(kglt::Colour::WHITE == mat->technique().pass(0).ambient()); //this->assert_true the default pass sets white as the default
        this->assert_true(kglt::Colour::WHITE == mat->technique().pass(0).specular()); //this->assert_true the default pass sets white as the default
        this->assert_equal(0.0, mat->technique().pass(0).shininess());
    }

    void test_material_applies_to_mesh() {
        kglt::MaterialID mid = window->new_material();
        kglt::MeshID mesh_id = window->new_mesh();
        auto mesh = window->mesh(mesh_id);
        kglt::SubMeshIndex idx = mesh->new_submesh(mid);
        this->assert_equal(mid, mesh->submesh(idx).material_id());
    }

    void test_reflectiveness() {
        kglt::MaterialID mid = window->new_material();
        auto mat = window->material(mid);
        uint32_t pass_id = mat->technique().new_pass(kglt::ShaderID());
        kglt::MaterialPass& pass = mat->technique().pass(pass_id);

        assert_false(pass.is_reflective());
        assert_false(mat->technique().has_reflective_pass());
        assert_equal(0.0, pass.albedo());
        assert_equal(0, pass.reflection_texture_unit());

        pass.set_albedo(0.5);

        assert_equal(0.5, pass.albedo());
        assert_true(pass.is_reflective());
        assert_true(mat->technique().has_reflective_pass());
    }
};

#endif // TEST_MATERIAL_H
