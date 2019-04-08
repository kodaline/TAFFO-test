/*
 * kmeans.c
 * 
 * Created on: Sep 9, 2013
 * 			Author: Amir Yazdanbakhsh <a.yazdanbakhsh@gatech.edu>
 */


#include <stdio.h>
#include <string>
#include "rgbimage.hpp"
#include "segmentation.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "benchmark.hpp"

int main (int argc, const char* argv[]) {

	RgbImage srcImage;
	Clusters clusters;

	initRgbImage(&srcImage);

	loadRgbImage(inImageName.c_str(), &srcImage, 256);

	initClusters(&clusters, 6, 1);
	
	AxBenchTimer timer;
	segmentImage(&srcImage, &clusters, 1);
	uint64_t kernel_time = timer.nanosecondsSinceInit();
	
	std::cout << "kernel time = " << ((double)kernel_time) / 1000000000.0 << " s" << std::endl;
		
	saveRgbImage(&srcImage, outImageName.c_str(), 255);


	freeRgbImage(&srcImage);
	//freeClusters(&clusters);
	return 0;
}

