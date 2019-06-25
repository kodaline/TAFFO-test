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

 void forwardk2j(float l1, float l2, float theta1, float theta2, float* x, float* y) ;
 void inversek2j(float l1, float l2, float x, float y, float* theta1, float* theta2) ;

 #endif