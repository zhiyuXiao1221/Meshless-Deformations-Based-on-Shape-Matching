//#####################################################################
// Opengl textures
// Copyright (c) (2018-), Bo Zhu, boolzhu@gmail.com
// This file is part of SLAX, whose distribution is governed by the LICENSE file.
//#####################################################################
#ifndef __OpenGLTexture_h__
#define __OpenGLTexture_h__
#include <string>
#include <memory>
#include <glad.h>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "Common.h"
#include "OpenGLShaderProgram.h"

class OpenGLTexture {
public:
	OpenGLTexture(GLuint texture) : texture(texture) {}
	~OpenGLTexture();

	void Bind(int textureSlot);
private:
	GLuint texture;
};


class OpenGLTextureLibrary
{
public:
	static OpenGLTextureLibrary* Instance();
	static std::shared_ptr<OpenGLTexture> Get_Texture(const std::string& name);
	std::shared_ptr<OpenGLTexture> Get(const std::string& name);
	void Add_Texture_From_File(std::string filename, std::string name);
protected:
	Hashtable<std::string, std::shared_ptr<OpenGLTexture> > texture_hashtable;
};
#endif
