//Code Credits: Calvin1602@github.com

#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <vector>

bool loadOBJ(
	const char * path, 
	std::vector<glm::vec3> & out_positions, 
	std::vector<glm::vec2> & out_uvs, 
	std::vector<glm::vec3> & out_normals
);

#endif
