//Code Credits: Calvin1602@github.com

#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <sstream>

#include <glm/glm.hpp>

#include "objloader.hpp"

// Very, VERY simple OBJ loader.
// Here is a short list of features a real function would provide : 
// - Binary files. Reading a model should be just a few memcpy's away, not parsing a file at runtime. In short : OBJ is not very great.
// - Animations & bones (includes bones weights)
// - Multiple UVs
// - All attributes should be optional, not "forced"
// - More stable. Change a line in the OBJ file and it crashes.
// - More secure. Change another line and you can inject code.
// - Loading from memory, stream, etc

void splitByDelim(char* s, char d, int* a, int* b, int* c) {
	std::stringstream ss(s);
	std::string str;
	int tmp[3] = {-1, -1, -1};
	for(int i = 0; getline(ss, str, d); i++) {
		if (!str.empty()) {
			tmp[i] = stoi(str);
		}
	}
	*a = tmp[0];
	*b = tmp[1];
	*c = tmp[2];
}

bool loadOBJ(
	const char * path, 
	std::vector<glm::vec3> & out_positions, 
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
){
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> position_indices, uv_indices, normal_indices;
	std::vector<glm::vec3> temp_positions; 
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if( file == NULL ){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while( 1 ){

		char line_header[128];
		// read the first word of the line
		int res = fscanf(file, "%s", line_header);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		
		if ( strcmp( line_header, "v" ) == 0 ){
			glm::vec3 position;
			fscanf(file, "%f %f %f\n", &position.x, &position.y, &position.z );
			temp_positions.push_back(position);
		}else if ( strcmp( line_header, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}else if ( strcmp( line_header, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( line_header, "f" ) == 0 ){
			int position_index[3], uv_index[3], normal_index[3];
			char tmp1[10], tmp2[10], tmp3[10];
			int matches = fscanf(file, "%s %s %s\n", tmp1, tmp2, tmp3);
			if (matches != 3){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				fclose(file);
				return false;
			}
			splitByDelim(tmp1, '/', &position_index[0], &uv_index[0], &normal_index[0]);
			splitByDelim(tmp2, '/', &position_index[1], &uv_index[1], &normal_index[1]);
			splitByDelim(tmp3, '/', &position_index[2], &uv_index[2], &normal_index[2]);

			position_indices.push_back(position_index[0]);
			position_indices.push_back(position_index[1]);
			position_indices.push_back(position_index[2]);
			uv_indices    .push_back(uv_index[0]);
			uv_indices    .push_back(uv_index[1]);
			uv_indices    .push_back(uv_index[2]);
			normal_indices.push_back(normal_index[0]);
			normal_indices.push_back(normal_index[1]);
			normal_indices.push_back(normal_index[2]);
		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for( unsigned int i=0; i<position_indices.size(); i++ ){

		// Get the indices of its attributes
		int position_index = position_indices[i];
		int uv_index = uv_indices[i];
		int normal_index = normal_indices[i];
		
		// Get the attributes and put the attributes in buffers
		if (position_index >= 0) {
			out_positions.push_back(temp_positions[ position_index-1 ]);
		}
		if (uv_index >= 0) {
			out_uvs     .push_back(temp_uvs[ uv_index-1 ]);
		}
		if (normal_index >= 0) {
			out_normals .push_back(temp_normals[ normal_index-1 ]);
		}
	
	}
	fclose(file);
	return true;
}

