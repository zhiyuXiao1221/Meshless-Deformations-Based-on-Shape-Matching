//#####################################################################
// Opengl object
// Copyright (c) (2018-), Bo Zhu, boolzhu@gmail.com
// This file is part of SLAX, whose distribution is governed by the LICENSE file.
//#####################################################################
#ifndef __OpenGLObject_h__
#define __OpenGLObject_h__
#include <memory>
#include <glad.h>
#include "OpenGLCommon.h"

////Forward declaration
class OpenGLShaderProgram;
class OpenGLColorMapper;
class OpenGLTexture;

class OpenGLData
{
public:
	std::string name;							////file name
	std::string key;							////key to toggle draw
	ColorType color_type=ColorType::Jet;		////ColorType::Jet, ColorType::Hot, ColorType::Den, or ColorType::Mat
	StoreType store_type=StoreType::Node;		////StoreType::Node, StoreType::Cell, or StoreType::None
	ArrayF<GLfloat,4> color;

	OpenGLData(std::string _name,std::string _key="",ColorType _color_idx=ColorType::Jet,StoreType _storage_type=StoreType::Node)
		:name(_name),key(_key),color_type(_color_idx),store_type(_storage_type){color={1.f};}
};

using Sym_Mat3_Eig=Eigen::SelfAdjointEigenSolver<Matrix3>;
using Sym_Mat2_Eig=Eigen::SelfAdjointEigenSolver<Matrix2>;

class OpenGLObject
{
public:
	std::string name;
	bool visible=true;
	bool interactive=false;
	OpenGLColor color;

	int dim=3;
	std::string output_dir;
	std::string frame_info_string;

	Array<GLfloat> opengl_vertices;
	Array<GLuint> opengl_elements;
	int vtx_size=0;
	int ele_size=0;
	real scale=(real)1;
	real line_norm=(real)1;
	GLfloat alpha=(GLfloat)1;
	GLfloat line_width=(GLfloat)1;
	bool normalize=false;
	bool draw_dis=true;
	bool use_vtx_color=true;
	bool use_vtx_normal = true;
	bool recomp_vtx_normal = false;
	bool use_vtx_tangent=false;
	bool recomp_vtx_tangent = false;

	bool use_vtx_tex=false;
	std::string tex_name="";

	Array<std::shared_ptr<OpenGLShaderProgram> > shader_programs;

	struct TextureBinding {
		std::string binding_name;
		std::shared_ptr<OpenGLTexture> texture;
	};
	Array<TextureBinding> textures;

	PolygonMode polygon_mode=PolygonMode::Fill;
	ShadingMode shading_mode=ShadingMode::None;

	GLuint vbo=0,vao=0,ebo=0;
	bool initialized=false;
	
	Array<OpenGLData> data;
	int data_idx=0;
	bool data_refreshed=false;
	
	Array<std::function<void(void)>*> callbacks;
	int cb_block_size=4;
	
	Array<OpenGLObject*> binded_objects;
	bool verbose=false;
	bool use_preprocess=false;
	bool use_depth_fbo=false;

	bool use_env=false;
	std::string env_name;

	OpenGLObject();

	virtual void Initialize(){Initialize_OpenGL_Buffers();initialized=true;}
	virtual void Initialize_OpenGL_Buffers();
	virtual void Update_Data_To_Render(){}
	virtual void Update_Color_Mapper(){}
	virtual void Preprocess(){}
    virtual void Display() const {}
	virtual void Refresh(const int frame){}
	virtual void Timer_Refresh(){}
	virtual std::string Get_String(){return frame_info_string;}
	virtual void Add_Shader_Program(std::shared_ptr<OpenGLShaderProgram> shader_program) { shader_programs.push_back(shader_program); }
	virtual void Add_Texture(std::string binding_name, std::shared_ptr<OpenGLTexture> texture) { textures.push_back(TextureBinding{ binding_name, texture }); }
	virtual void Set_Shader_Parameters(){}
	virtual void Set_Texture(const std::string& _tex_name){tex_name=_tex_name;use_vtx_tex=true;}
	virtual void Set_Polygon_Mode(const PolygonMode _mode){polygon_mode=_mode;}
	virtual void Set_Shading_Mode(const ShadingMode _mode){shading_mode=_mode;}
	virtual void Set_Env_Mapping(const std::string& _env_name);
	virtual void Set_Data_Refreshed(const bool _refreshed=true){data_refreshed=_refreshed;}

	////User interaction callbacks
	virtual bool Mouse_Drag(int x,int y,int w,int h){return false;}
	virtual bool Mouse_Click(int left,int right,int mid,int x,int y,int w,int h){return false;}
	virtual bool Mouse_Scroll(int wheel,int direction,int x,int y){return false;}
	virtual bool Keyboard(unsigned char key,int x,int y,bool down){return false;}
	virtual bool Keyboard_Special(int key,int x,int y,bool down){return false;}

	virtual bool Update_Data_To_Render_Pre();
	virtual void Update_Data_To_Render_Post();
	virtual void Normalize_Data(){}

	////Set rendering options
	virtual void Update_Polygon_Mode() const;
	virtual void Clear_OpenGL_Arrays();
	virtual void Set_OpenGL_Vertices();
	void Set_OpenGL_Vertices(Array<GLfloat>& opengl_vertices,int& vtx_size);
	virtual void Set_OpenGL_Elements();
	void Set_OpenGL_Elements(Array<GLuint>& opengl_elemetns,int& ele_size);
	virtual void Set_OpenGL_Vertex_Attribute(const GLuint idx,const GLint element_size,const GLuint stride_size=0,GLuint start_idx=0);
	virtual void Set_Color(const OpenGLColor& c){color=c;}

	virtual bool Use_Alpha_Blend() const;
	virtual void Enable_Alpha_Blend() const;
	virtual void Disable_Alpha_Blend() const;

	////Set mapping parameters from data to scalars
	void Update_Scalar_Range(const Array<Matrix3>& array,real& v_min,real& v_max) const;
	template<class T_ARRAY> void Update_Scalar_Range(const T_ARRAY& array,real& v_min,real& v_max) const
	{
		v_min=std::numeric_limits<real>::max();v_max=std::numeric_limits<real>::min();
		for(auto& v:array){real s=Scalar(v);if(s<v_min)v_min=s;else if(s>v_max)v_max=s;}
	}
	template<class T_VAL> real Normalized_Scale(Array<T_VAL>& array,real norm) const
	{real v_min,v_max;Update_Scalar_Range(array,v_min,v_max);if(v_max!=(real)0)return norm/v_max;else return (real)1;}
	real Scalar(const real& v) const;
	real Scalar(const Vector3& v) const;
	real Scalar(const Matrix3& v) const;

	////Callbacks
	virtual void Initialize_Callbacks(){}

	virtual void Toggle_Draw();
    Define_Function_Object(OpenGLObject,Toggle_Draw);

	virtual void Toggle_Normalize();
    Define_Function_Object(OpenGLObject,Toggle_Normalize);

	virtual void Toggle_Draw_Dis();
    Define_Function_Object(OpenGLObject,Toggle_Draw_Dis);

	virtual void Toggle_Increase_Scale();
	Define_Function_Object(OpenGLObject,Toggle_Increase_Scale);

	virtual void Toggle_Decrease_Scale();
	Define_Function_Object(OpenGLObject,Toggle_Decrease_Scale);

	////Additional helper functions
	////IO helper functions
	static std::string Object_File_Name(const std::string& output_dir,const int frame,const std::string& object_name);
	static bool Object_File_Exists(const std::string& output_dir,const int frame,const std::string& object_name);
};
#endif
