/*
 * kmeans.c
 * 
 * Created on: Sep 9, 2013
 * 			Author: Amir Yazdanbakhsh <a.yazdanbakhsh@gatech.edu>
 */


#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include "benchmark.hpp"


#ifdef LIBVC_DYN


#include "rgbimage.cpp"
#include "distance.cpp"
#include "segmentation.cpp"


extern "C" double kernel_func(std::string& inImageName, std::string& outImageName)
{
	srand(0xfeedface);
	RgbImage srcImage;

	initRgbImage(&srcImage);
	loadRgbImage(inImageName.c_str(), &srcImage, 256);

  Clusters clusters;
  initClusters(&clusters, 6, 1);
  
  AxBenchTimer timer;
	segmentImage(&srcImage, &clusters, 1);
	uint64_t kernel_time = timer.nanosecondsSinceInit();

	if (outImageName.size())
		saveRgbImage(&srcImage, outImageName.c_str(), 255);
  
  freeClusters(&clusters);
	freeRgbImage(&srcImage);

	return ((double)kernel_time) / 1000000000.0;
}


#else


#include <libgen.h>
#include "versioningCompiler/Version.hpp"
#include "versioningCompiler/CompilerImpl/TAFFOCompiler.hpp"
#include "versioningCompiler/CompilerImpl/SystemCompiler.hpp"


void do_version(
	vc::Version::Builder& builder, 
	std::string label, 
	std::string& inImageName, 
	std::string& outImageName)
{
	std::cout << label << " version..." << std::endl;
	vc::version_ptr_t v = builder.build();
	if (!v->compile()) {
		std::cout << "libVC compilation failed" << std::endl;
		return;
	}

	typedef double (*kernel_func_t)(std::string&, std::string&);
	kernel_func_t taffo_kfp = (kernel_func_t)v->getSymbol("kernel_func");
	double time_accum = 0;
	for (int i=0; i<20; i++) {
		std::string real_out_name;
		if (i == 0)
		 	real_out_name = outImageName + "." + label;
		time_accum += taffo_kfp(inImageName, real_out_name);
	}
	std::cout << label << " version avg time = " << time_accum / 20.0 << " s" << std::endl;
	v->fold();
}


int main (int argc, const char* argv[])
{
	std::string inImageName  = argv[1];
	std::string outImageName = argv[2];
	FILE *infp = fopen(inImageName.c_str(), "r");
	int w, h;
	fscanf(infp, "%d,%d", &w, &h);
	fclose(infp);

	char *selfpath = strdup(argv[0]);
	std::string basedir = dirname(selfpath);
	free(selfpath);
	
	std::shared_ptr<vc::TAFFOCompiler> taffo = std::make_shared<vc::TAFFOCompiler>(
		"taffo", "", vc::TAFFOCompiler::Language::CXX, "", basedir, basedir+"/test.log");
	taffo->setDisableVRA(true);
	vc::compiler_ptr_t systemcpp = vc::make_compiler<vc::SystemCompiler>(
		"baseline", "c++", basedir, basedir+"/test.log", "/usr/bin");

	vc::Version::Builder builder;
	builder.addSourceFile(basedir + "/../src/kmeans.cpp");
	builder.addIncludeDir(basedir + "/../src");
	builder.addIncludeDir(basedir + "/../../common/src");
	builder.addFunctionFlag("LIBVC_DYN");
	builder._functionName.push_back("kernel_func");
	builder._optionList.push_back(vc::Option("o", "-O", "3"));
	std::string imgw = std::to_string(w);
	std::string imgh = std::to_string(h);
	builder.addDefine("IMAGE_W", imgw.c_str());
	builder.addDefine("IMAGE_H", imgh.c_str());

	vc::Version::Builder taffoBuilder(builder);
	taffoBuilder._compiler = taffo;

	builder._compiler = systemcpp;
	
	do_version(builder, "baseline", inImageName, outImageName);
	do_version(taffoBuilder, "taffo", inImageName, outImageName);

	return 0;
}


#endif
