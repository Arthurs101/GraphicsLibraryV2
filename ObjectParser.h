#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class ObjParser {
public:
    ObjParser(const std::string& filename, const std::vector<float>& transformobj, const std::vector<float>& scaleobj, const std::vector<float>& rotationobj) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file");
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "v") { // Vertices
                std::vector<float> vertex;
                float value;
                while (iss >> value) {
                    vertex.push_back(value);
                }
                vertices.push_back(vertex);
            }
            else if (prefix == "vt") { // Texture Coordinates
                std::vector<float> texcoord;
                float value;
                while (iss >> value) {
                    texcoord.push_back(value);
                }
                textcoords.push_back(texcoord);
            }
            else if (prefix == "vn") { // Normals
                std::vector<float> normal;
                float value;
                while (iss >> value) {
                    normal.push_back(value);
                }
                normals.push_back(normal);
            }
            else if (prefix == "f") { // Faces
                std::vector<std::vector<float>> face;
                std::string vert;
                while (iss >> vert) {
                    std::istringstream viss(vert);
                    std::string v;
                    std::vector<float> vertex;
                    while (std::getline(viss, v, '/')) {
                        vertex.push_back(std::stoi(v));
                    }
                    face.push_back(vertex);
                }
                faces.push_back(face);
            }
        }

        if (transformobj.size() != 3) {
            std::runtime_error("Transform not supported, must be 3 dimensional");
        }
        if (scaleobj.size() != 3) {
            std::runtime_error("Scale not supported, must be 3 dimensional");
        }
        if (rotationobj.size() != 3) {
            std::runtime_error("Rotation not supported, must be 3 dimensional");
        }
        transform = std::move(transformobj);
        scale = std::move(scaleobj);
        rotation = std::move(rotationobj);
        std::cout << "faces" << faces.size() << std::endl;
        std::cout << "vertex" << vertices.size() << std::endl;



    }
    std::vector<std::vector<float>> vertices;
    std::vector<std::vector<float>> textcoords;
    std::vector<std::vector<float>> normals;
    std::vector<std::vector<std::vector<float>>> faces;
    std::vector<float> transform;
    std::vector<float> scale;
    std::vector<float> rotation;
};
