#include <iostream>
#include <sstream>
#include <map>
#include <tinyxml2.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include "util.h"
#include "model.h"
#include "texture.h"

using namespace glm;
using namespace std;
using namespace ogl;
using namespace tinyxml2;

// simple OBJ loader
void loadOBJ(
    const string& path,
    vector<vec3>& vertices,
    vector<vec2>& uvs,
    vector<vec3>& normals,
    vector<unsigned int>& indices
) {
    cout << "Loading OBJ file: " << path << endl;

    vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    vector<vec3> temp_vertices;
    vector<vec2> temp_uvs;
    vector<vec3> temp_normals;
    indices.clear();

    FILE * file = fopen(path.c_str(), "r");
    if (file == NULL) {
        throw runtime_error("Can't open the file.\n");
    }

    while (1) {
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader

        if (strcmp(lineHeader, "v") == 0) {
            vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        } else if (strcmp(lineHeader, "vt") == 0) {
            vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            // Invert V coordinate since we will only use DDS texture,
            // which are inverted. Remove if you want to use TGA or BMP loaders.
            uv.y = -uv.y;
            temp_uvs.push_back(uv);
        } else if (strcmp(lineHeader, "vn") == 0) {
            vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        } else if (strcmp(lineHeader, "f") == 0) {
            string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                                 &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                                 &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                                 &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9) {
                fclose(file);
                throw runtime_error("File can't be read by our simple parser.\n");
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        } else {
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }
    }

    // For each vertex of each triangle
    for (unsigned int i = 0; i < vertexIndices.size(); i++) {
        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];

        // Get the attributes thanks to the index
        vec3 vertex = temp_vertices[vertexIndex - 1];
        vec2 uv = temp_uvs[uvIndex - 1];
        vec3 normal = temp_normals[normalIndex - 1];

        // Put the attributes in buffers
        vertices.push_back(vertex);
        uvs.push_back(uv);
        normals.push_back(normal);
        indices.push_back(indices.size());
    }
    fclose(file);
}

void loadVTP(
    const string& path,
    vector<vec3>& vertices, vector<vec2>& uvs,
    vector<vec3>& normals,
    vector<unsigned int>& indices) {
    indices.clear();
    const char* method = "PolygonalMesh::loadVtpFile()";
    XMLDocument vtp;
    auto res = vtp.LoadFile(path.c_str());
    assert(res == 0);

    XMLElement* root = vtp.FirstChildElement("VTKFile");
    assert(root != nullptr);
    assert(root->Attribute("type", "PolyData"));

    XMLElement* polydata = root->FirstChildElement("PolyData");
    assert(polydata != nullptr);
    XMLElement* piece = polydata->FirstChildElement("Piece");
    assert(piece != nullptr);
    XMLElement* enormals = piece->FirstChildElement("PointData");
    assert(enormals != nullptr);
    XMLElement* points = piece->FirstChildElement("Points");
    assert(points != nullptr);

    int numPoints, numPolys;
    piece->QueryIntAttribute("NumberOfPoints", &numPoints);
    piece->QueryIntAttribute("NumberOfPolys", &numPolys);

    //assert(enormals->Attribute("format", "ascii"));
    const char* normalsStr = enormals->FirstChildElement("DataArray")->FirstChild()->Value();
    stringstream sNorm(normalsStr);
    vector<vec3> tempNormals;
    do {
        vec3 normal;
        sNorm >> normal.x >> normal.y >> normal.z;
        tempNormals.push_back(normal);
    } while (sNorm.good());
    assert(tempNormals.size() == numPoints + 1);

    XMLElement* pointData = points->FirstChildElement("DataArray");
    assert(pointData != nullptr);
    assert(pointData->Attribute("format", "ascii"));
    const char* coordsStr = points->FirstChildElement("DataArray")->FirstChild()->Value();

    stringstream sCoord(coordsStr);
    vector<vec3> coordinates;
    do {
        vec3 coord;
        sCoord >> coord.x >> coord.y >> coord.z;
        coordinates.push_back(coord);
    } while (sCoord.good());
    coordinates.pop_back();  // the last is corrupted
    assert(coordinates.size() == numPoints);

    XMLElement* polys = piece->FirstChildElement("Polys");
    assert(polys != nullptr);

    // TODO beter implementation
    XMLElement *econnectivity, *eoffsets;
    if (polys->FirstChildElement("DataArray")->Attribute("Name", "connectivity")) {
        econnectivity = polys->FirstChildElement("DataArray");
        assert(econnectivity != nullptr);
        assert(econnectivity->Attribute("format", "ascii"));
    } else {
        throw runtime_error("Can't access connectivity");
    }

    if (polys->LastChildElement("DataArray")->Attribute("Name", "offsets")) {
        eoffsets = polys->LastChildElement("DataArray");
        assert(eoffsets != nullptr);
        assert(eoffsets->Attribute("format", "ascii"));
    } else {
        throw runtime_error("Can't access offsets");
    }

    // read offsets
    const char* offsetsStr = eoffsets->FirstChild()->Value();
    stringstream sOffsets(offsetsStr);
    vector<int> offsets;
    do {
        int offset;
        sOffsets >> offset;
        offsets.push_back(offset);
    } while (sOffsets.good());
    assert(offsets.size() == numPolys + 1);

    // read connectivity
    const char* connStr = econnectivity->FirstChild()->Value();
    stringstream sConn(connStr);
    vector<int> connectivity;
    do {
        int conn;
        sConn >> conn;
        connectivity.push_back(conn);
    } while (sConn.good());
    assert(connectivity.size() == offsets.back() + 1);

    // construct vertices
    int startPoly = 0;
    for (int i = 0; i < numPolys; ++i) {
        vector<int> face = slice(connectivity, startPoly, offsets[i]);
        int i1 = 0, i2 = 1, i3 = 2;
        while (i3 != face.size()) {
            vertices.push_back(coordinates[face[i1]]);
            normals.push_back(tempNormals[face[i1]]);
            indices.push_back(indices.size());
            vertices.push_back(coordinates[face[i2]]);
            normals.push_back(tempNormals[face[i2]]);
            indices.push_back(indices.size());
            vertices.push_back(coordinates[face[i3]]);
            normals.push_back(tempNormals[face[i3]]);
            indices.push_back(indices.size());
            i2++;
            i3++;
        }
        startPoly = offsets[i];
    }
}

void loadOBJWithTiny(
    const string& path,
    vector<vec3>& vertices,
    vector<vec2>& uvs,
    vector<vec3>& normals,
    vector<unsigned int>& indices) {
    tinyobj::attrib_t attrib;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;

    string err;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str())) {
        throw runtime_error(err);
    }

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            vec3 vertex = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]};
            if (attrib.texcoords.size() != 0) {
                vec2 uv = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1 - attrib.texcoords[2 * index.texcoord_index + 1]};
                uvs.push_back(uv);
            }
            if (attrib.normals.size() != 0) {
                vec3 normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]};
                normals.push_back(normal);
            }

            vertices.push_back(vertex);
            indices.push_back(indices.size());
        }
    }

    std::clog << "Loaded obj: " << path << '\n';

    // TODO .mtl loader
}

struct PackedVertex {
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
    bool operator<(const PackedVertex that) const {
        return memcmp((void*) this, (void*) &that, sizeof(PackedVertex)) > 0;
    };
};

bool getSimilarVertexIndex(
    PackedVertex& packed,
    map<PackedVertex, unsigned int>& vertexToOutIndex,
    unsigned int& result) {
    map<PackedVertex, unsigned int>::iterator it = vertexToOutIndex.find(packed);
    if (it == vertexToOutIndex.end()) {
        return false;
    } else {
        result = it->second;
        return true;
    }
}

void indexVBO(
    const vector<vec3>& in_vertices,
    const vector<vec2>& in_uvs,
    const vector<vec3>& in_normals,
    vector<unsigned int>& out_indices,
    vector<vec3>& out_vertices,
    vector<vec2>& out_uvs,
    vector<vec3>& out_normals) {
    map<PackedVertex, unsigned int> vertexToOutIndex;

    // For each input vertex
    for (int i = 0; i < static_cast<int>(in_vertices.size()); i++) {
        vec3 vertices = in_vertices[i];
        vec2 uvs;
        vec3 normals;
        if (in_uvs.size() != 0) uvs = in_uvs[i];
        if (in_normals.size() != 0) normals = in_normals[i];
        PackedVertex packed = {vertices, uvs, normals};

        // Try to find a similar vertex in out_XXXX
        unsigned int index;
        bool found = getSimilarVertexIndex(packed, vertexToOutIndex, index);

        if (found) { // A similar vertex is already in the VBO, use it instead !
            out_indices.push_back(index);
        } else { // If not, it needs to be added in the output data.
            out_vertices.push_back(vertices);
            if (in_uvs.size() != 0) out_uvs.push_back(uvs);
            if (in_normals.size() != 0) out_normals.push_back(normals);
            unsigned int newindex = (unsigned int) out_vertices.size() - 1;
            out_indices.push_back(newindex);
            vertexToOutIndex[packed] = newindex;
        }
    }
}

Drawable::Drawable(string path) {
    if (path.substr(path.size() - 3, 3) == "obj") {
        loadOBJWithTiny(path.c_str(), vertices, uvs, normals, VEC_UINT_DEFAUTL_VALUE);
    } else if (path.substr(path.size() - 3, 3) == "vtp") {
        loadVTP(path.c_str(), vertices, uvs, normals, VEC_UINT_DEFAUTL_VALUE);
    } else {
        throw runtime_error("File format not supported: " + path);
    }

    createContext();
}

Drawable::Drawable(const vector<vec3>& vertices, const vector<vec2>& uvs,
                   const vector<vec3>& normals) : vertices(vertices), uvs(uvs), normals(normals) {
    createContext();
}

Drawable::~Drawable() {
    glDeleteBuffers(1, &verticesVBO);
    glDeleteBuffers(1, &uvsVBO);
    glDeleteBuffers(1, &normalsVBO);
    glDeleteBuffers(1, &elementVBO);
    glDeleteBuffers(1, &VAO);
}

void Drawable::bind() {
    glBindVertexArray(VAO);
}

void Drawable::draw(int mode) {
    glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, NULL);
}

void Drawable::createContext() {
    indices = vector<unsigned int>();
    indexVBO(vertices, uvs, normals, indices, indexedVertices, indexedUVS, indexedNormals);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &verticesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
    glBufferData(GL_ARRAY_BUFFER, indexedVertices.size() * sizeof(vec3),
                 &indexedVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    if (indexedNormals.size() != 0) {
        glGenBuffers(1, &normalsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
        glBufferData(GL_ARRAY_BUFFER, indexedNormals.size() * sizeof(vec3),
                     &indexedNormals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);
    }

    if (indexedUVS.size() != 0) {
        glGenBuffers(1, &uvsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, uvsVBO);
        glBufferData(GL_ARRAY_BUFFER, indexedUVS.size() * sizeof(vec2),
                     &indexedUVS[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);
    }

    // Generate a buffer for the indices as well
    glGenBuffers(1, &elementVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);
}

/*****************************************************************************/

Mesh::Mesh(
    const vector<vec3>& vertices,
    const vector<vec2>& uvs,
    const vector<vec3>& normals,
    const Material& mtl)
    : vertices{vertices}, uvs{uvs}, normals{normals}, mtl{mtl} {
    createContext();
}

Mesh::Mesh(Mesh&& other)
    : vertices{std::move(other.vertices)}, normals{std::move(other.normals)},
    indexedVertices{std::move(other.indexedVertices)}, indexedNormals{std::move(other.indexedNormals)},
    uvs{std::move(other.uvs)}, indexedUVS{std::move(other.indexedUVS)},
    indices{std::move(other.indices)}, mtl{std::move(other.mtl)},
    VAO{other.VAO}, verticesVBO{other.verticesVBO}, normalsVBO{other.normalsVBO},
    uvsVBO{other.uvsVBO}, elementVBO{other.elementVBO} {
    other.VAO = 0;
    other.verticesVBO = 0;
    other.normalsVBO = 0;
    other.uvsVBO = 0;
    other.elementVBO = 0;
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &verticesVBO);
    glDeleteBuffers(1, &uvsVBO);
    glDeleteBuffers(1, &normalsVBO);
    glDeleteBuffers(1, &elementVBO);
    glDeleteVertexArrays(1, &VAO);
}

void Mesh::bind() {
    glBindVertexArray(VAO);
}

void Mesh::draw(int mode) {
    glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, NULL);
}

void Mesh::createContext() {
    indices = vector<unsigned int>();
    indexVBO(vertices, uvs, normals, indices, indexedVertices, indexedUVS, indexedNormals);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &verticesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
    glBufferData(GL_ARRAY_BUFFER, indexedVertices.size() * sizeof(vec3),
                 &indexedVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    if (indexedNormals.size() != 0) {
        glGenBuffers(1, &normalsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
        glBufferData(GL_ARRAY_BUFFER, indexedNormals.size() * sizeof(vec3),
                     &indexedNormals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);
    }

    if (indexedUVS.size() != 0) {
        glGenBuffers(1, &uvsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, uvsVBO);
        glBufferData(GL_ARRAY_BUFFER, indexedUVS.size() * sizeof(vec2),
                     &indexedUVS[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);
    }

    // Generate a buffer for the indices as well
    glGenBuffers(1, &elementVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);
}

Model::Model(string path, Model::MTLUploadFunction* uploader)
    : uploadFunction{uploader} {
    if (path.substr(path.size() - 3, 3) == "obj") {
        loadOBJWithTiny(path.c_str());
    } else {
        throw runtime_error("File format not supported: " + path);
    }
}

Model::~Model() {
    for (const auto& t : textures) {
        glDeleteTextures(1, &t.second);
    }
}

void Model::draw() {
    for (auto& mesh : meshes) {
        mesh.bind();
        if (uploadFunction)
            uploadFunction(mesh.mtl);
        mesh.draw();
    }
}

void Model::loadOBJWithTiny(const std::string& filename) {
    tinyobj::attrib_t attrib;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;

    string err;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str())) {
        throw runtime_error(err);
    }

    for (const auto& material : materials) {
        loadTexture(material.ambient_texname);
        loadTexture(material.diffuse_texname);
        loadTexture(material.specular_texname);
        loadTexture(material.specular_highlight_texname);
    }

    for (const auto& shape : shapes) {
        vector<vec3> vertices{};
        vector<vec2> uvs{};
        vector<vec3> normals{};
        for (const auto& index : shape.mesh.indices) {
            int vertex_index = index.vertex_index;
            if (vertex_index < 0) vertex_index += attrib.vertices.size() / 3;
            vec3 vertex = {
                attrib.vertices[3 * vertex_index + 0],
                attrib.vertices[3 * vertex_index + 1],
                attrib.vertices[3 * vertex_index + 2]};
            if (attrib.texcoords.size() != 0) {
                int texcoord_index = index.texcoord_index;
                if (texcoord_index < 0) texcoord_index += attrib.texcoords.size() / 2;
                vec2 uv = {
                    attrib.texcoords[2 * texcoord_index + 0],
                    1 - attrib.texcoords[2 * texcoord_index + 1]};
                uvs.push_back(uv);
            }
            if (attrib.normals.size() != 0) {
                int normal_index = index.normal_index;
                if (normal_index < 0) normal_index += attrib.normals.size() / 3;
                vec3 normal = {
                    attrib.normals[3 * normal_index + 0],
                    attrib.normals[3 * normal_index + 1],
                    attrib.normals[3 * normal_index + 2]};
                normals.push_back(normal);
            }
            vertices.push_back(vertex);
        }
        Material mtl{};
        if (materials.size() > 0 && shape.mesh.material_ids.size() > 0) {
            int idx = shape.mesh.material_ids[0];
            if (idx < 0 || idx >= static_cast<int>(materials.size()))
                idx = static_cast<int>(materials.size()) - 1;
            tinyobj::material_t mat = materials[idx];
            mtl = {
                {mat.ambient[0], mat.ambient[1], mat.ambient[2], 1},
                {mat.diffuse[0], mat.diffuse[1], mat.diffuse[2], 1},
                {mat.specular[0], mat.specular[1], mat.specular[2], 1},
                mat.shininess,
                textures[mat.ambient_texname],
                textures[mat.diffuse_texname],
                textures[mat.specular_texname],
                textures[mat.specular_highlight_texname]
            };
            if (mtl.texKa) mtl.Ka.r = -1.0f;
            if (mtl.texKd) mtl.Kd.r = -1.0f;
            if (mtl.texKs) mtl.Ks.r = -1.0f;
            if (mtl.texNs) mtl.Ns = -1.0f;
        }
        meshes.emplace_back(vertices, uvs, normals, mtl);
    }

    printf("Loaded obj %s",filename );
}

void Model::loadTexture(const std::string& filename) {
    if (filename.length() == 0) return;
    if (textures.find(filename) == end(textures)) {
        GLuint id = loadSOIL(filename.c_str());
        if (!id) throw std::runtime_error("Failed to load texture: " + filename);
        textures[filename] = id;
    }
}
