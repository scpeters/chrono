///////////////////////////////////////////////////
//
//   ChLcpConstraint.cpp
//
//
//    file for CHRONO HYPEROCTANT LCP solver
//
// ------------------------------------------------
// 	 Copyright:Alessandro Tasora / DeltaKnowledge
//             www.deltaknowledge.com
// ------------------------------------------------
///////////////////////////////////////////////////
 
 
#include "ChLcpConstraint.h"


namespace chrono
{

// Register into the object factory, to enable run-time
// dynamic creation and persistence
//ChClassRegister<ChLcpConstraint> a_registration_ChLcpConstraint;



ChLcpConstraint& ChLcpConstraint::operator=(const ChLcpConstraint& other)
{
	if (&other == this) return *this;

	c_i=other.c_i;
	g_i=other.g_i;
	b_i=other.b_i;
	l_i=other.l_i;
	cfm_i= other.cfm_i;
	valid = other.valid;
	disabled = other.disabled;
	redundant = other.redundant;
	broken = other.broken;
	mode = other.mode;

	return *this;
}

 
bool ChLcpConstraint::operator==(const ChLcpConstraint& other) const
{
	return	other.cfm_i==cfm_i &&
			other.valid==valid &&
			other.disabled==disabled &&
			other.redundant==redundant &&
			other.broken==broken &&
			other.mode==mode;
}


void ChLcpConstraint::Project()
{
	if (mode==CONSTRAINT_UNILATERAL)
	{
		if (l_i < 0.)
			l_i=0.;
	}
};

double ChLcpConstraint::Violation(double mc_i)
{
	if (mode==CONSTRAINT_UNILATERAL)
	{
		if (mc_i > 0.)
			return 0.;
	}

	return mc_i;
}


void ChLcpConstraint::StreamOUT(ChStreamOutBinary& mstream)
{
		// class version number
	mstream.VersionWrite(1);

		// stream useful member data
	mstream << cfm_i;
	mstream << valid;
	mstream << disabled;
	mstream << redundant;
	mstream << broken;
	mstream << (int)mode;
}

void ChLcpConstraint::StreamIN(ChStreamInBinary& mstream)
{
		// class version number
	int version = mstream.VersionRead();

		// stream in member data
	 int mint;
	mstream >> cfm_i;
	mstream >> valid;
	mstream >> disabled;
	mstream >> redundant;
	mstream >> broken;
	mstream >> mint;	mode = (eChConstraintMode)mint;
}



} // END_OF_NAMESPACE____

