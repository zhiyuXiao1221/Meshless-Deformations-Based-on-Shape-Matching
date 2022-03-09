//#####################################################################
// Opengl particles
// Copyright (c) (2018-), Bo Zhu, boolzhu@gmail.com
// This file is part of SLAX, whose distribution is governed by the LICENSE file.
//#####################################################################
#ifndef __OpenGLParticles_h__
#define __OpenGLParticles_h__
#include "Common.h"
#include "Particles.h"
#include "File.h"
#include "OpenGLObject.h"
#include "OpenGLVectors.h"
#include "OpenGLShaderProgram.h"

class OpenGLPoints : public OpenGLObject
{typedef OpenGLObject Base;
public:
    const Array<Vector3>* points=nullptr;
	const Array<real>* colors=nullptr;

	GLfloat point_size=6.f;
	bool use_varying_point_size=false;
	Array<GLfloat> varying_point_size;

	OpenGLPoints(){color=OpenGLColor::Red();name="points";}

	virtual void Initialize()
	{
		Base::Initialize();
		Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("psize_ucolor"));	
	}

	void Set_Data_Pointers(const Array<Vector3>* _points,const Array<real>* _colors=nullptr){points=_points;colors=_colors;}
	
	virtual void Update_Data_To_Render()
	{
		if(!initialized)Initialize();

		use_vtx_color=(colors!=nullptr&&shading_mode!=ShadingMode::None);
		GLuint stride_size=4+(use_vtx_color?4:0);
		Clear_OpenGL_Arrays();
		GLfloat placeholder=(GLfloat)0;
		for(size_type i=0;i<(*points).size();i++){
			OpenGL_Vertex((*points)[i],opengl_vertices);		////position, 3 floats
			if(use_varying_point_size) OpenGL_Vertex(varying_point_size[i],opengl_vertices);
			else OpenGL_Vertex(placeholder,opengl_vertices);	////placeholder, 1 float
			if(use_vtx_color){
				//OpenGLColor color=color_mapper->Color((*colors)[i]);	////TOFIX
				OpenGL_Color4(color.rgba,opengl_vertices);}}	////color, 4 floats
		
		Set_OpenGL_Vertices();
		int idx=0;{Set_OpenGL_Vertex_Attribute(0,4,stride_size,0);idx++;}	////position
		if(use_vtx_color){Set_OpenGL_Vertex_Attribute(idx,4,stride_size,idx*4);idx++;}	////color
		Clear_OpenGL_Arrays();
	}

	virtual void Display() const
    {
    	using namespace OpenGLUbos;
		if(!visible)return;
		Update_Polygon_Mode();

		switch(shading_mode){
		case ShadingMode::None:{
			std::shared_ptr<OpenGLShaderProgram> shader=shader_programs[0];
			shader->Begin();
			Bind_Uniform_Block_To_Ubo(shader,"camera");
			shader->Set_Uniform_Vec4f("color",color.rgba);
			glEnable(GL_PROGRAM_POINT_SIZE);
			shader->Set_Uniform("point_size",point_size);
			glBindVertexArray(vao);
			glDrawArrays(GL_POINTS,0,vtx_size/4);
			shader->End();
		}break;}
    }
};

template<class T_PARTICLE=Particles<3> >
class OpenGLParticles : public OpenGLObject
{typedef OpenGLObject Base;
public:
    T_PARTICLE particles;
	
	OpenGLPoints opengl_points;
	Array<OpenGLVectors> opengl_vector_fields;

	OpenGLParticles(){color=OpenGLColor::Red();name="particles";}

	virtual void Initialize()
	{
		Base::Initialize();
		Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("particle"));
		T_PARTICLE* particles=nullptr;Initialize_Vector_Fields_Helper(particles);
	}

	void Set_Data_Pointers()
	{
		Set_Data_Pointers_Helper(particles);
	}

	virtual void Update_Data_To_Render()
	{
		if(!Update_Data_To_Render_Pre())return;

		Set_Data_Pointers();
		opengl_points.Update_Data_To_Render();
		for(auto& vf:opengl_vector_fields){vf.Update_Data_To_Render();}
		
		Update_Data_To_Render_Post();
	}

	virtual void Refresh(const int frame)
	{
		std::string file_name=output_dir+"/"+std::to_string(frame)+"/"+name;
		if(File::File_Exists(file_name)){
			File::Read_Binary_From_File(file_name,particles);
			Set_Data_Refreshed();}
	}

	virtual void Display() const
    {
		if(!visible)return;
		opengl_points.Display();
		for(auto& vf:opengl_vector_fields){vf.Display();}
    }

	virtual void Set_Color(const OpenGLColor& c){color=c;opengl_points.color=c;}

	virtual void Set_Shading_Mode(const ShadingMode _shading_mode)
	{shading_mode=_shading_mode;opengl_points.shading_mode=_shading_mode;}
	virtual void Set_Point_Size(const GLfloat point_size)
	{opengl_points.point_size=point_size;}
	virtual void Set_Point_Size(const Array<GLfloat>& point_size)
	{opengl_points.varying_point_size=point_size;opengl_points.use_varying_point_size=true;}

protected:
	void Initialize_Vector_Fields_Helper(Particles<3>* particles=nullptr)
	{opengl_vector_fields.resize(2);for(auto& vf:opengl_vector_fields){vf.Initialize();}}
	void Set_Data_Pointers_Helper(Particles<3>& particles)
	{opengl_points.Set_Data_Pointers(particles.X(),particles.C());
	opengl_vector_fields[0].Set_Data_Pointers(particles.X(),particles.V());opengl_vector_fields[0].visible=false;
	opengl_vector_fields[1].Set_Data_Pointers(particles.X(),particles.F());opengl_vector_fields[1].visible=false;}	////turn off visibility of vector fields by default
};
#endif
