//#####################################################################
// Particle Sand Driver
// Dartmouth COSC 89.18/189.02: Computational Methods for Physical Systems, Assignment starter code
// Contact: Bo Zhu (bo.zhu@dartmouth.edu)
//#####################################################################
#ifndef __ParticleDeformableDriver_h__
#define __ParticleDeformableDriver_h__
#include <random>
#include "Common.h"
#include "Driver.h"
#include "OpenGLMarkerObjects.h"
#include "OpenGLCommon.h"
#include "OpenGLWindow.h"
#include "OpenGLViewer.h"
#include "ParticleDeformable.h"
#include "InClassDemoDriver.h"

template<int d> class ParticleDeformableDriver : public Driver, public OpenGLViewer
{using VectorD=Vector<double,d>;using VectorDi=Vector<int,d>;using Base=Driver;
	double scale = 1.;
	double dt=.02;
	ParticleDeformable<d> deformable_object;
	std::vector<OpenGLPoint*> opengl_points;
	OpenGLSphere* handle_sphere;

public:

	void Initialize_Lattice_Points(double x_span, double y_span, double z_span, double dx, 
		double x_offset = 0., double y_offset = 0., double z_offset = 0., 
		double roll = 0., double pitch = 0., double yaw = 0.) {
		int n_x = (int)(x_span / dx);
		int n_y = (int)(y_span / dx);
		int n_z = (int)(z_span / dx);
		double center_x = x_span / 2.;
		double center_y = y_span / 2.;
		double center_z = z_span / 2.;
		for (int i = 0; i <= n_x; i++) {
			for (int j = 0; j <= n_y; j++) {
				for (int k = 0; k <= n_z; k++) {
					VectorD pos = VectorD::Unit(0) * (dx * i - center_x) + VectorD::Unit(1) * (dx * j - center_y) + VectorD::Unit(2) * (dx * k - center_z);
					Eigen::AngleAxisd rollAngle(roll, Eigen::Vector3d::UnitZ());
					Eigen::AngleAxisd yawAngle(yaw, Eigen::Vector3d::UnitY());
					Eigen::AngleAxisd pitchAngle(pitch, Eigen::Vector3d::UnitX());

					Eigen::Quaternion<double> q = rollAngle * yawAngle * pitchAngle;

					Eigen::Matrix3d rotationMatrix = q.matrix();
					
					pos = rotationMatrix * pos;
					VectorD offsets; offsets << x_offset, y_offset, z_offset;
					Add_Particle(pos + offsets, 0.01 * scale, 1.0);
				}
			}
		}
	}

	virtual void Initialize()
	{
		int test = 2;
		deformable_object.test = test;
		switch (test) {
		case 1:	Initialize_1(); break; //Lattice dropping to floor
		case 2:	Initialize_2(); break; //Lattice deformed by force
		}
	}

	void Initialize_1() {
		double dx = 0.03;
		deformable_object.dx = dx;
		Initialize_Lattice_Points(0.3, 0.3, 0.3, dx, 0., 1., 0., 0.5, 0.5, 0.5);

		//Bowl<d> *bowl=new Bowl<d>(VectorD::Unit(1)*8,8);
		//deformable_object.env_objects.push_back(bowl);
		Plane <d>* plane = new Plane<d>(VectorD::Unit(1), VectorD::Zero());
		deformable_object.env_objects.push_back(plane);

		////viewer initialization, initialize visualization data
		OpenGLViewer::Initialize();
		deformable_object.Initialize();
	}

	void Initialize_2() {
		double dx = 0.03;
		deformable_object.dx = dx;
		double two_sided_length = 0.3;
		double elevation = two_sided_length / 2.;
		Initialize_Lattice_Points(two_sided_length, two_sided_length, two_sided_length, dx, 0., elevation, 0.0, 0, 0., 0.0);

		VectorD desired_pos = (elevation + two_sided_length/2.) * VectorD::Unit(1) - two_sided_length / 2. * VectorD::Unit(0) - two_sided_length / 2. * VectorD::Unit(2);
		deformable_object.handle_sphere_idx = deformable_object.Find_Nearest_Nb(desired_pos);
		deformable_object.handle_sphere_pos = deformable_object.particles.X(deformable_object.handle_sphere_idx);
		deformable_object.init_handle_sphere_pos = deformable_object.handle_sphere_pos;
		deformable_object.handle_sphere_r = 3 * dx;

		for (int i = 0; i < deformable_object.particles.Size(); i++) {
			if (deformable_object.particles.X(i)[1] - (elevation - two_sided_length / 2.) < 0.5 * dx) {
				deformable_object.fixed.push_back(1);
			}
			else {
				deformable_object.fixed.push_back(0);
			}
		}

		double influence_radius = 6. * dx;
		deformable_object.handle_sphere_influenced_radius = influence_radius;

		//Bowl<d> *bowl=new Bowl<d>(VectorD::Unit(1)*8,8);
		//deformable_object.env_objects.push_back(bowl);
		Plane <d>* plane = new Plane<d>(VectorD::Unit(1), VectorD::Zero());
		deformable_object.env_objects.push_back(plane);

		////viewer initialization, initialize visualization data
		OpenGLViewer::Initialize();
		deformable_object.Initialize();
	}

	////synchronize simulation data to visualization data
	virtual void Initialize_Data()
	{
		for(int i=0;i< deformable_object.particles.Size();i++){
			Add_Solid_Point(i);
		}

		handle_sphere = Add_Interactive_Object<OpenGLSphere>();
		handle_sphere->pos = deformable_object.handle_sphere_pos;
		handle_sphere->radius = deformable_object.dx; // deformable_object.handle_sphere_r;
		handle_sphere->color = OpenGLColor(static_cast < float> (1.), static_cast < float>(0.2), static_cast < float>(0.));
		handle_sphere->Set_Data_Refreshed();
		handle_sphere->Initialize();
	}

	void Sync_Simulation_And_Visualization_Data()
	{
		for(int i=0;i<deformable_object.particles.Size();i++){
			auto opengl_point=opengl_points[i];
			opengl_point->pos=V3(deformable_object.particles.X(i));
			opengl_point->Set_Data_Refreshed();
		}

		handle_sphere->pos = deformable_object.handle_sphere_pos;
		if (deformable_object.dragging) {
			handle_sphere->color = OpenGLColor(static_cast <float> (0.2), static_cast <float>(1.0), static_cast <float>(0.));
		}
		else {
			handle_sphere->color = OpenGLColor(static_cast <float> (1.), static_cast <float>(0.2), static_cast <float>(0.));
		}
		handle_sphere->Set_Data_Refreshed();
	}

	////update simulation and visualization for each time step
	virtual void Toggle_Next_Frame()
	{
		deformable_object.Advance(dt);
		Sync_Simulation_And_Visualization_Data();
		OpenGLViewer::Toggle_Next_Frame();
	}

	virtual void Run()
	{
		OpenGLViewer::Run();
	}

	////Keyboard interaction
	virtual void Initialize_Common_Callback_Keys()
	{
		OpenGLViewer::Initialize_Common_Callback_Keys();
		Bind_Callback_Key('A', &Keyboard_Event_A_Func, "move left");
		Bind_Callback_Key('D', &Keyboard_Event_D_Func, "move right");
		Bind_Callback_Key('W', &Keyboard_Event_W_Func, "move top");
		Bind_Callback_Key('S', &Keyboard_Event_S_Func, "move bottom");
		Bind_Callback_Key('Q', &Keyboard_Event_Q_Func, "move front");
		Bind_Callback_Key('E', &Keyboard_Event_E_Func, "move back");
		Bind_Callback_Key('R', &Keyboard_Event_R_Func, "dragging or not");
		Bind_Callback_Key('T', &Keyboard_Event_T_Func, "relocate handle");
	}

	virtual void Keyboard_Event_A()
	{
		std::cout << "a: moving left" << std::endl;
		deformable_object.just_released = false;
		deformable_object.Move_Left();
	}
	Define_Function_Object(ParticleDeformableDriver, Keyboard_Event_A);

	virtual void Keyboard_Event_D()
	{
		std::cout << "d: moving right" << std::endl;
		deformable_object.just_released = false;
		deformable_object.Move_Right();
	}
	Define_Function_Object(ParticleDeformableDriver, Keyboard_Event_D);

	virtual void Keyboard_Event_W()
	{
		std::cout << "w: moving top" << std::endl;
		deformable_object.just_released = false;
		deformable_object.Move_Top();
	}
	Define_Function_Object(ParticleDeformableDriver, Keyboard_Event_W);

	virtual void Keyboard_Event_S()
	{
		std::cout << "s: moving bottom" << std::endl;
		deformable_object.just_released = false;
		deformable_object.Move_Bottom();
	}
	Define_Function_Object(ParticleDeformableDriver, Keyboard_Event_S);

	virtual void Keyboard_Event_Q()
	{
		std::cout << "q: moving front" << std::endl;
		deformable_object.just_released = false;
		deformable_object.Move_Front();
	}
	Define_Function_Object(ParticleDeformableDriver, Keyboard_Event_Q);

	virtual void Keyboard_Event_E()
	{
		std::cout << "e: moving back" << std::endl;
		deformable_object.just_released = false;
		deformable_object.Move_Back();
	}
	Define_Function_Object(ParticleDeformableDriver, Keyboard_Event_E);

	virtual void Keyboard_Event_R()
	{
		std::cout << "R: on/off" << std::endl;
		deformable_object.Toggle_On_Off();
	}
	Define_Function_Object(ParticleDeformableDriver, Keyboard_Event_R);

	virtual void Keyboard_Event_T()
	{
		std::cout << "T: relocate handle" << std::endl;
		deformable_object.just_released = false;
		deformable_object.Relocate_Handle();
	}
	Define_Function_Object(ParticleDeformableDriver, Keyboard_Event_T);


protected:
	void Add_Particle(VectorD pos,double r=.1,double m=1.)
	{
		int i=deformable_object.particles.Add_Element();	////return the last element's index
		deformable_object.particles.X(i)=pos;
		deformable_object.particles.V(i)=VectorD::Zero();
		deformable_object.particles.R(i)=r;
		deformable_object.particles.M(i)=m;
	}

	void Add_Solid_Point(const int i)
	{
		OpenGLColor c;
		for (int i = 0; i < 3; i++) {
			c.rgba[i] = static_cast<float>(rand() % 1000) / 1000.f;
		}
		auto opengl_point = Add_Interactive_Object<OpenGLPoint>();
		opengl_points.push_back(opengl_point);
		opengl_point->pos = V3(deformable_object.particles.X(i));
		opengl_point->point_size = (GLfloat) (1000. * deformable_object.particles.R(i));
		opengl_point->color = c;
		opengl_point->Set_Data_Refreshed();
		opengl_point->Initialize();
	}

	////Helper function to convert a vector to 3d, for c++ template
	Vector3 V3(const Vector2& v2){return Vector3(v2[0],v2[1],.0);}
	Vector3 V3(const Vector3& v3){return v3;}
};
#endif
