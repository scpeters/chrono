#ifndef CHJSBODY_H
#define CHJSBODY_H

///////////////////////////////////////////////////
//  
//   ChJs_system.h
//
//	 CHRONO 
//   ------
//   Multibody dinamics engine
//
//   define JS wrapper class
//
// ------------------------------------------------
// 	 Copyright:Alessandro Tasora / DeltaKnowledge
//             www.deltaknowledge.com
// ------------------------------------------------
///////////////////////////////////////////////////


#include "jsapi.h"

namespace chrono 
{


// CLASSES

extern JSClass chjs_RBody; 
JSClass* achjs_RBody();  // get address of static 'Javascript class' structure

 

// Initiates the class 
// Returns created 'class object'. Object 'parent' can 
// be null if no inheritance desired, or other class if inheritance
// hierarchy must be built.
 
JSObject* ChJS_InitClass_RBody(JSContext* cx, JSObject* glob, JSObject* parent);


} // END_OF_NAMESPACE____

#endif