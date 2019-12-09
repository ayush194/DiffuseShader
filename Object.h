#ifndef POOLTABLE_H
#define POOLTABLE_H

#include "opengl_includes.h"
//glm for matrix operations
#include "glm_includes.h"
//object loading library
#include "objloader.hpp"

#include <iostream>
#include <vector>

class Object {
public:
    unsigned int vao;
    unsigned int vbo;
    //unsigned int texture_id;
    static unsigned int instance_count;
    unsigned int id;

    glm::mat4 model;
    //all objects share the same common vertex data
    //and the common material data
    static std::vector<Vertex> vertices;
    static std::vector<Material*> mtls;

    Object(const char*);
    ~Object();
    glm::mat4& getModelMatrix();
    static void loadVertexData(const char*);
    void render();
};

#endif