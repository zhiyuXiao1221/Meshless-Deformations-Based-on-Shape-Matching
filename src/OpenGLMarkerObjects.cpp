//#####################################################################
// OpenGL marker object
// Copyright (c) (2018-), Bo Zhu, boolzhu@gmail.com
// This file is part of SLAX, whose distribution is governed by the LICENSE file.
//#####################################################################
#include <glad.h>
#include "OpenGLBufferObjects.h"
#include "OpenGLMarkerObjects.h"

using namespace OpenGLUbos;

//////////////////////////////////////////////////////////////////////////
////OpenGLBackground

OpenGLBackground::OpenGLBackground()
{
	color=OpenGLColor(0.f,0.f,0.f,1.f);name="background";
	box=Box<2>(Vector2::Ones()*(real)-1,Vector2::Ones());polygon_mode=PolygonMode::Fill;
	Set_Depth((real).9999);
}

void OpenGLBackground::Initialize()
{
	Base::Initialize();
	Add_Shader_Program(OpenGLShaderLibrary::Get_Shader(shader_name));	////gradient color

	Update_Data_To_Render_Pre();

	Array<Vector3> vtx={Vector3(box.min_corner[0],box.min_corner[1],depth),
						Vector3(box.max_corner[0],box.min_corner[1],depth),
						Vector3(box.max_corner[0],box.max_corner[1],depth),
        
                        Vector3(box.min_corner[0],box.min_corner[1],depth),
                        Vector3(box.max_corner[0],box.max_corner[1],depth),
						Vector3(box.min_corner[0],box.max_corner[1],depth)};
	Array<Vector2> uv={Vector2((real)0.,(real)0.),Vector2((real)1.,(real)0.),Vector2((real)1.,(real)1.),
                       Vector2((real)0.,(real)0.),Vector2((real)1.,(real)1.),Vector2((real)0.,(real)1.)};

	for(auto& p:vtx){
		OpenGL_Vertex4(p,opengl_vertices);			////position, 4 floats
		OpenGL_Color4(color.rgba,opengl_vertices);}	////color, 4 floats

	Set_OpenGL_Vertices();
	Set_OpenGL_Vertex_Attribute(0,4,8,0);	////position
	Set_OpenGL_Vertex_Attribute(1,4,8,4);	////color/tex
		
	Update_Data_To_Render_Post();	
}

void OpenGLBackground::Display() const
{
	if(!visible)return;
	Update_Polygon_Mode();

	{std::shared_ptr<OpenGLShaderProgram> shader=shader_programs[0];
	shader->Begin();
	glDepthMask(GL_FALSE);
	shader->Set_Uniform_Vec4f("color",color.rgba);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES,0,vtx_size/8);
	glDepthMask(GL_TRUE);
	shader->End();}
}

//////////////////////////////////////////////////////////////////////////
////OpenGLAxes

void OpenGLAxes::Initialize()	////No data update
{
	Base::Initialize();
	Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("vcolor"));
	Update_Data_To_Render_Pre();
	ArrayF<OpenGLColor,3> colors={OpenGLColor::Red(),OpenGLColor::Green(),OpenGLColor::Blue()};
	int dim=use_2d_display?2:3;
	for(int i=0;i<dim;i++)for(int j=0;j<2;j++){
		Vector3 pos=Vector3::Zero()+Vector3::Unit(i)*axis_length*(real)j;
		OpenGL_Vertex4_And_Color4(pos,colors[i].rgba,opengl_vertices);}		////position, 4 floats; color, 4 floats
	Set_OpenGL_Vertices();
	Set_OpenGL_Vertex_Attribute(0,4,8,0);	////position
	Set_OpenGL_Vertex_Attribute(1,4,8,4);	////color
	Update_Data_To_Render_Post();
}

void OpenGLAxes::Display() const
{
	using namespace OpenGLUbos;
	if(!visible)return;
	Update_Polygon_Mode();
	{std::shared_ptr<OpenGLShaderProgram> shader=shader_programs[0];
	shader->Begin();
	Bind_Uniform_Block_To_Ubo(shader,"camera");
	glBindVertexArray(vao);
	glDrawArrays(GL_LINES,0,vtx_size/8);
	shader->End();}
}

//////////////////////////////////////////////////////////////////////////
////OpenGLPoint

void OpenGLPoint::Initialize()
{
	Base::Initialize();
	Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("psize_ucolor"));
}

void OpenGLPoint::Update_Data_To_Render()
{
	if(!Update_Data_To_Render_Pre())return;
	OpenGL_Vertex4(pos,opengl_vertices);	////position, 4 floats
	Set_OpenGL_Vertices();
	Set_OpenGL_Vertex_Attribute(0,4,4,0);	////position
	Update_Data_To_Render_Post();	
}

void OpenGLPoint::Display() const
{
	std::shared_ptr<OpenGLShaderProgram> shader=shader_programs[0];
	shader->Begin();
	Bind_Uniform_Block_To_Ubo(shader,"camera");
	shader->Set_Uniform_Vec4f("color",color.rgba);
	glEnable(GL_PROGRAM_POINT_SIZE);
	shader->Set_Uniform("point_size",point_size);
	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS,0,vtx_size/4);
	shader->End();
}
	
//////////////////////////////////////////////////////////////////////////
////OpenGLTriangle

void OpenGLTriangle::Initialize()
{
	Base::Initialize();
	Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("vpos"));
}

void OpenGLTriangle::Update_Data_To_Render()
{
	if(!Update_Data_To_Render_Pre())return;
	for(int i=0;i<3;i++)OpenGL_Vertex4(vtx[i],opengl_vertices);	////position, 4 floats
	Set_OpenGL_Vertices();
	Set_OpenGL_Vertex_Attribute(0,4,4,0);	////position
	Update_Data_To_Render_Post();	
}

void OpenGLTriangle::Display() const
{
	std::shared_ptr<OpenGLShaderProgram> shader=shader_programs[0];
	shader->Begin();
	Bind_Uniform_Block_To_Ubo(shader,"camera");
	glLineWidth(line_width);
	shader->Set_Uniform_Vec4f("color",color.rgba);
	glBindVertexArray(vao);
	glDrawArrays(GL_LINE_LOOP,0,vtx_size/4);
	shader->End();
}

//////////////////////////////////////////////////////////////////////////
////OpenGLPolygon
void OpenGLPolygon::Initialize()
{
	Base::Initialize();
	Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("vpos"));
}

void OpenGLPolygon::Update_Data_To_Render()
{
	if(!Update_Data_To_Render_Pre())return;
	for(int i=0;i<vtx.size();i++)OpenGL_Vertex4(vtx[i],opengl_vertices);	////position, 4 floats
	Set_OpenGL_Vertices();
	Set_OpenGL_Vertex_Attribute(0,4,4,0);	////position
	Update_Data_To_Render_Post();	
}

void OpenGLPolygon::Display() const
{
	std::shared_ptr<OpenGLShaderProgram> shader=shader_programs[0];
	shader->Begin();
	Bind_Uniform_Block_To_Ubo(shader,"camera");
	glLineWidth(line_width);
	shader->Set_Uniform_Vec4f("color",color.rgba);
	glBindVertexArray(vao);
	glDrawArrays(GL_LINE_LOOP,0,vtx_size/4);
	shader->End();
}

//////////////////////////////////////////////////////////////////////////
////OpenGLCircle

void OpenGLCircle::Initialize()
{
	Base::Initialize();
	Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("vpos_model"));

	real step=(real)3.1415927*(real)2/(real)n;
	for(int i=0;i<n;i++)OpenGL_Vertex4(Vector3(cos((real)i*step),sin((real)i*step),(real)0),opengl_vertices);	////position, 4 floats
	Set_OpenGL_Vertices();
	Set_OpenGL_Vertex_Attribute(0,4,4,0);	////position
}

void OpenGLCircle::Update_Data_To_Render()
{
	if(!Update_Data_To_Render_Pre())return;
	Update_Model_Matrix();
	Update_Data_To_Render_Post();
}

void OpenGLCircle::Display() const
{
	Update_Polygon_Mode();
	std::shared_ptr<OpenGLShaderProgram> shader=shader_programs[0];
	shader->Begin();
	//glPushAttrib(GL_LINE_BIT);
	Bind_Uniform_Block_To_Ubo(shader,"camera");
	//glLineWidth(line_width);
	shader->Set_Uniform_Vec4f("color",color.rgba);
	shader->Set_Uniform_Matrix4f("model",glm::value_ptr(model));
	glBindVertexArray(vao);
	glDrawArrays(GL_LINE_LOOP,0,vtx_size/4);
	//glPopAttrib();
	shader->End();
}

void OpenGLCircle::Update_Model_Matrix()
{
	model=glm::translate(glm::mat4(1.f),glm::vec3((GLfloat)pos[0],(GLfloat)pos[1],(GLfloat)pos[2]));
	model=glm::scale(model,glm::vec3(radius,radius,radius));
}

void OpenGLSolidCircle::Initialize()
{
	Base::Initialize();
	Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("vpos_model"));

	OpenGL_Vertex4(Vector3(0,0,0),opengl_vertices);
	real step=(real)3.1415927*(real)2/(real)n;
	for(int i=0;i<n;i++)OpenGL_Vertex4(Vector3(cos((real)i*step),sin((real)i*step),(real)0),opengl_vertices);	////position, 4 floats
	OpenGL_Vertex4(Vector3((real)1,(real)0,(real)0),opengl_vertices);

	Set_OpenGL_Vertices();
	Set_OpenGL_Vertex_Attribute(0,4,4,0);	////position
}

void OpenGLSolidCircle::Update_Data_To_Render()
{
	if(!Update_Data_To_Render_Pre())return;
	Update_Model_Matrix();
	Update_Data_To_Render_Post();
}

void OpenGLSolidCircle::Display() const
{
	Update_Polygon_Mode();
	std::shared_ptr<OpenGLShaderProgram> shader=shader_programs[0];
	shader->Begin();
	Bind_Uniform_Block_To_Ubo(shader,"camera");
	shader->Set_Uniform_Vec4f("color",color.rgba);
	shader->Set_Uniform_Matrix4f("model",glm::value_ptr(model));
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_FAN,0,vtx_size/4);
	shader->End();
}

void OpenGLSolidCircle::Update_Model_Matrix()
{
	model=glm::translate(glm::mat4(1.f),glm::vec3((GLfloat)pos[0],(GLfloat)pos[1],(GLfloat)pos[2]));
	model=glm::scale(model,glm::vec3(radius,radius,radius));
}

//////////////////////////////////////////////////////////////////////////
////OpenGLMarkerTriangleMesh

void OpenGLMarkerTriangleMesh::Initialize()
{
	Base::Initialize();
	Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("vpos_model_vnormal_dl_fast"));

	Update_Mesh_Data_To_Render();
}

void OpenGLMarkerTriangleMesh::Update_Mesh_Data_To_Render()
{
	Array<Vector3> normals;Update_Normals(mesh,normals);
	for(auto i=0;i<mesh.Vertices().size();i++){
		OpenGL_Vertex4(mesh.Vertices()[i],opengl_vertices);
		OpenGL_Vertex4(normals[i],opengl_vertices);}	////position, 4 floats
	
	Set_OpenGL_Vertices();
	Set_OpenGL_Vertex_Attribute(0,4,8,0);	////position
	Set_OpenGL_Vertex_Attribute(1,4,8,4);	////normal
		
	for(auto& e:mesh.elements)
		OpenGL_Vertex(e,opengl_elements);
	Set_OpenGL_Elements();
}

void OpenGLMarkerTriangleMesh::Update_Data_To_Render()
{
	if(!Update_Data_To_Render_Pre())return;
	Update_Model_Matrix();
	Update_Data_To_Render_Post();	
}

void OpenGLMarkerTriangleMesh::Display() const
{
	std::shared_ptr<OpenGLShaderProgram> shader=shader_programs[0];
	shader->Begin();
	Bind_Uniform_Block_To_Ubo(shader,"camera");
	Bind_Uniform_Block_To_Ubo(shader,"lights");
	shader->Set_Uniform_Vec4f("mat_dif",color.rgba);
	shader->Set_Uniform_Vec4f("mat_spec",color.rgba);
	shader->Set_Uniform_Matrix4f("model",glm::value_ptr(model));
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES,ele_size,GL_UNSIGNED_INT,0);
	shader->End();
}

//////////////////////////////////////////////////////////////////////////
////OpenGLSphere

void OpenGLSphere::Initialize()
{
	Base::Initialize();
	Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("vpos_model_vnormal_dl_fast"));

	Initialize_Sphere_Mesh(radius,&mesh);

	Update_Mesh_Data_To_Render();
}

void OpenGLSphere::Update_Model_Matrix()
{
	model=glm::translate(glm::mat4(1.f),glm::vec3((GLfloat)pos[0],(GLfloat)pos[1],(GLfloat)pos[2]));
}
