#ifndef TEST_OBJECT_H
#define TEST_OBJECT_H

#include "kglt/kglt.h"
#include <kaztest/kaztest.h>

#include "global.h"

class ObjectTest : public KGLTTestCase {
public:
    void set_up() {
        KGLTTestCase::set_up();
        camera_id_ = window->new_camera();
        stage_id_ = window->new_stage();
    }

    void tear_down() {
        KGLTTestCase::tear_down();
        window->delete_camera(camera_id_);
        window->delete_stage(stage_id_);
    }

    void test_set_absolute_rotation() {
        kglt::ActorID act = window->stage(stage_id_)->new_actor();
        auto actor = window->stage(stage_id_)->actor(act);

        actor->set_absolute_rotation(kglt::Degrees(10), 0, 0, 1);

        assert_equal(actor->relative_rotation(), actor->absolute_rotation());

        kglt::ActorID act2 = window->stage(stage_id_)->new_actor();
        auto actor2 = window->stage(stage_id_)->actor(act2);

        actor2->set_parent(act);

        assert_equal(actor2->absolute_rotation(), actor->absolute_rotation());

        actor2->set_absolute_rotation(kglt::Degrees(20), 0, 0, 1);

        kglt::Quaternion expected_rel, expected_abs;
        kmQuaternionRotationAxisAngle(&expected_abs, &KM_VEC3_POS_Z, kmDegreesToRadians(20));
        kmQuaternionRotationAxisAngle(&expected_rel, &KM_VEC3_POS_Z, kmDegreesToRadians(10));

        assert_equal(expected_abs, actor2->absolute_rotation());
        assert_equal(expected_rel, actor2->relative_rotation());
    }

    void test_set_absolute_position() {
        kglt::ActorID act = window->stage(stage_id_)->new_actor();
        auto actor = window->stage(stage_id_)->actor(act);

        actor->set_absolute_position(10, 10, 10);

        assert_equal(kglt::Vec3(10, 10, 10), actor->absolute_position());

        kglt::ActorID act2 = window->stage(stage_id_)->new_actor();
        auto actor2 = window->stage(stage_id_)->actor(act2);

        actor2->set_parent(act);

        //Should be the same as its parent
        assert_equal(actor2->absolute_position(), actor->absolute_position());

        //Make sure relative position is correctly calculated
        actor2->set_absolute_position(20, 10, 10);
        assert_equal(kglt::Vec3(10, 0, 0), actor2->relative_position());
        assert_equal(kglt::Vec3(20, 10, 10), actor2->absolute_position());

        //Make sure setting by vec3 works
        actor2->set_absolute_position(kglt::Vec3(0, 0, 0));
        assert_equal(kglt::Vec3(), actor2->absolute_position());
    }

    void test_set_relative_position() {
        kglt::ActorID act = window->stage(stage_id_)->new_actor();
        auto actor = window->stage(stage_id_)->actor(act);

        actor->set_relative_position(10, 10, 10);

        //No parent, both should be the same
        assert_equal(kglt::Vec3(10, 10, 10), actor->relative_position());
        assert_equal(kglt::Vec3(10, 10, 10), actor->absolute_position());

        kglt::ActorID act2 = window->stage(stage_id_)->new_actor();
        auto actor2 = window->stage(stage_id_)->actor(act2);

        actor2->set_parent(act);

        actor2->set_relative_position(kglt::Vec3(10, 0, 0));

        assert_equal(kglt::Vec3(20, 10, 10), actor2->absolute_position());
        assert_equal(kglt::Vec3(10, 0, 0), actor2->relative_position());
    }

private:
    CameraID camera_id_;
    StageID stage_id_;
};

#endif // TEST_OBJECT_H
