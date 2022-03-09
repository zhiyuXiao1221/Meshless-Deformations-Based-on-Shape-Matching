//#####################################################################
// Particle Sand Driver
// Dartmouth COSC 89.18/189.02: Computational Methods for Physical Systems, Assignment starter code
// Contact: Bo Zhu (bo.zhu@dartmouth.edu)
//#####################################################################
#ifndef __DeformableObjectInteractiveDriver_h__
#define __DeformableObjectInteractiveDriver_h__
#include <memory>
#include "Common.h"
#include "Mesh.h"
#include "InClassDemoDriver.h"
#include "DeformableObject.h"
#include "OpenGLMesh.h"
#include "OpenGLCommon.h"
#include "OpenGLWindow.h"
#include "OpenGLViewer.h"
#include "TinyObjLoader.h"


class DeformableObjectInteractiveDriver : public InClassDemoDriver
{
	using Base=Driver;
public:
	DeformableObject deformable_object;
	const double dt=(double).02;

	////visualization data
	Segments segments;
	std::vector<Point> points;
    Curve my_object;
	bool use_obj_mesh=true;											////flag for use obj, set it to be true if you want to load an obj mesh
	std::string obj_mesh_name="/Users/dhyscuduke/Downloads/dartmouth-cg-starter-code-master/assignments/a1/bunny.obj";								////obj file name
	OpenGLTriangleMesh* opengl_tri_mesh=nullptr;						////mesh
	TriangleMesh<3>* tri_mesh=nullptr;
	////initialize simulation data and its visualizations
	virtual void Initialize_Data()
	{
        Initialize_My_Object();
		Initialize_Simulation_Data();
		
		segments.Initialize(this);
		segments.Sync_Data(deformable_object.particles.XRef(),deformable_object.springs);

		int n=deformable_object.particles.Size();
		points.resize(n);
		for(int i=0;i<n;i++){
			points[i].Set_Radius(.02);
			points[i].Initialize(this);
			points[i].Sync_Data(deformable_object.particles.X(i));
		}
	}

	////advance simulation timesteps
	virtual void Advance(const double dt)
	{
		deformable_object.Advance(dt);
	}

	////update simulation data to its visualization counterparts
	virtual void Sync_Simulation_And_Visualization_Data()
	{
		segments.Sync_Data(deformable_object.particles.XRef());
		int n=deformable_object.particles.Size();
		for(int i=0;i<n;i++){
			points[i].Sync_Data(deformable_object.particles.X(i));
		}
        my_object.Sync_Data(deformable_object.my_object_vertices);
	}

	////here we initialize three tests for rod, cloth, and beam
	virtual void Initialize_Simulation_Data()
	{
		switch(test){
		case 1:{	////1d rod
			////initialize spring vertices
			double length=(double)1;int n=8;double dx=length/(double)n;
			deformable_object.particles.Resize(n);
			for(int i=0;i<n;i++){
				deformable_object.particles.X(i)=Vector3::Unit(0)*(double)i*dx;
				deformable_object.particles.M(i)=(double)1;}
			////initialize springs
			for(int i=0;i<n-1;i++){
                Vector2i s(i,i+1);
				deformable_object.springs.push_back(s);
                }
			////set boundary conditions
			deformable_object.Set_Boundary_Node(0);
		}break;
		case 2:{	////2d cloth
			////create a cloth mesh
			double length=(double)1;int width=4*scale;int height=6*scale;double step=length/(double)width;
			TriangleMesh<3> cloth_mesh;
			Build_Cloth_Mesh(width,height,step,&cloth_mesh,0,2);
			int n=(int)cloth_mesh.Vertices().size();
			std::vector<Vector2i> edges;Get_Mesh_Edges(cloth_mesh,edges);
			
			////copy cloth mesh vertices to spring particles 
			deformable_object.particles.Resize(n);
			for(int i=0;i<n;i++){
				deformable_object.particles.X(i)=cloth_mesh.Vertices()[i];
				deformable_object.particles.M(i)=(double)1;}
			////copy cloth mesh edges to springs
			deformable_object.springs=edges;

			////set boundary conditions
			deformable_object.Set_Boundary_Node(0);
			deformable_object.Set_Boundary_Node(width-1);
		}break;

		case 3:{	////3d volumetric beam
			int n=4*scale;double dx=(double)1/(double)n;
			Build_Beam_Particles_And_Springs(deformable_object.particles,deformable_object.springs,n,dx);
			for(int i=0;i<4;i++)deformable_object.Set_Boundary_Node(i);
			
			// opengl_tri_mesh=Add_Interactive_Object<OpenGLTriangleMesh>();
			// tri_mesh=&opengl_tri_mesh->mesh;
			// if(use_obj_mesh){
			// 	Array<std::shared_ptr<TriangleMesh<3> > > meshes;
			// 	Obj::Read_From_Obj_File(obj_mesh_name,meshes);
			// 	*tri_mesh=*meshes[0];
			// 	//std::cout<<"load tri_mesh, #vtx: "<<tri_mesh->Vertices().size()<<", #ele: "<<tri_mesh->Elements().size()<<std::endl;		
			// }
			// 	int n=(int)tri_mesh.Vertices().size();
			// 	deformable_object.particles.Resize(n);
			// 	for(int i=0;i<n;i++){
			// 	deformable_object.particles.X(i)=tri_mesh.Vertices()[i];
			// 	deformable_object.particles.M(i)=(double)1;}
			// 	std::vector<Vector2i> edges;Get_Mesh_Edges(tri_mesh,edges);
			// 	deformable_object.springs=edges;

			

			// // Set_Polygon_Mode(opengl_tri_mesh,PolygonMode::Fill);
			// // Set_Shading_Mode(opengl_tri_mesh,ShadingMode::Lighting);
			// // opengl_tri_mesh->Set_Data_Refreshed();
			// // opengl_tri_mesh->Initialize();
		}break;
		case 4:{
		auto mesh_obj=Add_Interactive_Object<OpenGLTriangleMesh>();

		Array<std::shared_ptr<TriangleMesh<3> > > meshes;
		std::string obj_file_name = "/Users/dhyscuduke/Downloads/dartmouth-cg-starter-code-master/assignments/a1/cap.obj";
		Obj::Read_From_Obj_File(obj_file_name,meshes);
		mesh_obj->mesh=*meshes[0];
		//std::cout<<"load tri_mesh from obj file, #vtx: "<<mesh_obj->mesh.Vertices().size()<<", #ele: "<<mesh_obj->mesh.Elements().size()<<std::endl;		
		int n=(int)mesh_obj->mesh.Vertices().size();
		deformable_object.particles.Resize(n);
		for(int i=0;i<n;i++){
			deformable_object.particles.X(i)=mesh_obj->mesh.Vertices()[i];
			deformable_object.particles.M(i)=(double)1;}
		std::vector<Vector2i> edges;Get_Mesh_Edges(mesh_obj->mesh,edges);
		deformable_object.springs=edges;

		}break;

		}
		deformable_object.Initialize();
        
	}
    	void Initialize_My_Object()
	{
		my_object.Initialize(this);
		my_object.Sync_Data(deformable_object.my_object_vertices);	
		my_object.Set_Linewidth(7.);
		my_object.Set_Color(1.,0.,0.);
	}

	//////////////////////////////////////////////////////////////////////////
	////These helper functions are all for creating meshes
protected:
	////Helper functions
	void Build_Cloth_Mesh(const int cell_num_0,const int cell_num_1,const double dx,TriangleMesh<3>* mesh,int axis_0=0,int axis_1=1)
	{
		mesh->elements.resize(2*(cell_num_0-1)*(cell_num_1-1));int t=0;
		for(int i=1;i<=cell_num_0-1;i++)for(int j=1;j<=cell_num_1-1;j++){ // counterclockwise node ordering
			if(i%2){mesh->elements[t++]=Vector3i(i+cell_num_0*(j-1),i+1+cell_num_0*(j-1),i+cell_num_0*j);mesh->elements[t++]=Vector3i(i+1+cell_num_0*(j-1),i+1+cell_num_0*j,i+cell_num_0*j);}
			else{mesh->elements[t++]=Vector3i(i+cell_num_0*(j-1),i+1+cell_num_0*(j-1),i+1+cell_num_0*j);mesh->elements[t++]=Vector3i(i+cell_num_0*(j-1),i+1+cell_num_0*j,i+cell_num_0*j);}}
		for(size_type i=0;i<mesh->elements.size();i++){mesh->elements[i]-=Vector3i::Ones();
		/*swap y and z*/int tmp=mesh->elements[i][1];mesh->elements[i][1]=mesh->elements[i][2];mesh->elements[i][2]=tmp;}
		for(int j=0;j<cell_num_1;j++)for(int i=0;i<cell_num_0;i++){Vector3 pos=Vector3::Zero();pos[axis_0]=(double)i*dx;pos[axis_1]=(double)j*dx;mesh->Vertices().push_back(pos);}
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

	void Build_Beam_Particles_And_Springs(Particles<3>& particles,std::vector<Vector2i>& edges,int n,double dx,Vector3 pos=Vector3::Zero())
	{
		particles.Resize(n*4);
		for(int i=0;i<particles.Size();i++){
			particles.M(i)=(double)1;
            particles.R(i)=.02;}
		for(int i=0;i<n;i++){
			particles.X(i*4)=pos+Vector3(dx*(double)i,(double)0,(double)0);
			particles.X(i*4+1)=pos+Vector3(dx*(double)i,(double)0,(double)dx);
			particles.X(i*4+2)=pos+Vector3(dx*(double)i,(double)dx,(double)0);
			particles.X(i*4+3)=pos+Vector3(dx*(double)i,(double)dx,(double)dx);
			edges.push_back(Vector2i(i*4,i*4+1));
			edges.push_back(Vector2i(i*4+1,i*4+3));
			edges.push_back(Vector2i(i*4+3,i*4+2));
			edges.push_back(Vector2i(i*4+2,i*4));
			if(i<n-1){
				edges.push_back(Vector2i(i*4,i*4+4));
				edges.push_back(Vector2i(i*4+1,i*4+5));
				edges.push_back(Vector2i(i*4+2,i*4+6));
				edges.push_back(Vector2i(i*4+3,i*4+7));
				
				edges.push_back(Vector2i(i*4,i*4+7));
				edges.push_back(Vector2i(i*4+1,i*4+6));
				edges.push_back(Vector2i(i*4+2,i*4+5));
				edges.push_back(Vector2i(i*4+3,i*4+4));}}
	}

	Vector2i Sorted(const Vector2i& v){return v[0]>v[1]?v:Vector2i(v[1],v[0]);}
};
#endif