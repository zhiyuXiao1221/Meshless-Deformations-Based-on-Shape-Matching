#ifndef __Driver_h__
#define __Driver_h__
#include "Common.h"
#include "File.h"

class Driver
{
public:
	int test=1;
	std::string output_dir="output";
	std::string frame_dir;
	int first_frame=0,last_frame=200,current_frame=0;
	real frame_rate=50;
	real time=(real)0,current_time=(real)0;
	int scale=1;
	real time_step=(real)1;
	bool verbose=true;

	real Time_At_Frame(const int frame){return (real)frame/frame_rate;}
	int Frame_At_Time(const real time){return (int)((real)time*frame_rate);}
	virtual real Timestep(){return time_step;}

	virtual void Initialize(){}

	virtual void Run()
	{
		Write_Output_Files(current_frame);
		while(current_frame<last_frame){
			current_frame++;
			Advance_To_Target_Time(Time_At_Frame(current_frame));
			Write_Output_Files(current_frame);}
	}

	virtual void Advance_To_Target_Time(const real target_time)
	{
		bool done=false;
		for(int substep=1;!done;substep++){
			real dt=Timestep();
			if(time+dt>=target_time){dt=target_time-time;done=true;}
			else if(time+2*dt>=target_time){dt=(real).5*(target_time-time);}
			Advance_One_Time_Step(dt,time);
			time+=dt;}
	}

	virtual void Advance_One_Time_Step(const real dt,const real time)
	{
		if(verbose)std::cout<<"Advance one time step by dt="<<dt<<" to time "<<time<<std::endl;
	}

	virtual void Write_Output_Files(const int frame)
	{	
		if(frame==0){
			if(!File::Directory_Exists(output_dir.c_str()))
				File::Create_Directory(output_dir);}

		frame_dir=output_dir+"/"+std::to_string(frame);
		if(!File::Directory_Exists(frame_dir.c_str()))File::Create_Directory(frame_dir);
		
		{std::string file_name=output_dir+"/0/last_frame.txt";
		File::Write_Text_To_File(file_name,std::to_string(frame));}

		if(verbose)std::cout<<"Write output files for frame "<<frame<<" to path: "<<frame_dir<<std::endl;
	}
};
#endif
