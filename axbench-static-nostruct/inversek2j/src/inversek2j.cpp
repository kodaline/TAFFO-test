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

#ifdef _MIOSIX
#include <stdlib.h>
#else
#include <boost/math/special_functions/fpclassify.hpp>
#endif

int main(int argc, const char* argv[])
{

	int n;
	
	#ifdef _MIOSIX
	  std::string inputFilename	    = INPUT;
	  std::string outputFilename 	= OUTPUT;
	#else
	  std::string inputFilename	    = argv[1];
	  std::string outputFilename 	= argv[2];
	#endif

	// prepare the output file for writting the theta values
	std::ofstream outputFileHandler;
	outputFileHandler.open(outputFilename);

	// prepare the input file for reading the theta data
	std::ifstream inputFileHandler (inputFilename, std::ifstream::in);

	// first line defins the number of enteries
	inputFileHandler >> n;



	float* __attribute((annotate("no_float 4 28")))t1t2xy = (float*)malloc(n * 2 * 2 * sizeof(float));

	if(t1t2xy == NULL)
	{
		std::cerr << "# Cannot allocate memory for the coordinates an angles!" << std::endl;
		return -1 ;
	}

	srand (time(NULL));
	
	for (int i=0; i<n*2*2; i+=2*2) {
		float theta1, theta2;
		inputFileHandler >> theta1 >> theta2;

		t1t2xy[i] = theta1;
		t1t2xy[i + 1] = theta2;
	}

	AxBenchTimer timer;

	int curr_index1 = 0;		
	for(int i = 0 ; i < n * 2 * 2 ; i += 2 * 2)
	{
		forwardk2j(t1t2xy[i + 0], t1t2xy[i + 1], t1t2xy + (i + 2), t1t2xy + (i + 3));
	}

	for(int i = 0 ; i < n * 2 * 2 ; i += 2 * 2)
	{
		inversek2j(t1t2xy[i + 2], t1t2xy[i + 3], t1t2xy + (i + 0), t1t2xy + (i + 1));
	}
	
	uint64_t time = timer.nanosecondsSinceInit();
	std::cout << "kernel time = " << ((double)time) / 1000000000.0 << " s\n";

	for(int i = 0 ; i < n * 2 * 2 ; i += 2 * 2)
	{
		outputFileHandler <<  t1t2xy[i+0] << "\t" << t1t2xy[i+1] << "\n";
	}

	inputFileHandler.close();
	outputFileHandler.close();

	free(t1t2xy) ; 
	t1t2xy = NULL ;

	return 0 ;
}
