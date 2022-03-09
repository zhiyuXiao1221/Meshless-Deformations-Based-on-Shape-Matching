#pragma once

//#ifndef __SKELETON_h__
//#define __SKELETON_h__

#include "Common.h"
#include <glm.hpp>

////Skeleton
////template parameter d specifies the dimension of a vertex, e.g., d=2 -- 2d vertex; d=3 -- 3d vertex
template<int d> class Skeleton {
public:
	int root_joint_index;
	Array<glm::mat4> inv_bind_matrices;
	Array<int> joints;
	Array<glm::mat4> skinning_matrices;
};

//#endif