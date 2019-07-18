#include <algorithm>
#include <iostream>
#include <libgen.h>
#include <string.h>

#include "versioningCompiler/CompilerImpl/SystemCompilerOptimizer.hpp"
#include "versioningCompiler/CompilerImpl/TAFFOCompiler.hpp"
#include "versioningCompiler/Version.hpp"

double do_version(
		vc::Version::Builder& builder, std::string label, bool split_compile)
{
	std::cout << label << " version..." << std::endl;
	vc::version_ptr_t v = builder.build();
	if (split_compile)
	{
		if (!v->prepareIR())
		{
			std::cout << "libVC compilation failed" << std::endl;
			return -1.0;
		}
	}
	if (!v->compile())
	{
		std::cout << "libVC compilation failed" << std::endl;
		return -1.0;
	}

	v->fold();
}

int main(int argc, const char* argv[])
{
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
	builder.addSourceFile("src/jmeint.cpp");
	builder.addSourceFile("src/tritri.cpp");
	builder.addIncludeDir("src");
	builder.addIncludeDir("../common/src");
	builder.addFunctionFlag("LIBVC_DYN");
	builder._functionName.push_back("kernel_func");
	builder._optionList.push_back(vc::Option("o", "-O", "3"));
	builder._genIROptionList.push_back(vc::Option("", "-fPIC", ""));

	builder._compiler = taffo;

	do_version(builder, "baseline", true);

	return 0;
}
