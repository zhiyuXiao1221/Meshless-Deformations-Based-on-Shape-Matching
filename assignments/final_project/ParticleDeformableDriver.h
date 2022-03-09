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
#include "OpenGLMesh.h"
#include "OpenGLCommon.h"
#include "TinyObjLoader.h"

template<int d> class ParticleDeformableDriver : public InClassDemoDriver
{using VectorD=Vector<double,d>;using VectorDi=Vector<int,d>;using Base=Driver;
	double scale = 1.;
	double dt=.02;
	ParticleDeformable<d> deformable_object;
	std::vector<OpenGLPoint*> opengl_points;
	std::string obj_mesh_name="/Users/dhyscuduke/Desktop/PhysicalComputingFinal/physical_computing_final_project/obj/bunny.obj";	

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

	virtual void Initialize_Simulation_Data()
	{
		ReadFromObjFile(obj_mesh_name);
		Plane <d>* plane = new Plane<d>(VectorD::Unit(1), VectorD::Zero());
		deformable_object.env_objects.push_back(plane);
		deformable_object.Initialize();
	}

	////synchronize simulation data to visualization data
	virtual void Initialize_Data()
	{
		Initialize_Simulation_Data();
		for(int i=0;i< deformable_object.particles.Size();i++){
			Add_Solid_Point(i);
		}
	}
	////advance simulation timesteps
	virtual void Advance(const double dt)
	{
		deformable_object.Advance(dt);
	}
	void Sync_Simulation_And_Visualization_Data()
	{
		for(int i=0;i<deformable_object.particles.Size();i++){
			auto opengl_point=opengl_points[i];
			opengl_point->pos=V3(deformable_object.particles.X(i));
			opengl_point->Set_Data_Refreshed();
		}
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
	void ReadFromObjFile(std::string obj_file_name )
	{
		auto mesh_obj=Add_Interactive_Object<OpenGLTriangleMesh>();

		Array<std::shared_ptr<TriangleMesh<3> > > meshes;
		Obj::Read_From_Obj_File(obj_file_name,meshes);
		mesh_obj->mesh=*meshes[0];
		//std::cout<<"load tri_mesh from obj file, #vtx: "<<mesh_obj->mesh.Vertices().size()<<", #ele: "<<mesh_obj->mesh.Elements().size()<<std::endl;		
		int n=(int)mesh_obj->mesh.Vertices().size();
		deformable_object.particles.Resize(n);
		for(int i=0;i<n;i++)
		{
			deformable_object.particles.X(i)=mesh_obj->mesh.Vertices()[i] + VectorD(0.,0.2,0.);
			deformable_object.particles.M(i)=(double)1;
			deformable_object.particles.R(i)=(double).01;
		}
	}

	////Helper function to convert a vector to 3d, for c++ template
	Vector3 V3(const Vector2& v2){return Vector3(v2[0],v2[1],.0);}
	Vector3 V3(const Vector3& v3){return v3;}
};
#endif
