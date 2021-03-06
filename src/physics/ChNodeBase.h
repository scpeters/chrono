//
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2010 Alessandro Tasora
// Copyright (c) 2013 Project Chrono
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be 
// found in the LICENSE file at the top level of the distribution
// and at http://projectchrono.org/license-chrono.txt.
//
// File author: A.Tasora

#ifndef CHNODEBASE_H
#define CHNODEBASE_H


//#include <math.h>

#include "core/ChShared.h"
#include "physics/ChPhysicsItem.h"
#include "physics/ChVariablesInterface.h"
#include "lcp/ChLcpVariablesBodyOwnMass.h"


namespace chrono
{


/// Class for a node, that has some degrees of 
/// freedom and that contain a proxy to the solver.
/// It is like a lightweight version of a ChPhysicsItem,
/// often a ChPhysicsItem is used as a container for a cluster 
/// of these ChNodeBase.

class ChApi ChNodeBase : public virtual ChShared,
						 public ChVariablesInterface	
{
public:
	ChNodeBase ();
	virtual ~ChNodeBase ();

	ChNodeBase (const ChNodeBase& other); // Copy constructor
	ChNodeBase& operator= (const ChNodeBase& other); //Assignment operator

					//
					// FUNCTIONS
					//


			/// Get the number of degrees of freedom
	int Get_ndof() { return this->Variables().Get_ndof();}


			//
			// Functions for interfacing to the LCP solver
			//

				/// Sets the 'fb' part (the known term) of the encapsulated ChLcpVariables to zero.
	virtual void VariablesFbReset() { Variables().Get_fb().FillElem(0.0); }

				/// Adds the current forces (applied to node) into the
				/// encapsulated ChLcpVariables, in the 'fb' part: qf+=forces*factor
	virtual void VariablesFbLoadForces(double factor=1.) {};

				/// Initialize the 'qb' part of the ChLcpVariables with the 
				/// current value of speeds. 
	virtual void VariablesQbLoadSpeed() {};

				/// Adds M*q (masses multiplied current 'qb') to Fb, ex. if qb is initialized
				/// with v_old using VariablesQbLoadSpeed, this method can be used in 
				/// timestepping schemes that do: M*v_new = M*v_old + forces*dt
	virtual void VariablesFbIncrementMq() {};

				/// Fetches the item speed (ex. linear velocity, in xyz nodes) from the
				/// 'qb' part of the ChLcpVariables and sets it as the current item speed.
				/// If 'step' is not 0, also should compute the approximate acceleration of
				/// the item using backward differences, that is  accel=(new_speed-old_speed)/step.
				/// Mostly used after the LCP provided the solution in ChLcpVariables.
	virtual void VariablesQbSetSpeed(double step=0.) {};

				/// Increment node positions by the 'qb' part of the ChLcpVariables,
				/// multiplied by a 'step' factor.
				///     pos+=qb*step
				/// If qb is a speed, this behaves like a single step of 1-st order
				/// numerical integration (Eulero integration).
	virtual void VariablesQbIncrementPosition(double step) {};


};



} // END_OF_NAMESPACE____


#endif
