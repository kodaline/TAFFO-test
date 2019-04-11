/*
 * sobel.cpp
 * 
 * Created on: Sep 9, 2013
 * 			Author: Amir Yazdanbakhsh <a.yazdanbakhsh@gatech.edu>
 */
#include "rgb_image.hpp"
#include "convolution.hpp"
#include <iostream>
#include <cmath>
#include "benchmark.hpp"

#include "convolution.cpp"
#include "rgb_image.cpp"


int main ( int argc, const char* argv[])
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


	#ifdef _MIOSIX
	  srcImagePtr->loadRgbImage( INPUT ); // source image
	  dstImagePtr->loadRgbImage( INPUT ); // destination image
	#else
	  srcImagePtr->loadRgbImage( argv[1] ); // source image
	  dstImagePtr->loadRgbImage( argv[1] ); // destination image
	#endif

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
	std::cout << "kernel time = " << ((double)kernel_time) / 1000000000.0 << " s" << std::endl;

	return 0 ;
}
