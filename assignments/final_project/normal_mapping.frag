/*This is your fragment shader for texture and normal mapping*/

#version 330 core
# define PI 3.1415926535898
/*default camera matrices. do not modify.*/
layout (std140) uniform camera
{
	mat4 projection;	/*camera's projection matrix*/
	mat4 view;			/*camera's view matrix*/
	mat4 pvm;			/*camera's projection*view*model matrix*/
	mat4 ortho;			/*camera's ortho projection matrix*/
	vec4 position;		/*camera's position in world space*/
};

/*uniform variables*/
uniform float iTime;					////time
uniform sampler2D tex_albedo;			////texture color
uniform sampler2D tex_normal;			////texture normal

/*input variables*/
//// TODO: declare your input variables
in vec3 vtx_pos;
in vec4 vtx_color;
in vec2 vtx_uv;
in vec3 vtx_normal;
in vec3 vtx_tangent;
in vec3 vtx_binormal;
/*output variables*/
out vec4 frag_color;

/*This part is the same as your previous assignment. Here we provide a default parameter set for the hard-coded lighting environment. Feel free to change them.*/
//const vec3 LightPosition = vec3(3, 1, 3);
const vec3 LightPosition = vec3(110, -70, -10);
const vec3 LightIntensity = 1.7*vec3(1.0, 1.0, 1.0);
const vec3 ka = 0.08*vec3(1., 1., 1.);
const vec3 kd = 0.7*vec3(1., 1., 1.);
const vec3 ks = vec3(3.);
const float n = 400.0;

void main()							
{		
	bool use_normal_mapping = false;	////TODO: set this flag to be true when you move from Step 2 to Step 3

	if(!use_normal_mapping){
		//// TODO (Step 1 and 2): texture with shading
		////Here are some useful hints:
		////Step 1.0: load the texture color from tex_albedo and then set it to frag_color
		////Step 2.0: use the loaded texture color as the local material color and multiply the local color with the Lambertian shading model you implemented previously to render a textured and shaded sphere.
		////The way to read a texture is to call texture(texture_name,uv). It will return a vec4.
		//Texture
		vec2 uv=vtx_uv;
		//or calculae uv in the fragment shader
		float r = sqrt(pow(vtx_pos.x,2)+pow(vtx_pos.y,2)+pow(vtx_pos.z,2));
		float u = atan(vtx_pos.y,vtx_pos.x+1E-18)/(2*PI);
		float v = acos(vtx_pos.z/r)/(PI);
		uv = vec2(u,v);
		vec4 albedo=texture(tex_albedo,uv);
		albedo = vec4(247.0/255.0,158.0/255.0,170.0/255.0,1);
		//Shading
		vec3 lightDir = normalize(vtx_pos.xyz-LightPosition);
		vec3 ambient= LightIntensity*ka*albedo.xyz;
		vec3 diffuse = LightIntensity*kd*albedo.xyz*max(0,dot(vtx_normal,lightDir));
		frag_color = vec4(ambient+diffuse,1.0);
	}
	else{
		//// TODO (Step 3): texture with normal mapping
		////Here are some useful hints:
		////Step 3.0: load the texture color from tex_albedo
		//or calculae uv in the fragment shader
		float r = sqrt(pow(vtx_pos.x,2)+pow(vtx_pos.y,2)+pow(vtx_pos.z,2));
		float u = atan(vtx_pos.y,vtx_pos.x+1E-18)/(2*PI);
		float v = acos(vtx_pos.z/r)/(PI);
		vec2 uv=vtx_uv;
		uv = vec2(u,v);
		vec4 albedo=texture(tex_albedo,uv);
		////Step 3.1: load the texture normal from tex_normal, and remap each component from [0, 1] to [-1, 1] (notice that currently the loaded normal is in the local tangent space)
		vec3 normal = texture(tex_normal, uv).rgb;
		normal = normal * 2.0 - 1.0;
		////Step 3.2: calculate the TBN matrix using the vertex normal and tangent
		mat3 TBN = mat3(vtx_tangent,vtx_binormal,vtx_normal);
		////Step 3.3: transform the texture normal from the local tangent space to the global world space
		vec3 normal_world = normalize(TBN * normal); 
		////Step 3.4 and following: use the transformed normal and the loaded texture color to conduct the further lighting calculation
		vec3 lightDir = normalize(vtx_pos.xyz-LightPosition);
		vec3 ambient= LightIntensity*ka*albedo.xyz;
		vec3 diffuse = LightIntensity*kd*albedo.xyz*max(0,dot(normal_world,lightDir));
		frag_color = vec4(ambient+diffuse,1.0);
	}
}	