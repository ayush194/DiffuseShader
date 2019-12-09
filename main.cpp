//
//  Created by Ayush Kumar on 27/09/19.
//  Copyright © 2018 Ayush Kumar. All rights reserved.
//

#include <App.h>
#include <shader.h>
#include <Object.h>

int main() {
	App();
	Object* obj = new Object("icosahedron.obj");
	
	//building shaders
	Shader diffuse_shader("shaders/diffuse.vs", "shaders/diffuse.fs");
	Shader axes_shader("shaders/axes.vs", "shaders/axes.fs");

	while (!glfwWindowShouldClose(App::window)) {
		//process input
		App::updateFrame();
		//if (gamestate->simulation_complete) {
		App::processInput();
		//}
		App::clearColor();

		//create transform matrices
		glm::mat4 model = obj->getModelMatrix();
		glm::mat4 view = App::getViewMatrix();
		glm::mat4 persp_projection = App::getPerspectiveProjectionMatrix();

		diffuse_shader.use();
		diffuse_shader.setMat4("model", model);
		diffuse_shader.setMat4("view", view);
		diffuse_shader.setMat4("projection", persp_projection);
		diffuse_shader.setVec3("glightpos", glm::vec3(3,4,5));
		//render object
		obj->render();

		axes_shader.use();
		axes_shader.setMat4("view", view);
		axes_shader.setMat4("projection", persp_projection);
		//draw axes
		App::drawAxes();

		//accesorial actions
		App::endFrame();
	}
	glfwTerminate();
	//deallocate everything that was dynamically created (using the new keyword);
	delete obj;

	return 0;
}
