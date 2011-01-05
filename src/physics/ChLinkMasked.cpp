///////////////////////////////////////////////////
//
//   ChLinkMasked.cpp
//
// ------------------------------------------------
// 	 Copyright:Alessandro Tasora / DeltaKnowledge
//             www.deltaknowledge.com
// ------------------------------------------------
///////////////////////////////////////////////////



#include "physics/ChLinkMasked.h"
#include "physics/ChSystem.h"
#include "physics/ChGlobal.h"
#include "physics/ChCollide.h"

#include "core/ChMemory.h" // must be last include (memory leak debugger). In .cpp only.


namespace chrono
{



using namespace collision;
using namespace geometry;




// Register into the object factory, to enable run-time
// dynamic creation and persistence
ChClassRegister<ChLinkMasked> a_registration_ChLinkMasked;


                // BUILDERS
ChLinkMasked::ChLinkMasked ()
{
    force_D =  new ChLinkForce;    // defeault no forces in link dof
    force_R =  new ChLinkForce;
    force_X =  new ChLinkForce;
    force_Y =  new ChLinkForce;
    force_Z =  new ChLinkForce;
    force_Rx = new ChLinkForce;
    force_Ry = new ChLinkForce;
    force_Rz = new ChLinkForce;

	d_restlenght = 0;

	ndoc_d = ndoc_c = ndoc =0;

    C = C_dt = C_dtdt = NULL;   // initialize matrices.

    react = 0;
	cache_li_speed = cache_li_pos = 0;
    Qc = Ct = 0;
    Cq1 = Cq2 = 0;
    Cqw1 = Cqw2 = 0;

    mask = new ChLinkMask(1);     // create the mask;
	mask->Constr_N(0).SetMode(CONSTRAINT_FREE); // default: one constr.eq. but not working

    BuildLink ();           // setup all matrices - if any (i.e. none in this base link)-
                            // setting automatically  n. of DOC and DOF,
}



            // DESTROYER
ChLinkMasked::~ChLinkMasked ()
{
    DestroyLink();

    if (force_D) delete force_D;
    if (force_R) delete force_R;
    if (force_X) delete force_X;
    if (force_Y) delete force_Y;
    if (force_Z) delete force_Z;
    if (force_Rx) delete force_Rx;
    if (force_Ry) delete force_Ry;
    if (force_Rz) delete force_Rz;

    delete mask;  mask = NULL;
}


void ChLinkMasked::Copy(ChLinkMasked* source)
{
    // first copy the parent class data...
    ChLinkMarkers::Copy(source);

    mask->Copy(source->mask);

    // setup -alloc all needed matrices!!
    ChangedLinkMask();

    force_D->Copy (source->force_D);    // copy int.forces
    force_R->Copy (source->force_R);
    force_X->Copy (source->force_X);
    force_Y->Copy (source->force_Y);
    force_Z->Copy (source->force_Z);
    force_Rx->Copy (source->force_Rx);
    force_Ry->Copy (source->force_Ry);
    force_Rz->Copy (source->force_Rz);

	d_restlenght = source->d_restlenght;
}


ChLink* ChLinkMasked::new_Duplicate ()   // inherited classes:  Link* MyInheritedLink::new_Duplicate()
{
    ChLinkMasked* m_l;
    m_l = new ChLinkMasked;  // inherited classes should write here: m_l = new MyInheritedLink;
    m_l->Copy(this);
    return (m_l);
}


void ChLinkMasked::BuildLink ()
{
                // set ndoc by counting non-dofs
    ndoc   = mask->GetMaskDoc();
	ndoc_c = mask->GetMaskDoc_c();
	ndoc_d = mask->GetMaskDoc_d();


                // create matrices
    if (ndoc > 0)
    {
        C           = new ChMatrixDynamic<>(ndoc,1);
        C_dt        = new ChMatrixDynamic<>(ndoc,1);
        C_dtdt      = new ChMatrixDynamic<>(ndoc,1);
        react       = new ChMatrixDynamic<>(ndoc,1);
        Qc          = new ChMatrixDynamic<>(ndoc,1);
        Ct          = new ChMatrixDynamic<>(ndoc,1);
        Cq1         = new ChMatrixDynamic<>(ndoc,BODY_QDOF);
        Cq2         = new ChMatrixDynamic<>(ndoc,BODY_QDOF);
        Cqw1        = new ChMatrixDynamic<>(ndoc,BODY_DOF);
        Cqw2        = new ChMatrixDynamic<>(ndoc,BODY_DOF);
		cache_li_speed = new ChMatrixDynamic<>(ndoc,1);
		cache_li_pos   = new ChMatrixDynamic<>(ndoc,1);
    }
    else
    {
        C           = 0;
        C_dt        = 0;
        C_dtdt      = 0;
        react       = 0;
        Qc          = 0;
        Ct          = 0;
        Cq1         = 0;
        Cq2         = 0;
        Cqw1        = 0;
        Cqw2        = 0;
		cache_li_speed = 0;
		cache_li_pos   = 0;
    }

}


void ChLinkMasked::BuildLink (ChLinkMask* new_mask)
{
                    // set mask
    mask->Copy(new_mask);
                    // setup matrices;
    BuildLink();
}


void ChLinkMasked::DestroyLink()
{
    if (ndoc > 0)
    {
        if (C)      {delete C;      C = NULL;}
        if (C_dt)   {delete C_dt;   C_dt = NULL;}
        if (C_dtdt) {delete C_dtdt; C_dtdt = NULL;}
        if (react)  {delete react;  react = NULL;}
        if (Qc)     {delete Qc;     Qc = NULL;}
        if (Ct)     {delete Ct;     Ct = NULL;}
        if (Cq1)    {delete Cq1;    Cq1 = NULL;}
        if (Cq2)    {delete Cq2;    Cq2 = NULL;}
        if (Cqw1)   {delete Cqw1;   Cqw1 = NULL;}
        if (Cqw2)   {delete Cqw2;   Cqw2 = NULL;}
		if (react)  {delete react;  react = NULL;}
		if (cache_li_speed)  {delete cache_li_speed;  cache_li_speed = 0;}
		if (cache_li_pos)  {delete cache_li_pos;  cache_li_pos = 0;}
    }

    ndoc=0;
}


void ChLinkMasked::ChangeLinkMask (ChLinkMask* new_mask)
{
    DestroyLink ();
    BuildLink (new_mask);
}

void ChLinkMasked::ChangedLinkMask ()
{
    DestroyLink ();
    BuildLink();
}



void ChLinkMasked::SetDisabled(bool mdis)
{
	ChLinkMarkers::SetDisabled(mdis);

    if (mask->SetAllDisabled(mdis) >0)
        ChangedLinkMask ();
}

void ChLinkMasked::SetBroken(bool mbro)
{
	ChLinkMarkers::SetBroken(mbro);

    if (mask->SetAllBroken(mbro) >0)
        ChangedLinkMask ();
}


int ChLinkMasked::RestoreRedundant()
{
    int mchanges = mask->RestoreRedundant();
    if (mchanges)
        ChangedLinkMask ();
    return mchanges;
}


void ChLinkMasked::SetMarker1 (ChMarker* mark1)
{
	ChLinkMarkers::SetMarker1 (mark1);

	if(this->Body1 && this->Body2)
	{
		((ChLinkMaskLF*)this->mask)->SetTwoBodiesVariables(&Body1->Variables(), &Body2->Variables());
					// This is needed because only if all constraints in mask are now active, and C,Ct,etc.
					// matrices must be allocated accordingly, otherwise are null.
		DestroyLink();
		BuildLink();
	}
}

void ChLinkMasked::SetMarker2 (ChMarker* mark2)
{
	ChLinkMarkers::SetMarker2 (mark2);

	if(this->Body1 && this->Body2)
	{
		((ChLinkMaskLF*)this->mask)->SetTwoBodiesVariables(&Body1->Variables(), &Body2->Variables());
					// This is needed because only if all constraints in mask are now active, and C,Ct,etc.
					// matrices must be allocated accordingly, otherwise are null.
		DestroyLink();
		BuildLink();
	}
}


 
////////////////////////////////////
///
///    LCP SYSTEM FUNCTIONS


void ChLinkMasked::InjectConstraints(ChLcpSystemDescriptor& mdescriptor)
{
	if (!this->IsActive())
		return;

	for (int i=0; i< mask->nconstr; i++)
	{
		if (mask->Constr_N(i).IsActive())
			mdescriptor.InsertConstraint(&mask->Constr_N(i));
	}
}

void ChLinkMasked::ConstraintsBiReset()
{
	for (int i=0; i< mask->nconstr; i++)
	{
		mask->Constr_N(i).Set_b_i(0.);
	}
}
 
void ChLinkMasked::ConstraintsBiLoad_C(double factor, double recovery_clamp, bool do_clamp)
{
	int cnt=0;
	for (int i=0; i< mask->nconstr; i++)
	{
		if (mask->Constr_N(i).IsActive())
		{
			if (do_clamp)
			{
				if (mask->Constr_N(i).IsUnilateral())
					mask->Constr_N(i).Set_b_i( mask->Constr_N(i).Get_b_i() + ChMax (factor * C->ElementN(cnt), -recovery_clamp)  );
				else 
					mask->Constr_N(i).Set_b_i( mask->Constr_N(i).Get_b_i() + ChMin(ChMax (factor * C->ElementN(cnt), -recovery_clamp), recovery_clamp)  );
			}
			else
				mask->Constr_N(i).Set_b_i( mask->Constr_N(i).Get_b_i() + factor * C->ElementN(cnt)  );

			cnt++;
		}
	}
}

void ChLinkMasked::ConstraintsBiLoad_Ct(double factor)
{
	int cnt=0;
	for (int i=0; i< mask->nconstr; i++)
	{
		if (mask->Constr_N(i).IsActive())
		{
			mask->Constr_N(i).Set_b_i( mask->Constr_N(i).Get_b_i() + factor * Ct->ElementN(cnt)  );
			cnt++;
		}
	} 
}

void ChLinkMasked::ConstraintsBiLoad_Qc(double factor)
{
	int cnt=0;
	for (int i=0; i< mask->nconstr; i++)
	{
		if (mask->Constr_N(i).IsActive())
		{
			mask->Constr_N(i).Set_b_i( mask->Constr_N(i).Get_b_i() + factor * Qc->ElementN(cnt)  );
			cnt++;
		}
	}
}




void ChLinkMasked::ConstraintsLoadJacobians()
{
	if (!this->ndoc)
		return;

	int cnt=0;
	for (int i=0; i< mask->nconstr; i++)
	{
		if (mask->Constr_N(i).IsActive())
		{
			mask->Constr_N(i).Get_Cq_a()->PasteClippedMatrix(this->Cqw1, cnt, 0, 1, this->Cqw1->GetColumns(), 0,0);
			mask->Constr_N(i).Get_Cq_b()->PasteClippedMatrix(this->Cqw2, cnt, 0, 1, this->Cqw2->GetColumns(), 0,0);
			cnt++;

			// sets also the CFM term
			//mask->Constr_N(i).Set_cfm_i(this->attractor);
		}
	}
}
 

void ChLinkMasked::ConstraintsFetch_react(double factor)
{
	react_force = VNULL;		// Do not update 'intuitive' react force and torque here: just set as 0. 
    react_torque = VNULL;		// Child classes implementations should compute them. 
	
	// From constraints to react vector:
	int cnt=0;
	for (int i=0; i< mask->nconstr; i++)
	{
		if (mask->Constr_N(i).IsActive())
		{
			react->ElementN(cnt) = mask->Constr_N(i).Get_l_i() * factor;
			cnt++;
		}
	}
}

void  ChLinkMasked::ConstraintsLiLoadSuggestedSpeedSolution()
{
	int cnt=0;
	for (int i=0; i< mask->nconstr; i++)
	{
		if (mask->Constr_N(i).IsActive())
		{
			mask->Constr_N(i).Set_l_i( cache_li_speed->ElementN(cnt) );
			cnt++;
		}
	}
}

void  ChLinkMasked::ConstraintsLiLoadSuggestedPositionSolution()
{
	int cnt=0;
	for (int i=0; i< mask->nconstr; i++)
	{
		if (mask->Constr_N(i).IsActive())
		{
			mask->Constr_N(i).Set_l_i( cache_li_pos->ElementN(cnt) ) ;
			cnt++;
		}
	}
}

void  ChLinkMasked::ConstraintsLiFetchSuggestedSpeedSolution()
{
	int cnt=0;
	for (int i=0; i< mask->nconstr; i++)
	{
		if (mask->Constr_N(i).IsActive())
		{
			cache_li_speed->ElementN(cnt) = mask->Constr_N(i).Get_l_i();
			cnt++;
		}
	}
}

void  ChLinkMasked::ConstraintsLiFetchSuggestedPositionSolution()
{
	int cnt=0;
	for (int i=0; i< mask->nconstr; i++)
	{
		if (mask->Constr_N(i).IsActive())
		{
			cache_li_pos->ElementN(cnt) = mask->Constr_N(i).Get_l_i();
			cnt++;
		}
	}
}





////////////////////////////////////
///
///    UPDATING PROCEDURES





/////////   4.5- UPDATE Cqw1 and Cqw2
/////////
void ChLinkMasked::Transform_Cq_to_Cqw(ChMatrix<>* mCq, ChMatrix<>* mCqw, ChBody* mbody)
{
    if (!mCq) return;

    // traslational part - not changed
    mCqw->PasteClippedMatrix(mCq,0,0,mCq->GetRows(),3,0,0);

    // rotational part [Cq_w] = [Cq_q]*[Gl]'*1/4
    register int col, row, colres;
    double sum;

	ChMatrixNM<double,3,4> mGl;
	ChFrame<>::SetMatrix_Gl(mGl, mbody->GetCoord().rot);

    for (colres=0; colres < 3; colres++)    {
        for (row=0; row < (mCq->GetRows()); row++)      {
            sum = 0;
            for (col=0; col < 4; col++)         {
                sum+= ((mCq->GetElement (row,col+3))*(mGl.GetElement (colres,col)));
            }
            mCqw->SetElement (row, colres+3, sum * 0.25);
        }
    }
}

void ChLinkMasked::UpdateCqw()
{
    if (!Cq1 || !Cq2) return;

    ChLinkMasked::Transform_Cq_to_Cqw(Cq1, Cqw1, Body1);
	ChLinkMasked::Transform_Cq_to_Cqw(Cq2, Cqw2, Body2);
}

/////////   5-   UPDATE FORCES
/////////

void ChLinkMasked::UpdateForces(double mytime)
{
	ChLinkMarkers::UpdateForces(mytime);

    // ** Child class can inherit this method. The parent implementation must
    //    be called _before_ adding further custom forces.

    Vector m_force = VNULL;         // initialize to zero the m1-m2 force/torque
    Vector m_torque = VNULL;        // 'intuitive' vectors (can be transformed&added later into Qf)


    // COMPUTE THE FORCES IN THE LINK, FOR EXAMPLE
    // CAUSED BY SPRINGS
    // NOTE!!!!!   C_force and C_torque   are considered in the reference coordsystem
    // of marker2  (the MAIN marker), and their application point is considered the
    // origin of marker1 (the SLAVE marker)

    // 1)========== the generic spring-damper

    if (force_D) {
    if (force_D->Get_active())
    {
        double dfor;
        dfor = force_D->Get_Force((dist - d_restlenght), dist_dt, ChTime);
        m_force = Vmul (Vnorm(relM.pos), dfor);

        C_force = Vadd(C_force, m_force);
    } }


    // 2)========== the generic torsional spring / torsional damper

    if (force_R) {
    if (force_R->Get_active())
    {
        double tor;
            // 1) the tors. spring
        tor = force_R->Get_Force(relAngle, 0, ChTime);
        m_torque = Vmul (relAxis, tor);
        C_torque = Vadd(C_torque, m_torque);
            // 2) the tors. damper
        double angle_dt = Vlenght (relWvel);
        tor = force_R->Get_Force(0, angle_dt, ChTime);
        m_torque = Vmul (Vnorm(relWvel), tor);
        C_torque = Vadd(C_torque, m_torque);
    } }


    // 3)========== the XYZ forces

    m_force = VNULL;

    if (force_X) {
    if (force_X->Get_active())
    {
        m_force.x = force_X->Get_Force(relM.pos.x, relM_dt.pos.x, ChTime);
    } }

    if (force_Y) {
    if (force_Y->Get_active())
    {
        m_force.y = force_Y->Get_Force(relM.pos.y, relM_dt.pos.y, ChTime);
    } }

    if (force_Z) {
    if (force_Z->Get_active())
    {
        m_force.z = force_Z->Get_Force(relM.pos.z, relM_dt.pos.z, ChTime);
    } }

    C_force = Vadd(C_force, m_force);


    // 4)========== the RxRyRz forces (torques)

    m_torque = VNULL;

    if (force_Rx) {
    if (force_Rx->Get_active())
    {
        m_torque.x = force_Rx->Get_Force(relRotaxis.x, relWvel.x, ChTime);
    } }

    if (force_Ry) {
    if (force_Ry->Get_active())
    {
        m_torque.y = force_Ry->Get_Force(relRotaxis.y, relWvel.y, ChTime);
    } }

    if (force_Rz) {
    if (force_Rz->Get_active())
    {
        m_torque.z = force_Rz->Get_Force(relRotaxis.z, relWvel.z, ChTime);
    } }

    C_torque = Vadd(C_torque, m_torque);

}





/////////
/////////   COMPLETE UPDATE
/////////
/////////

void ChLinkMasked::Update (double time)
{
    // 1 -
    UpdateTime(time);

    // 2 -
    UpdateRelMarkerCoords();

    // 3 -
    UpdateState();

    // 3b-
    UpdateCqw();

    // 4 -
    UpdateForces(time);

}



/////////
///////// FILE I/O
/////////


// Define some  link-specific flags for backward compatibility

#define LF_INACTIVE		(1L << 0)
#define LF_BROKEN		(1L << 2)
#define LF_DISABLED	    (1L << 4)



void ChLinkMasked::StreamOUT(ChStreamOutBinary& mstream)
{
			// class version number
	mstream.VersionWrite(10);
		// serialize parent class too
	ChLinkMarkers::StreamOUT(mstream);

		// stream out all member data
	mstream.AbstractWrite(mask);
    mstream << d_restlenght;
    mstream << *force_D;
    mstream << *force_R;
    mstream << *force_X;
    mstream << *force_Y;
    mstream << *force_Z;
    mstream << *force_Rx;
    mstream << *force_Ry;
    mstream << *force_Rz;
}

void ChLinkMasked::StreamIN(ChStreamInBinary& mstream)
{
		// class version number
	int version = mstream.VersionRead();
		// deserialize parent class too
	ChLinkMarkers::StreamIN(mstream);

		// stream in all member data
	if (mask) delete (mask); mask=NULL;
	mstream.AbstractReadCreate(&mask);
    mstream >> d_restlenght;
    mstream >> *force_D;
    mstream >> *force_R;
    mstream >> *force_X;
    mstream >> *force_Y;
    mstream >> *force_Z;
    mstream >> *force_Rx;
    mstream >> *force_Ry;
    mstream >> *force_Rz;
	
}




 
} // END_OF_NAMESPACE____

