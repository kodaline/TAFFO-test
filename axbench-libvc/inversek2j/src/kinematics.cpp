/*
 * kinematics.cpp
 * 
 *  Created on: Sep. 10 2013
 *			Author: Amir Yazdanbakhsh <yazdanbakhsh@wisc.edu>
 */

#include <cmath>
#include "kinematics.hpp"


void forwardk2j(
		float  __attribute((annotate(ANNOTATION_L1))) l1,
		float  __attribute((annotate(ANNOTATION_L2))) l2,
		float  __attribute((annotate("scalar()"))) theta1,
		float  __attribute((annotate("scalar()"))) theta2,
		float* __attribute((annotate("scalar(range(-1.57,1.57))"))) x, // range(-1,1)
		float* __attribute((annotate("scalar(range(-1.57,1.57))"))) y)  { // range(-1,1)
	*x = l1 * cos(theta1) + l2 * cos(theta1 + theta2) ;
	*y = l1 * sin(theta1) + l2 * sin(theta1 + theta2) ;
}

void inversek2j(
		float  __attribute((annotate(ANNOTATION_L1))) l1,
		float  __attribute((annotate(ANNOTATION_L2))) l2,
		float __attribute((annotate("scalar()"))) x,
		float __attribute((annotate("scalar()"))) y,
		float* __attribute((annotate("scalar(range(-1.57,1.57))"))) theta1,
		float* __attribute((annotate("scalar(range(-1.57,1.57))"))) theta2) {

	double __attribute((annotate("scalar()"))) dataIn[2];
	dataIn[0] = x;
	dataIn[1] = y;

	double __attribute((annotate("scalar()"))) dataOut[2];

#pragma parrot(input, "inversek2j", [2]dataIn)

	*theta2 = (float)acos(((x * x) + (y * y) - (l1 * l1) - (l2 * l2))/(2 * l1 * l2));
	*theta1 = (float)asin((y * (l1 + l2 * cos(*theta2)) - x * l2 * sin(*theta2))/(x * x + y * y));

	dataOut[0] = (*theta1);
	dataOut[1] = (*theta2);

#pragma parrot(output, "inversek2j", [2]<0.0; 2.0>dataOut)


	*theta1 = dataOut[0];
	*theta2 = dataOut[1];
}