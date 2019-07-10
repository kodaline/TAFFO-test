#include <algorithm>
#include <iostream>
#include <libgen.h>
#include <string.h>
#include "benchmark.hpp"

#include "versioningCompiler/CompilerImpl/SystemCompilerOptimizer.hpp"
#include "versioningCompiler/CompilerImpl/TAFFOCompiler.hpp"
#include "versioningCompiler/Version.hpp"

double do_version(
	vc::Version::Builder& builder,
	std::string label,
	std::string inputFile,
	std::string outputFile,
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
		if (i == 0 && !outputFile.empty())
		 	real_out_name = outputFile + "." + label;
		time_accum = kfp(inputFile, real_out_name);
		times.push_back(time_accum);
	}
	std::sort(times.begin(), times.end());
	std::cout << label << " version median time = " << times[times.size()/2] << " s" << std::endl;
	v->fold();
	return comp_t;
}

int main(int argc, const char* argv[])
{

	std::string inputFile = argv[1];
	std::string outputFile = argv[2];

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

	vc::compiler_ptr_t systemcpp = vc::make_compiler<vc::SystemCompilerOptimizer>(
		"baseline", "clang++", "opt", ".", "./test.log", "/usr/local/bin", "/usr/local/bin");

	vc::Version::Builder builder;

	builder.addSourceFile("src/blackscholes.cpp");
	builder.addIncludeDir("src");
	builder.addIncludeDir("../common/src");
	builder.addFunctionFlag("LIBVC_DYN");
	builder._functionName.push_back("kernel_func");
	builder._optionList.push_back(vc::Option("o", "-O", "3"));
	builder._genIROptionList.push_back(vc::Option("fpic", "-fPIC", ""));

	vc::Version::Builder taffoBuilder(builder);

	taffoBuilder._genIROptionList.push_back(vc::Option(
						"AnnotationInserter", "annotationFile", "./annotations.json"));

	taffoBuilder._genIROptionList.push_back(vc::Option("AnnotationInserter",
						"-I",
						"/usr/local/lib/clang/8.0.1/include/"));

	taffoBuilder._compiler = taffo;
	builder._compiler = systemcpp;

	vc::Version::Builder defineBuilder(builder);
	float sptprice_MIN = 0.8;
	float sptprice_MAX = 0.84;
	builder.addDefine("sptprice_MIN", sptprice_MIN);
	builder.addDefine("sptprice_MAX", sptprice_MAX);

	vc::Version::Builder taffoDefineBuilder(taffoBuilder);
	taffoDefineBuilder.addDefine("sptprice_MIN", sptprice_MIN);
	taffoDefineBuilder.addDefine("sptprice_MAX", sptprice_MAX);

	do_version(builder, "baseline", inputFile, outputFile, true);
	double compile_t = do_version(taffoBuilder, "taffo", inputFile, outputFile, true);
	do_version(defineBuilder, "baseline+define", inputFile, "", true);
	do_version(taffoDefineBuilder, "taffo+define", inputFile, "", true);

	std::cout << "compilation time: " << compile_t << " s" << std::endl;

	return 0;
}
