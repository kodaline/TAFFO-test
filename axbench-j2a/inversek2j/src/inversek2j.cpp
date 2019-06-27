/*
 * inversek2j.cpp
 * 
 *  Created on: Sep. 10 2013
 *			Author: Amir Yazdanbakhsh <yazdanbakhsh@wisc.edu>
 */

#include <iostream>
#include <cstdlib>
#include "kinematics.hpp"
#include <fstream> 

#include <time.h>
#include <iomanip>
#include <string> 

#include "benchmark.hpp"


#define PI 3.141592653589


extern "C" void kernel_func(const float *l1l2t1t2xy_flt, int n, const std::string& outputFilename)
{
	float* __attribute((annotate("target('t1t2xy') scalar(range(-1.57,1.57))"))) l1l2t1t2xy = (float*)malloc(n * 2 * 2 * sizeof(float));
	for (int i=0; i<n*2*2; i+=2*2) {
		l1l2t1t2xy[i] = l1l2t1t2xy_flt[i];
		l1l2t1t2xy[i + 1] = l1l2t1t2xy_flt[i + 1];
	}

	AxBenchTimer timer;

	int curr_index1 = 0;
	for(int i = 0 ; i < n * 2 * 2; i += 2 * 2)
	{
		forwardk2j(l1l2t1t2xy[i+0], l1l2t1t2xy[i+1], l1l2t1t2xy + (i + 2), l1l2t1t2xy + (i + 3));
	}

	for(int i = 0 ; i < n * 2 * 2; i += 2 * 2)
	{
		inversek2j(l1l2t1t2xy[i+0], l1l2t1t2xy[i+1], l1l2t1t2xy + (i + 0), l1l2t1t2xy + (i + 1));
	}

	uint64_t time = timer.nanosecondsSinceInit();
	std::cout << "kernel time = " << ((double)time) / 1000000000.0 << " s\n";

	// prepare the output file for writing the theta values
	std::ofstream outputFileHandler;
	outputFileHandler.open(outputFilename);

	for(int i = 0 ; i < n * 2 * 2; i += 2 * 2)
	{
		outputFileHandler << l1l2t1t2xy[i+0] << "\t" << l1l2t1t2xy[i+1] << "\n";
	}
	
	outputFileHandler.close();
	
	free(l1l2t1t2xy);
}
