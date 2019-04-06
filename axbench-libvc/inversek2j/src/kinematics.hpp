/*
 * kinematics.hpp
 * 
 *  Created on: Sep. 10 2013
 *			Author: Amir Yazdanbakhsh <yazdanbakhsh@wisc.edu>
 */

 #ifndef __KINEMATICS_HPP__
 #define __KINEMATICS_HPP__

 #ifndef ANNOTATION_L1
 #define ANNOTATION_L1 "scalar()"
 #endif

 #ifndef ANNOTATION_L2
 #define ANNOTATION_L2 "scalar()"
 #endif

 extern float __attribute((annotate(ANNOTATION_L1))) l1 ;
 extern float __attribute((annotate(ANNOTATION_L2))) l2 ;

 void forwardk2j(float theta1, float theta2, float* x, float* y) ;
 void inversek2j(float x, float y, float* theta1, float* theta2) ;

 #endif