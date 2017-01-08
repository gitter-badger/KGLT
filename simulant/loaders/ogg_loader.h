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

#ifndef OGG_LOADER_H
#define OGG_LOADER_H

#include <map>
#include <vector>
#include "../types.h"
#include "../loader.h"

namespace smlt {
namespace loaders {

class OGGLoader : public Loader {
public:
    OGGLoader(const unicode& filename, std::shared_ptr<std::stringstream> data):
        Loader(filename, data) {}

    void into(Loadable& resource, const LoaderOptions &options=LoaderOptions());

};

class OGGLoaderType : public LoaderType {
public:
    unicode name() { return "ogg"; }
    bool supports(const unicode& filename) const {
        //FIXME: check magic
        return filename.lower().contains(".ogg");
    }

    Loader::ptr loader_for(const unicode& filename, std::shared_ptr<std::stringstream> data) const {
        return Loader::ptr(new OGGLoader(filename, data));
    }
};


}
}

#endif // OGG_LOADER_H
