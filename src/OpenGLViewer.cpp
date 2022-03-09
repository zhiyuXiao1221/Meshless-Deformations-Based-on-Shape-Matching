//#####################################################################
// Opengl viewer
// Copyright (c) (2018-), Bo Zhu, boolzhu@gmail.com
// This file is part of SLAX, whose distribution is governed by the LICENSE file.
//#####################################################################
#include "Mesh.h"
#include "File.h"
#include "OpenGLWindow.h"
#include "OpenGLViewer.h"
#include "OpenGLMesh.h"
#include "OpenGLMarkerObjects.h"

//////////////////////////////////////////////////////////////////////////
////Initialization and run

void OpenGLViewer::Initialize()
{
	opengl_window=std::make_shared<OpenGLWindow>();
    opengl_window->Init();
	opengl_window->opengl_viewer.reset(this);
	Initialize_Common_Callback_Keys();
	Initialize_Common_Data();
	Initialize_Data();
	Initialize_Camera();
	Initialize_UI();
	opengl_window->Update_Data_To_Render();
	Print_Keyboard_Callbacks();
}

void OpenGLViewer::Run()
{
	opengl_window->Run();
}

void OpenGLViewer::Add_OpenGL_Object(OpenGLObject* object)
{
	opengl_window->Add_Object(object);
}

void OpenGLViewer::Initialize_Common_Data()
{
	opengl_window->use_2d_display=use_2d_display;
	if(draw_bk)Add_Interactive_Object<OpenGLBackground>(/*init*/true);
	if(draw_axes){
		auto axes=Add_Interactive_Object<OpenGLAxes>();
		axes->use_2d_display=use_2d_display;axes->Initialize();}
	
	std::string file_name=output_dir+"/0/last_frame.txt";
	if(File::File_Exists(file_name))
		File::Read_Text_From_File(file_name,last_frame);
	if(verbose)std::cout<<"Read last frame: "<<last_frame<<std::endl;
}

//////////////////////////////////////////////////////////////////////////
////Animation

void OpenGLViewer::Update_Frame()
{
	for(auto& obj:opengl_window->object_list){
		if(!obj->interactive)obj->Refresh(frame);
		obj->Update_Data_To_Render();}

	opengl_window->texts["frame"]="Frame: "+std::to_string(frame);

	if(opengl_window->display_offscreen){
		opengl_window->frame_offscreen=frame;
		opengl_window->frame_offscreen_rendered=-1;}
}

//////////////////////////////////////////////////////////////////////////
////UI

void OpenGLViewer::Initialize_UI()
{
}

void OpenGLViewer::Finish()
{
}

void OpenGLViewer::Toggle_Command(const std::string cmd)
{
	std::cout<<"cmd: "<<cmd<<std::endl;
}

//////////////////////////////////////////////////////////////////////////
////Set viewer and object properties

void OpenGLViewer::Set_Offscreen_Output_Dir(const std::string offscreen_output)
{
	opengl_window->offscreen_output_dir=offscreen_output;
}

//////////////////////////////////////////////////////////////////////////
////Basic callbacks

void OpenGLViewer::Print_Keyboard_Callbacks()
{
	std::cout<<"--- Keyboard callback functions ---\n";
	for(auto iter=key_data_hashtable.begin();iter!=key_data_hashtable.end();iter++){
		std::cout<<"Key: "<<iter->first<<", func: "<<iter->second<<std::endl;}
}

void OpenGLViewer::Toggle_Increase_Scale()
{
	for(auto& obj:opengl_window->object_list){
		auto opengl_object=dynamic_cast<OpenGLObject*>(obj.get());
		if(opengl_object->visible)opengl_object->Toggle_Increase_Scale();}
}

void OpenGLViewer::Toggle_Decrease_Scale()
{
	for(auto& obj:opengl_window->object_list){
		auto opengl_object=dynamic_cast<OpenGLObject*>(obj.get());
		if(opengl_object->visible)opengl_object->Toggle_Decrease_Scale();}
}

void OpenGLViewer::Toggle_Normalize_Data()
{
	for(auto& obj:opengl_window->object_list){
		auto opengl_object=dynamic_cast<OpenGLObject*>(obj.get());
		if(opengl_object->visible)opengl_object->Normalize_Data();}
}

void OpenGLViewer::Toggle_Draw_Dis()
{
	for(auto& obj:opengl_window->object_list){
		auto opengl_object=dynamic_cast<OpenGLObject*>(obj.get());
		if(opengl_object->visible)opengl_object->Toggle_Draw_Dis();}
}

void OpenGLViewer::Toggle_Next_Frame()
{
	frame++;
	if(last_frame!=-1&&frame>=last_frame)return;
	Update_Frame();
}

void OpenGLViewer::Toggle_Prev_Frame()
{
	frame--;
	if(frame<0)frame=0;
	else Update_Frame();
}

void OpenGLViewer::Toggle_First_Frame()
{
	frame=first_frame;
	Update_Frame();
}

void OpenGLViewer::Toggle_Play()
{
	play=!play;
	opengl_window->Set_Timer_Callback(play?&Toggle_Next_Frame_Func:nullptr);
}

void OpenGLViewer::Initialize_Common_Callback_Keys()
{
	Bind_Callback_Key('>',&Toggle_Increase_Scale_Func,"scale+");
	Bind_Callback_Key('<',&Toggle_Decrease_Scale_Func,"scale-");
	Bind_Callback_Key('K',&Print_Keyboard_Callbacks_Func,"print binded keys");
	Bind_Callback_Key(']',&Toggle_Next_Frame_Func,"next frame");
	Bind_Callback_Key('[',&Toggle_Prev_Frame_Func,"prev frame");
	Bind_Callback_Key('r',&Toggle_First_Frame_Func,"first frame");
	Bind_Callback_Key('p',&Toggle_Play_Func,"play");
	Bind_Callback_Key('q',&opengl_window->Quit_Func,"quit");
	Bind_Callback_Key('w',&opengl_window->Toggle_Offscreen_Func,"offscreen rendering");
}

void OpenGLViewer::Bind_Callback_Key(const uchar key, std::function<void(void)>* callback, const std::string& discription)
{
	opengl_window->Set_Keyboard_Callback(std::string(1,key),callback);
	key_data_hashtable.insert(std::make_pair(key,discription));
}

void OpenGLViewer::Bind_Callback_Keys(const Array<OpenGLData>& data, Array<std::function<void(void)>*> data_idx_callbacks, int start_idx/*=0*/)
{
	for(size_type i=0;i<data.size();i++){if(data[i].key.size()>0){
		Bind_Callback_Key(data[i].key[0],data_idx_callbacks[i+start_idx],data[i].name);}}
}
