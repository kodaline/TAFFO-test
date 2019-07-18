#include <algorithm>
#include <iostream>
#include <libgen.h>
#include <string.h>
#include <fstream> 

#include "versioningCompiler/CompilerImpl/SystemCompilerOptimizer.hpp"
#include "versioningCompiler/CompilerImpl/TAFFOCompiler.hpp"
#include "versioningCompiler/Version.hpp"


int main(int argc, const char* argv[])
{
	int n;
	std::string inputFilename	= argv[1];
	std::string outputFilename 	= argv[2];

	// prepare the input file for reading the theta data
	std::ifstream inputFileHandler (inputFilename, std::ifstream::in);

	// first line defins the number of enteries
	inputFileHandler >> n;

	float* l1l2t1t2xy = (float*)malloc(n * 2 * 2 * sizeof(float));

	if(l1l2t1t2xy == NULL)
	{
		std::cerr << "# Cannot allocate memory for the coordinates an angles!" << std::endl;
		return -1 ;
	}

	srand (time(NULL));

	for (int i=0; i<n*2*2; i+=2*2) {
		float theta1, theta2;
		inputFileHandler >> theta1 >> theta2;

		l1l2t1t2xy[i + 0] = theta1;
		l1l2t1t2xy[i + 1] = theta2;
	}
	
	char *selfpath = strdup(argv[0]);
	std::string basedir = dirname(selfpath);
	free(selfpath);

	vc::compiler_ptr_t systemcpp = vc::make_compiler<vc::SystemCompiler>(
		"baseline", "c++", basedir, basedir+"/test.log", "/usr/bin");

	std::shared_ptr<vc::TAFFOCompiler> taffo =
			std::make_shared<vc::TAFFOCompiler>(
					"taffo",
					"/usr/local",
					vc::TAFFOCompiler::Language::CXX,
					"../../../dist/usr/local",
					".",
					"./test.log",
					"../../../dist/usr/local/bin/taffo-j2a");

	vc::Version::Builder builder;
	builder.genIRoptions(
			{ vc::Option(
						"AnnotationInserter", "annotationFile", "./annotations.json"),
				vc::Option(
						"AnnotationInserter",
						"-I",
						"/usr/local/lib/clang/8.0.1/include/") });
	builder.addSourceFile("src/inversek2j.cpp");
	builder.addSourceFile("src/kinematics.cpp");
	builder.addIncludeDir("src");
	builder.addIncludeDir("../common/src");
	builder.addFunctionFlag("LIBVC_DYN");
	builder._functionName.push_back("kernel_func");
	builder._optionList.push_back(vc::Option("o", "-O", "3"));
	builder._genIROptionList.push_back(vc::Option("", "-fPIC", ""));

	builder._compiler = taffo;

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

	return 0;
}
