//#####################################################################
// Opengl viewer
// Copyright (c) (2018-), Bo Zhu, boolzhu@gmail.com
// This file is part of SLAX, whose distribution is governed by the LICENSE file.
//#####################################################################
#ifndef __OpenGLViewer_h__
#define __OpenGLViewer_h__
#include <iostream>
#include <memory>
#include "Common.h"
#include "OpenGLObject.h"

////Forward declaration
class OpenGLWindow;
template<int d> class TriangleMesh;
template<int d> class TetrahedronMesh;
class OpenGLSegmentMesh;
class OpenGLTriangleMesh;
class OpenGLTetrahedronMesh;
class OpenGLUICommand;

class OpenGLViewer
{
public:
	std::string output_dir;
	std::string config_file_name;
	std::shared_ptr<OpenGLWindow> opengl_window=nullptr;
	int first_frame=0,last_frame=-1,frame=0;
	bool draw_bk=false;
	bool draw_axes=true;
	bool use_ui=true;
	bool play=false;
	bool use_2d_display=false;
	bool verbose=false;
	HashtableMultiValue<uchar,std::string> key_data_hashtable;
	std::shared_ptr<OpenGLUICommand> ui_command=nullptr;

	//////////////////////////////////////////////////////////////////////////
	////Initialization and run
	virtual void Initialize();
	virtual void Initialize_Data(){}
	virtual void Run();
	virtual void Initialize_Common_Data();
	virtual void Initialize_Camera(){}
	////Customized mouse func, the same as OpenGLObject
	virtual bool Mouse_Drag(int x,int y,int w,int h){return false;}
	virtual bool Mouse_Click(int left,int right,int mid,int x,int y,int w,int h){return false;}

	//////////////////////////////////////////////////////////////////////////
	////Animation
	void Update_Frame();

	//////////////////////////////////////////////////////////////////////////
	////UI
	virtual void Initialize_UI();
	virtual void Finish();
	virtual void Toggle_Command(const std::string cmd);

	//////////////////////////////////////////////////////////////////////////
	////Add objects
	template<class T_OBJECT> T_OBJECT* Add_Object(const std::string object_name,const Array<OpenGLData> data=Array<OpenGLData>())
	{
		T_OBJECT* opengl_object=nullptr;
		if(Initialize_From_File(output_dir,opengl_object,object_name,data,first_frame)){
			Add_OpenGL_Object(opengl_object);
			if(verbose){std::cout<<"Add opengl object: "<<object_name;}}
		return opengl_object;
	}

	template<class T_OBJECT> T_OBJECT* Add_Object(const char* object_name,const Array<OpenGLData> data=Array<OpenGLData>())
	{return Add_Object<T_OBJECT>(std::string(object_name),data);}

	template<class T_OBJECT> T_OBJECT* Add_Object(T_OBJECT* opengl_object,bool init=true,bool interactive=false)
	{
		if(init)opengl_object->Initialize();
		opengl_object->interactive=interactive;
		Add_OpenGL_Object(opengl_object);
		return opengl_object;
	}

	template<class T_OBJECT> T_OBJECT* Add_Object(bool init=true,bool interactive=false)
	{
		T_OBJECT* opengl_object=new T_OBJECT();
		return Add_Object<T_OBJECT>(opengl_object,init,interactive);
	}

	template<class T_OBJECT> T_OBJECT* Add_Interactive_Object(bool init=false)
	{
		return Add_Object<T_OBJECT>(init,true);
	}

	void Add_OpenGL_Object(OpenGLObject* object);

	template<class T_OBJECT> bool Initialize_From_File(const std::string& output_dir,T_OBJECT* & opengl_object,
		const std::string& object_name,const Array<OpenGLData>& data,const int frame=0)
	{
		if(OpenGLObject::Object_File_Exists(output_dir,frame,object_name)){
			opengl_object=new T_OBJECT();
			opengl_object->output_dir=output_dir;
			opengl_object->name=object_name;
			opengl_object->data=data;
			opengl_object->Refresh(frame);return true;}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	////Set viewer and object properties
	template<class T_OBJECT> void Set_Visibility(T_OBJECT* obj,const uchar key,const bool visible=true,const std::string draw="")
	{if(obj==nullptr)return;obj->visible=visible;Bind_Draw_Callback_Key(key,obj,draw);}
	
	template<class T_OBJECT> void Set_Color(T_OBJECT* obj,const OpenGLColor& color)
	{if(obj==nullptr)return;obj->Set_Color(color);}

	template<class T_OBJECT> void Set_Alpha(T_OBJECT* obj,const GLfloat alpha)
	{if(obj==nullptr)return;obj->alpha=alpha;}

	template<class T_OBJECT> void Set_Data_Color(T_OBJECT* obj,const int data_idx,const OpenGLColor& color)
	{if(obj==nullptr||data_idx>=obj->data.size())return;for(int i=0;i<4;i++)obj->data[data_idx].color[i]=color.rgba[i];}

	template<class T_OBJECT> void Set_Scale(T_OBJECT* obj,const real scale)
	{if(obj==nullptr)return;obj->scale=scale;}

	template<class T_OBJECT> void Set_Polygon_Mode(T_OBJECT* obj,PolygonMode polygon_mode)
	{if(obj==nullptr)return;obj->polygon_mode=polygon_mode;}

	template<class T_OBJECT> void Set_Shading_Mode(T_OBJECT* obj,ShadingMode shading_mode)
	{if(obj==nullptr)return;obj->Set_Shading_Mode((ShadingMode)shading_mode);}

	template<class T_OBJECT> void Set_Line_Width(T_OBJECT* obj,const GLfloat line_width)
	{if(obj==nullptr)return;obj->line_width=line_width;}

	template<class T_OBJECT> void Set_Point_Size(T_OBJECT* obj,const GLfloat point_size)
	{if(obj==nullptr)return;obj->Set_Point_Size(point_size);}

	void Set_Offscreen_Output_Dir(const std::string _offscreen_output);

	//////////////////////////////////////////////////////////////////////////
	////Basic callbacks
	void Print_Keyboard_Callbacks();
	Define_Function_Object(OpenGLViewer,Print_Keyboard_Callbacks);

	virtual void Toggle_Increase_Scale();
	Define_Function_Object(OpenGLViewer,Toggle_Increase_Scale);

	virtual void Toggle_Decrease_Scale();
	Define_Function_Object(OpenGLViewer,Toggle_Decrease_Scale);

	virtual void Toggle_Normalize_Data();
	Define_Function_Object(OpenGLViewer,Toggle_Normalize_Data);

	virtual void Toggle_Draw_Dis();
	Define_Function_Object(OpenGLViewer,Toggle_Draw_Dis);

	virtual void Toggle_Next_Frame();
	Define_Function_Object(OpenGLViewer,Toggle_Next_Frame);

	virtual void Toggle_Prev_Frame();
	Define_Function_Object(OpenGLViewer,Toggle_Prev_Frame);

	virtual void Toggle_First_Frame();
	Define_Function_Object(OpenGLViewer,Toggle_First_Frame);

	virtual void Toggle_Play();
	Define_Function_Object(OpenGLViewer,Toggle_Play);

	virtual void Initialize_Common_Callback_Keys();

	void Bind_Callback_Key(const uchar key,std::function<void(void)>* callback,const std::string& discription);
	
	void Bind_Callback_Keys(const Array<OpenGLData>& data,Array<std::function<void(void)>*> data_idx_callbacks,int start_idx=0);

	template<class T_OBJECT> void Bind_Draw_Callback_Key(const uchar key,T_OBJECT* obj,const std::string _draw="")
	{if(obj==nullptr)return;std::string draw=_draw;if(draw=="")draw="draw "+obj->name;Bind_Callback_Key(key,&obj->Toggle_Draw_Func,draw);}

	template<class T_OBJECT> void Bind_Func_Idx_0_Callback_Key(const uchar key,T_OBJECT* obj,const std::string _func="")
	{if(obj==nullptr)return;std::string func=_func;if(func=="")func="func "+obj->name;Bind_Callback_Key(key,&obj->Toggle_Func_Idx_0_Func,func);}

protected:

};
#endif
