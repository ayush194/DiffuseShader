#include <Object.h>

std::vector<Vertex> Object::vertices;
std::vector<Material*> Object::mtls;
unsigned int Object::instance_count = 0;

Object::Object(const char* filename) : id(Object::instance_count) {
    Object::instance_count++;
    Object::loadVertexData(filename);
    this->model = glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glGenVertexArrays(1, &(this->vao));
    glGenBuffers(1, &(this->vbo));

    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, Object::vertices.size()*sizeof(Vertex), &(Object::vertices[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
   
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(glm::vec3)));

    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void*)(2*sizeof(glm::vec3) + sizeof(glm::vec2)));

    std::cerr << "Object id: " << this->id << std::endl;
}

Object::~Object() {
    glDeleteVertexArrays(1, &(this->vao));
    glDeleteBuffers(1, &(this->vbo));
}

void Object::loadVertexData(const char* filename) {
    loadOBJ(filename, Object::vertices, Object::mtls);
}

glm::mat4& Object::getModelMatrix() {
    return this->model;
}

void Object::render() {
    //application needs to set shader before calling render
    glBindVertexArray(this->vao);
    glDrawArrays(GL_TRIANGLES, 0, Object::vertices.size());
}