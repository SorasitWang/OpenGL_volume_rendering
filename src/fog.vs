#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aText;
uniform mat4 model;
//uniform mat4 projection;
//uniform mat4 view;
out vec4 ioEyeSpacePosition;
out vec4 pos;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
    vec4 clippingPlane;
};
void main()
{
    
    mat4 mvMatrix =view * model;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    pos = model * vec4(aPos, 1.0);
    ioEyeSpacePosition = mvMatrix * vec4(aPos, 1.0);
    gl_ClipDistance[0] = dot(pos,clippingPlane);

} 