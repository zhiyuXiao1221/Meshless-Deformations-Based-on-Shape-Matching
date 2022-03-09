//#####################################################################
// Tiny obj loader
// Bo Zhu
//#####################################################################
#include <iostream>
#include <memory>
#include "Common.h"
#include "Mesh.h"
#include "tiny_obj_loader.h"
#include "TinyObjLoader.h"

namespace Obj{

template<class T_MESH> void Read_From_Obj_File(const std::string& file_name,Array<std::shared_ptr<T_MESH> >& meshes)
{
	auto file = std::ifstream(file_name);

	std::string warn, err;
	tinyobj::attrib_t attrib; 
	Array<tinyobj::shape_t> shapes; 
	Array<tinyobj::material_t> materials; 
	
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, &file)) {
		std::cerr << "Read obj error: " << err << std::endl;
		return;
	}

	std::cout<<"Read obj file: "<<file_name<<", #shapes="<<shapes.size()<<", #materials="<<materials.size()<<std::endl;

    meshes.resize((int)shapes.size());
    for(auto i=0;i<shapes.size();i++){
		meshes[i]=std::make_shared<T_MESH>();
		tinyobj::mesh_t& mesh=shapes[i].mesh;
		
		int vtx_num=(int)attrib.vertices.size()/3;
		for(int j=0;j<vtx_num;j++){
			meshes[i]->vertices->push_back(
				Vector3((real)attrib.vertices[j * 3 + 0],
						(real)attrib.vertices[j * 3 + 1],
						(real)attrib.vertices[j * 3 + 2]));
		}

		int ele_num=(int)mesh.indices.size()/3;
		for(auto j=0;j<ele_num;j++){
			meshes[i]->elements.push_back(
				Vector3i(mesh.indices[j*3].vertex_index,
					mesh.indices[j*3+1].vertex_index,
					mesh.indices[j*3+2].vertex_index));
		}
		int uv_num = (int)attrib.texcoords.size()/2;

		for(auto j=0;j<uv_num;j++){
			meshes[i]->uvs->push_back(
				Vector2((real)attrib.texcoords[j * 2 + 0],
						(real)attrib.texcoords[j * 2 + 1]));
		}

		int normal_num=(int)attrib.normals.size()/3;

		if(normal_num>0){
			if (!meshes[i]->normals) meshes[i]->normals = std::make_shared< Array<Vector3> >();
			for(int j=0;j<normal_num;j++){
				meshes[i]->normals->push_back(
					Vector3((real)attrib.normals[j * 3 + 0],
							(real)attrib.normals[j * 3 + 1],
							(real)attrib.normals[j * 3 + 2]));
			}			
		}
	}
}

template<class T_MESH> void Read_From_Obj_File_Discrete_Triangles(const std::string& file_name,Array<std::shared_ptr<T_MESH> >& meshes)
{
	auto file = std::ifstream(file_name);

	std::string warn, err;
	tinyobj::attrib_t attrib; 
	Array<tinyobj::shape_t> shapes; 
	Array<tinyobj::material_t> materials; 
	
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, &file)) {
		std::cerr << "Read obj error: " << err << std::endl;
		return;
	}

	std::cout<<"Read obj file: "<<file_name<<", #shapes="<<shapes.size()<<", #materials="<<materials.size()<<std::endl;

    meshes.resize((int)shapes.size());
    for(auto i=0;i<shapes.size();i++){
		meshes[i]=std::make_shared<T_MESH>();
		tinyobj::mesh_t& mesh=shapes[i].mesh;
		 
		for (int f = 0; f < mesh.indices.size(); f++) {

			auto v0 = mesh.indices[f];

			meshes[i]->vertices->push_back(
				Vector3((real)attrib.vertices[v0.vertex_index * 3 + 0],
						(real)attrib.vertices[v0.vertex_index * 3 + 1],
						(real)attrib.vertices[v0.vertex_index * 3 + 2]));

			if (attrib.normals.size() > 0) {
				meshes[i]->normals->push_back(
					Vector3((real)attrib.normals[v0.normal_index * 3 + 0],
							(real)attrib.normals[v0.normal_index * 3 + 1],
							(real)attrib.normals[v0.normal_index * 3 + 2]));
			}

			if (attrib.texcoords.size() > 0) {
				meshes[i]->uvs->push_back(
					Vector2((real)attrib.texcoords[v0.texcoord_index * 2 + 0],
							1.0 - (real)attrib.texcoords[v0.texcoord_index * 2 + 1]));

			}

			if (f % 3 == 0) {
				meshes[i]->elements.push_back(Vector3i(f + 0, f + 1, f + 2));
			}
		}
	}
}

template void Read_From_Obj_File<TriangleMesh<3> >(const std::string&,Array<std::shared_ptr<TriangleMesh<3> > >&);
template void Read_From_Obj_File_Discrete_Triangles<TriangleMesh<3> >(const std::string&,Array<std::shared_ptr<TriangleMesh<3> > >&);
};