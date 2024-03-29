#include <App.h>

unsigned int App::SCR_WIDTH = 800;
unsigned int App::SCR_HEIGHT = 600;
float App::LAST_TIME = 0.0;
float App::DELTA_TIME = 0.0;
float App::CURSOR_POS[2] = {static_cast<float>(App::SCR_WIDTH) / 2, 
                            static_cast<float>(App::SCR_HEIGHT) / 2};
bool App::FIRST_MOUSE = true;

// Open a window and create its OpenGL context
GLFWwindow* App::window;
Camera App::CAMERA = Camera();
//GameState* App::gamestate = NULL;

App::App() {
    // Set error callback
    glfwSetErrorCallback(App::errorCallback);

    // Initialise GLFW
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        //return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);    
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    window = glfwCreateWindow(App::SCR_WIDTH, App::SCR_HEIGHT, "DiffuseShader", NULL, NULL);
    if( App::window == NULL ){
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        //return -1;
    }
    glfwMakeContextCurrent(App::window);
    glfwSetFramebufferSizeCallback(App::window, App::framebufferSizeCallback);
    glfwSetCursorPosCallback(App::window, App::cursorPosCallback);
    glfwSetScrollCallback(App::window, App::scrollCallback);
    //glfwSetWindowSizeCallback(window, framebuffer_size_callback);
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(App::window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(App::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwSetCursorPos(App::window, 1024/2, 768/2);
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        //return -1;
    }
    
    //Enable depth test
    glEnable(GL_DEPTH_TEST);

    //Enable Anti-Aliasing
    glEnable(GL_LINE_SMOOTH);

    //if you enable GL_CULL_FACE, then the cubemaps will not be rendered
    //this is because at any time, any face of the cube will not
    //lie inside the view frustum and so it will be culled
    glDisable(GL_CULL_FACE);


    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    //glEnable(GL_CULL_FACE);

    // For rendering fonts
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void App::updateFrame() {
    float curr_time = glfwGetTime();
	App::DELTA_TIME = curr_time - App::LAST_TIME;
	App::LAST_TIME = curr_time;
}

void App::endFrame() {
    glfwSwapBuffers(App::window);
	glfwPollEvents();
}

void App::clearColor() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

glm::mat4 App::getViewMatrix() {
    return App::CAMERA.getViewMatrix();
}

glm::mat4 App::getPerspectiveProjectionMatrix() {
    return glm::perspective(glm::radians(App::CAMERA.zoom), (float)App::SCR_WIDTH / App::SCR_HEIGHT, 0.1f, 10000.0f);
}

glm::mat4 App::getOrthographicProjectionMatrix() {
    return glm::ortho(0.0f, static_cast<GLfloat>(App::SCR_WIDTH), 0.0f, static_cast<GLfloat>(App::SCR_HEIGHT));
}

void App::drawAxes() {
    float xyz[] = { 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,   100.0, 0.0, 0.0, 1.0, 0.0, 0.0, 
                    0.0, 0.0, 0.0, 0.0, 1.0, 0.0,   0.0, 100.0, 0.0, 0.0, 1.0, 0.0, 
                    0.0, 0.0, 0.0, 0.0, 0.0, 1.0,   0.0, 0.0, 100.0, 0.0, 0.0, 1.0};
    unsigned int vbo_axes, vao_axes;
    glGenVertexArrays(1, &vao_axes);
    glGenBuffers(1, &vbo_axes);
    glBindVertexArray(vao_axes);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_axes);
    glBufferData(GL_ARRAY_BUFFER, sizeof(xyz), xyz, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glDrawArrays(GL_LINES, 0, 6);

    //unbind vertex arrays and buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao_axes);
    glDeleteBuffers(1, &vbo_axes);
}

void App::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void App::processInput() {
    if (glfwGetKey(App::window, GLFW_KEY_W) == GLFW_PRESS) {
        App::CAMERA.processKeyboard(FORWARD, DELTA_TIME);
    }
    if (glfwGetKey(App::window, GLFW_KEY_S) == GLFW_PRESS) {
        App::CAMERA.processKeyboard(BACKWARD, DELTA_TIME);
    }
    if (glfwGetKey(App::window, GLFW_KEY_A) == GLFW_PRESS) {
        App::CAMERA.processKeyboard(LEFT, DELTA_TIME);
    }
    if (glfwGetKey(App::window, GLFW_KEY_D) == GLFW_PRESS) {
        App::CAMERA.processKeyboard(RIGHT, DELTA_TIME);
    }
    if (glfwGetKey(App::window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(App::window, GL_TRUE);
    }
}

void App::cursorPosCallback(GLFWwindow* window, double posx, double posy) {
    if (FIRST_MOUSE) {
        App::CURSOR_POS[0] = posx;
        App::CURSOR_POS[1] = posy;
        App::FIRST_MOUSE = false;
    }
    float posx_offset = posx - App::CURSOR_POS[0];
    float posy_offset = App::CURSOR_POS[1] - posy;
    App::CURSOR_POS[0] = posx;
    App::CURSOR_POS[1] = posy;
    App::CAMERA.processMouseMovement(posx_offset, posy_offset);
}

void App::scrollCallback(GLFWwindow* window, double offsetx, double offsety) {
    App::CAMERA.processMouseScroll(offsety);
}

void App::errorCallback(int error, const char* description) {
    puts(description);
}

