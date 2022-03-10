//#####################################################################
// Main
// Dartmouth COSC 89.18/189.02: Computational Methods for Physical Systems, Assignment starter code
// Contact: Bo Zhu (bo.zhu@dartmouth.edu)
//#####################################################################
#include <iostream>
#include "ParticleDeformableDriver.h"
#ifndef __Main_cpp__
#define __Main_cpp__

int main(int argc,char* argv[])
{
	//// change the driver id to switch between sand and fluid 
	int driver=1;

	switch(driver){
	case 1:{
		ParticleDeformableDriver<3> driver;
		driver.Initialize();
		driver.Run();	
	}break;
	}
}

#endif
