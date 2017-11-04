
#include <simulant/simulant.h>

class MainScene:
    public smlt::Scene<MainScene> {

public:
    MainScene(smlt::Window* window):
        smlt::Scene<MainScene>(window) {}

    void load() {
        stage_ = window->new_stage();
        camera_ = stage_.fetch()->new_camera_with_orthographic_projection(0, window->width(), 0, window->height());

        window->render(stage_, camera_).with_clear(smlt::BUFFER_CLEAR_ALL, smlt::Colour::BLACK);

        smlt::StagePtr stage = stage_.fetch();

        //stage->ui->transform_input_with_camera(camera_);

        auto title = stage->ui->new_widget_as_label("UI Sample demonstrating widgets").fetch();
        title->move_to(window->coordinate_from_normalized(0.5, 0.9));

        auto button = stage->ui->new_widget_as_button("Button 1").fetch();
        button->move_to(window->coordinate_from_normalized(0.1, 0.25));

        auto pg = stage->ui->new_widget_as_progress_bar().fetch_as<smlt::ui::ProgressBar>();
        pg->move_to(window->coordinate_from_normalized(0.5, 0.5));
        pg->resize(400, 10);
        pg->pulse();

        button->signal_clicked().connect([&]() {
            title->set_text("Clicked!");
        });
    }

private:
    smlt::StageID stage_;
    smlt::CameraID camera_;
};

class App : public smlt::Application {
public:
    App(const smlt::AppConfig& config):
        smlt::Application(config) {

    }

    bool init() {
        scenes->register_scene<MainScene>("main");
        return true;
    }
};

int main(int argc, char* argv[]) {
    smlt::AppConfig config;
    config.title = "UI Demo";

    App app(config);
    return app.run();
}