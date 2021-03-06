#include <fstream>
#include <stdexcept>

#include "wavefront_loader.h"

namespace kglt {

void WavefrontLoader::load_into(Loadable& resource, const std::string& filename) {

    std::ifstream file(filename.c_str());
    if(!file.good()) {
        throw std::runtime_error("Couldn't load the Wavefront object from: " + filename);
    }

    std::string line;

    while(std::getline(file, line)) {
        //Do stuff
    }
}

}
