/* *   Copyright (c) 2011-2017 Luke Benstead https://simulant-engine.appspot.com
 *
 *     This file is part of Simulant.
 *
 *     Simulant is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     Simulant is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Simulant.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include "../loader.h"

namespace smlt {
namespace loaders {

class OBJLoader : public Loader {
public:
    OBJLoader(const unicode& filename, std::shared_ptr<std::stringstream> data):
        Loader(filename, data) {}

    void into(Loadable& resource, const LoaderOptions& options = LoaderOptions());
};

void parse_face(const unicode& input, int32_t& vertex_index, int32_t& tex_index, int32_t& normal_index);

class OBJLoaderType : public LoaderType {
public:
    OBJLoaderType() {

    }

    ~OBJLoaderType() {}

    unicode name() { return "obj"; }
    bool supports(const unicode& filename) const override {
        return filename.lower().contains(".obj");
    }

    Loader::ptr loader_for(const unicode& filename, std::shared_ptr<std::stringstream> data) const {
        return Loader::ptr(new OBJLoader(filename, data));
    }
};

}
}

#endif // OBJ_LOADER_H
