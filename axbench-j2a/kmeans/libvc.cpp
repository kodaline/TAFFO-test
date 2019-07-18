#include <algorithm>
#include <iostream>
#include <libgen.h>
#include <string.h>

#include "versioningCompiler/CompilerImpl/SystemCompilerOptimizer.hpp"
#include "versioningCompiler/CompilerImpl/TAFFOCompiler.hpp"
#include "versioningCompiler/Version.hpp"
#include "benchmark.hpp"

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
		if (i == 0)
		 	real_out_name = outImageName + "." + label;
		time_accum = kfp(inImageName, real_out_name);
		times.push_back(time_accum);
	}
	std::sort(times.begin(), times.end());
	std::cout << label << " version median time = " << times[times.size()/2] << " s" << std::endl;
	v->fold();
	return comp_t;
}


int main(int argc, const char* argv[])
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

	std::shared_ptr<vc::TAFFOCompiler> taffo =
			std::make_shared<vc::TAFFOCompiler>(
					"taffo",
					"/usr/local",
					vc::TAFFOCompiler::Language::CXX,
					"../../../dist/usr/local",
					".",
					"./test.log",
					"../../../dist/usr/local/bin/taffo-j2a");
	//taffo->setDisableVRA(true);
	vc::compiler_ptr_t systemcpp = vc::make_compiler<vc::SystemCompilerOptimizer>(
		"baseline", "clang++", "opt", ".", "./test.log", "/usr/local/bin", "/usr/local/bin");

	vc::Version::Builder builder;
	builder.addSourceFile("src/kmeans.cpp");
	builder.addSourceFile("src/distance.cpp");
	builder.addSourceFile("src/rgbimage.cpp");
	builder.addSourceFile("src/segmentation.cpp");
	builder.addIncludeDir("src");
	builder.addIncludeDir("../common/src");
	builder.addFunctionFlag("LIBVC_DYN");
	builder._functionName.push_back("kernel_func");
	builder._optionList.push_back(vc::Option("o", "-O", "3"));
	builder._genIROptionList.push_back(vc::Option("fpic", "-fPIC", ""));
	std::string imgw = std::to_string(w);
	std::string imgh = std::to_string(h);

	vc::Version::Builder taffoBuilder(builder);
	taffoBuilder._compiler = taffo;

	builder._compiler = systemcpp;

	taffoBuilder._genIROptionList.push_back(vc::Option("AnnotationInserter", "annotationFile", "./annotations.json"));
	taffoBuilder._genIROptionList.push_back(vc::Option("AnnotationInserter","-I","/usr/local/lib/clang/8.0.1/include/"));

	vc::Version::Builder defineBuilder(builder);
	builder.addDefine("IMAGE_W", imgw.c_str());
	builder.addDefine("IMAGE_H", imgh.c_str());

	vc::Version::Builder taffoDefineBuilder(taffoBuilder);
	taffoDefineBuilder.addDefine("IMAGE_W", imgw.c_str());
	taffoDefineBuilder.addDefine("IMAGE_H", imgh.c_str());

	do_version(builder, "baseline", inImageName, outImageName, true);
	double compile_t = do_version(taffoBuilder, "taffo", inImageName, outImageName, true);
	do_version(defineBuilder, "baseline+define", inImageName, "", true);
	do_version(taffoDefineBuilder, "taffo+define", inImageName, "", true);

	std::cout << "compilation time: " << compile_t << " s" << std::endl;


	return 0;
}
