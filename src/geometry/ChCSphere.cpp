//////////////////////////////////////////////////
//  
//   ChCSphere.cpp
//
// ------------------------------------------------
// 	 Copyright 2006 Alessandro Tasora 
// ------------------------------------------------
///////////////////////////////////////////////////


#include <stdio.h>


#include "ChCSphere.h"


namespace chrono 
{
namespace geometry 
{


// Register into the object factory, to enable run-time
// dynamic creation and persistence
ChClassRegister<ChSphere> a_registration_ChSphere;


void ChSphere::StreamOUT(ChStreamOutBinary& mstream)
{
		// class version number
	mstream.VersionWrite(1);

		// serialize parent class too
	ChGeometry::StreamOUT(mstream);

		// stream out all member data
	mstream << center;
	mstream << rad;
}

void ChSphere::StreamIN(ChStreamInBinary& mstream)
{
		// class version number
	int version = mstream.VersionRead();

		// deserialize parent class too
	ChGeometry::StreamIN(mstream);

		// stream in all member data
	mstream >> center;
	mstream >> rad;
}


} // END_OF_NAMESPACE____
} // END_OF_NAMESPACE____
