/*
 * kmeans.c
 * 
 * Created on: Sep 9, 2013
 * 			Author: Amir Yazdanbakhsh <a.yazdanbakhsh@gatech.edu>
 */


#include <stdio.h>
#include <string>
#include "rgbimage.h"
#include "segmentation.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "benchmark.hpp"
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


