#ifndef MANAGERS_H
#define MANAGERS_H

#include "generic/generic_tree.h"
#include "generic/manager.h"
#include "types.h"
#include "interfaces.h"

namespace kglt {


class BackgroundManager:
    public generic::TemplatedManager<WindowBase, Background, BackgroundID>,
    public virtual Updateable {

public:
    BackgroundManager(WindowBase* window);

    BackgroundID new_background();
    BackgroundID new_background_from_file(const unicode& filename, float scroll_x=0.0, float scroll_y=0.0);
    BackgroundPtr background(BackgroundID bid);
    bool has_background(BackgroundID bid) const;
    void delete_background(BackgroundID bid);
    uint32_t background_count() const;

    void update(double dt) override;

private:
    WindowBase* window_;
};

class CameraManager:
    public generic::TemplatedManager<WindowBase, Camera, CameraID> {

public:
    CameraManager(WindowBase* window);

    CameraID new_camera();
    CameraPtr camera();
    CameraPtr camera(CameraID c);
    void delete_camera(CameraID cid);
    uint32_t camera_count() const;

    CameraID default_camera_id() const { return default_camera_id_; }

private:
    WindowBase* window_;

    CameraID default_camera_id_;
};

class StageManager:
    public generic::TemplatedManager<WindowBase, Stage, StageID>,
    public virtual Updateable {

public:
    StageManager(WindowBase* window);

    StageID new_stage(AvailablePartitioner partitioner=PARTITIONER_OCTREE);
    StagePtr stage();
    StagePtr stage(StageID s);
    void delete_stage(StageID s);
    uint32_t stage_count() const;

    void print_tree();

    StageID default_stage_id() const { return default_stage_id_; }

    void update(double dt) override;

private:
    void print_tree(GenericTreeNode* node, uint32_t& level);

    WindowBase* window_;

    StageID default_stage_id_;
};

class UIStageManager:
    public generic::TemplatedManager<WindowBase, UIStage, UIStageID> {

public:
    UIStageManager(WindowBase* window);

    UIStageID new_ui_stage();
    UIStagePtr ui_stage();
    UIStagePtr ui_stage(UIStageID s);
    void delete_ui_stage(UIStageID s);
    uint32_t ui_stage_count() const;

    UIStageID default_ui_stage_id() const { return default_ui_stage_id_; }

protected:
    void create_default_ui_stage();

private:
    WindowBase* window_;
    UIStageID default_ui_stage_id_;
};

}

#endif // MANAGERS_H