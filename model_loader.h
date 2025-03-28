#pragma once
#include <string>
#include <fstream>
#include <sstream>

#include <vector>
#include <unordered_map>

#include <DirectXMath.h>

#include "vertex.h"

namespace ml {
    bool loadObj(std::string filename, std::vector<Vertex>& vertexes, std::vector<unsigned int>& indexes);
}
