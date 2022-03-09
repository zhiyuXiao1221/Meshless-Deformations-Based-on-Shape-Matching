//#####################################################################
// Opengl common
// Copyright (c) (2018-), Bo Zhu, boolzhu@gmail.com
// This file is part of SLAX, whose distribution is governed by the LICENSE file.
//#####################################################################
#ifndef __OpenGLCommon_h__
#define __OpenGLCommon_h__
#include "Common.h"

#include <glad.h>

////Callback function object
#define Define_Function_Object(class_name,function_name) \
std::function<void(void)> function_name##_Func=std::bind(&class_name::function_name,this);

////OpenGL Enums
enum class ColorType:int{Jet=0,Hot=1,Den=2,Mat=3,MC=4};
enum class StoreType:int{None=-1,Node=0,Cell=1};
enum class FieldDrawType:int{Color=0,Line=1};	
enum class TensorFieldDrawType:int{Eigenvector=0,Frame=1};
enum class TextureType:int{Tx1d,Tx2d,Tx3d,Tx3d2,Tx3d3,Tx3d4,TxCube};
enum class PolygonMode:int{Fill=0,Wireframe,SurfOnly};
enum class ShadingMode:int{None=0,Lighting,TexOnly,TexLighting,Sprite,ColorSprite,SizeSprite,TexSprite,Shadow,EnvMapping,Custom,A2,Texture};

class OpenGLColor
{
public:
    float rgba[4];

    OpenGLColor(const float red=0,const float green=0,const float blue=0,const float alpha=1)
    {rgba[0]=red;rgba[1]=green;rgba[2]=blue;rgba[3]=alpha;}

	OpenGLColor(const ArrayF<float,4>& _rgba)
	{rgba[0]=_rgba[0];rgba[1]=_rgba[1];rgba[2]=_rgba[2];rgba[3]=_rgba[3];}

    static OpenGLColor Red(){return OpenGLColor(1,0,0,1);}
    static OpenGLColor Green(){return OpenGLColor(0,1,0,1);}
    static OpenGLColor Blue(){return OpenGLColor(0,0,1,1);}
    static OpenGLColor Yellow(){return OpenGLColor(1,1,0,1);}
    static OpenGLColor White(){return OpenGLColor(1,1,1);}
    static OpenGLColor Black(){return OpenGLColor(0,0,0);}
};

class OpenGLColorMapper
{
public:
	Array<float> values;
	Array<OpenGLColor> colors;

	void Initialize(const Array<float>& _values,const Array<OpenGLColor>& _colors){values=_values;colors=_colors;}

	OpenGLColor Get(float x)
	{
		int n=(int)values.size();
		if(x<values[0])return colors[0];
		else if(x>=values[n-1])return colors[n-1];
		else{
			for(int i=0;i<n-1;i++){
				if(x>=values[i]&&x<values[i+1]){
					float c=(x-values[i])/(values[i+1]-values[i]);
					OpenGLColor color;
					for(int j=0;j<4;j++)color.rgba[j]=(1.f-c)*colors[i].rgba[j]+c*colors[i+1].rgba[j];
					return color;}}
			return colors[n-1];}
	}
};

////OpenGL pipeline commands
inline void OpenGL_Vertex(const int& v,Array<GLuint>& vertices)
{vertices.push_back((GLuint)v);}

inline void OpenGL_Vertex(const Vector2i& v,Array<GLuint>& vertices)
{vertices.push_back((GLuint)v[0]);vertices.push_back((GLuint)v[1]);}

inline void OpenGL_Vertex(const Vector3i& v,Array<GLuint>& vertices)
{vertices.push_back((GLuint)v[0]);vertices.push_back((GLuint)v[1]);vertices.push_back((GLuint)v[2]);}

inline void OpenGL_Vertex(const Vector4i& v,Array<GLuint>& vertices)
{vertices.push_back((GLuint)v[0]);vertices.push_back((GLuint)v[1]);vertices.push_back((GLuint)v[2]);vertices.push_back((GLuint)v[3]);}

inline void OpenGL_Vertex(const int v0,const int v1,const int v2,Array<GLuint>& vertices)
{vertices.push_back((GLuint)v0);vertices.push_back((GLuint)v1);vertices.push_back((GLuint)v2);}

inline void OpenGL_Vertex(const GLfloat v,Array<GLfloat>& vertices)
{vertices.push_back(v);}

inline void OpenGL_Vertex(const Vector2& v,Array<GLfloat>& vertices)
{vertices.push_back((GLfloat)v[0]);vertices.push_back((GLfloat)v[1]);}

inline void OpenGL_Vertex(const Vector3& v,Array<GLfloat>& vertices)
{vertices.push_back((GLfloat)v[0]);vertices.push_back((GLfloat)v[1]);vertices.push_back((GLfloat)v[2]);}

inline void OpenGL_Vertex4(const Vector2& v,Array<GLfloat>& vertices,const GLfloat placeholder=(GLfloat)0)
{vertices.push_back((GLfloat)v[0]);vertices.push_back((GLfloat)v[1]);vertices.push_back(placeholder);vertices.push_back(placeholder);}

inline void OpenGL_Vertex4(const Vector3& v,Array<GLfloat>& vertices,const GLfloat placeholder=(GLfloat)0)
{vertices.push_back((GLfloat)v[0]);vertices.push_back((GLfloat)v[1]);vertices.push_back((GLfloat)v[2]);vertices.push_back(placeholder);}

inline void OpenGL_Vertex4(const Vector4& v, Array<GLfloat>& vertices)
{vertices.push_back((GLfloat)v[0]); vertices.push_back((GLfloat)v[1]); vertices.push_back((GLfloat)v[2]); vertices.push_back((GLfloat)v[3]);}

inline void OpenGL_WeightsAndJoints(const Vector4& w, const Vector4i& j, Array<GLfloat>& vertices) {
	vertices.push_back((GLfloat)w[0]); vertices.push_back((GLfloat)w[1]); vertices.push_back((GLfloat)w[2]); vertices.push_back((GLfloat)w[3]);
	vertices.push_back(*(GLfloat*)&j[0]); vertices.push_back(*(GLfloat*)&j[1]); vertices.push_back(*(GLfloat*)&j[2]); vertices.push_back(*(GLfloat*)&j[3]);
}

inline void OpenGL_Color3(const GLfloat* color,Array<GLfloat>& colors)
{colors.push_back(color[0]);colors.push_back(color[1]);colors.push_back(color[2]);}

inline void OpenGL_Color4(const GLfloat* color,Array<GLfloat>& colors)
{colors.push_back(color[0]);colors.push_back(color[1]);colors.push_back(color[2]);colors.push_back(color[3]);}

inline void OpenGL_Vertex4_And_Color4(const Vector2& v,const GLfloat* c,Array<GLfloat>& vertices)
{OpenGL_Vertex4(v,vertices);OpenGL_Color4(c,vertices);}

inline void OpenGL_Vertex4_And_Color4(const Vector3& v,const GLfloat* c,Array<GLfloat>& vertices)
{OpenGL_Vertex4(v,vertices);OpenGL_Color4(c,vertices);}

inline void OpenGL_Color(const GLfloat* color,Array<GLfloat>& colors){OpenGL_Color4(color,colors);}

#endif
