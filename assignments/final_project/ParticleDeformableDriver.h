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
#include "Mesh.h"
#include "OpenGLMarkerObjects.h"
#include "OpenGLCommon.h"
#include "OpenGLWindow.h"
#include "OpenGLViewer.h"
#include "OpenGLMesh.h"
#include "ParticleDeformable.h"
#include "InClassDemoDriver.h"
#include "TinyObjLoader.h"

template<int d> class ParticleDeformableDriver : public InClassDemoDriver
{using VectorD=Vector<double,d>;using VectorDi=Vector<int,d>;using Base=Driver;
	double scale = 1.;
	double dt=.02;
	ParticleDeformable<d> deformable_object;
	std::vector<OpenGLPoint*> opengl_points;
	OpenGLSphere* handle_sphere;							////obj file name
	std::vector<Point> points;
	Segments segments;
	OpenGLTriangleMesh *mesh_object;
public:
	virtual void Initialize_Simulation_Data()
	{
		int test = 6;
		deformable_object.test = test;
		switch (test) {
		case 1:	Initialize_1(); break; //Lattice dropping to floor
		case 2:	Initialize_2(); break; //Lattice deformed by force
		case 3:	Initialize_3(); break; //Bunny deformed by force
		case 4:	Initialize_4(); break; //Bunny spring deformed by force
		case 5:	Initialize_5(); break; //Cube spring deformed by force
		case 6:	Initialize_4(); break; //Bunny mesh(with texture) deformed by force
		}
		deformable_object.Initialize();
	}

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


	void Initialize_1() {
		double dx = 0.03;
		deformable_object.dx = dx;
		Initialize_Lattice_Points(0.3, 0.3, 0.3, dx, 0., 1., 0., 0.5, 0.5, 0.5);

		//Bowl<d> *bowl=new Bowl<d>(VectorD::Unit(1)*8,8);
		//deformable_object.env_objects.push_back(bowl);
		Plane <d>* plane = new Plane<d>(VectorD::Unit(1), VectorD::Zero());
		deformable_object.env_objects.push_back(plane);

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
		deformable_object.handle_sphere_r = dx;

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
	}
	
	void Initialize_3() {
		double dx = 0.03;
		deformable_object.dx = dx;
		double two_sided_length = 0.3;
		double elevation = two_sided_length / 2.;
		std::string obj_mesh_name="/Users/dhyscuduke/Desktop/PhysicalComputingFinal/physical_computing_final_project/obj/bunny.obj";	
		Read_From_Obj(obj_mesh_name,0.005);
		VectorD desired_pos = (elevation + two_sided_length/2.) * VectorD::Unit(1) - two_sided_length / 2. * VectorD::Unit(0) - two_sided_length / 2. * VectorD::Unit(2);
		deformable_object.handle_sphere_idx = deformable_object.Find_Nearest_Nb(desired_pos);
		deformable_object.handle_sphere_pos = deformable_object.particles.X(deformable_object.handle_sphere_idx);
		deformable_object.init_handle_sphere_pos = deformable_object.handle_sphere_pos;
		deformable_object.handle_sphere_r = 0.01;

		for (int i = 0; i < deformable_object.particles.Size(); i++) {
			if (deformable_object.particles.X(i)[1] <=  0.02) {
				deformable_object.fixed.push_back(1);
			}
			else {
				deformable_object.fixed.push_back(0);
			}
		}

		double influence_radius = 3. * dx;
		deformable_object.handle_sphere_influenced_radius = influence_radius;

		//Bowl<d> *bowl=new Bowl<d>(VectorD::Unit(1)*8,8);
		//deformable_object.env_objects.push_back(bowl);
		Plane <d>* plane = new Plane<d>(VectorD::Unit(1), VectorD::Zero());
		deformable_object.env_objects.push_back(plane);
	}

		
	void Initialize_4() {
		double dx = 0.02;
		deformable_object.dx = dx;
		double two_sided_length = 0.3;
		double elevation = two_sided_length / 2.;
		std::string obj_mesh_name="/Users/dhyscuduke/Desktop/PhysicalComputingFinal/physical_computing_final_project/obj/bunny.obj";	
		Read_From_Obj(obj_mesh_name,0.001);
		VectorD desired_pos = (elevation + two_sided_length/2.) * VectorD::Unit(1) - two_sided_length / 2. * VectorD::Unit(0) - two_sided_length / 2. * VectorD::Unit(2);
		deformable_object.handle_sphere_idx = deformable_object.Find_Nearest_Nb(desired_pos);
		deformable_object.handle_sphere_pos = deformable_object.particles.X(deformable_object.handle_sphere_idx);
		deformable_object.init_handle_sphere_pos = deformable_object.handle_sphere_pos;
		deformable_object.handle_sphere_r = 0.01;

		for (int i = 0; i < deformable_object.particles.Size(); i++) {
			if (deformable_object.particles.X(i)[1] <= 0.02) {
				deformable_object.fixed.push_back(1);
			}
			else {
				deformable_object.fixed.push_back(0);
			}
		}

		double influence_radius = 3. * dx;
		deformable_object.handle_sphere_influenced_radius = influence_radius;

		//Bowl<d> *bowl=new Bowl<d>(VectorD::Unit(1)*8,8);
		//deformable_object.env_objects.push_back(bowl);
		Plane <d>* plane = new Plane<d>(VectorD::Unit(1), VectorD::Zero());
		deformable_object.env_objects.push_back(plane);
	}
		
	void Initialize_5() {
		double dx = 0.02;
		std::string obj_mesh_name="/Users/dhyscuduke/Desktop/PhysicalComputingFinal/physical_computing_final_project/obj/cube.obj";	
		Read_From_Obj(obj_mesh_name,0.03);
		deformable_object.handle_sphere_idx = 5;
		deformable_object.handle_sphere_pos = deformable_object.particles.X(deformable_object.handle_sphere_idx);
		deformable_object.init_handle_sphere_pos = deformable_object.handle_sphere_pos;
		deformable_object.handle_sphere_r = 0.1;

		for (int i = 0; i < deformable_object.particles.Size(); i++) {
			if (deformable_object.particles.X(i)[1] <= 0.082379) {
				deformable_object.fixed.push_back(1);
			}
			else {
				deformable_object.fixed.push_back(0);
			}
		}

		double influence_radius = 3. * dx;
		deformable_object.handle_sphere_influenced_radius = influence_radius;

		//Bowl<d> *bowl=new Bowl<d>(VectorD::Unit(1)*8,8);
		//deformable_object.env_objects.push_back(bowl);
		Plane <d>* plane = new Plane<d>(VectorD::Unit(1), VectorD::Zero());
		deformable_object.env_objects.push_back(plane);
	}

	

	////synchronize simulation data to visualization data
	virtual void Initialize_Data()
	{
		//synchronize simulation data
		Initialize_Simulation_Data();
		//synchronize visualization data
		if(deformable_object.test !=6) {
			if(deformable_object.test >3) {
			Add_Solid_Sphere();
			}
			else {
			for(int i=0;i< deformable_object.particles.Size();i++){
			Add_Solid_Point(i);}
			}
		}	
		if(deformable_object.test != 1 ) 
		{
			Add_handle();
		}
		//mesh project part for test = 6
		if(deformable_object.test == 6) {
			Update_Vertex_Color_And_Normal_For_Mesh_Object(mesh_object);
			Update_Vertex_UV_For_Mesh_Object(mesh_object);
			////initialize shader
		std::string fileRoot = "/Users/dhyscuduke/Desktop/PhysicalComputingFinal/physical_computing_final_project/assignments/final_project/";
		std::string vertex_shader_file_name= fileRoot + "normal_mapping.vert";		
		std::string fragment_shader_file_name=fileRoot + "normal_mapping.frag";		
		OpenGLShaderLibrary::Instance()->Add_Shader_From_File(vertex_shader_file_name,fragment_shader_file_name,"my_shader");

		////specifying the textures
		OpenGLTextureLibrary::Instance()->Add_Texture_From_File(fileRoot+"bunny.jpg", "albedo");		
		OpenGLTextureLibrary::Instance()->Add_Texture_From_File(fileRoot+"earth_normal.png", "normal");		
		////bind the shader with each mesh object in the object array
			mesh_object->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("my_shader"));
			mesh_object->Add_Texture("tex_albedo", OpenGLTextureLibrary::Get_Texture("albedo"));
			mesh_object->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("normal"));
			Set_Polygon_Mode(mesh_object,PolygonMode::Fill);
			Set_Shading_Mode(mesh_object,ShadingMode::Texture);
			mesh_object->Set_Data_Refreshed();
			mesh_object->Initialize();	
		}
			

	}

	void Sync_Simulation_And_Visualization_Data()
	{
		if(deformable_object.test < 4)
		{
			for(int i=0;i<deformable_object.particles.Size();i++){
			auto opengl_point=opengl_points[i];
			opengl_point->pos=V3(deformable_object.particles.X(i));
			opengl_point->Set_Data_Refreshed();
			}
		}else {
			if(deformable_object.test!=6) {
				segments.Sync_Data(deformable_object.particles.XRef());
				int n=deformable_object.particles.Size();
				for(int i=0;i<n;i++){
				points[i].Sync_Data(deformable_object.particles.X(i));
			}
			}
		}

		if(deformable_object.test != 1)
		{
			handle_sphere->pos = deformable_object.handle_sphere_pos;
			if (deformable_object.dragging) {
			handle_sphere->color = OpenGLColor(static_cast <float> (0.2), static_cast <float>(1.0), static_cast <float>(0.));
			}
			else {
			handle_sphere->color = OpenGLColor(static_cast <float> (1.), static_cast <float>(0.2), static_cast <float>(0.));
			}
			handle_sphere->Set_Data_Refreshed();
		}
		if(deformable_object.test == 6) {
			int n=deformable_object.particles.Size();
			for(int i=0;i<n;i++){
				mesh_object->mesh.Vertices()[i] = deformable_object.particles.X(i);
				mesh_object->Set_Data_Refreshed();
			}
		}
	}

	////advance simulation timesteps
	virtual void Advance(const double dt)
	{
		deformable_object.Advance(dt);
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
    void Add_Solid_Sphere()
	{
		segments.Initialize(this);
		segments.Sync_Data(deformable_object.particles.XRef(),deformable_object.springs);
		int n=deformable_object.particles.Size();
		points.resize(n);
		for(int i=0;i<n;i++){
			points[i].Set_Radius(deformable_object.particles.R(i));
			points[i].Initialize(this);
			points[i].Sync_Data(deformable_object.particles.X(i));
		}
	}
	void Add_Solid_Point(const int i)
	{
		OpenGLColor c;
		if(deformable_object.test == 3) {
			double random = (double) ((rand() % 1000) / 1000.f);
			c = OpenGLColor(1.0 * random, (1.0- random) * 1.0,0.,1.);
		}else {
			for (int i = 0; i < 3; i++) {
			c.rgba[i] = static_cast<float>(rand() % 1000) / 1000.f;
			}
		}
		
		auto opengl_point = Add_Interactive_Object<OpenGLPoint>();
		opengl_points.push_back(opengl_point);
		opengl_point->pos = V3(deformable_object.particles.X(i));
		opengl_point->point_size = (GLfloat) (1000. * deformable_object.particles.R(i));
		opengl_point->color = c;
		opengl_point->Set_Data_Refreshed();
		opengl_point->Initialize();
	}
	void Add_handle() 
	{
		handle_sphere = Add_Interactive_Object<OpenGLSphere>();
		handle_sphere->pos = deformable_object.handle_sphere_pos;
		handle_sphere->radius = deformable_object.handle_sphere_r;
		handle_sphere->color = OpenGLColor(static_cast < float> (1.), static_cast < float>(0.2), static_cast < float>(0.));
		handle_sphere->Set_Data_Refreshed();
		handle_sphere->Initialize();
	}
	void Read_From_Obj(std::string objFile,double raduis) {
		auto mesh_obj=Add_Interactive_Object<OpenGLTriangleMesh>();
		Array<std::shared_ptr<TriangleMesh<3> > > meshes;
		Obj::Read_From_Obj_File(objFile,meshes);
		mesh_obj->mesh=*meshes[0];
		mesh_object = mesh_obj;
		int n=(int)mesh_obj->mesh.Vertices().size();
		deformable_object.particles.Resize(n);
		for(int i=0;i<n;i++){
			deformable_object.particles.X(i)=mesh_obj->mesh.Vertices()[i];
			deformable_object.particles.M(i)=(double)1;
			deformable_object.particles.V(i)=VectorD::Zero();
			deformable_object.particles.R(i)=raduis;
			}
		if(deformable_object.test > 3) {
			std::vector<Vector2i> edges;Get_Mesh_Edges(mesh_obj->mesh,edges);
			deformable_object.springs=edges;
		}
	}
	void Get_Mesh_Edges(const TriangleMesh<3>& mesh,std::vector<Vector2i>& edges)
	{
		Hashset<Vector2i> edge_hashset;ArrayF<Vector2i,6> element_edges;
		for(const auto& vtx:mesh.elements){
			edge_hashset.insert(Sorted(Vector2i(vtx[0],vtx[1])));
			edge_hashset.insert(Sorted(Vector2i(vtx[1],vtx[2])));
			edge_hashset.insert(Sorted(Vector2i(vtx[2],vtx[0])));}
		for(const auto& edge:edge_hashset)edges.push_back(edge);
	}
	
	//mesh part
		void Update_Vertex_Color_And_Normal_For_Mesh_Object(OpenGLTriangleMesh* obj)
	{
		int vn=(int)obj->mesh.Vertices().size();					////number of vertices of a mesh
		std::vector<Vector3>& vertices=obj->mesh.Vertices();		////you might find this array useful
		std::vector<Vector3i>& elements=obj->mesh.Elements();		////you might find this array also useful

		std::vector<Vector4f>& vtx_color=obj->vtx_color;
		vtx_color.resize(vn);
		std::fill(vtx_color.begin(),vtx_color.end(),Vector4f::Zero());

		for(int i=0;i<vn;i++){
			vtx_color[i]=Vector4f(0.,1.,0.,1.);	////specify color for each vertex
		}
		std::vector<Vector2>& uv=obj->mesh.Uvs();
		
	}
		void Update_Vertex_UV_For_Mesh_Object(OpenGLTriangleMesh* obj)
	{
		int vn=(int)obj->mesh.Vertices().size();					////number of vertices of a mesh
		std::vector<Vector3>& vertices=obj->mesh.Vertices();		////you might find this array useful
		std::vector<Vector2>& uv=obj->mesh.Uvs();					////you need to set values in uv to specify the texture coordinates
		uv.resize(vn);
		for(int i=0;i<vn;i++){uv[i]=Vector2(0.,0.);}				////set uv to be zero by default

		Update_Uv_Using_Spherical_Coordinates(vertices,uv);
	}

	////TODO [Step 0]: update the uv coordinates for each vertex using the spherical coordinates.
	////NOTICE: This code updates the vertex color array on the CPU end. The array will then be sent to GPU and read it the vertex shader as v_color.
	////You don't need to implement the CPU-GPU data transfer code.
	void Update_Uv_Using_Spherical_Coordinates(const std::vector<Vector3>& vertices,std::vector<Vector2>& uv)
	{
		/*Your implementation starts*/	
		for(int i=0;i<vertices.size();i++){
			Vector3 vertex = vertices[i];
			double r = sqrt(pow(vertex[0],2)+pow(vertex[1],2)+pow(vertex[2],2));
			float u = (float)atan2(vertex[1],vertex[0])/(2*M_PI);
			float v = (float)acos(vertex[2]/r)/(M_PI);
			uv[i]=Vector2(u,v);
		}
		/*Your implementation ends*/
	}

	////Helper function to convert a vector to 3d, for c++ template
	Vector3 V3(const Vector2& v2){return Vector3(v2[0],v2[1],.0);}
	Vector3 V3(const Vector3& v3){return v3;}
};
#endif
