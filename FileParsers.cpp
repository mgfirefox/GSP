#include "FileParsers.h"

std::string getFileFormat(std::string filename) {
    size_t index = filename.find_last_of('.');
    if (index >= filename.size() - 1) {
        return "";
    }

    return filename.substr(index + 1);
}

bool ImageFileParser::parseFile(std::string filename, ImageData& imageData) {
    std::string format = getFileFormat(filename);
    if (format == "dds") {
        return parseDdsFile(filename, imageData);
    }

    return false;
}

bool ImageFileParser::parseDdsFile(std::string filename, ImageData& imageData) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    // MagicNumber
    unsigned int magicNumber = 0;
    file.read((char*)&magicNumber, DDS_MAGIC_NUMBER_SIZE);
    if (!file || file.gcount() != DDS_MAGIC_NUMBER_SIZE) {
        return false;
    }
    if (magicNumber != DDS_MAGIC_NUMBER_DDS.number) {
        return false;
    }

    // HeaderData
    DDS_HEADER header = {};
    file.read((char*)&header, sizeof(DDS_HEADER));
    if (!file || file.gcount() != sizeof(DDS_HEADER)) {
        return false;
    }
    if (header.dwSize != sizeof(DDS_HEADER)) {
        return false;
    }
    if (!(header.dwFlags & (DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT))) {
        return false;
    }

    if (header.dwWidth == 0 || header.dwHeight == 0) {
        return false;
    }
    imageData.width = header.dwWidth;
    imageData.height = header.dwHeight;

    if (header.dwFlags & DDSD_MIPMAPCOUNT) {
        imageData.mipmapLevels = header.dwMipMapCount;
    }
    else {
        imageData.mipmapLevels = 1;        
    }
    imageData.mipmapDataItems.reserve(imageData.mipmapLevels);

    if (header.dwFlags & DDSD_DEPTH) {
        if (header.dwWidth == 0) {
            return false;
        }
        imageData.depth = header.dwDepth;
    }
    else {
        imageData.depth = 1;
    }
    
    // PixelFormatData
    DDS_PIXELFORMAT& pixelFormat = header.ddspf;
    if (pixelFormat.dwSize != sizeof(DDS_PIXELFORMAT)) {
        return false;
    }
    if (pixelFormat.dwFlags & (DDPF_RGB | DDPF_ALPHA)) {
        if (pixelFormat.dwRGBBitCount == 32) {
            if (pixelFormat.dwRBitMask & DDS_32BIT_MASK_FIRST_8BIT.number && pixelFormat.dwGBitMask & DDS_32BIT_MASK_SECOND_8BIT.number
                && pixelFormat.dwBBitMask & DDS_32BIT_MASK_THIRD_8BIT.number && pixelFormat.dwABitMask & DDS_32BIT_MASK_FOURTH_8BIT.number) {
                imageData.format = DXGI_FORMAT_R8G8B8A8_UNORM;

                for (unsigned int j = 0, k = 0; j < imageData.mipmapLevels; j++) {
                    unsigned int width;
                    unsigned int height;
                    if (j == 0) {
                        width = imageData.width;
                        height = imageData.height;

                        k = 2;
                    }
                    else {
                        width = imageData.width / k;
                        if (width == 0) {
                            width = 1;
                        }
                        height = imageData.height / k;
                        if (height == 0) {
                            height = 1;
                        }

                        k *= 2;
                    }

                    MipmapData mipmapData = {};
                    mipmapData.rowPitch = (width * pixelFormat.dwRGBBitCount + 7) / 8;
                    mipmapData.depthPitch = mipmapData.rowPitch * height;

                    mipmapData.data = std::vector<unsigned char>(mipmapData.depthPitch);
                    for (unsigned int i = 0; i < height; i++) {
                        file.read((char*)(mipmapData.data.data() + i * mipmapData.rowPitch), mipmapData.rowPitch);
                        if (!file || file.gcount() != mipmapData.rowPitch) {
                            return false;
                        }
                    }
                    imageData.mipmapDataItems.push_back(mipmapData);
                }
            }
            else {
                return false;
            }
        }
        else {
            return false;
        }
    }
    else if (pixelFormat.dwFlags & DDPF_FOURCC) {
        if (header.dwPitchOrLinearSize == 0) {
            return false;
        }

        unsigned int magicNumber1 = pixelFormat.dwFourCC;
        if (magicNumber1 == DDS_MAGIC_NUMBER_DXT5.number) {
            imageData.format = DXGI_FORMAT_BC3_UNORM;

            unsigned int blockHeight;
            for (unsigned int j = 0, k = 0; j < imageData.mipmapLevels; j++) {
                unsigned int width;
                unsigned int height;
                if (j == 0) {
                    width = imageData.width;

                    k = 2;
                }
                else {
                    width = imageData.width / k;
                    if (width == 0) {
                        width = 1;
                    }
                    height = blockHeight / k;
                    if (height == 0) {
                        height = 1;
                    }

                    k *= 2;
                }

                MipmapData mipmapData = {};
                mipmapData.rowPitch = (width + 3) / 4 * DDS_BC2_BLOCK_SIZE;
                if (j == 0) {
                    mipmapData.depthPitch = header.dwPitchOrLinearSize;

                    blockHeight = mipmapData.depthPitch / mipmapData.rowPitch;
                    height = blockHeight;
                }
                else {
                    mipmapData.depthPitch = mipmapData.rowPitch * height;
                }

                mipmapData.data = std::vector<unsigned char>(mipmapData.depthPitch);
                for (unsigned int i = 0; i < height; i++) {
                    file.read((char*)(mipmapData.data.data() + i * mipmapData.rowPitch), mipmapData.rowPitch);
                    if (!file || file.gcount() != mipmapData.rowPitch) {
                        return false;
                    }
                }
                imageData.mipmapDataItems.push_back(mipmapData);
            }
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
    file.close();

    return true;
}

bool ModelFileParser::parseFile(std::string filename, ModelData& modelData) {
    std::string format = getFileFormat(filename);
    if (format == "obj") {
        return parseObjFile(filename, modelData);
    }

    return false;
}

bool ModelFileParser::parseObjFile(std::string filename, ModelData& modelData) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);

        std::string keyword;
        lineStream >> keyword;

        // MaterialData
        if (keyword == "mtllib") {
            lineStream >> std::ws;
            std::string mtlFilename;
            std::getline(lineStream, mtlFilename);

            bool result = parseMtlFile(mtlFilename, modelData);
            if (!result) {
                return false;
            }

            break;
        }
    }

    MeshData meshData = {};

    std::vector<DirectX::XMFLOAT3> positions;
    std::vector<DirectX::XMFLOAT3> textureCoordinatesItems;
    std::vector<DirectX::XMFLOAT3> normals;

    std::unordered_map<Vertex, unsigned int> uniqueVertexes;

    while (std::getline(file, line)) {
        std::istringstream lineStream(line);

        std::string keyword;
        lineStream >> keyword;

        // VertexData
        if (keyword == "v") {
            DirectX::XMFLOAT3 position = {};
            lineStream >> position.x >> position.y >> position.z;
            positions.push_back(position);
        }
        else if (keyword == "vn") {
            DirectX::XMFLOAT3 normal = {};
            lineStream >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (keyword == "vt") {
            DirectX::XMFLOAT3 textureCoordinates = {};
            lineStream >> textureCoordinates.x >> textureCoordinates.y >> textureCoordinates.z;
            textureCoordinatesItems.push_back(textureCoordinates);
        }
        // MeshData
        else if (keyword == "o") {
            if (!(meshData.indexes.empty() && meshData.materialName.empty())) {
                modelData.meshDataItems.push_back(meshData);

                meshData = {};
            }

            lineStream >> std::ws;
            std::getline(lineStream, meshData.name);
        }
        else if (keyword == "g") {
        }
        else if (keyword == "usemtl") {
            if (!meshData.materialName.empty()) {
                return false;
            }

            lineStream >> meshData.materialName;
        }
        else if (keyword == "s") {
        }
        else if (keyword == "f") {
            for (int i = 0; i < 3; i++) {
                std::string vertexDataIndexes;
                lineStream >> vertexDataIndexes;

                std::istringstream vertexDataIndexesStream(vertexDataIndexes);

                std::string positionIndex;
                std::getline(vertexDataIndexesStream, positionIndex, '/');

                std::string textureCoordinatesIndex;
                std::getline(vertexDataIndexesStream, textureCoordinatesIndex, '/');

                std::string normalIndex;
                std::getline(vertexDataIndexesStream, normalIndex, ' ');

                Vertex vertex = {};
                try {
                    vertex.position = positions[(unsigned int)(std::stoul(positionIndex) - 1)];
                    vertex.textureCoordinates = textureCoordinatesItems[(unsigned int)(std::stoul(textureCoordinatesIndex) - 1)];
                    vertex.normal = normals[(unsigned int)(std::stoul(normalIndex) - 1)];
                }
                catch (std::invalid_argument e) {
                    return false;
                }
                catch (std::out_of_range e) {
                    return false;
                }

                if (uniqueVertexes.count(vertex) == 0) {
                    uniqueVertexes[vertex] = (unsigned int)modelData.vertexes.size();

                    modelData.vertexes.push_back(vertex);
                }
                meshData.indexes.push_back(uniqueVertexes[vertex]);
            }
            lineStream >> std::ws;
            if (!lineStream.eof()) {
                return false;
            }
        }
    }
    file.close();

    if (!(meshData.indexes.empty() && meshData.materialName.empty())) {
        modelData.meshDataItems.push_back(meshData);
    }

    return true;
}

bool ModelFileParser::parseMtlFile(std::string filename, ModelData& modelData) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string materialName;
    MaterialData materialData = {};

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);

        std::string keyword;
        lineStream >> keyword;

        // MaterialData
        if (keyword == "newmtl") {
            if (!materialName.empty()) {
                materialData.name = materialName;
                modelData.materialsDataItems[materialName] = materialData;

                materialData = {};
                materialName.clear();
            }

            lineStream >> std::ws;
            std::getline(lineStream, materialName);            

            if (modelData.materialsDataItems.count(materialName) != 0) {
                return false;
            }
        }
        else if (keyword == "Kd") {
            lineStream >> materialData.diffuseColor.x >> materialData.diffuseColor.y >> materialData.diffuseColor.z;
        }
        else if (keyword == "d") {
            lineStream >> materialData.opacity;
        }
        else if (keyword == "Tr") {
            float transparency;
            lineStream >> transparency;

            materialData.opacity = 1.0f - transparency;
        }
        else if (keyword == "map_Kd") {
            std::string diffuseColorFilename;
            lineStream >> std::ws;
            std::getline(lineStream, diffuseColorFilename);

            ImageFileParser imageFileParser;
            bool result = imageFileParser.parseFile(diffuseColorFilename, materialData.diffuseColorImageData);
            if (!result) {
                return false;
            }

            materialData.hasDiffuseColorImage = true;
        }
    }
    file.close();

    if (!materialName.empty()) {
        modelData.materialsDataItems[materialName] = materialData;
    }

    return true;
}

bool SceneFileParser::parseFile(std::string filename, SceneData& sceneData) {
    std::string format = getFileFormat(filename);
    if (format == "scene") {
        return parseSceneFile(filename, sceneData);
    }

    return false;
}

bool SceneFileParser::parseSceneFile(std::string filename, SceneData& sceneData) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    SceneModelData sceneModelData = {};

    ModelData modelData = {};

    std::vector<std::string> filenames;
    std::vector<DirectX::XMFLOAT3> positions;
    std::vector<DirectX::XMFLOAT3> rotations;
    std::vector<DirectX::XMFLOAT3> scalingItems;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);

        std::string keyword;
        lineStream >> keyword;

        if (keyword == "f") {
            std::string filenameData;
            lineStream >> filenameData;

            std::istringstream filenameDataStream(filenameData);

            std::getline(filenameDataStream, filename, '\n');
            filenames.push_back(filename);
        }
        else if (keyword == "p") {
            DirectX::XMFLOAT3 position = {};
            lineStream >> position.x >> position.y >> position.z;
            positions.push_back(position);
        }
        else if (keyword == "r") {
            DirectX::XMFLOAT3 rotation = {};
            lineStream >> rotation.x >> rotation.y >> rotation.z;
            rotations.push_back(rotation);
        }
        else if (keyword == "s") {
            DirectX::XMFLOAT3 scaling = {};
            lineStream >> scaling.x >> scaling.y >> scaling.z;
            scalingItems.push_back(scaling);
        }
        else if (keyword == "m") {
            std::string modelDataIndexes;
            lineStream >> modelDataIndexes;

            std::istringstream modelDataStream(modelDataIndexes);

            std::string filenameIndex;
            std::getline(modelDataStream, filenameIndex, '/');

            std::string positionIndex;
            std::getline(modelDataStream, positionIndex, '/');

            std::string rotationIndex;
            std::getline(modelDataStream, rotationIndex, '/');

            std::string scalingIndex;
            std::getline(modelDataStream, scalingIndex, ' ');

            try {
                sceneModelData.filename = filenames[(unsigned int)(std::stoul(filenameIndex) - 1)];

                sceneModelData.transformation.position = positions[(unsigned int)(std::stoul(positionIndex) - 1)];
                sceneModelData.transformation.rotation = rotations[(unsigned int)(std::stoul(rotationIndex) - 1)];
                sceneModelData.transformation.scaling = scalingItems[(unsigned int)(std::stoul(scalingIndex) - 1)];
            }
            catch (std::invalid_argument e) {
                return false;
            }
            catch (std::out_of_range e) {
                return false;
            }
            sceneData.sceneModelDataItems.push_back(sceneModelData);

            if (sceneData.modelDataItems.count(sceneModelData.filename) == 0) {
                ModelData modelData = {};

                ModelFileParser modelFileParser;
                bool result = modelFileParser.parseFile(sceneModelData.filename, modelData);
                if (!result) {
                    return false;
                }

                sceneData.modelDataItems[sceneModelData.filename] = modelData;
            }
        }
    }
    file.close();

    return true;
}
