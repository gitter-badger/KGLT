#include <kglt/kglt.h>
#include <iostream>

using namespace kglt;


enum XBoxAxis { 
    // Left Thumbstick
    AxisX, // LXAxis,
    AxisY, // LYAxis,
    // Right Thumbstick
    Axis4, // RXAxis,
    Axis5, // RYAxis,
    // Triggers
    RightTrigger, // Axis3
    LeftTrigger // Axis3
};

enum XBoxButtons { 
    A, // Face Buttons
    B,
    X,
    Y,
    LB, // Left Bumper
    RB, // Right Bumper
    Back,
    Start,
    Home, // Guide - The big X
    LeftStick,
    RightStick
};

// enum XBoxHat {
//     Centered = 0,
//     Up = 1,
//     Right = 2,
//     Down = 4,
//     Left = 8,
//     RightUp = Right + Up,
//     RightDown = Right + Down,
//     LeftUp = Left + Up,
//     LeftDown = Left + Down
// };

kglt::ActorID actor_id;
kmVec3 pos = { 0.f, 0.f, -5.f };
kmVec2 rot = { 1.f, 0.5f };
const float sensibility = 50.f;

void joypad_button(kglt::Button button) {
}

void joypad_axis_left(kglt::AxisRange axis_range, kglt::Axis axis) {
    if (axis % 2)
        pos.y += -axis_range / sensibility;
    else
        pos.x += axis_range / sensibility;

    // g_actor->move_to(pos);
}

void joypad_axis_right(kglt::AxisRange axis_range, kglt::Axis axis) {
    if (axis % 2)
        rot.y += -axis_range / sensibility;
    else
        rot.x += axis_range / sensibility;

    // g_actor->rotate_x(rot.y);
    // g_actor->rotate_y(rot.x);
}

class GameScreen : public kglt::Screen<GameScreen> {
public:
    GameScreen(WindowBase& window):
        kglt::Screen<GameScreen>(window) {}

    void do_load() {
        prepare_basic_scene_with_overlay(stage_id_, camera_id_, overlay_id_, overlay_camera_id_);

        window->camera(camera_id_)->set_perspective_projection(
            45.0,
            float(window->width()) / float(window->height()),
            1.0,
            1000.0
        );

        // Thanks to other samples
        auto ui = window->ui_stage(overlay_id_);
        ui->set_styles("body { font-family: \"Ubuntu\"; } .thing { font-size: 14; padding-left: 10;};");
        ui->append("<p>").text("Left x-y axis move the cube.");
        ui->append("<p>").text("Right x-y axis rotate the cube.");
        ui->append("<p>").text("Button 0 (A) reset the cube.");
        ui->$("p").add_class("thing");

        ///Shortcut function for loading images
        kglt::TextureID tid = window->stage(stage_id_)->new_texture_from_file("sample_data/sample.tga");
        kglt::MaterialID matid = window->stage(stage_id_)->new_material_from_texture(tid);

        window->stage(stage_id_)->set_ambient_light(kglt::Colour::WHITE);
        {
            auto light = window->stage(stage_id_)->light(window->stage(stage_id_)->new_light());
            light->set_absolute_position(5.0, 0.0, -5.0);
            light->set_diffuse(kglt::Colour::GREEN);
            light->set_attenuation_from_range(10.0);
        }

        actor_id = window->stage(stage_id_)->geom_factory().new_cube(2);

        window->stage(stage_id_)->actor(actor_id)->mesh()->set_material_id(matid);
        window->stage(stage_id_)->actor(actor_id)->set_absolute_position(pos);

        // It would be nice to check if a joypad is connected
        // and the create the reference..
        //

        window->enable_virtual_joypad(kglt::VIRTUAL_DPAD_DIRECTIONS_TWO, 2);

        for(int i = 0; i < window->joypad_count(); ++i) {
            kglt::Joypad& joypad = window->joypad(i);

            // Currently A button on XBOX Controller
            joypad.button_pressed_connect(XBoxButtons::RightStick, [=](kglt::Button button) mutable {
                    rot.x = -rot.x;
                    rot.y = -rot.y;
            });
            joypad.button_pressed_connect(1, [&](kglt::Button button) mutable {
                    /* Reset positions and rotations */
                    pos = { 0, 0, -5.f };
                    // rot = { 0, 0 };

                    window->stage(stage_id_)->actor(actor_id)->set_absolute_rotation(kglt::Degrees(0), 0, 0, pos.z);
                    window->stage(stage_id_)->actor(actor_id)->set_absolute_position(pos);
            });

            // Left x-axis
            joypad.axis_while_nonzero_connect(0, joypad_axis_left);
            // Left y-axis
            joypad.axis_while_nonzero_connect(1, joypad_axis_left);
            // Right x-axis
            joypad.axis_while_nonzero_connect(2, joypad_axis_right);
            // Right y-axis
            joypad.axis_while_nonzero_connect(4, [=](kglt::AxisRange range, kglt::Axis) mutable {
                    if (range > 0)
                        std::cout << (float) range << std::endl;
            });


            auto hat_cb = [=](kglt::HatPosition position, kglt::Hat hat, double dt) mutable {
                std::cout << "Hat: " << (int) hat << std::endl;
                std::cout << "Position " << (int) position << std::endl;
            };

            // Triggers should work too
            // NOTE: horizontal axis have an even number..
            // Hat experimental
            joypad.hat_changed_connect(0, std::bind(hat_cb, std::placeholders::_1, std::placeholders::_2, 0));
            joypad.hat_while_not_centered_connect(0, hat_cb);
        }
    }

    void do_step(double dt) {
        auto actor = window->stage(stage_id_)->actor(actor_id);
        actor->rotate_x(kglt::Degrees(rot.y * dt * 10));
        actor->rotate_y(kglt::Degrees(rot.x * dt * 10));
    }

private:
    CameraID camera_id_, overlay_camera_id_;
    StageID stage_id_;
    UIStageID overlay_id_;
};

class JoypadSample: public kglt::Application {
public:
    JoypadSample():
        Application("KGLT Combined Sample") {
    }

private:
    bool do_init() {
        register_screen("/", screen_factory<GameScreen>());
        return true;
    }
};

int main(int argc, char* argv[]) {
    JoypadSample app;
    return app.run();
}

