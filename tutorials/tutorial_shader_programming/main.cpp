//#####################################################################
// Main
// Dartmouth COSC 77/177 Computer Graphics, starter code
// Contact: Bo Zhu (bo.zhu@dartmouth.edu)
//#####################################################################
#include <iostream>

#include <random>
#include "Common.h"
#include "Driver.h"
#include "Particles.h"
#include "OpenGLMesh.h"
#include "OpenGLCommon.h"
#include "OpenGLWindow.h"
#include "OpenGLViewer.h"
#include "OpenGLMarkerObjects.h"
#include "OpenGLParticles.h"
#include "TinyObjLoader.h"

template<int d> class ToolkitDriver : public Driver, public OpenGLViewer
{
	using VectorD = Vector<real, d>; using VectorDi = Vector<int, d>; using Base = Driver;
	real dt = .02;

	OpenGLTriangleMesh* opengl_tri_mesh = nullptr;


public:
	virtual void Initialize()
	{
		OpenGLViewer::Initialize();
	}

	virtual void Initialize_Data()
	{
		OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/tutorial.vert.glsl", "shaders/tutorial.frag.glsl", "tutorial");
		OpenGLTextureLibrary::Instance()->Add_Texture_From_File("models/Default_albedo.jpg", "helmet_albedo");

		Array<std::shared_ptr<TriangleMesh<3> > > meshes;
		Obj::Read_From_Obj_File("models/helmet.obj", meshes);

		////initialize meshes
		{
			opengl_tri_mesh = Add_Interactive_Object<OpenGLTriangleMesh>();
			opengl_tri_mesh->mesh = *meshes[0];
			opengl_tri_mesh->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

			Set_Polygon_Mode(opengl_tri_mesh, PolygonMode::Fill);
			Set_Shading_Mode(opengl_tri_mesh, ShadingMode::Custom);

			opengl_tri_mesh->Set_Data_Refreshed();
			opengl_tri_mesh->Initialize();
			opengl_tri_mesh->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("tutorial"));
			opengl_tri_mesh->Add_Texture("albedo_tex", OpenGLTextureLibrary::Get_Texture("helmet_albedo"));
		}
	}

	void Sync_Simulation_And_Visualization_Data()
	{
		//opengl_tri_mesh->model_matrix = rotate(opengl_tri_mesh->model_matrix, (float)dt * 10.0f, vec3(0.0f, 1.0f, 0.0f));
	}

	virtual void Toggle_Next_Frame()
	{
		Sync_Simulation_And_Visualization_Data();
		OpenGLViewer::Toggle_Next_Frame();
	}

	virtual void Run()
	{
		OpenGLViewer::Run();
	}

	virtual void Initialize_Common_Callback_Keys()
	{
		OpenGLViewer::Initialize_Common_Callback_Keys();
	}
};


int main(int argc,char* argv[])
{
	int driver=1;

	switch(driver){
	case 1:{
		ToolkitDriver<3> driver;
		driver.Initialize();
		driver.Run();	
	}break;
	}
}

