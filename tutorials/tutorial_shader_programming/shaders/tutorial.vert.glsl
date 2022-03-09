#version 330 core

layout(std140) uniform camera
{
	mat4 projection;
	mat4 view;
	mat4 pvm;
	mat4 ortho;
	vec4 position;
};

layout(location = 0) in vec4 pos;

void main()
{
	gl_Position = pvm * vec4(pos.xyz, 1.0);
}