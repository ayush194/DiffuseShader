//
//  Created by Ayush Kumar on 6/26/18.
//  Copyright © 2018 Ayush Kumar. All rights reserved.
//

//GLEW is for external OpenGL function loading
//Flag -lglew required for compiling with g++
#include <GL/glew.h>
//GLFW for window and input handling as well as the main game loop
//Flag -lglfw required for compiling with g++
#include <GLFW/glfw3.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
//GLUT doesn't give you control over the main loop which is why it should only be used for testing purposes
//In other cases we use GLFW
//#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
//#include <GL/glut.h>
#endif

//glm for matrix operations
//Flag -I/usr/local/include required for compiling with g++
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//model loading library
#include "objloader.hpp"

//stb_image.h required for texture loading
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

#include "camera.h"
#include "shader.h"

#include <iostream>
#include <vector>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
glm::vec3 OBJECT_COLOR(1.0f, 0.5f, 0.31f);
float LAST_TIME = 0.0;
float DELTA_TIME = 0.0;
float CURSOR_POS_X = SCR_WIDTH / 2;
float CURSOR_POS_Y = SCR_HEIGHT / 2;
bool FIRST_MOUSE = true;
Camera CAMERA;

void mouseCallback(GLFWwindow*, double, double);
void scrollCallback(GLFWwindow*, double, double);
void framebufferSizeCallback(GLFWwindow*, int, int);
void processInput(GLFWwindow*);
unsigned int loadTexture(char const*);

int main() {
    // Initialise GLFW
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);    
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Open a window and create its OpenGL context
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "DiffuseShader", NULL, NULL);
    if( window == NULL ){
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    //glfwSetWindowSizeCallback(window, framebuffer_size_callback);
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwSetCursorPos(window, 1024/2, 768/2);
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    
    //Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);
    
    //building shaders
    Shader diffuse_shader("shaders/diffuse.vs", "shaders/diffuse.fs");
    
    unsigned int vbo_normals, vbo_positions, vao_cube;
    glGenVertexArrays(1, &vao_cube);
    glGenBuffers(1, &vbo_positions);
    glGenBuffers(1, &vbo_normals);
    
    std::vector<glm::vec3> positions, normals;
    std::vector<glm::vec2> tmp;
    loadOBJ("icosahedron.obj", positions, tmp, normals);
    
    /*
    //Debug Statements
    for(glm::vec3 pos : positions) {
        printf("Position : %f %f %f\n", pos[0], pos[1], pos[2]);
    }
    for(glm::vec3 normal : normals) {
        printf("Normal : %f %f %f\n", normal[0], normal[1], normal[2]);
    }
    */

    glBindVertexArray(vao_cube);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_positions);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    //calling glVertexAttribPointer while vbo_positions was bound ensures that the data for
    //0th attribute comes from vbo_positions
    glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    while (!glfwWindowShouldClose(window)) {
        //process input
        float curr_time = glfwGetTime();
        DELTA_TIME = curr_time - LAST_TIME;
        LAST_TIME = curr_time;
        
        processInput(window);
        
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //create transform matrices
        glm::mat4 model, view, projection;
        view = CAMERA.getViewMatrix();
        //view = glm::lookAt(glm::vec3(5.0, 5.0, 5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
        //projection = glm::perspective(45.0f, (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 10000.0f);
        projection = glm::perspective(glm::radians(CAMERA.zoom), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 10000.0f);
        
        diffuse_shader.use();
        diffuse_shader.setMat4("view", view);
        diffuse_shader.setMat4("projection", projection);
        diffuse_shader.setVec3("glightpos", glm::vec3(3,4,5));
        
        //transform all local coordinates to global coordinates
        for(glm::vec3& pos : positions) {
            pos = model * glm::vec4(pos, 1.0);
        }
        //transform all local normals to global normals without accounting for translations
        for(glm::vec3& normal : normals) {
            normal = glm::mat3(model) * normal;
        }
        glBindBuffer(GL_ARRAY_BUFFER, vbo_positions);
        glBufferData(GL_ARRAY_BUFFER, positions.size()*sizeof(glm::vec3), &positions[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
        glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
        
        //glBindVertexArray(vao_cube);
        glDrawArrays(GL_TRIANGLES, 0, positions.size());

        //accesorial actions
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &vao_cube);
    glDeleteBuffers(1, &vbo_positions);
    glDeleteBuffers(1, &vbo_normals);
    //glDeleteTextures(1, &crateDiffuse);
    //glDeleteTextures(1, &crateSpecular);
    glfwTerminate();
    return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        CAMERA.processKeyboard(FORWARD, DELTA_TIME);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        CAMERA.processKeyboard(BACKWARD, DELTA_TIME);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        CAMERA.processKeyboard(LEFT, DELTA_TIME);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        CAMERA.processKeyboard(RIGHT, DELTA_TIME);
    }
}

void mouseCallback(GLFWwindow* window, double posx, double posy) {
    if (FIRST_MOUSE) {
        CURSOR_POS_X = posx;
        CURSOR_POS_Y = posy;
        FIRST_MOUSE = false;
    }
    float posx_offset = posx - CURSOR_POS_X;
    float posy_offset = CURSOR_POS_Y - posy;
    CURSOR_POS_X = posx;
    CURSOR_POS_Y = posy;
    CAMERA.processMouseMovement(posx_offset, posy_offset);
}

void scrollCallback(GLFWwindow* window, double offsetx, double offsety) {
    CAMERA.processMouseScroll(offsety);
}
/*
// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    
    return textureID;
}
*/
