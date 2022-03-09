//#####################################################################
// Tiny gltf loader
// Bo Zhu
//#####################################################################
#include <iostream>
#include <memory>
#include "Common.h"
#include "Mesh.h"
#include "Skeleton.h"

#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"
#include "TinyGltfLoader.h"
#include "SceneGraph.h"
#include "glm.hpp"
#include <gtx/matrix_decompose.hpp>

namespace Gltf{

static std::string GetFilePathExtension(const std::string &FileName) {
	if (FileName.find_last_of(".") != std::string::npos)
		return FileName.substr(FileName.find_last_of(".") + 1);
	return "";
}

template<typename T>
T* GetPointer(tinygltf::Model& model, int accIdx, int* count = nullptr) {
	auto& acc = model.accessors[accIdx];
	auto& buffer_view = model.bufferViews[acc.bufferView];
	auto& buffer = model.buffers[buffer_view.buffer];

	if (count) {
		*count = acc.count;
	}

	return (T*)(buffer.data.data() + buffer_view.byteOffset + acc.byteOffset);
}

template<typename T>
T* GetPointer(tinygltf::Model& model, tinygltf::Primitive& prim, std::string name, int* count = nullptr) {
	auto it = prim.attributes.find(name);
	T* data = nullptr;
	if (it != prim.attributes.end()) {
		data = GetPointer<T>(model, it->second, count);
	}
	return data;
}

template<class T_SCENEGRAPH, class T_MESH, class T_SKELETON>
void Read_From_Gltf_File(
	const std::string& file_name,
	std::shared_ptr<T_SCENEGRAPH>& scenegraph,
	Array<std::shared_ptr<T_MESH> >& meshes, 
	Array<std::shared_ptr<T_SKELETON> >& skeletons)
{
	auto file = std::ifstream(file_name);

	tinygltf::Model model;
	tinygltf::TinyGLTF gltf_ctx;
	std::string err;
	std::string warn;
	std::string ext = GetFilePathExtension(file_name);

	bool ret = false;
	if (ext.compare("glb") == 0) {
		std::cout << "Reading binary glTF" << std::endl;
		// assume binary glTF.
		ret = gltf_ctx.LoadBinaryFromFile(&model, &err, &warn, file_name.c_str());
	}
	else {
		std::cout << "Reading ASCII glTF" << std::endl;
		// assume ascii glTF.
		ret = gltf_ctx.LoadASCIIFromFile(&model, &err, &warn, file_name.c_str());
	}

	if (!ret) {
		std::cerr << "Read gltf error: " << err << std::endl;
		return;
	}

	if (warn != "") {
		std::cout << "Read gltf warning: " << warn << std::endl;
	}

	scenegraph = std::make_shared<T_SCENEGRAPH>();


	for (int i = 0; i < model.nodes.size(); i++) {
		auto& node = model.nodes[i];

		SceneGraph<3>::Node out_node = {};
		
		if (node.translation.size() != 0) {
			out_node.pos = glm::vec3{ (float)node.translation[0], (float)node.translation[1], (float)node.translation[2] };
		} else {
			out_node.pos = glm::vec3{ 0, 0, 0 };
		}

		if (node.scale.size() != 0) {
			out_node.scl = glm::vec3{ (float)node.scale[0], (float)node.scale[1], (float)node.scale[2] };
		} else {
			out_node.scl = glm::vec3{ 1, 1, 1 };
		}

		if (node.rotation.size() != 0) {
			out_node.rot = glm::quat{ (float)node.rotation[3], (float)node.rotation[0], (float)node.rotation[1], (float)node.rotation[2]  };
		} else {
			out_node.rot = glm::quat{ 1, 0, 0, 0 };
		}

		if (node.matrix.size() != 0) {
			glm::mat4 node_matrix;

			for (int i = 0; i < 16; i++) {
				node_matrix[i%4][i/4] = node.matrix[i];
			}

			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(node_matrix, out_node.scl, out_node.rot, out_node.pos, skew, perspective);
		}

		out_node.children_indices = node.children;

		scenegraph->nodes.push_back(out_node);
		scenegraph->node_world_transforms.push_back(glm::mat4(1));
	}

	for (int i = 0; i < model.animations.size(); i++) {
		auto& anim = model.animations[i];

		SceneGraph<3>::Animation oanim;


		for (auto& channel : anim.channels) {
			SceneGraph<3>::AnimChannel ochannel;
			ochannel.node_index = channel.target_node;


			if (channel.target_path == "translation") {
				ochannel.target = SceneGraph<3>::AnimTarget::pos;
			} else if (channel.target_path == "rotation") {
				ochannel.target = SceneGraph<3>::AnimTarget::rot;
			} else if (channel.target_path == "scale") {
				ochannel.target = SceneGraph<3>::AnimTarget::scl;
			} else {
				continue;
			}


			auto& sampler = anim.samplers[channel.sampler];

			int times_count;
			float* times_data = GetPointer<float>(model, sampler.input, &times_count);
			ochannel.times = Array<float>(times_data, times_data + times_count);

			int key_count;
			float* key_data = GetPointer<float>(model, sampler.output, &key_count);

			for (int k = 0; k < key_count; k++) {
				glm::vec4 keyframe = {};

				if (ochannel.target == SceneGraph<3>::AnimTarget::rot) {
					keyframe = {
						key_data[k * 4 + 3],
						key_data[k * 4 + 0],
						key_data[k * 4 + 1],
						key_data[k * 4 + 2],
					};
				}
				else {
					keyframe = {
						key_data[k * 3 + 0],
						key_data[k * 3 + 1],
						key_data[k * 3 + 2],
						0
					};
				}

				ochannel.frames.push_back(keyframe);
			}

			oanim.channels.push_back(ochannel);
		}

		scenegraph->animations.push_back(oanim);
	}

	
	for (int i = 0; i < model.skins.size(); i++) {
		auto& skin = model.skins[i];

		auto out_skin = std::make_shared<T_SKELETON>();
		

		glm::mat4* inv_bind_mat_data = GetPointer<glm::mat4>(model, skin.inverseBindMatrices);

		out_skin->root_joint_index = skin.skeleton;
		out_skin->joints = skin.joints;

		for (int j = 0; j < skin.joints.size(); j++) {
			out_skin->inv_bind_matrices.push_back(inv_bind_mat_data[j]);
			out_skin->skinning_matrices.push_back(glm::mat4(1));
		}

		skeletons.push_back(out_skin);
	}

	for (int i = 0; i < model.meshes.size(); i++) {
		auto& mesh = model.meshes[i];

		for (auto& prim : mesh.primitives) {
			auto positions_it = prim.attributes.find("POSITION");
			if (positions_it == prim.attributes.end()) continue;

			auto out_mesh = std::make_shared<T_MESH>();

			int vert_count = 0;
			float* pos_data = GetPointer<float>(model, positions_it->second, &vert_count);

			int idx_count = 0;
			std::uint16_t* idx_data = GetPointer<std::uint16_t>(model, prim.indices, &idx_count);


			// All other attributes must be the same size as vertices (we don't support sparse accessors)
			float* texcoord_data = GetPointer<float>(model, prim, "TEXCOORD_0");
			float* normal_data = GetPointer<float>(model, prim, "NORMAL");
			float* weights_data = GetPointer<float>(model, prim, "WEIGHTS_0");
			std::uint16_t* joints_data = GetPointer<std::uint16_t>(model, prim, "JOINTS_0");

			for (int v = 0; v < vert_count; v++) {
				out_mesh->vertices->push_back(
					Vector3((real)pos_data[v * 3 + 0],
							(real)pos_data[v * 3 + 1],
							(real)pos_data[v * 3 + 2]));


				if (texcoord_data) {
					out_mesh->uvs->push_back(
						Vector2((real)texcoord_data[v * 2 + 0],
								(real)texcoord_data[v * 2 + 1]));
				}

				if (normal_data) {
					out_mesh->normals->push_back(
						Vector3((real)normal_data[v * 3 + 0],
								(real)normal_data[v * 3 + 1],
								(real)normal_data[v * 3 + 2]));
				}


				if (weights_data) {

					float total_weights = weights_data[v * 4 + 0] + weights_data[v * 4 + 1] + weights_data[v * 4 + 2] + weights_data[v * 4 + 3];

					if (total_weights < 1.0f) {
						weights_data[v * 4 + 0] /= total_weights;
						weights_data[v * 4 + 1] /= total_weights;
						weights_data[v * 4 + 2] /= total_weights;
						weights_data[v * 4 + 3] /= total_weights;
					}

					out_mesh->weights->push_back(
						Vector4((real)weights_data[v * 4 + 0],
								(real)weights_data[v * 4 + 1],
								(real)weights_data[v * 4 + 2],
								(real)weights_data[v * 4 + 3]));
				}

				if (joints_data) {
					out_mesh->joints->push_back(
						Vector4i((int)joints_data[v * 4 + 0],
								 (int)joints_data[v * 4 + 1],
								 (int)joints_data[v * 4 + 2],
								 (int)joints_data[v * 4 + 3]));
				}
			}

			for (int idx = 0; idx < idx_count; idx+=3) {
				int id0 = idx_data[idx+0];
				int id1 = idx_data[idx+1];
				int id2 = idx_data[idx+2];

				out_mesh->elements.push_back(Vector3i(id0, id1, id2));
			}

			meshes.push_back(out_mesh);
		}
	}
}

template void Read_From_Gltf_File<SceneGraph<3>, TriangleMesh<3>, Skeleton<3> >(
	const std::string&,
	std::shared_ptr<SceneGraph<3> >&,
	Array<std::shared_ptr<TriangleMesh<3> > >&,
	Array<std::shared_ptr<Skeleton<3> > >&);

};