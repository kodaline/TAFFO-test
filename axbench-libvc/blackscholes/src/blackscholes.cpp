/*
 * kmeans.c
 *
 * Created on: Sep 9, 2013
 * 			Author: Amir Yazdanbakhsh <a.yazdanbakhsh@gatech.edu>
 */


 #include <stdio.h>
 #include <stdlib.h>
 #include <math.h>
 #include <string.h>
 #include <fstream>
 #include <iostream>
 #include <vector>
 #include <iomanip>
 #include "benchmark.hpp"


#ifdef LIBVC_DYN

//Precision to use for calculations
#define fptype float
#define DIVIDE 120.0
#define NUM_RUNS 1

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#ifndef sptprice_MIN
#define _sptprice_MIN "0.8"
#else
#define _sptprice_MIN STR(sptprice_MIN)
#endif

#ifndef sptprice_MAX
#define _sptprice_MAX "0.84"
#else
#define _sptprice_MAX STR(sptprice_MAX)
#endif
#define sptprice_annotation "scalar(range(" _sptprice_MIN "," _sptprice_MAX ") error(1e-8))"

#ifndef strike_MIN
#define _strike_MIN "0.75"
#else
#define _strike_MIN STR(strike_MIN)
#endif

#ifndef strike_MAX
#define _strike_MAX "0.92"
#else
#define _strike_MAX STR(strike_MAX)
#endif
#define strike_annotation "scalar(range(" _strike_MIN "," _strike_MAX ") error(1e-8))"

#ifndef volatility_MIN
#define _volatility_MIN "0.5"
#else
#define _volatility_MIN STR(volatility_MIN)
#endif

#ifndef volatility_MAX
#define _volatility_MAX "0.65"
#else
#define _volatility_MAX STR(volatility_MAX)
#endif
#define volatilty_annotation "scalar(range(" _volatility_MIN "," _volatility_MAX ") error(1e-8))"

typedef struct OptionData_ {
        fptype r;          // risk-free interest rate
        fptype divq;       // dividend rate
        fptype v;          // volatility
        fptype t;          // time to maturity or option expiration in years
                           //     (1yr = 1.0, 6mos = 0.5, 3mos = 0.25, ..., etc)
        char OptionType;   // Option type.  "P"=PUT, "C"=CALL
        fptype divs;       // dividend vals (not used in this test)
        fptype DGrefval;   // DerivaGem Reference Value (unused)
} OptionData;

OptionData *data;
fptype *s;      // spot price  // TEMPORARY: USED ONLY BY PARSER
fptype *stk;    // strike price // TEMPORARY: USED ONLY BY PARSER
fptype __attribute((annotate("scalar()"))) *prices;
int numOptions;

int    * otype;
fptype __attribute((annotate(sptprice_annotation))) *sptprice;
fptype __attribute((annotate(strike_annotation))) *strike; // range(0.33,1.0)
fptype __attribute((annotate("scalar(range(0.0275,0.1) error(0))"))) *rate;		//always
fptype __attribute((annotate(volatilty_annotation))) *volatility; // range(0.05,0.65)
fptype __attribute((annotate("scalar(range(0.1,1) error(0))"))) *otime;				// always
int numError = 0;

int bs_thread(void *tid_ptr);

extern "C" double kernel_func(std::string& inputFile, std::string& outputFile)
{
	FILE *file;
	int i;
	int loopnum;
	fptype * buffer;
	int * buffer2;
	int rv = 0; // utility to check print error
	srand(0xfeedface);

	//Read input data from file
	file = fopen(inputFile.c_str(), "r");
	if(file == NULL) {
		printf("ERROR: Unable to open file `%s'.\n", inputFile.c_str());
		exit(1);
	}
	rv = fscanf(file, "%i", &numOptions);
	// if(rv != 1) {
	// 	printf("ERROR: Unable to read from file `%s'.\n", inputFile);
	// 	fclose(file);
	// 	exit(1);
	// }

	// alloc spaces for the option data
	data = (OptionData*)malloc(numOptions*sizeof(OptionData));
	s = (fptype*)malloc(numOptions*sizeof(fptype));
	stk = (fptype*)malloc(numOptions*sizeof(fptype));
	prices = (fptype*)malloc(numOptions*sizeof(fptype));
	for ( loopnum = 0; loopnum < numOptions; ++ loopnum )
	{

		rv = fscanf(file, "%f %f ", &s[loopnum], &stk[loopnum]);
		rv += fscanf(file, "%f %f %f %f %c %f %f", &data[loopnum].r, &data[loopnum].divq, &data[loopnum].v, &data[loopnum].t, &data[loopnum].OptionType, &data[loopnum].divs, &data[loopnum].DGrefval);
		// if(rv != 9) {
		// 	printf("ERROR: Unable to read from file `%s'.\n", inputFile);
		// 	fclose(file);
		// 	exit(1);
		// }
	}
	rv = fclose(file);
	// if(rv != 0) {
	// 	printf("ERROR: Unable to close file `%s'.\n", inputFile);
	// 	exit(1);
	// }

	#define PAD 256
	#define LINESIZE 64

	buffer = (fptype *) malloc(5 * numOptions * sizeof(fptype) + PAD);
	sptprice = (fptype *) (((unsigned long long)buffer + PAD) & ~(LINESIZE - 1));
	strike = sptprice + numOptions;
	rate = strike + numOptions;
	volatility = rate + numOptions;
	otime = volatility + numOptions;

	buffer2 = (int *) malloc(numOptions * sizeof(fptype) + PAD);
	otype = (int *) (((unsigned long long)buffer2 + PAD) & ~(LINESIZE - 1));

	for (i=0; i<numOptions; i++) {
		otype[i]      = (data[i].OptionType == 'P') ? 1 : 0;
		sptprice[i]   = s[i] / DIVIDE;
		strike[i]     = stk[i] / DIVIDE;
		rate[i]       = data[i].r;
		volatility[i] = data[i].v;
		otime[i]      = data[i].t;
	}


  AxBenchTimer timer;
	int thread_id = 0;
	bs_thread(&thread_id);
	uint64_t kernel_time = timer.nanosecondsSinceInit();

	//Write prices to output file
	if (!outputFile.empty()) {
		file = fopen(outputFile.c_str(), "w");
		if(file == NULL) {
			printf("ERROR: Unable to open file `%s'.\n", outputFile.c_str());
		}
		// // rv = fprintf(file, "%i\n", numOptions);
		// if(rv < 0) {
		// 	printf("ERROR: Unable to write to file `%s'.\n", outputFile.c_str());
		// 	fclose(file);
		// 	exit(1);
		// }
		for(i=0; i<numOptions; i++) {
			rv = fprintf(file, "%.18f\n", prices[i]);
			if(rv < 0) {
				printf("ERROR: Unable to write to file `%s'.\n", outputFile.c_str());
				// fclose(file);
				// exit(1);
				break;
			}
		}
		fclose(file);
	}
	// if(rv != 0) {
	// 	printf("ERROR: Unable to close file `%s'.\n", outputFile.c_str());
	// }

	if (data) free(data);
	if (s) free(s);
	if (prices)	free(prices);
	if (stk) free(stk);

	return ((double)kernel_time) / 1000000000.0;
}


// Cumulative Normal Distribution Function
// See Hull, Section 11.8, P.243-244
#define inv_sqrt_2xPI 0.39894228040143270286

fptype CNDF ( fptype __attribute((annotate("scalar()"))) InputX )
{
    int sign;

    fptype __attribute((annotate("scalar()"))) OutputX; //range(-106.998909,107.998909)
    fptype __attribute((annotate("scalar()"))) xInput;
    fptype __attribute((annotate("scalar()"))) xNPrimeofX;
    fptype __attribute((annotate("scalar()"))) expValues; //range(0.144059,6.941593)
    fptype __attribute((annotate("scalar()"))) xK2;
    fptype __attribute((annotate("scalar()"))) xK2_2, xK2_3;
    fptype __attribute((annotate("scalar()"))) xK2_4, xK2_5;
    fptype __attribute((annotate("scalar()"))) xLocal, xLocal_1;
    fptype __attribute((annotate("scalar()"))) xLocal_2, xLocal_3;

    // Check for negative value of InputX
    if (InputX < 0.0) {
        InputX = -InputX;
        sign = 1;
    } else
        sign = 0;

    xInput = InputX;

    // Compute NPrimeX term common to both four & six decimal accuracy calcs
    expValues = exp(-0.5f * InputX * InputX);
    xNPrimeofX = expValues;
    xNPrimeofX = xNPrimeofX * inv_sqrt_2xPI;

    xK2 = 0.2316419 * xInput;
    xK2 = 1.0 + xK2;
    xK2 = 1.0 / xK2;
    xK2_2 = xK2 * xK2;
    xK2_3 = xK2_2 * xK2;
    xK2_4 = xK2_3 * xK2;
    xK2_5 = xK2_4 * xK2;

    xLocal_1 = xK2 * 0.319381530;
    xLocal_2 = xK2_2 * (-0.356563782);
    xLocal_3 = xK2_3 * 1.781477937;
    xLocal_2 = xLocal_2 + xLocal_3;
    xLocal_3 = xK2_4 * (-1.821255978);
    xLocal_2 = xLocal_2 + xLocal_3;
    xLocal_3 = xK2_5 * 1.330274429;
    xLocal_2 = xLocal_2 + xLocal_3;

    xLocal_1 = xLocal_2 + xLocal_1;
    xLocal   = xLocal_1 * xNPrimeofX;

    //printf("# xLocal: %10.10f\n", xLocal);



    xLocal   = 1.0 - xLocal;

    OutputX  = xLocal;

    //printf("# Output: %10.10f\n", OutputX);

    if (sign) {
        OutputX = 1.0 - OutputX;
    }

    return OutputX;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
fptype BlkSchlsEqEuroNoDiv( fptype __attribute((annotate("scalar()"))) sptprice,
                            fptype __attribute((annotate("scalar()"))) strike,
			    fptype __attribute((annotate("scalar()"))) rate,
                            fptype __attribute((annotate("scalar()"))) volatility,
			    fptype __attribute((annotate("scalar()"))) time,
                            int otype,
			    float __attribute((annotate("scalar()"))) timet,
                            fptype* __attribute((annotate("scalar(disabled)"))) N1,
			    fptype* __attribute((annotate("scalar(disabled)"))) N2 )
{
    //printf("BlkSchlsEqEuroNoDiv %f %f %f %f %f %f\n", sptprice, strike, rate, volatility, time, timet);
    fptype __attribute((annotate("scalar()"))) OptionPrice;

    // local private working variables for the calculation
    //fptype xStockPrice;
    //fptype xStrikePrice;
    fptype __attribute((annotate("scalar()"))) xRiskFreeRate;
    fptype __attribute((annotate("scalar()"))) xVolatility;
    fptype __attribute((annotate("scalar()"))) xTime;
    fptype __attribute((annotate("scalar()"))) xSqrtTime;

    fptype __attribute((annotate("scalar()"))) logValues;
    fptype __attribute((annotate("scalar()"))) xLogTerm;
    fptype __attribute((annotate("scalar()"))) xD1;
    fptype __attribute((annotate("scalar()"))) xD2;
    fptype __attribute((annotate("scalar()"))) xPowerTerm;
    fptype __attribute((annotate("scalar()"))) xDen;
    fptype __attribute((annotate("scalar()"))) d1;
    fptype __attribute((annotate("scalar()"))) d2;
    fptype __attribute((annotate("scalar()"))) FutureValueX;
    fptype __attribute((annotate("scalar()"))) NofXd1;
    fptype __attribute((annotate("scalar()"))) NofXd2;
    fptype __attribute((annotate("scalar()"))) NegNofXd1;
    fptype __attribute((annotate("scalar()"))) NegNofXd2;

    //xStockPrice = sptprice;
    //xStrikePrice = strike;
    xRiskFreeRate = rate;
    xVolatility = volatility;
    xTime = time;


    xSqrtTime = sqrt(xTime);

    logValues = log( sptprice / strike );

    xLogTerm = logValues;


    xPowerTerm = xVolatility * xVolatility;
    xPowerTerm = xPowerTerm * 0.5;

    xD1 = xRiskFreeRate + xPowerTerm;
    xD1 = xD1 * xTime;
    xD1 = xD1 + xLogTerm;

    xDen = xVolatility * xSqrtTime;
    xD1 = xD1 / xDen;
    xD2 = xD1 -  xDen;

    d1 = xD1;
    d2 = xD2;

    NofXd1 = CNDF( d1 );

    // if(NofXd1 > 1.0) {
    //     //std::cerr << "Greater than one!" << std::endl ;
    // }
    //printf("# d1: %10.10f\n", NofXd1);

    NofXd2 = CNDF( d2 );
    // if(NofXd2 > 1.0) {
    //      // std::cerr << "Greater than one!" << std::endl ;
    // }
    //printf("# d2: %10.10f\n", NofXd2);

    *N1 = NofXd1 ;
    *N2 = NofXd2 ;

    FutureValueX = strike * ( exp( -(rate)*(time) ) );
    if (otype == 0) {
        OptionPrice = (sptprice * NofXd1) - (FutureValueX * NofXd2);

    } else {
        NegNofXd1 = (1.0 - NofXd1);
        NegNofXd2 = (1.0 - NofXd2);
        OptionPrice = (FutureValueX * NegNofXd2) - (sptprice * NegNofXd1);
    }

    return OptionPrice;
}


double normalize(double in, double min, double max, double min_new, double max_new)
{
    return (((in - min) / (max - min)) * (max_new - min_new)) + min_new ;
}

int bs_thread(void *tid_ptr) {
    int i, j;

    int tid = *(int *)tid_ptr;
    int start = tid * (numOptions);
    int end = start + (numOptions);
    fptype __attribute((annotate("target('price_orig') scalar(range(-128.071959,128.071959))"))) price_orig;

    for (j=0; j<NUM_RUNS; j++) {
        for (i=start; i<end; i++) {
            /* Calling main function to calculate option value based on
             * Black & Scholes's equation.
             */
            fptype __attribute((annotate("scalar(disabled)"))) N1,
	      __attribute((annotate("scalar(disabled)"))) N2;
            float __attribute((annotate("scalar(range(0,1) error(0))"))) timet = 0;
/*
            double dataIn[6];
            double dataOut[1];

            dataIn[0]   = sptprice[i];
            dataIn[1]   = strike[i];
            dataIn[2]   = rate[i];
            dataIn[3]   = volatility[i];
            dataIn[4]   = otime[i];
            dataIn[5]   = otype[i];

#pragma parrot(input, "blackscholes", [6]dataIn)
*/
                price_orig = BlkSchlsEqEuroNoDiv( sptprice[i], strike[i],
                                         rate[i], volatility[i], otime[i],
                                         otype[i], timet, &N1, &N2);
/*
                dataOut[0] = price_orig;

#pragma parrot(output, "blackscholes", [1]<0.1; 0.9>dataOut)
*/
                prices[i] = price_orig;
        }
    }
    return 0;
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


int main (int argc, const char* argv[])
{
	std::string inputFile = argv[1];
	std::string outputFile = argv[2];

	char *selfpath = strdup(argv[0]);
	std::string basedir = dirname(selfpath);
	free(selfpath);

	std::shared_ptr<vc::TAFFOCompiler> taffo = std::make_shared<vc::TAFFOCompiler>(
		"taffo", "/usr/local", vc::TAFFOCompiler::Language::CXX, "", basedir, basedir+"/test.log");
	vc::compiler_ptr_t systemcpp = vc::make_compiler<vc::SystemCompilerOptimizer>(
		"baseline", "clang++", "opt", basedir, basedir+"/test.log", "/usr/local/bin", "/usr/local/bin");

	vc::Version::Builder builder;
	builder.addSourceFile(basedir + "/../src/blackscholes.cpp");
	builder.addIncludeDir(basedir + "/../src");
	builder.addIncludeDir(basedir + "/../../common/src");
	builder.addFunctionFlag("LIBVC_DYN");
	builder._functionName.push_back("kernel_func");
	builder._optionList.push_back(vc::Option("o", "-O", "3"));

	vc::Version::Builder taffoBuilder(builder);
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


#endif
