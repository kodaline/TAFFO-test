/*
 * sobel.cpp
 * 
 * Created on: Sep 9, 2013
 * 			Author: Amir Yazdanbakhsh <a.yazdanbakhsh@gatech.edu>
 */
#include <iostream>
#include <cmath>
#include <cstring>
#include "benchmark.hpp"


#ifdef LIBVC_DYN


#include "rgb_image.hpp"
#include "convolution.hpp"
#include "convolution.cpp"
#include "rgb_image.cpp"


extern "C" double kernel_func(std::string& inImageName, std::string& outImageName)
{
	int x, y;
	float __attribute((annotate("range -128 127"))) s = 0;

	// Source and destination image	
	Image srcImage;
	Image dstImage;
	Image *srcImagePtr = &srcImage;
	Image *dstImagePtr = &dstImage;

	float __attribute((annotate("range -128 127"))) w[][3] = {
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0}
	};

  srcImagePtr->loadRgbImage(inImageName); // source image
	dstImagePtr->loadRgbImage(inImageName); // destination image

	srcImagePtr->makeGrayscale( ); // convert the source file to grayscale

	y = 0 ;
	
	AxBenchTimer timer;
	
	// Start performing Sobel operation
	for( x = 0 ; x < srcImagePtr->width ; x++ ) {
		HALF_WINDOW(srcImagePtr, x, y, w) ;


			s = sobel(w);


		dstImagePtr->putPixel_r(x, y, s) ;
		dstImagePtr->putPixel_g(x, y, s) ;
		dstImagePtr->putPixel_b(x, y, s) ;
	}

	for (y = 1 ; y < (srcImagePtr->height - 1) ; y++) {
		x = 0 ;
		HALF_WINDOW(srcImagePtr, x, y, w);

			s = sobel(w);

	
		dstImagePtr->putPixel_r(x, y, s) ;
		dstImagePtr->putPixel_g(x, y, s) ;
		dstImagePtr->putPixel_b(x, y, s) ;


		for( x = 1 ; x < srcImagePtr->width - 1 ; x++ ) {
			WINDOW(srcImagePtr, x, y, w) ;
				
				s = sobel(w);

			dstImagePtr->putPixel_r(x, y, s) ;
			dstImagePtr->putPixel_g(x, y, s) ;
			dstImagePtr->putPixel_b(x, y, s) ;

		}

		x = srcImagePtr->width - 1 ;
		HALF_WINDOW(srcImagePtr, x, y, w) ;
		

			s = sobel(w);

		dstImagePtr->putPixel_r(x, y, s) ;
		dstImagePtr->putPixel_g(x, y, s) ;
		dstImagePtr->putPixel_b(x, y, s) ;
	}

	y = srcImagePtr->height - 1;

	for(x = 0 ; x < srcImagePtr->width ; x++) {
		HALF_WINDOW(srcImagePtr, x, y, w) ;
		
			s = sobel(w);

		dstImagePtr->putPixel_r(x, y, s) ;
		dstImagePtr->putPixel_g(x, y, s) ;
		dstImagePtr->putPixel_b(x, y, s) ;

	}
	
	uint64_t kernel_time = timer.nanosecondsSinceInit();
	
	if (!outImageName.empty())
	  dstImagePtr->saveRgbImage(outImageName, std::sqrt(256 * 256 + 256 * 256)) ;
	
	return ((double)kernel_time) / 1000000000.0;
}


#else


#include <algorithm>
#include <libgen.h>
#include <vector>
#include "versioningCompiler/Version.hpp"
#include "versioningCompiler/CompilerImpl/TAFFOCompiler.hpp"
#include "versioningCompiler/CompilerImpl/SystemCompilerOptimizer.hpp"


double do_version(
	vc::Version::Builder& builder,
	std::string label,
	std::string inImageName,
	std::string outImageName,
	bool split_compile)
{
	std::cout << label << " version..." << std::endl;
	vc::version_ptr_t v = builder.build();
	AxBenchTimer compile_time;
	if (split_compile) {
		if (!v->prepareIR()) {
			std::cout << "libVC compilation failed" << std::endl;
			return -1.0;
		}
	}
	if (!v->compile()) {
		std::cout << "libVC compilation failed" << std::endl;
		return -1.0;
	}
	double comp_t = compile_time.nanosecondsSinceInit() / 1000000000.0;

	typedef double (*kernel_func_t)(std::string&, std::string&);
	kernel_func_t kfp = (kernel_func_t)v->getSymbol("kernel_func");
	double time_accum = 0;
	std::vector<double> times;
	for (int i=0; i<21; i++) {
		std::string real_out_name;
		if (i == 0 && !outImageName.empty())
		 	real_out_name = outImageName + "." + label;
		time_accum = kfp(inImageName, real_out_name);
		times.push_back(time_accum);
	}
	std::sort(times.begin(), times.end());
	std::cout << label << " version median time = " << times[times.size()/2] << " s" << std::endl;
	v->fold();
	return comp_t;
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
		"taffo", "/usr/local", vc::TAFFOCompiler::Language::CXX, "", basedir, basedir+"/test.log");
	taffo->setDisableVRA(true);
	taffo->setRestrictiveFunctionCloning(true);
	vc::compiler_ptr_t systemcpp = vc::make_compiler<vc::SystemCompilerOptimizer>(
		"baseline", "clang++", "opt", basedir, basedir+"/test.log", "/usr/local/bin", "/usr/local/bin");

	vc::Version::Builder builder;
	builder.addSourceFile(basedir + "/../src/sobel.cpp");
	builder.addIncludeDir(basedir + "/../src");
	builder.addIncludeDir(basedir + "/../../common/src");
	builder.addFunctionFlag("LIBVC_DYN");
	builder._functionName.push_back("kernel_func");
	builder._optionList.push_back(vc::Option("o", "-O", "3"));
	std::string imgw = std::to_string(w);
	std::string imgh = std::to_string(h);

	vc::Version::Builder taffoBuilder(builder);
	taffoBuilder._compiler = taffo;

	builder._compiler = systemcpp;

	vc::Version::Builder defineBuilder(builder);
	builder.addDefine("IMAGE_WIDTH_CONST", imgw.c_str());
	builder.addDefine("IMAGE_HEIGHT_CONST", imgh.c_str());

	vc::Version::Builder taffoDefineBuilder(taffoBuilder);
	taffoDefineBuilder.addDefine("IMAGE_WIDTH_CONST", imgw.c_str());
	taffoDefineBuilder.addDefine("IMAGE_HEIGHT_CONST", imgh.c_str());

	do_version(builder, "baseline", inImageName, outImageName, true);
	double compile_t = do_version(taffoBuilder, "taffo", inImageName, outImageName, true);
	do_version(defineBuilder, "baseline+define", inImageName, "", true);
	do_version(taffoDefineBuilder, "taffo+define", inImageName, "", true);

	std::cout << "compilation time: " << compile_t << " s" << std::endl;

	return 0;
}


#endif

