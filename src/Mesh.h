#ifndef __Mesh_h__
#define __Mesh_h__
#include <fstream>
#include "Common.h"
#include "File.h"
#include "mikktspace.h"

////Simplicial mesh
////template parameter d specifies the dimension of a vertex, e.g., d=2 -- 2d vertex; d=3 -- 3d vertex
////template parameter e_d specifies the dimension of an element (the number of vertices of a simplex), e.g., e_d=2 -- segment; e_d=3, triangle; e_d=4, tetrahedra
template<int d,int e_d> class SimplicialMesh
{using VectorD=Vector<real,d>;using VectorDi=Vector<int,d>;using VectorEi=Vector<int,e_d>;
public:
	////Basic attributes
	ArrayPtr<VectorD> vertices = nullptr;
	ArrayPtr<VectorD> normals = nullptr;
	ArrayPtr<Vector2> uvs = nullptr;
	ArrayPtr<Vector4> tangents=nullptr;

	////Skinning
	ArrayPtr<Vector4> weights = nullptr;
	ArrayPtr<Vector4i> joints = nullptr;


	std::vector<VectorEi> elements;

	////Constructors
	SimplicialMesh(
		const ArrayPtr<VectorD> _vertices=nullptr, 
		const ArrayPtr<VectorD> _normals = nullptr, 
		const ArrayPtr<Vector2> _uvs = nullptr, 
		const ArrayPtr<Vector4> _tangets = nullptr,
		const ArrayPtr<Vector4> _weights = nullptr,
		const ArrayPtr<Vector4i> _joints = nullptr)
	{
		if (_vertices == nullptr)vertices = std::make_shared<std::vector<VectorD> >();
		else vertices = _vertices;

		if (_normals == nullptr)normals = std::make_shared<std::vector<VectorD> >();
		else normals = _normals;

		if (_uvs == nullptr)uvs = std::make_shared<std::vector<Vector2> >();
		else uvs = _uvs;
	
		if(_tangets ==nullptr)tangents =std::make_shared<std::vector<Vector4> >();
		else tangents = _tangets;

		if (_weights == nullptr)weights = std::make_shared<std::vector<Vector4> >();
		else weights = _weights;

		if (_joints == nullptr)joints = std::make_shared<std::vector<Vector4i> >();
		else joints = _joints;
	}

	////Copy constructor
	SimplicialMesh(const SimplicialMesh<d,e_d>& copy){*this=copy;}
	SimplicialMesh<d,e_d>& operator=(const SimplicialMesh<d,e_d>& copy)
	{
		if(vertices==nullptr)vertices=std::make_shared<std::vector<VectorD> >();
		*vertices = *(copy.vertices);
		*normals = *(copy.normals);
		*uvs = *(copy.uvs);
		*tangents = *(copy.tangents);
		*weights = *(copy.weights);
		*joints =*(copy.joints);

		elements=copy.elements;
		return *this;
	}

	////Access attributes
	static constexpr int Dim() {return d;}
	static constexpr int Element_Dim() {return e_d;}
	virtual std::vector<VectorD>& Vertices(){return *vertices.get();}
	virtual const std::vector<VectorD>& Vertices() const {return *vertices.get();}
	virtual std::vector<VectorD>& Normals() { return *normals.get(); }
	virtual const std::vector<VectorD>& Normals() const { return *normals.get(); }
	virtual std::vector<Vector2>& Uvs() { return *uvs.get(); }
	virtual const std::vector<Vector2>& Uvs() const { return *uvs.get(); }
	virtual std::vector<Vector4>& Tangents() { return *tangents.get(); }
	virtual const std::vector<Vector4>& Tangents() const { return *tangents.get(); }
	virtual std::vector<Vector4>& Weights() { return *weights.get(); }
	virtual const std::vector<Vector4>& Weights() const { return *weights.get(); }
	virtual std::vector<Vector4i>& Joints() { return *joints.get(); }
	virtual const std::vector<Vector4i>& Joints() const { return *joints.get(); }
	virtual std::vector<VectorEi>& Elements(){return elements;}
	virtual const std::vector<VectorEi>& Elements() const {return elements;}

	virtual void Clear()
	{
		if (vertices)vertices->clear();
		if (normals)normals->clear();
		if (uvs)uvs->clear();
		if (tangents)tangents->clear();
		if (weights)weights->clear();
		if (joints)joints->clear();
		elements.clear();
	}

	////IO
	virtual void Write_Binary(std::ostream& output) const
	{
		int vtx_n=(int)(*vertices).size();
		File::Write_Binary(output,vtx_n);
		File::Write_Binary_Array(output,&(*vertices)[0],vtx_n);
		int e_n=(int)elements.size();
		File::Write_Binary(output,e_n);
		if(e_n>0)File::Write_Binary_Array(output,&elements[0],e_n);
	}

	virtual void Read_Binary(std::istream& input)
	{
		int vtx_n=0;File::Read_Binary(input,vtx_n);
		(*vertices).resize(vtx_n);
		File::Read_Binary_Array(input,&(*vertices)[0],vtx_n);
		int e_n=0;File::Read_Binary(input,e_n);
		if(e_n>0){
			elements.resize(e_n);
			File::Read_Binary_Array(input,&elements[0],e_n);}
	}

	virtual void Write_To_File_3d(const std::string& file_name) const
	{
		SimplicialMesh<3,e_d> s3;Dim_Conversion(*this,s3);
		File::Write_Binary_To_File(file_name,s3);
	}

	virtual void Read_Text(std::istream& input)
	{
		int vtx_n=0;File::Read_Text(input,vtx_n);
		if(vtx_n>0){
			(*vertices).resize(vtx_n);
			for(int i=0;i<vtx_n;i++)File::Read_Text_Array(input,(*vertices)[i],d);}
		int e_n=0;File::Read_Text(input,e_n);
		if(e_n>0){
			elements.resize(e_n);
			for(int i=0;i<e_n;i++)File::Read_Text_Array(input,elements[i],e_d);}
	}

	virtual void Write_Text(std::ostream& output) const
	{
		int vtx_n=(int)(*vertices).size();File::Write_Text(output,vtx_n);File::Write_Text(output,'\n');
		if(vtx_n>0){for(int i=0;i<vtx_n;i++){File::Write_Text_Array(output,(*vertices)[i],d,' ');File::Write_Text(output,'\n');}}
		int e_n=(int)elements.size();
		File::Write_Text(output,'\n');File::Write_Text(output,e_n);File::Write_Text(output,'\n');
		if(e_n>0){for(int i=0;i<e_n;i++){File::Write_Text_Array(output,elements[i],e_d,' ');File::Write_Text(output,'\n');}}
	}
};


template<int d> class TetrahedronMesh : public SimplicialMesh<d,4>
{using VectorD=Vector<real,d>;using VectorDi=Vector<int,d>;using VectorEi=Vector4i;using Base=SimplicialMesh<d,4>;
public:
	TetrahedronMesh(const ArrayPtr<VectorD> _vertices=nullptr):Base(_vertices){}
};

template<int d> class TriangleMesh : public SimplicialMesh<d,3>
{using VectorD=Vector<real,d>;using VectorDi=Vector<int,d>;using VectorEi=Vector3i;using Base=SimplicialMesh<d,3>;
public:
	TriangleMesh(const ArrayPtr<VectorD> _vertices=nullptr):Base(_vertices){}
};

template<int d> class SegmentMesh : public SimplicialMesh<d,2>
{using VectorD=Vector<real,d>;using VectorDi=Vector<int,d>;using VectorEi=Vector2i;using Base=SimplicialMesh<d,2>;
public:
	SegmentMesh(const ArrayPtr<VectorD> _vertices=nullptr):Base(_vertices){}
};

////Helper functions

////Dimension and type conversion
////The reason to do dimension conversion on meshes is to guarantee the data can be visualized by our OpenGL viewer.
////Our current OpenGL viewer reads 3D data only. So if you want to visualize a 2D mesh, you need to convert it to 3D before writing to files.
template<class MESH_T1,class MESH_T2> void Dim_Conversion(const MESH_T1& mesh2,/*result*/MESH_T2& mesh3)
{
	mesh3.vertices->resize((int)(mesh2.vertices->size()));
	for(size_type i=0;i<(*mesh3.vertices).size();i++)
		Dim_Conversion<real,MESH_T1::Dim(),MESH_T2::Dim()>((*mesh2.vertices)[i],(*mesh3.vertices)[i],(real)0);
	mesh3.elements.resize((int)mesh2.elements.size());
	for(size_type i=0;i<mesh2.elements.size();i++)
		Dim_Conversion<int,MESH_T1::Element_Dim(),MESH_T2::Element_Dim()>(mesh2.elements[i],mesh3.elements[i],(int)-1);
}

namespace std{
template<> struct hash<Vector2i>
{typedef Vector2i argument_type;typedef std::size_t result_type;
	result_type operator()(argument_type const& arg) const
	{result_type const h1(std::hash<int>()(arg[0]));result_type const h2(std::hash<int>()(arg[1]));return h1^(h2<<1);}
};
template<> struct hash<Vector3i>
{typedef Vector3i argument_type;typedef std::size_t result_type;
	result_type operator()(argument_type const& arg) const
	{result_type const h1(std::hash<int>()(arg[0]));result_type const h2(std::hash<int>()(arg[1]));
	result_type const h3(std::hash<int>()(arg[2]));return h1^(h2<<1)^h3;}
};
template<> struct hash<Vector4i>
{typedef Vector4i argument_type;typedef std::size_t result_type;
	result_type operator()(argument_type const& arg) const
	{result_type const h1(std::hash<int>()(arg[0]));result_type const h2(std::hash<int>()(arg[1]));
	result_type const h3(std::hash<int>()(arg[2]));result_type const h4(std::hash<int>()(arg[3]));return h1^(h2<<1)^h3^(h4<<2);}
};}

inline Vector3 Normal(const Vector3& p1,const Vector3& p2,const Vector3& p3){return (p2-p1).cross(p3-p1).normalized();}

inline void Update_Normals(const TriangleMesh<3>& mesh,std::vector<Vector3>& normals)
{
    normals.resize(mesh.Vertices().size(),Vector3::Zero());
    for(const auto& v:mesh.elements){Vector3 n=Normal(mesh.Vertices()[v[0]],mesh.Vertices()[v[1]],mesh.Vertices()[v[2]]);for(int j=0;j<3;j++){normals[v[j]]+=n;}}
    for(auto& n:normals){n.normalize();}
}

inline void Update_Tangents(TriangleMesh<3>& mesh)
{
	mesh.Tangents().resize(mesh.Vertices().size());

	SMikkTSpaceInterface iTSpace;
	iTSpace.m_getNumFaces = [](const SMikkTSpaceContext * pContext) -> int {
		TriangleMesh<3>* mesh = (TriangleMesh<3>*)pContext->m_pUserData;
		return (int)mesh->Elements().size();
	};

	iTSpace.m_getNumVerticesOfFace = [](const SMikkTSpaceContext * pContext, const int iFace) -> int {
		TriangleMesh<3>* mesh = (TriangleMesh<3>*)pContext->m_pUserData;
		return (int)mesh->Elements()[iFace].size();
	};

	iTSpace.m_getPosition = [](const SMikkTSpaceContext * pContext, float fvPosOut[], const int iFace, const int iVert) -> void {
		TriangleMesh<3>* mesh = (TriangleMesh<3>*)pContext->m_pUserData;

		auto& face = mesh->Elements()[iFace];
		auto& vert = mesh->Vertices()[face(iVert)];

		fvPosOut[0] = (float)vert(0);
		fvPosOut[1] = (float)vert(1);
		fvPosOut[2] = (float)vert(2);
	};

	iTSpace.m_getNormal = [](const SMikkTSpaceContext * pContext, float fvNormOut[], const int iFace, const int iVert) -> void {
		TriangleMesh<3>* mesh = (TriangleMesh<3>*)pContext->m_pUserData;

		auto& face = mesh->Elements()[iFace];
		auto& norm = mesh->Normals()[face(iVert)];

		fvNormOut[0] = (float)norm(0);
		fvNormOut[1] = (float)norm(1);
		fvNormOut[2] = (float)norm(2);
	};

	iTSpace.m_getTexCoord = [](const SMikkTSpaceContext * pContext, float fvTexcOut[], const int iFace, const int iVert) -> void {
		TriangleMesh<3>* mesh = (TriangleMesh<3>*)pContext->m_pUserData;

		auto& face = mesh->Elements()[iFace];
		auto& uv = mesh->Uvs()[face(iVert)];

		fvTexcOut[0] = (float)uv(0);
		fvTexcOut[1] = (float)uv(1);
	};

	iTSpace.m_setTSpaceBasic = [](const SMikkTSpaceContext * pContext, const float fvTangent[], const float fSign, const int iFace, const int iVert) -> void {
		TriangleMesh<3>* mesh = (TriangleMesh<3>*)pContext->m_pUserData;

		auto& face = mesh->Elements()[iFace];
		auto& tangent = mesh->Tangents()[face(iVert)];

		tangent(0) = fvTangent[0];
		tangent(1) = fvTangent[1];
		tangent(2) = fvTangent[2];
		tangent(3) = fSign;
	};

	iTSpace.m_setTSpace = nullptr;


	SMikkTSpaceContext ctx;
	ctx.m_pInterface = &iTSpace;
	ctx.m_pUserData = &mesh;

	if (!genTangSpaceDefault(&ctx)) {
		std::cerr << "Failed to generate tangents." << std::endl;
	}
}

inline void Update_Uvs(const TriangleMesh<3>& mesh, std::vector<Vector2>& uvs)
{
	uvs.resize(mesh.Vertices().size(), Vector2::Zero());
}

inline int Element_Edges(const Vector3i& v,std::vector<Vector2i>& edges)
{edges[0]=Vector2i(v[0],v[1]);edges[1]=Vector2i(v[1],v[2]);edges[2]=Vector2i(v[2],v[0]);return 3;}

inline Vector2i Sorted(const Vector2i& v){return v[0]>v[1]?v:Vector2i(v[1],v[0]);}

template<int d> void Get_Edges(const TriangleMesh<d>& mesh,std::vector<Vector2i>& edges)
{
	Hashset<Vector2i> edge_hashset;std::vector<Vector2i> element_edges(3);
	for(const auto& vtx:mesh.elements){
		int n=Element_Edges(vtx,element_edges);
		for(int i=0;i<n;i++)edge_hashset.insert(Sorted(element_edges[i]));}
	for(const auto& edge:edge_hashset)edges.push_back(edge);
}

template<int d> void Subdivide(TriangleMesh<d>* mesh)
{
	std::vector<Vector2i> edges;Get_Edges(*mesh,edges);
	Hashtable<Vector2i,int> edge_vtx_hashtable;
	for(const auto& e:edges){
		Vector<real,d> pos=(real).5*(mesh->Vertices()[e[0]]+mesh->Vertices()[e[1]]);
		mesh->Vertices().push_back(pos);
		int i=(int)mesh->Vertices().size()-1;
		edge_vtx_hashtable.insert(std::make_pair(e,i));}

	auto n=mesh->elements.size();
	for(auto i=0;i<n;i++){const Vector3i v=mesh->elements[i];int v3,v4,v5;
		{auto search=edge_vtx_hashtable.find(Sorted(Vector2i(v[0],v[1])));if(search==edge_vtx_hashtable.end())continue;v3=search->second;}
		{auto search=edge_vtx_hashtable.find(Sorted(Vector2i(v[1],v[2])));if(search==edge_vtx_hashtable.end())continue;v4=search->second;}
		{auto search=edge_vtx_hashtable.find(Sorted(Vector2i(v[2],v[0])));if(search==edge_vtx_hashtable.end())continue;v5=search->second;}
		mesh->elements.push_back(Vector3i(v[0],v3,v5));
		mesh->elements.push_back(Vector3i(v3,v[1],v4));
		mesh->elements.push_back(Vector3i(v5,v4,v[2]));
		mesh->elements[i]=Vector3i(v3,v4,v5);}
}

inline void Initialize_Icosahedron_Mesh(const real scale,TriangleMesh<3>* mesh)
{
	////http://donhavey.com/blog/tutorials/tutorial-3-the-icosahedron-sphere/
	const real tao=1.61803399f;
	real vtx_pos[12][3]={{1,tao,0},{-1,tao,0},{1,-tao,0},{-1,-tao,0},{0,1,tao},{0,-1,tao},{0,1,-tao},{0,-1,-tao},{tao,0,1},{-tao,0,1},{tao,0,-1},{-tao,0,-1}};
	int ele[20][3]={{0,1,4},{1,9,4},{4,9,5},{5,9,3},{2,3,7},{3,2,5},{7,10,2},{0,8,10},{0,4,8},{8,2,10},{8,4,5},{8,5,2},{1,0,6},{11,1,6},{3,9,11},{6,10,7},{3,11,7},{11,6,7},{6,0,10},{9,1,11}};		

	mesh->Clear();
	int vtx_num=12;mesh->Vertices().resize(vtx_num);for(int i=0;i<vtx_num;i++){mesh->Vertices()[i]=Vector3(vtx_pos[i][0],vtx_pos[i][1],vtx_pos[i][2])*scale;}
	int ele_num=20;mesh->elements.resize(ele_num);for(int i=0;i<ele_num;i++)mesh->elements[i]=Vector3i(ele[i][0],ele[i][1],ele[i][2]);
}

inline void Initialize_Sphere_Mesh(const real r,TriangleMesh<3>* mesh,const int sub=2)
{
	Initialize_Icosahedron_Mesh(r,mesh);for(int i=0;i<sub;i++)Subdivide(mesh);
	for(auto& v:mesh->Vertices()){real length=v.norm();real rs=r/length;v*=rs;}
}

#endif

