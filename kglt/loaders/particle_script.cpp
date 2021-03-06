#include <kazbase/json/json.h>
#include <kazbase/file_utils.h>
#include "particle_script.h"
#include "../particles.h"

namespace kglt {
namespace loaders {

void KGLPLoader::into(Loadable &resource, const LoaderOptions &options) {
    ParticleSystem* ps = loadable_to<ParticleSystem>(resource);
    json::JSON js = json::loads(data_->str());

    ps->set_name((js.has_key("name")) ? _u(js.dict_value("name")): "");

    if(js.has_key("quota")) ps->set_quota(js.dict_value("quota").get_number());
    if(js.has_key("particle_width")) ps->set_particle_width(js.dict_value("particle_width").get_number());    
    if(js.has_key("cull_each")) ps->set_cull_each(js.dict_value("cull_each").get_bool());

    if(js.has_key("emitters")) {
        json::Node& emitters = js.dict_value("emitters");
        for(int i = 0; i < emitters.length(); ++i) {
            json::Node& emitter = emitters.array_value(i);

            auto new_emitter = ps->push_emitter();

            if(emitter.has_key("type")) {
                new_emitter->set_type((emitter["type"].get() == _u("point")) ? PARTICLE_EMITTER_POINT : PARTICLE_EMITTER_BOX);
            }

            if(emitter.has_key("direction")) {
                auto parts = emitter["direction"].get().split(" ");
                //FIXME: check length
                new_emitter->set_direction(kglt::Vec3(
                    parts.at(0).to_float(),
                    parts.at(1).to_float(),
                    parts.at(2).to_float()
                ));
            }

            if(emitter.has_key("velocity")) {
                new_emitter->set_velocity(emitter["velocity"]);
            }

            if(emitter.has_key("width")) {
                new_emitter->set_width(emitter["width"]);
            }

            if(emitter.has_key("height")) {
                new_emitter->set_height(emitter["height"]);
            }

            if(emitter.has_key("depth")) {
                new_emitter->set_depth(emitter["depth"]);
            }

            if(emitter.has_key("ttl")) {
                new_emitter->set_ttl(emitter["ttl"]);
            } else {
                if(emitter.has_key("ttl_min")) {
                    new_emitter->set_ttl_range(emitter["ttl_min"], new_emitter->ttl_range().second);
                }

                if(emitter.has_key("ttl_max")) {
                    new_emitter->set_ttl_range(new_emitter->ttl_range().first, emitter["ttl_max"]);
                }
            }

            if(emitter.has_key("duration")) {
                new_emitter->set_duration(emitter["duration"]);
            }

            if(emitter.has_key("repeat_delay")) {
                new_emitter->set_repeat_delay(emitter["repeat_delay"]);
            }

            if(emitter.has_key("angle")) {
                new_emitter->set_angle(Degrees(emitter["angle"]));
            }

            if(emitter.has_key("colour")) {
                auto parts = unicode(emitter["colour"]).split(" ");
                new_emitter->set_colour(kglt::Colour(
                    parts.at(0).to_float(), parts.at(1).to_float(), parts.at(2).to_float(), parts.at(3).to_float()
                ));
            }

            if(emitter.has_key("emission_rate")) {
                new_emitter->set_emission_rate(emitter["emission_rate"]);
            }
        }
    }
}

}
}

