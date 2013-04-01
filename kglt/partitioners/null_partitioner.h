#ifndef NULL_PARTITIONER_H
#define NULL_PARTITIONER_H

#include "../partitioner.h"

namespace kglt {

class SubEntity;

class NullPartitioner : public Partitioner {
public:
    NullPartitioner(SubScene& ss):
        Partitioner(ss) {}

    void add_entity(EntityID obj) {
        all_entities_.insert(obj);
    }

    void remove_entity(EntityID obj) {
        all_entities_.erase(obj);
    }

    void add_light(LightID obj) {
        all_lights_.insert(obj);
    }

    void remove_light(LightID obj) {
        all_lights_.erase(obj);
    }

    std::vector<LightID> lights_within_range(const kmVec3& location);
    std::vector<std::tr1::shared_ptr<SubEntity>> geometry_visible_from(CameraID camera_id);

private:
    std::set<EntityID> all_entities_;
    std::set<LightID> all_lights_;
};

}

#endif // NULL_PARTITIONER_H
