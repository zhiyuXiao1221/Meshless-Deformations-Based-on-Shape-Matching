//#####################################################################
// Tiny obj loader
// Bo Zhu
//#####################################################################
#ifndef __TinyObjLoader_h__
#define __TinyObjLoader_h__
#include <string>
#include <memory>
#include <vector>

namespace Obj
{
    template<class T_MESH> void Read_From_Obj_File(const std::string& file_name,std::vector<std::shared_ptr<T_MESH> >& meshes);
	template<class T_MESH> void Read_From_Obj_File_Discrete_Triangles(const std::string&,Array<std::shared_ptr<T_MESH> >&);
};

#endif