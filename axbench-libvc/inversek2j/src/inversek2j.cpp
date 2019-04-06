/*
 * inversek2j.cpp
 * 
 *  Created on: Sep. 10 2013
 *			Author: Amir Yazdanbakhsh <yazdanbakhsh@wisc.edu>
 */

#include <iostream>
#include <cstdlib>
#include <libgen.h>
#include <fstream> 

#include <time.h>
#include <iomanip>
#include <string> 

#include "benchmark.hpp"

#define PI 3.141592653589


#ifdef LIBVC_DYN


#include "kinematics.hpp"
#include "kinematics.cpp"


extern "C" void kernel_func(const float *l1l2t1t2xy_flt, int n, const std::string& outputFilename)
{
	float* __attribute((annotate("target('t1t2xy') scalar(range(-1.57,1.57))"))) l1l2t1t2xy = (float*)malloc(n * 2 * 2 * 2 * sizeof(float));
	for (int i=0; i<n*2*2*2; i+=2*2*2) {
		l1l2t1t2xy[i] = l1l2t1t2xy_flt[i];
		l1l2t1t2xy[i + 1] = l1l2t1t2xy_flt[i + 1];
		l1l2t1t2xy[i + 2] = l1l2t1t2xy_flt[i + 2];
		l1l2t1t2xy[i + 3] = l1l2t1t2xy_flt[i + 3];
	}

	AxBenchTimer timer;

	int curr_index1 = 0;
	for(int i = 0 ; i < n * 2 * 2 * 2; i += 2 * 2 * 2)
	{
		forwardk2j(l1l2t1t2xy[i+0], l1l2t1t2xy[i+1], l1l2t1t2xy[i + 2], l1l2t1t2xy[i + 3], l1l2t1t2xy + (i + 4), l1l2t1t2xy + (i + 5));
	}

	for(int i = 0 ; i < n * 2 * 2 * 2; i += 2 * 2 * 2)
	{
		inversek2j(l1l2t1t2xy[i+0], l1l2t1t2xy[i+1], l1l2t1t2xy[i + 4], l1l2t1t2xy[i + 5], l1l2t1t2xy + (i + 2), l1l2t1t2xy + (i + 3));
	}

	uint64_t time = timer.nanosecondsSinceInit();
	std::cout << "kernel time = " << ((double)time) / 1000000000.0 << " s\n";

	// prepare the output file for writing the theta values
	std::ofstream outputFileHandler;
	outputFileHandler.open(outputFilename);

	for(int i = 0 ; i < n * 2 * 2 * 2; i += 2 * 2 * 2)
	{
		outputFileHandler << l1l2t1t2xy[i+2] << "\t" << l1l2t1t2xy[i+3] << "\n";
	}
	
	outputFileHandler.close();
	
	free(l1l2t1t2xy);
}


#else

#include "versioningCompiler/Version.hpp"
#include "versioningCompiler/CompilerImpl/TAFFOCompiler.hpp"
#include "versioningCompiler/CompilerImpl/SystemCompiler.hpp"

int main(int argc, const char* argv[])
{
	int n;
	std::string inputFilename	= argv[1];
	std::string outputFilename 	= argv[2];

	// prepare the input file for reading the theta data
	std::ifstream inputFileHandler (inputFilename, std::ifstream::in);

	// first line defins the number of enteries
	inputFileHandler >> n;

	float* l1l2t1t2xy = (float*)malloc(n * 2 * 2 * 2 * sizeof(float));

	if(l1l2t1t2xy == NULL)
	{
		std::cerr << "# Cannot allocate memory for the coordinates an angles!" << std::endl;
		return -1 ;
	}

	srand (time(NULL));

	for (int i=0; i<n*2*2*2; i+=2*2*2) {
		float l1, l2, theta1, theta2;
		inputFileHandler >> l1 >> l2 >> theta1 >> theta2;

		l1l2t1t2xy[i + 0] = l1;
		l1l2t1t2xy[i + 1] = l2;
		l1l2t1t2xy[i + 2] = theta1;
		l1l2t1t2xy[i + 3] = theta2;
	}
	
	char *selfpath = strdup(argv[0]);
	std::string basedir = dirname(selfpath);
	free(selfpath);

	vc::compiler_ptr_t taffo = vc::make_compiler<vc::TAFFOCompiler>(
		"taffo", "", vc::TAFFOCompiler::Language::CXX, "", basedir, basedir+"/test.log");
	vc::compiler_ptr_t systemcpp = vc::make_compiler<vc::SystemCompiler>(
		"baseline", "c++", basedir, basedir+"/test.log", "/usr/bin");

	vc::Version::Builder builder;
	builder.addSourceFile(basedir + "/../src/inversek2j.cpp");
	builder.addIncludeDir(basedir + "/../src");
	builder.addIncludeDir(basedir + "/../../common/src");
	builder.addFunctionFlag("LIBVC_DYN");
	builder._functionName.push_back("kernel_func");
	builder._optionList.push_back(vc::Option("o", "-O", "3"));
	
	for (int i=-1; i<4; i++) {
		vc::Version::Builder thisBuilder = builder;
		std::string vOutFn;

		if (i < 0) {
			thisBuilder._compiler = systemcpp;
			vOutFn = outputFilename;
		} else {
			thisBuilder._compiler = taffo;
			double al1 = i % 2 ? 2.0 : 1.0;
			double al2 = (i / 2) % 2 ? 2.0 : 1.0;
			thisBuilder._optionList.push_back(vc::TAFFOCompiler::getScalarAnnotationDefine("ANNOTATION_L1", al1, al1));
			thisBuilder._optionList.push_back(vc::TAFFOCompiler::getScalarAnnotationDefine("ANNOTATION_L2", al2, al2));
			vOutFn = outputFilename + ".v" + std::to_string(i);
		}

		std::cout << "libVC compilation in progress (v = " << i << ")..." << std::endl;
		vc::version_ptr_t v = thisBuilder.build();
		if (!v->compile()) {
			std::cout << "libVC compilation failed" << std::endl;
			return 1;
		} else {
			std::cout << "libVC compilation OK" << std::endl;
		}
		
		typedef void (*kernel_func_t)(float *, int, const std::string&);
		kernel_func_t kfp = (kernel_func_t)v->getSymbol("kernel_func");
		kfp(l1l2t1t2xy, n, vOutFn);

		v->fold();
	}

	inputFileHandler.close();

	free(l1l2t1t2xy) ; 

	return 0 ;
}

#endif
