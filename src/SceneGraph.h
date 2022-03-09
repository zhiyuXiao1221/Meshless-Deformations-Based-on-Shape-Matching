#pragma once

//#ifndef __SCEME_GRAPH_h__
//#define __SCEME_GRAPH_h__

#include "Common.h"
#include "glm.hpp"

////SceneGraph
////template parameter d specifies the dimension of a vertex, e.g., d=2 -- 2d vertex; d=3 -- 3d vertex
template<int d> class SceneGraph {
public:
	struct Node {
		glm::vec3 pos;
		glm::vec3 scl;
		glm::quat rot;
		Array<int> children_indices;
	};


	enum class AnimTarget {
		pos,
		rot,
		scl
	};

	struct AnimChannel {
		int node_index;
		AnimTarget target;

		Array<float> times;
		Array<glm::vec4> frames;
	};

	struct Animation {
		Array<AnimChannel> channels;
	};

	Array<Node> nodes;
	Array<glm::mat4> node_world_transforms;
	Array<Animation> animations;
};

//#endif