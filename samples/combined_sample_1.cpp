#include "kglt/kglt.h"

using namespace kglt;

class GameScreen : public kglt::Screen<GameScreen> {
public:
    GameScreen(WindowBase& window):
        kglt::Screen<GameScreen>(window) {}

    void do_load() {
        prepare_basic_scene(stage_id_, camera_id_);

        //and the passed height. For example, passing a height of 2.0 would mean the view would extend
        //+1 and -1 in the vertical direction, -1.0 - +1.0 near/far, and width would be calculated from the aspect
        //window.scene().pass().viewport().configure(kglt::VIEWPORT_TYPE_BLACKBAR_16_BY_9);
        window->camera(camera_id_)->set_orthographic_projection_from_height((float) 224 / (float) 40, 16.0 / 9.0);

        {
            auto stage = window->stage(stage_id_);

            kglt::SpriteID sprite = stage->new_sprite_from_file("sample_data/sonic.png", 64, 64);
            stage->sprite(sprite)->add_animation("running", 31, 34, 0.5);
            stage->sprite(sprite)->set_render_dimensions_from_height(1.5);
            stage->sprite(sprite)->move_to(0, -2.0, -1.0);
        }

        window->new_background_from_file("sample_data/parallax/back_layer.png", 0.1);
        window->new_background_from_file("sample_data/parallax/middle_layer.png", 0.2);
        window->new_background_from_file("sample_data/parallax/front_layer.png", 1.0);
    }

private:
    CameraID camera_id_;
    StageID stage_id_;
};

class CombinedSample: public kglt::Application {
public:
    CombinedSample():
        Application("KGLT Combined Sample") {
    }

private:
    bool do_init() {
        register_screen("/", screen_factory<GameScreen>());
        return true;
    }
};

int main(int argc, char* argv[]) {
    CombinedSample app;
    return app.run();
}

