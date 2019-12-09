//
//  Created by Ayush Kumar on 6/26/18.
//  Copyright © 2018 Ayush Kumar. All rights reserved.
//

#version 330 core
layout (location = 0) in vec3 gpos;     //global pos
layout (location = 1) in vec3 gnormal;  //global normal
layout (location = 2) in vec2 guv;  //global normal
layout (location = 3) in uint gmtl_idx;  //global normal

out vec3 vnormal;                       //view normal
out vec3 vpos;                          //view pos of fragment
out vec3 vlightpos;

//do not perform any model calculations in the vertex shader
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 glightpos;                 //global lightpos

void main() {
    gl_Position = projection * view * model * vec4(gpos, 1.0);
    //Normal = mat3(transpose(inverse(view * model))) * aNormal;
    vlightpos = vec3(view * vec4(glightpos, 1.0));
    //we do not want to apply the translation due to the view and model matrices
    //we only need the rotations caused by the view and model matrices, hence multiply
    //the normals by mat3(view * model)
    vnormal = vec3(mat3(view * model) * gnormal);
    vpos = vec3(view * vec4(gpos, 1.0));
    //texcoords = gtexcoords;
}
