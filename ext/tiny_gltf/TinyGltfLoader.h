//#####################################################################
// Tiny gltf loader
// Bo Zhu
//#####################################################################
#ifndef __TinyGltfLoader_h__
#define __TinyGltfLoader_h__
#include <string>
#include <memory>
#include <vector>

namespace Gltf
{
    template<class T_SCENEGRAPH, class T_MESH, class T_SKELETON>
	void Read_From_Gltf_File(
		const std::string& file_name,
		std::shared_ptr<T_SCENEGRAPH>& scenegraph,
		std::vector<std::shared_ptr<T_MESH> >& meshes,
		std::vector<std::shared_ptr<T_SKELETON> >& skeletons);
};

#endif