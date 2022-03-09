#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <Eigen/Dense>

using namespace std;

void Test_Pointer_Reference()
{
	////reference and pointer for variable
	int a=1;
	int b=a;
	int& c=a;	////reference
	int* d=&a;	////pointer

	std::cout<<"a = "<<a<<", b = "<<b<<", c = "<<c<<", d = "<<*d<<std::endl;

	a=2;
	std::cout<<"a = "<<a<<", b = "<<b<<", c = "<<c<<", d = "<<*d<<std::endl;

	b=3;
	std::cout<<"a = "<<a<<", b = "<<b<<", c = "<<c<<", d = "<<*d<<std::endl;

	c=4;
	std::cout<<"a = "<<a<<", b = "<<b<<", c = "<<c<<", d = "<<*d<<std::endl;

	*d=5;
	std::cout<<"a = "<<a<<", b = "<<b<<", c = "<<c<<", d = "<<*d<<std::endl;

	////reference and pointer for array 
	std::vector<int> array_a={1,1,1};
	std::vector<int> array_b=array_a;
	std::vector<int>& array_c=array_a;
	std::vector<int>* array_d=&array_a;
	
	std::cout<<"\n\narray_a: ";for(auto& x:array_a)std::cout<<x<<", ";
	std::cout<<"\narray_b: ";for(auto& x:array_b)std::cout<<x<<", ";
	std::cout<<"\narray_c: ";for(auto& x:array_c)std::cout<<x<<", ";
	std::cout<<"\narray_d: ";for(auto& x:(*array_d))std::cout<<x<<", ";

	array_a[0]=2;
	std::cout<<"\n\narray_a: ";for(auto& x:array_a)std::cout<<x<<", ";
	std::cout<<"\narray_b: ";for(auto& x:array_b)std::cout<<x<<", ";
	std::cout<<"\narray_c: ";for(auto& x:array_c)std::cout<<x<<", ";
	std::cout<<"\narray_d: ";for(auto& x:(*array_d))std::cout<<x<<", ";

	array_b[0]=3;
	std::cout<<"\n\narray_a: ";for(auto& x:array_a)std::cout<<x<<", ";
	std::cout<<"\narray_b: ";for(auto& x:array_b)std::cout<<x<<", ";
	std::cout<<"\narray_c: ";for(auto& x:array_c)std::cout<<x<<", ";
	std::cout<<"\narray_d: ";for(auto& x:(*array_d))std::cout<<x<<", ";

	array_c[0]=4;
	std::cout<<"\n\narray_a: ";for(auto& x:array_a)std::cout<<x<<", ";
	std::cout<<"\narray_b: ";for(auto& x:array_b)std::cout<<x<<", ";
	std::cout<<"\narray_c: ";for(auto& x:array_c)std::cout<<x<<", ";
	std::cout<<"\narray_d: ";for(auto& x:(*array_d))std::cout<<x<<", ";
	
	(*array_d)[0]=5;
	std::cout<<"\n\narray_a: ";for(auto& x:array_a)std::cout<<x<<", ";
	std::cout<<"\narray_b: ";for(auto& x:array_b)std::cout<<x<<", ";
	std::cout<<"\narray_c: ";for(auto& x:array_c)std::cout<<x<<", ";
	std::cout<<"\narray_d: ";for(auto& x:(*array_d))std::cout<<x<<", ";

	////reference and pointer on an array element
	int val=array_a[0];
	int& val_ref=array_a[0];
	int* val_ptr=&array_a[0];

	val_ref=6;
	std::cout<<"\n\narray_a: ";for(auto& x:array_a)std::cout<<x<<", ";

	*val_ptr=7;
	std::cout<<"\n\narray_a: ";for(auto& x:array_a)std::cout<<x<<", ";
}

void Test_Macro()
{
#define a 10
	std::cout<<"a="<<a<<std::endl;

#define for_loop(i,start,end) \
	for(int i=start;i<end;i++)

#define for_loop_2d(i,j,start,end) \
	for(int i=start;i<end;i++)	\
		for(int j=start;j<end;j++)

	for_loop(i,0,10){
		std::cout<<i<<", ";
	}

	for_loop_2d(i,j,0,10){
		std::cout<<"["<<i<<","<<j<<"], ";
	}
}

void Test_Eigen()
{
	using Vector2=Eigen::Matrix<float,2,1>;
	Vector2 v=Vector2(1,2);
	Vector2 v2=Vector2(2,3);
	Vector2 v3=v+v2;
	float dot_prod=v.dot(v2);
	float norm=v.norm();
	Vector2 normal=v.normalized();
	v.normalize();

	std::cout<<"v: "<<v.transpose()<<std::endl;
}


int main()
{
	Test_Eigen();
	Test_Pointer_Reference();
	Test_Macro();
}
