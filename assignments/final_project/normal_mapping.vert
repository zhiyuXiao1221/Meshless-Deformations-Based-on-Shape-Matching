/*This is your first vertex shader!*/

#version 330 core

#define PI 3.14159265

/*default camera matrices. do not modify.*/
layout (std140) uniform camera
{
	mat4 projection;	/*camera's projection matrix*/
	mat4 view;			/*camera's view matrix*/
	mat4 pvm;			/*camera's projection*view*model matrix*/
	mat4 ortho;			/*camera's ortho projection matrix*/
	vec4 position;		/*camera's position in world space*/
};

/*input variables*/


layout (location=0) in vec4 pos;		/*vertex position*/
layout (location=1) in vec4 color;		/*vertex color*/
layout (location=2) in vec4 normal;		/*vertex normal*/	////ATTENTION: this attribute is important for TBN matrix calculation!
layout (location=3) in vec4 uv;			/*vertex uv*/		////ATTENTION: this is the texture coordinates!
layout (location=4) in vec4 tangent;	/*vertex tangent*/	////ATTENTION: this attribute is important for TBN matrix calculation!

/*output variables*/
//// TODO: declare your output variables
out vec3 vtx_pos;
out vec4 vtx_color;
out vec2 vtx_uv;
out vec3 vtx_normal;
out vec3 vtx_tangent;
out vec3 vtx_binormal;
out mat3 FBN;
void main()												
{
	/*camera-transformed position. do not modify.*/
	gl_Position=pvm*vec4(pos.xyz,1.f);

	//// TODO: set your out varialbes
	vtx_pos = pos.xyz;
	vtx_uv = uv.xy;
	vtx_color = color;
	vtx_normal = normal.xyz;
	mat4 model = inverse(view)*inverse(projection)*pvm;
	vtx_tangent = normalize(vec3(model * tangent));
    vtx_normal = normalize(vec3(model *normal));
	vtx_binormal = cross(vtx_normal,vtx_tangent)*tangent.w;

	
}	