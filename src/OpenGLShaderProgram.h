//#####################################################################
// Opengl shader program
// Copyright (c) (2018-), Bo Zhu, boolzhu@gmail.com
// This file is part of SLAX, whose distribution is governed by the LICENSE file.
//#####################################################################
#ifndef __OpenGLShaderProgram_H__
#define __OpenGLShaderProgram_H__
#include <string>
#include <glad.h>
#include "glm.hpp"
#include "Common.h"
#include "das_file_watcher.h"

class Material
{public:
	glm::vec4 mat_amb=glm::vec4(1.f);
	glm::vec4 mat_dif=glm::vec4(1.f,1.f,1.f,1.f);
	glm::vec4 mat_spec=glm::vec4(1.f);
	glm::vec4 mat_shinness=glm::vec4(32.f,0.f,0.f,0.f);
};

////shader object
class OpenGLShaderProgram
{
public:
	std::string name;
	std::string vtx_shader;
	std::string frg_shader;
    std::string geo_shader;

	void Initialize(const std::string& vtx_shader_input, const std::string& frg_shader_input);
	bool Reload(const std::string& vtx_shader_input,const std::string& frg_shader_input);
    void Initialize(const std::string& vtx_shader_input,const std::string& frg_shader_input,
        const std::string& _geo_shader_input,GLenum _geo_input_type,GLenum _geo_output_type,int _max_geo_vtx_output=4);

	template<class T_VAL> void Set_Uniform(const std::string& name,T_VAL value);
	void Set_Uniform(const std::string& name,GLint value);
	void Set_Uniform(const std::string& name,GLfloat value);
	void Set_Uniform(const std::string& name,Vector2f value);
	void Set_Uniform(const std::string& name,Vector3f value);
	void Set_Uniform(const std::string& name,Vector4f value);
	void Set_Uniform(const std::string& name,glm::vec2 value);
	void Set_Uniform(const std::string& name,glm::vec3 value);
	void Set_Uniform(const std::string& name,glm::vec4 value);
	void Set_Uniform_Array(const std::string& name,GLsizei count,const GLint* value);
	void Set_Uniform_Array(const std::string& name,GLsizei count,const GLfloat* value);
	void Set_Uniform_Matrix4f(const std::string& name,const GLfloat* value);
	void Set_Uniform_Vec4f(const std::string& name,const GLfloat* value);
	void Set_Uniform_Mat(const Material* mat);
	void Bind_Uniform_Block(const std::string& name,const GLuint binding_point);
	void Bind_Texture2D(const std::string& name,GLuint tex_id,GLint tex_unit);
	void Begin();
	void End();
	bool Compile();

protected:
	GLuint vtx_id;	////vertex shader index
	GLuint frg_id;	////fragment shader index
    GLuint geo_id;  ////geometry shader index
	GLuint prg_id;	////program object index
	bool compiled;
    bool use_geo;
    GLenum geo_input_type,geo_output_type;
	int max_geo_vtx_output;
};

class OpenGLShaderLibrary
{public:
	static OpenGLShaderLibrary* Instance();
	static std::shared_ptr<OpenGLShaderProgram> Get_Shader(const std::string& name);
	std::shared_ptr<OpenGLShaderProgram> Get(const std::string& name);
	void Update_Shaders();
	void Add_Shader_From_File(const std::string& vtx_shader_file, const std::string& frg_shader_file, const std::string& name);

	// Mimic shader toy
	void Create_Screen_Shader(const std::string& drawFunc, const std::string& name);

protected:
	Hashtable<std::string,std::shared_ptr<OpenGLShaderProgram> > shader_hashtable;
	Hashtable<std::string,std::string> shader_header_hashtable;
	
	struct ShaderFile {
		std::string   vtx_file, frg_file;
		FileWatchInfo vtx_info, frg_info;
	};
	Hashtable<std::string, ShaderFile> shader_file_hashtable;

	OpenGLShaderLibrary();
	void Initialize_Shaders();
	void Initialize_Headers();
	std::string Parse(const std::string& shader) const;

public:
	bool Load_Shader_From_File(const ShaderFile& file, std::shared_ptr<OpenGLShaderProgram> shader);
	void Add_Shader(const std::string& vtx_shader, const std::string& frg_shader, const std::string& name);
};

#endif
