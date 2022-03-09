#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <string>

using namespace std;

void Test_Array()
{
	//vector<int> array={1,2,3,4};
	vector<int> array;
	//array.resize(5,1);
	array.resize(5);

	vector<float> array2;

	for(int i=0;i<array.size();i++){
		cout<<array[i]<<endl;}
	
	array[1]=0;
	array.push_back(5);
	for(auto& a:array){
		cout<<a<<endl;
	}

	array.clear();
}

void Test_Grammar()
{
	////&& - and; || - or; ! - not
	int a=0;int b=1;
	if(!(a>-1))cout<<"a<b"<<endl;
	else cout<<"a>=b"<<endl;

	a=0;
	b=1;
	while(a<5){
		a++;	////a=a+1; ////a+=1;
		int c=b+(a++);
		cout<<"a = "<<a<<endl;
	}

	do{
		a++;
	}while(a<5);

	int d=0;
	switch(d){
	case 0:
		cout<<"case 0"<<endl;
		break;
	case 1:
		cout<<"case 1"<<endl;
		break;
	default:
		cout<<"case default"<<endl;
		break;
	}
}

void Test_Containers()
{
	set<int> s={1,2,3,4};
	s.insert(5);
	s.insert(2);
	//s.erase(2);
	//s.clear();

	for(auto& a:s){
		cout<<"set ele "<<a<<endl;
	}

	auto result=s.find(6);
	if(result!=s.end()){
		cout<<"find "<<(*result)<<endl;
	}

	map<int,string> m;
	m[1]="one";
	m[3]="three";
	for(auto& iter:m){
		std::cout<<"map ele key: "<<iter.first
			<<", value: "<<iter.second<<endl;
	}

	auto result2=m.find(1);
	if(result2!=m.end()){
		cout<<(*result2).first<<", "<<(*result2).second<<endl;
	}

	unordered_set<int> hashset;
	unordered_map<int,string> hashmap;
}

int main()
{
	//Test_Container();
	//Test_Grammar();
	Test_Containers();
}
