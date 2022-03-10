#ifndef __InClassDemoDriver_h__
#define __InClassDemoDriver_h__
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

////example data array:{Vector3(0,0,0),Vector3(0,1,0),Vector3(1,0,2),...}
////the viewer will draw a curve by connecting all the input points in sequence
class Curve
{
public:
	OpenGLSegmentMesh* opengl_trace=nullptr;
	OpenGLViewer* driver=nullptr;
	OpenGLColor default_color=OpenGLColor(.0,1.,.0,1.);

	void Initialize(OpenGLViewer* _driver)
	{
		driver=_driver;
		opengl_trace=driver->Add_Interactive_Object<OpenGLSegmentMesh>();
		opengl_trace->mesh.Elements().resize(1);
		opengl_trace->mesh.Vertices().resize(1);
		opengl_trace->mesh.Vertices()[0]=Vector3(0.,0.,0.);
		opengl_trace->mesh.elements[0]=Vector2i(0,0);
		opengl_trace->Set_Data_Refreshed();
		opengl_trace->Initialize();
	}

	void Sync_Data(const std::vector<Vector3>& vertices)
	{
		int n=(int)vertices.size();
		opengl_trace->mesh.Vertices()=vertices;
		opengl_trace->mesh.Elements().resize(n-1);
		for(int i=0;i<n-1;i++)opengl_trace->mesh.Elements()[i]=Vector2i(i,i+1);
		opengl_trace->Set_Data_Refreshed();
	}

	void Sync_Data(const std::vector<Vector2>& vertices)
	{
		int n=(int)vertices.size();
		opengl_trace->mesh.Vertices().resize(n);
		for(int i=0;i<n;i++)opengl_trace->mesh.Vertices()[i]=Vector3(vertices[i][0],vertices[i][1],(double)0);
		opengl_trace->mesh.Elements().resize(n-1);
		for(int i=0;i<n-1;i++)opengl_trace->mesh.Elements()[i]=Vector2i(i,i+1);
		opengl_trace->Set_Data_Refreshed();
	}

	void Sync_Data(const Vector3* vertices,const int n)
	{
		opengl_trace->mesh.Vertices().resize(n);
		for(int i=0;i<n;i++)opengl_trace->mesh.Vertices()[i]=vertices[i];
		opengl_trace->mesh.Elements().resize(n-1);
		for(int i=0;i<n-1;i++)opengl_trace->mesh.Elements()[i]=Vector2i(i,i+1);
		opengl_trace->Set_Data_Refreshed();
	}

	void Sync_Data(const Vector2* vertices,const int n)
	{
		opengl_trace->mesh.Vertices().resize(n);
		for(int i=0;i<n;i++)opengl_trace->mesh.Vertices()[i]=Vector3(vertices[i][0],vertices[i][1],(double)0);
		opengl_trace->mesh.Elements().resize(n-1);
		for(int i=0;i<n-1;i++)opengl_trace->mesh.Elements()[i]=Vector2i(i,i+1);
		opengl_trace->Set_Data_Refreshed();
	}

	void Set_Color(const double r,const double g,const double b)
	{
		default_color=OpenGLColor((float)r,(float)g,(float)b,1.);
		if(opengl_trace!=nullptr)opengl_trace->color=default_color;
	}

	void Set_Linewidth(const double line_width)
	{
		if(opengl_trace!=nullptr)opengl_trace->line_width=(GLfloat)line_width;
	}
};

////the viewer will draw a sphere with the specified position and radius
class Point
{
public:
	OpenGLSphere* opengl_sphere=nullptr;
	OpenGLViewer* driver=nullptr;
	double default_radius=(double).1;
	OpenGLColor default_color=OpenGLColor(.0,1.,.0,1.);

	void Initialize(OpenGLViewer* _driver)
	{
		driver=_driver;
		opengl_sphere=driver->Add_Interactive_Object<OpenGLSphere>();
		opengl_sphere->pos=Vector3::Zero();
		opengl_sphere->radius=default_radius;
		opengl_sphere->Set_Color(default_color);
		opengl_sphere->Set_Data_Refreshed();
		opengl_sphere->Initialize();
	}

	void Sync_Data(const Vector3& vertex)
	{
		opengl_sphere->pos=vertex;
		opengl_sphere->Set_Data_Refreshed();
	}

	void Sync_Data(const Vector2& vertex)
	{
		opengl_sphere->pos=Vector3(vertex[0],vertex[1],(double)0);
		opengl_sphere->Set_Data_Refreshed();
	}

	////This function needs to be called before Initialize
	void Set_Radius(const double _radius)
	{
		default_radius=_radius;
		if(opengl_sphere!=nullptr)opengl_sphere->radius=default_radius;
	}

	void Set_Color(const double r,const double g,const double b)
	{
		default_color=OpenGLColor((float)r,(float)g,(float)b,1.);
		if(opengl_sphere!=nullptr)opengl_sphere->Set_Color(default_color);
	}
};

////the viewer will draw a set of segments with the specified vertices and edges
class Segments
{
public:
	OpenGLSegmentMesh* opengl_segments=nullptr;
	OpenGLViewer* driver=nullptr;
	OpenGLColor default_color=OpenGLColor(.0,1.,.0,0.5);

	void Initialize(OpenGLViewer* _driver)
	{
		driver=_driver;
		opengl_segments=driver->Add_Interactive_Object<OpenGLSegmentMesh>();
		opengl_segments->mesh.Elements().resize(1);
		opengl_segments->mesh.Vertices().resize(1);
		opengl_segments->mesh.Vertices()[0]=Vector3(0.,0.,0.);
		opengl_segments->mesh.elements[0]=Vector2i(0,0);
		opengl_segments->Set_Data_Refreshed();
		opengl_segments->Initialize();
	}

	void Set_Color(const double r,const double g,const double b)
	{
		default_color=OpenGLColor((float)r,(float)g,(float)b,1.);
		if(opengl_segments!=nullptr)opengl_segments->color=default_color;
	}

	void Set_Linewidth(const double line_width)
	{
		if(opengl_segments!=nullptr)opengl_segments->line_width=(GLfloat)line_width;
	}

	////We do not update connectivities if only vertices are passed in
	void Sync_Data(const std::vector<Vector3>& vertices)
	{
		opengl_segments->mesh.Vertices()=vertices;
		opengl_segments->Set_Data_Refreshed();
	}

	void Sync_Data(const std::vector<Vector2>& vertices)
	{
		int n=(int)vertices.size();
		opengl_segments->mesh.Vertices().resize(n);
		for(int i=0;i<n;i++)opengl_segments->mesh.Vertices()[i]=Vector3(vertices[i][0],vertices[i][1],(double)0);
		opengl_segments->Set_Data_Refreshed();
	}

	void Sync_Data(const Vector3* vertices,const int n)
	{
		opengl_segments->mesh.Vertices().resize(n);
		for(int i=0;i<n;i++)opengl_segments->mesh.Vertices()[i]=vertices[i];
		opengl_segments->Set_Data_Refreshed();
	}

	void Sync_Data(const Vector2* vertices,const int n)
	{
		opengl_segments->mesh.Vertices().resize(n);
		for(int i=0;i<n;i++)opengl_segments->mesh.Vertices()[i]=Vector3(vertices[i][0],vertices[i][1],(double)0);
		opengl_segments->Set_Data_Refreshed();
	}

	void Sync_Data(const std::vector<Vector3>& vertices,const std::vector<Vector2i>& edges)
	{
		opengl_segments->mesh.Vertices()=vertices;
		opengl_segments->mesh.Elements()=edges;
		opengl_segments->Set_Data_Refreshed();
	}

	void Sync_Data(const Vector3* vertices,const int n,const std::vector<Vector2i>& edges)
	{
		opengl_segments->mesh.Vertices().resize(n);
		for(int i=0;i<n;i++)opengl_segments->mesh.Vertices()[i]=vertices[i];
		opengl_segments->mesh.Elements()=edges;
		opengl_segments->Set_Data_Refreshed();
	}

	void Sync_Data(const std::vector<Vector2>& vertices,const std::vector<Vector2i>& edges)
	{
		int n=(int)vertices.size();
		opengl_segments->mesh.Vertices().resize(n);
		for(int i=0;i<n;i++)opengl_segments->mesh.Vertices()[i]=Vector3(vertices[i][0],vertices[i][1],(double)0);
		opengl_segments->mesh.Elements()=edges;
		opengl_segments->Set_Data_Refreshed();
	}

	void Sync_Data(const Vector2* vertices,const int n,const std::vector<Vector2i>& edges)
	{
		opengl_segments->mesh.Vertices().resize(n);
		for(int i=0;i<n;i++)opengl_segments->mesh.Vertices()[i]=Vector3(vertices[i][0],vertices[i][1],(double)0);
		opengl_segments->mesh.Elements()=edges;
		opengl_segments->Set_Data_Refreshed();
	}
};

class Circle
{
public:
	OpenGLSphere* opengl_sphere=nullptr;
	OpenGLViewer* driver=nullptr;
	double default_radius=(double).1;
	OpenGLColor default_color=OpenGLColor(.0,1.,.0,1.);

	void Initialize(OpenGLViewer* _driver)
	{
		driver=_driver;
		opengl_sphere=driver->Add_Interactive_Object<OpenGLSphere>();
		opengl_sphere->pos=Vector3::Zero();
		opengl_sphere->radius=default_radius;
		opengl_sphere->Set_Color(default_color);
		opengl_sphere->Set_Data_Refreshed();
		opengl_sphere->Initialize();
	}

	void Sync_Data(const Vector3& vertex)
	{
		opengl_sphere->pos=vertex;
		opengl_sphere->Set_Data_Refreshed();
	}

	void Sync_Data(const Vector2& vertex)
	{
		opengl_sphere->pos=Vector3(vertex[0],vertex[1],(double)0);
		opengl_sphere->Set_Data_Refreshed();
	}

	////This function needs to be called before Initialize
	void Set_Radius(const double _radius)
	{
		default_radius=_radius;
		if(opengl_sphere!=nullptr)opengl_sphere->radius=default_radius;
	}

	void Set_Color(const double r,const double g,const double b)
	{
		default_color=OpenGLColor((float)r,(float)g,(float)b,1.);
		if(opengl_sphere!=nullptr)opengl_sphere->Set_Color(default_color);
	}
};

class InClassDemoDriver : public Driver, public OpenGLViewer
{using VectorD=Vector3;using VectorDi=Vector2i;using Base=Driver;
	////simulation data
	double dt=.02;

	////visualization data
	////put your own vis objects here

public:
	
	virtual void Initialize()
	{
		OpenGLViewer::Initialize();		////Initialize_Data is called within OpenGLViewer::Initialize()

		////set OpenGL rendering environments
		auto dir_light=OpenGLUbos::Add_Directional_Light(glm::vec3(-1.f,-.1f,-.2f));
		OpenGLUbos::Set_Ambient(glm::vec4(.1f,.1f,.1f,1.f));
		OpenGLUbos::Update_Lights_Ubo();
	}	
	
	virtual void Run(){OpenGLViewer::Run();}

	////update simulation and visualization for each time step
	virtual void Toggle_Next_Frame()
	{
		Advance(dt);
		Sync_Simulation_And_Visualization_Data();
		OpenGLViewer::Toggle_Next_Frame();
	}

	//////////////////////////////////////////////////////////////////////////
	////The following three functions will be customized 
	//////////////////////////////////////////////////////////////////////////

	////initialize simulation data and its visualizations
	virtual void Initialize_Data(){}

	////advance simulation timesteps
	virtual void Advance(const double dt){}

	////update simulation data to its visualization counterparts
	virtual void Sync_Simulation_And_Visualization_Data(){}
};
#endif