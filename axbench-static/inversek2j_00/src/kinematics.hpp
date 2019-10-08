/*
 * kinematics.hpp
 * 
 *  Created on: Sep. 10 2013
 *			Author: Amir Yazdanbakhsh <yazdanbakhsh@wisc.edu>
 */

 #ifndef __KINEMATICS_HPP__
 #define __KINEMATICS_HPP__

 extern const float l1 ;
 extern const float l2 ;

 void forwardk2j(float theta1, float theta2, float* x, float* y) ;
 void inversek2j(float x, float y, float* theta1, float* theta2) ;

 #endif
