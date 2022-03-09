//#####################################################################
// Opengl uniform buffer object
// Copyright (c) (2018-), Bo Zhu, boolzhu@gmail.com
// This file is part of SLAX, whose distribution is governed by the LICENSE file.
//#####################################################################
#include <iostream>
#include "OpenGLWindow.h"
#include "OpenGLBufferObjects.h"
#ifdef USE_STB
#include "StbImage.h"
#endif

namespace OpenGLFbos{
void OpenGLFboInstance::Initialize(const std::string& _name,GLuint _width,GLuint _height)
{
	name=_name;
	glGenFramebuffers(1,&buffer_index);
	Resize(width,height);
}

void OpenGLFboInstance::Resize(GLuint _width,GLuint _height)
{
	if(_width==width&&_height==height||_width==0||_height==0)return;
	width=_width;height=_height;

	switch(type){
	case Color:case Position:Bind_Texture_Color();break;
	case Depth:Bind_Texture_Depth();break;}
}

void OpenGLFboInstance::Clear()
{
	Bind();
	glClearColor(0,0,0,0);
	glClearDepth(1.);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	Unbind();
}

void OpenGLFboInstance::Resize_To_Window()
{
	if(width!=(GLuint)Win_Width()||height!=(GLuint)Win_Height())Resize((GLuint)Win_Width(),(GLuint)Win_Height());
}

void OpenGLFboInstance::Write_To_File(const std::string& file_name)
{
#ifndef USE_STB
	std::cerr<<"Error: [OpenGLFbo] stb is required for image io"<<std::endl;
#else
	glBindFramebuffer(GL_FRAMEBUFFER,buffer_index);
	switch(type){
	case Color:{
		int num_pixel=width*height;int num_comp=3;if(num_pixel==0)return;
		GLubyte* pixels=new GLubyte[num_comp*num_pixel];
		GLubyte* pixels_flipped_y=new GLubyte[num_comp*num_pixel];
		glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,pixels);
		for(int i=0;i<height;i++){int offset=width*num_comp*(height-i-1);
			std::memcpy(pixels_flipped_y+offset,pixels+width*num_comp*i,width*num_comp);}
		std::stringstream ss;ss<<file_name<<".png";
		Stb::Write_Png(ss.str().c_str(),width,height,num_comp,pixels_flipped_y,0);
		delete pixels;delete pixels_flipped_y;		
	}break;
	case Depth:{
		int num_pixel=width*height;int num_comp=3;if(num_pixel==0)return;
		GLfloat* pixels=new GLfloat[num_pixel];
		GLubyte* pixels_flipped_y=new GLubyte[num_comp*num_pixel];
		glReadPixels(0,0,width,height,GL_DEPTH_COMPONENT,GL_FLOAT,pixels);
		for(int ii=0;ii<height;ii++){int i=height-ii-1;for(int j=0;j<width;j++){
			float depth=pixels[width*ii+j];
			if(use_linearize_plane)depth=Linearize_Depth(depth,near_plane,far_plane)/far_plane;
			int p=(int)(depth*255.);
			for(int k=0;k<num_comp;k++)pixels_flipped_y[width*num_comp*i+j*num_comp+k]=(GLubyte)p;}}
		std::stringstream ss;ss<<file_name<<".png";
		Stb::Write_Png(ss.str().c_str(),width,height,num_comp,pixels_flipped_y,0);
		delete pixels;delete pixels_flipped_y;			
	}break;}
	glBindFramebuffer(GL_FRAMEBUFFER,0);
#endif
}

void OpenGLFboInstance::Bind_As_Texture(GLuint idx){glActiveTexture(GL_TEXTURE0+idx);glBindTexture(GL_TEXTURE_2D,tex_index);}
void OpenGLFboInstance::Set_Near_And_Far_Plane(float _near,float _far){near_plane=_near;far_plane=_far;use_linearize_plane=true;}
void OpenGLFboInstance::Bind(){glBindFramebuffer(GL_FRAMEBUFFER,buffer_index);}
void OpenGLFboInstance::Unbind(){glBindFramebuffer(GL_FRAMEBUFFER,0);}

GLuint OpenGLFboInstance::Generate_Attachment_Texture(const AttachmentType& att_type,GLuint width,GLuint height)
{
	GLenum gl_att_type;GLenum gl_att_type_int;
	switch(att_type){
	case Color:{gl_att_type=gl_att_type_int=GL_RGB;}break;
	case Position:{gl_att_type=GL_RGB16F;gl_att_type_int=GL_RGB;}break;
	case Depth:{gl_att_type=gl_att_type_int=GL_DEPTH_COMPONENT;}break;
	case Stencil:{gl_att_type=GL_DEPTH24_STENCIL8;gl_att_type_int=GL_DEPTH_STENCIL;}break;}
	GLuint tex_id;glGenTextures(1,&tex_id);glBindTexture(GL_TEXTURE_2D,tex_id);
	switch(att_type){
	case Color:glTexImage2D(GL_TEXTURE_2D,0,gl_att_type,width,height,0,gl_att_type_int,GL_UNSIGNED_BYTE,0);break;
	case Position:glTexImage2D(GL_TEXTURE_2D,0,gl_att_type,width,height,0,gl_att_type_int,GL_FLOAT,0);break;
	case Depth:glTexImage2D(GL_TEXTURE_2D,0,gl_att_type,width,height,0,gl_att_type,GL_FLOAT,0);break;
	case Stencil:glTexImage2D(GL_TEXTURE_2D,0,gl_att_type,width,height,0,gl_att_type_int,GL_UNSIGNED_INT_24_8,0);break;}
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D,0);
	return tex_id;
}

void OpenGLFboInstance::Bind_Texture_Color()
{
	glBindFramebuffer(GL_FRAMEBUFFER,buffer_index);
	if(tex_index!=0)glDeleteTextures(1,&tex_index);
	tex_index=Generate_Attachment_Texture(type,width,height);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,tex_index,0);
	if(rbo_index!=0)glDeleteRenderbuffers(1,&rbo_index);
	glGenRenderbuffers(1,&rbo_index);glBindRenderbuffer(GL_RENDERBUFFER,rbo_index);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8,width,height);
	glBindRenderbuffer(GL_RENDERBUFFER,0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,rbo_index);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)std::cerr<<"Error: [OpenGLFboInstance] framebuffer not complete"<<std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER,0);	
}

void OpenGLFboInstance::Bind_Texture_Depth()
{
	glBindFramebuffer(GL_FRAMEBUFFER,buffer_index);
	if(tex_index!=0)glDeleteTextures(1,&tex_index);
	tex_index=Generate_Attachment_Texture(Depth,width,height);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,tex_index,0);
	glBindFramebuffer(GL_FRAMEBUFFER,0);		
}

float OpenGLFboInstance::Linearize_Depth(float depth,float near_plane,float far_plane)
{float z=depth*2.f-1.f;/*Back to NDC*/return (2.f*near_plane*far_plane)/(far_plane+near_plane-z*(far_plane-near_plane));}

Fbo_Library* Fbo_Library::Instance(){static Fbo_Library instance;return &instance;}

std::shared_ptr<OpenGLFbo> Fbo_Library::Get(const std::string& name,const int init_type)
{
	auto search=fbo_hashtable.find(name);
	if(search!=fbo_hashtable.end())return search->second;
	else return Lazy_Initialize_Fbo(name,init_type);
}

std::shared_ptr<OpenGLFbo> Fbo_Library::Lazy_Initialize_Fbo(const std::string& name,const int type)
{
	std::shared_ptr<OpenGLFbo> ptr=nullptr;if(name=="")return ptr;
	OpenGLFboInstance* fbo=new OpenGLFboInstance((OpenGLFboInstance::AttachmentType)type);
	fbo->Initialize(name);ptr.reset(fbo);
	fbo_hashtable.insert(std::make_pair(fbo->name,ptr));return ptr;
}

std::shared_ptr<OpenGLFbo> Get_Fbo(const std::string& name,const int init_type)	////0-color,1-depth
{return Fbo_Library::Instance()->Get(name,init_type);}

std::shared_ptr<OpenGLFbo> Get_Depth_Fbo(const std::string& name)	////0-color,1-depth
{return Fbo_Library::Instance()->Get(name,/*depth*/1);}

OpenGLFboInstance* Get_Fbo_Instance(const std::string& name,const int init_type)
{return dynamic_cast<OpenGLFboInstance*>(Get_Fbo(name,init_type).get());}

void Bind_Fbo(const std::string& name,const int init_type)
{auto fbo=Get_Fbo(name,init_type);if(fbo==nullptr)return;glBindFramebuffer(GL_FRAMEBUFFER,fbo->buffer_index);}

std::shared_ptr<OpenGLFbo> Get_And_Bind_Fbo(const std::string& name,const int init_type)
{auto fbo=Get_Fbo(name,init_type);if(fbo!=nullptr)glBindFramebuffer(GL_FRAMEBUFFER,fbo->buffer_index);return fbo;}

void Unbind_Fbo(){glBindFramebuffer(GL_FRAMEBUFFER,0);}

void Clear_Fbo(const std::string& name,const int init_type)
{
	auto fbo=Get_Fbo("depth",init_type);
	if(fbo==nullptr||fbo->width==0||fbo->height==0)return;
	fbo->Clear();
}
};

namespace OpenGLUbos{
//////////////////////////////////////////////////////////////////////////
////OpenGL UBO classes and library

void Bind_Shader_Ubo_Headers(Hashtable<std::string,std::string>& shader_header_hashtable)
{
	shader_header_hashtable.insert(std::make_pair("camera",camera));
	//shader_header_hashtable.insert(std::make_pair("lighting",lighting));
	shader_header_hashtable.insert(std::make_pair("lights",lights));
}

template<class T_UBO> void OpenGLUboInstance<T_UBO>::Initialize(const std::string& _uniform_block_name,GLuint _binding_point,GLuint _ubo,GLint _block_offset)
{
	name=_uniform_block_name;binding_point=_binding_point;
	if(_ubo!=0){buffer_index=_ubo;block_offset=_block_offset;}
	else{
		buffer_size=sizeof(T_UBO);
		glGenBuffers(1,&buffer_index);glBindBuffer(GL_UNIFORM_BUFFER,buffer_index);
		glBufferData(GL_UNIFORM_BUFFER,buffer_size,0,GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER,0);	
		block_offset=0;block_size=buffer_size;}
	Bind_Block(binding_point,block_offset,block_size);
	std::cout<<"[OpenGLUboInstance] Initialized buffer_index: "<<buffer_index<<", name: "<<name<<", buffer_size: "<<buffer_size<<", binding_point: "<<binding_point<<
		", block_offset: "<<block_offset<<", block_size: "<<block_size<<std::endl;
}

template<class T_UBO> void OpenGLUboInstance<T_UBO>::Set_Block_Attributes(){Set_Block_Attributes_Helper(object,block_offset);}

template<class T_UBO> void OpenGLUboInstance<T_UBO>::Bind_Block(){Bind_Block(binding_point,block_offset,block_size);}

template<class T_UBO> void OpenGLUboInstance<T_UBO>::Bind_Block(GLuint binding_point,GLint block_offset,size_type block_size)
{glBindBufferRange(GL_UNIFORM_BUFFER,binding_point,buffer_index,block_offset,block_size);}

template<class T_UBO> void OpenGLUboInstance<T_UBO>::Set_Block_Attributes_Helper(const Camera& obj,GLint block_offset)
{
	GLint offset=block_offset;
	offset=Set_Block_Attribute(obj.projection,offset);
	offset=Set_Block_Attribute(obj.view,offset);
	offset=Set_Block_Attribute(obj.pvm,offset);
	offset=Set_Block_Attribute(obj.ortho,offset);
	offset=Set_Block_Attribute(obj.position,offset);
}

template<class T_UBO> void OpenGLUboInstance<T_UBO>::Set_Block_Attributes_Helper(const Lights& obj,GLint block_offset)
{
	GLint offset=block_offset;
	offset=Set_Block_Attribute(obj.amb,offset);
	offset=Set_Block_Attribute(obj.lt_att,offset);
	for(int i=0;i<2;i++){
		offset=Set_Block_Attribute(obj.lt[i].att,offset);
		offset=Set_Block_Attribute(obj.lt[i].pos,offset);
		offset=Set_Block_Attribute(obj.lt[i].dir,offset);
		offset=Set_Block_Attribute(obj.lt[i].amb,offset);
		offset=Set_Block_Attribute(obj.lt[i].dif,offset);
		offset=Set_Block_Attribute(obj.lt[i].spec,offset);
		offset=Set_Block_Attribute(obj.lt[i].atten,offset);
		offset=Set_Block_Attribute(obj.lt[i].r,offset);}
}

template class OpenGLUboInstance<Camera>;
template class OpenGLUboInstance<Lights>;

//////////////////////////////////////////////////////////////////////////
////OpenGL UBO classes and library

std::shared_ptr<OpenGLUbo> Ubo_Library::Get(const std::string& name)
{
	auto search=ubo_hashtable.find(name);
	if(search!=ubo_hashtable.end())return search->second;
	else return std::shared_ptr<OpenGLUbo>(nullptr);
}

GLuint Ubo_Library::Get_Binding_Point(const std::string& name)
{std::shared_ptr<OpenGLUbo> ptr=Get(name);if(ptr!=nullptr)return ptr->binding_point;else return GL_INVALID_INDEX;}

Ubo_Library::Ubo_Library(){Initialize_Ubos();}

void Ubo_Library::Initialize_Ubos()
{using namespace OpenGLUbos;
	std::cout<<"Initialize ubo library"<<std::endl;
	int binding_point=0;
	////camera
	{OpenGLUboInstance<Camera>* ubo=new OpenGLUboInstance<Camera>();
	ubo->Initialize("camera",binding_point++);ubo->Set_Block_Attributes();
	ubo_hashtable.insert(std::make_pair(ubo->name,std::shared_ptr<OpenGLUbo>(ubo)));}
	////lights
	{OpenGLUboInstance<Lights>* ubo=new OpenGLUboInstance<Lights>();
	ubo->Initialize("lights",binding_point++);ubo->Set_Block_Attributes();
	ubo_hashtable.insert(std::make_pair(ubo->name,std::shared_ptr<OpenGLUbo>(ubo)));}
}

//////////////////////////////////////////////////////////////////////////
////UBO functions

void Initialize_Ubos()
{Ubo_Library::Instance()->Get("");}

std::shared_ptr<OpenGLUbo> Get_Ubo(const std::string& name)
{return Ubo_Library::Instance()->Get(name);}

GLuint Get_Ubo_Binding_Point(const std::string& name)
{return Ubo_Library::Instance()->Get_Binding_Point(name);}

bool Bind_Uniform_Block_To_Ubo(std::shared_ptr<OpenGLShaderProgram>& shader,const std::string& ubo_name)	////assuming uniform block name=ubo name
{GLuint binding_point=Get_Ubo_Binding_Point(ubo_name);if(binding_point==GL_INVALID_INDEX)return false;
shader->Bind_Uniform_Block(ubo_name,binding_point);return true;}

////Camera
OpenGLUboInstance<Camera>* Get_Camera_Ubo()
{
	std::shared_ptr<OpenGLUbo> ubo=Get_Ubo("camera");
	OpenGLUboInstance<Camera>* camera_ubo=dynamic_cast<OpenGLUboInstance<Camera>* >(ubo.get());return camera_ubo;	
}

Camera* Get_Camera()
{
	auto* camera_ubo=Get_Camera_Ubo();
	if(camera_ubo!=nullptr)return &camera_ubo->object;return nullptr;
}

Vector3 Get_Camera_Pos()
{
	auto camera=Get_Camera_Ubo();
	return Vector3(camera->object.position[0],camera->object.position[1],camera->object.position[2]);
}

////Lighting
OpenGLUboInstance<Lights>* Get_Lights_Ubo()
{
	std::shared_ptr<OpenGLUbo> ubo=Get_Ubo("lights");
	OpenGLUboInstance<Lights>* lights_ubo=dynamic_cast<OpenGLUboInstance<Lights>* >(ubo.get());return lights_ubo;
}

Lights* Get_Lights()
{
	auto* lights_ubo=Get_Lights_Ubo();
	if(lights_ubo!=nullptr)return &lights_ubo->object;return nullptr;
}

void Update_Lights_Ubo()
{auto* lights_ubo=Get_Lights_Ubo();lights_ubo->Set_Block_Attributes();}

Light* Get_Light(const int i)
{
	Lights* lights=Get_Lights();if(lights==nullptr)return nullptr;
	Light* light=lights->Get(i);return light;
}

void Clear_Lights()
{
	Lights* lights=Get_Lights();if(lights==nullptr)return;
	int& i=lights->Light_Num();i=0;
}

Light* Add_Directional_Light(const glm::vec3& dir)
{
	Lights* lights=Get_Lights();if(lights==nullptr)return nullptr;

	int& i=lights->Light_Num();Light* lt=&lights->lt[i];i++;
	lt->Initialize();
	lt->Set_Directional();
	lt->dir=glm::vec4(dir,1.f);
	lt->pos=glm::vec4(-dir*2.f,1.f);
	Update_Lights_Ubo();return lt;
}

Light* Add_Point_Light(const glm::vec3& pos)
{
	Lights* lights=Get_Lights();if(lights==nullptr)return nullptr;

	int& i=lights->Light_Num();Light* lt=&lights->lt[i];i++;
	lt->Initialize();
	lt->Set_Point();
	lt->pos=glm::vec4(pos,1.f);
	
	Update_Lights_Ubo();return lt;
}

Light* Add_Spot_Light(const glm::vec3& pos,glm::vec3& dir)
{
	Lights* lights=Get_Lights();if(lights==nullptr)return nullptr;

	int& i=lights->Light_Num();Light* lt=&lights->lt[i];i++;
	lt->Initialize();
	lt->Set_Spot();
	lt->pos=glm::vec4(pos,1.f);
	lt->dir=glm::vec4(dir,1.f);

	Update_Lights_Ubo();return lt;
}

void Set_Ambient(const glm::vec4& amb)
{
	Lights* lights=Get_Lights();if(lights==nullptr)return;
	lights->amb=amb;
}
};
