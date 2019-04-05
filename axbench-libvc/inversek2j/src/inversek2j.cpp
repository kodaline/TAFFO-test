/*
 * inversek2j.cpp
 * 
 *  Created on: Sep. 10 2013
 *			Author: Amir Yazdanbakhsh <yazdanbakhsh@wisc.edu>
 */

#include <iostream>
#include <cstdlib>
#include <fstream> 

#include <time.h>
#include <iomanip>
#include <string> 

#include "benchmark.hpp"

#define PI 3.141592653589


#ifdef LIBVC_DYN


#include "kinematics.hpp"
#include "kinematics.cpp"


extern "C" void kernel_func(float *t1t2xy_flt, int n)
{
	float* __attribute((annotate("target('t1t2xy') scalar(range(-1.57,1.57))"))) t1t2xy = (float*)malloc(n * 2 * 2 * sizeof(float));
	for (int i=0; i<n*2*2; i+=2*2) {
		t1t2xy[i] = t1t2xy_flt[i];
		t1t2xy[i + 1] = t1t2xy_flt[i + 1];
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

	for (int i=0; i<n*2*2; i+=1) {
		t1t2xy_flt[i] = t1t2xy[i];
	}
}


#else

#include "versioningCompiler/Version.hpp"
#include "versioningCompiler/CompilerImpl/TAFFOCompiler.hpp"

int main(int argc, const char* argv[])
{
	int n;
	std::string inputFilename	= argv[1];
	std::string outputFilename 	= argv[2];

	// prepare the output file for writting the theta values
	std::ofstream outputFileHandler;
	outputFileHandler.open(outputFilename);

	// prepare the input file for reading the theta data
	std::ifstream inputFileHandler (inputFilename, std::ifstream::in);

	// first line defins the number of enteries
	inputFileHandler >> n;

	float* t1t2xy = (float*)malloc(n * 2 * 2 * sizeof(float));

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
	
	vc::compiler_ptr_t taffo = vc::make_compiler<vc::TAFFOCompiler>(
    "taffo", "", "", "", "", ".", "./test.log");
	vc::Version::Builder builder;
	builder.addSourceFile("../src/inversek2j.cpp");
	builder.addIncludeDir("../src");
	builder.addIncludeDir("../../common/src");
	builder.addFunctionFlag("LIBVC_DYN");
	builder._functionName.push_back("kernel_func");
	builder._compiler = taffo;
	std::cout << "libVC compilation in progress..." << std::endl;
	vc::version_ptr_t v = builder.build();
	if (!v->compile()) {
		std::cout << "libVC compilation failed" << std::endl;
		return 1;
	} else {
		std::cout << "libVC compilation OK" << std::endl;
	}
	typedef void (*kernel_func_t)(float *, int);
	kernel_func_t kfp = (kernel_func_t)v->getSymbol("kernel_func");
	kfp(t1t2xy, n);
	
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

#endif
