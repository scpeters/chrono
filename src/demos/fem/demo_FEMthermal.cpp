//
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2013 Project Chrono
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be 
// found in the LICENSE file at the top level of the distribution
// and at http://projectchrono.org/license-chrono.txt.
//

//
//   Demo code about 
//
//     - FEM visualization using Irrlicht

  
     
// Include some headers used by this tutorial...

#include "physics/ChSystem.h"
#include "lcp/ChLcpIterativeMINRES.h"
#include "unit_FEM/ChElementSpring.h"
#include "unit_FEM/ChElementBar.h"
#include "unit_FEM/ChElementTetra_4.h"
#include "unit_FEM/ChElementTetra_10.h"
#include "unit_FEM/ChElementHexa_8.h"
#include "unit_FEM/ChElementHexa_20.h"
#include "unit_FEM/ChContinuumThermal.h"
#include "unit_FEM/ChContinuumElectrostatics.h"
#include "unit_FEM/ChNodeFEMxyzP.h"
#include "unit_FEM/ChMesh.h"
#include "unit_FEM/ChLinkPointFrame.h"
#include "unit_FEM/ChVisualizationFEMmesh.h"
#include "unit_IRRLICHT/ChIrrApp.h"


// Remember to use the namespace 'chrono' because all classes 
// of Chrono::Engine belong to this namespace and its children...

using namespace chrono;
using namespace fem;

using namespace irr;



int main(int argc, char* argv[])
{
	// Create a Chrono::Engine physical system
	ChSystem my_system;


	// Create the Irrlicht visualization (open the Irrlicht device, 
	// bind a simple user interface, etc. etc.)
	ChIrrApp application(&my_system, L"FEM thermal",core::dimension2d<u32>(800,600),false, true);

	// Easy shortcuts to add camera, lights, logo and sky in Irrlicht scene:
	application.AddTypicalLogo();
	application.AddTypicalSky();
	application.AddTypicalLights(core::vector3df(20,20,20),core::vector3df(-20,20,-20),90,90,irr::video::SColorf(0.5,0.5,0.5));
	application.AddTypicalCamera(core::vector3df(0,0.7,-1),core::vector3df(0,0.4,0));



				// Create a mesh, that is a container for groups
				// of elements and their referenced nodes.
	ChSharedPtr<ChMesh> my_mesh(new ChMesh);
	
				// Create a material, that must be assigned to each element,
				// and set its parameters
	ChSharedPtr<ChContinuumThermal> mmaterial(new ChContinuumThermal);
	mmaterial->SetMassSpecificHeatCapacity(2);
	mmaterial->SetThermalConductivityK(200);
 
	//
	// Add some TETAHEDRONS:
	//

				// Load a .node file and a .ele  file from disk, defining a complicate tetahedron mesh.
				// This is much easier than creating all nodes and elements via C++ programming.
				// You can generate these files using the TetGen tool.
	try 
	{
    my_mesh->LoadFromTetGenFile(GetChronoDataFile("unit_FEM/beam.node").c_str(),
                                GetChronoDataFile("unit_FEM/beam.ele").c_str(),
                                mmaterial);
	}
	catch (ChException myerr) {
			GetLog() << myerr.what();
			return 0;
	}

	for (int inode = 0; inode < my_mesh->GetNnodes(); ++inode)
	{
		if (my_mesh->GetNode(inode).IsType<ChNodeFEMxyzP>())
		{
			ChSharedPtr<ChNodeFEMxyzP> mnode ( my_mesh->GetNode(inode).DynamicCastTo<ChNodeFEMxyzP>() ); // downcast
			mnode->SetPos(mnode->GetPos()*ChVector<>(3,1,3));
		}
	}

	//
	// Set some BOUNDARY CONDITIONS on nodes:
	//

		// Impose load on the 180th node
	ChSharedPtr<ChNodeFEMxyzP> mnode3 (my_mesh->GetNode(180).DynamicCastTo<ChNodeFEMxyzP>() );
	mnode3->SetF( 20 ); // thermal load: heat flux [W] into node

		// Impose field on two top nodes (remember the SetFixed(true); )
	ChSharedPtr<ChNodeFEMxyzP> mnode1 (my_mesh->GetNode(my_mesh->GetNnodes()-1).DynamicCastTo<ChNodeFEMxyzP>());
	mnode1->SetFixed(true); 
	mnode1->SetP(0.5); // field: temperature [K]
	ChSharedPtr<ChNodeFEMxyzP> mnode2 (my_mesh->GetNode(my_mesh->GetNnodes()-2).DynamicCastTo<ChNodeFEMxyzP>());
	mnode2->SetFixed(true); 
	mnode2->SetP(0.5); // field: temperature [K]

		// Impose field on the base points:
	for (int inode = 0; inode < my_mesh->GetNnodes(); ++inode)
	{
		if (my_mesh->GetNode(inode).IsType<ChNodeFEMxyzP>())
		{
			ChSharedPtr<ChNodeFEMxyzP> mnode ( my_mesh->GetNode(inode).DynamicCastTo<ChNodeFEMxyzP>() ); // downcast
			if (mnode->GetPos().y <0.01)
			{
				mnode->SetFixed(true); 
				mnode->SetP(10);  // field: temperature [K]
			}
		}
	}


				// This is necessary in order to precompute the 
				// stiffness matrices for all inserted elements in mesh
	my_mesh->SetupInitial();

				// Remember to add the mesh to the system!
	my_system.Add(my_mesh);

	
	


			// ==Asset== attach a visualization of the FEM mesh.
			// This will automatically update a triangle mesh (a ChTriangleMeshShape
			// asset that is internally managed) by setting  proper
			// coordinates and vertex colours as in the FEM elements.
			// Such triangle mesh can be rendered by Irrlicht or POVray or whatever
			// postprocessor that can handle a coloured ChTriangleMeshShape).
			// Do not forget AddAsset() at the end!

		// This will paint the colored mesh with temperature scale (E_PLOT_NODE_P is the scalar field of the Poisson problem)
	ChSharedPtr<ChVisualizationFEMmesh> mvisualizemesh(new ChVisualizationFEMmesh(*(my_mesh.get_ptr())));
	mvisualizemesh->SetFEMdataType(ChVisualizationFEMmesh::E_PLOT_NODE_P);
	mvisualizemesh->SetColorscaleMinMax(-1,12);
	mvisualizemesh->SetShrinkElements(false,0.85);
	mvisualizemesh->SetSmoothFaces(true);
	my_mesh->AddAsset(mvisualizemesh);

		// This will paint the wireframe
	ChSharedPtr<ChVisualizationFEMmesh> mvisualizemeshB(new ChVisualizationFEMmesh(*(my_mesh.get_ptr())));
	mvisualizemeshB->SetFEMdataType(ChVisualizationFEMmesh::E_PLOT_SURFACE);
	mvisualizemeshB->SetWireframe(true);
	my_mesh->AddAsset(mvisualizemeshB);	

		// This will paint the heat flux as line vectors
	ChSharedPtr<ChVisualizationFEMmesh> mvisualizemeshC(new ChVisualizationFEMmesh(*(my_mesh.get_ptr())));
	mvisualizemeshC->SetFEMdataType(ChVisualizationFEMmesh::E_PLOT_NONE);
	mvisualizemeshC->SetFEMglyphType(ChVisualizationFEMmesh::E_GLYPH_ELEM_VECT_DP);
	mvisualizemeshC->SetSymbolsScale(0.003);
	mvisualizemeshC->SetDefaultSymbolsColor(ChColor(0.1,0.2,0.2));
	mvisualizemeshC->SetZbufferHide(false);
	my_mesh->AddAsset(mvisualizemeshC);




			// ==IMPORTANT!== Use this function for adding a ChIrrNodeAsset to all items
			// in the system. These ChIrrNodeAsset assets are 'proxies' to the Irrlicht meshes.
			// If you need a finer control on which item really needs a visualization proxy in 
			// Irrlicht, just use application.AssetBind(myitem); on a per-item basis.

	application.AssetBindAll();

			// ==IMPORTANT!== Use this function for 'converting' into Irrlicht meshes the assets
			// that you added to the bodies into 3D shapes, they can be visualized by Irrlicht!

	application.AssetUpdateAll();


	// 
	// THE SOFT-REAL-TIME CYCLE
	//

	my_system.SetLcpSolverType(ChSystem::LCP_ITERATIVE_MINRES); // <- NEEDED because other solvers can't handle stiffness matrices
	my_system.SetIterLCPwarmStarting(false); // this helps a lot to speedup convergence in this class of problems
	my_system.SetIterLCPmaxItersSpeed(160);


	// Note: if you are interested only in a single LINEAR STATIC solution 
	// (not a transient thermal solution, but rather the steady-state solution), 
	// at this point you can uncomment the following line:
	//
 	//	 my_system.DoStaticLinear();
	//
	// Also, in the following while() loop, remove  application.DoStep();
	// so you can spin the 3D view and look at the solution.

	
	application.SetTimestep(0.01);

	while(application.GetDevice()->run()) 
	{
		application.BeginScene();

		application.DrawAll();
		
		application.DoStep();

		application.EndScene();
	}


	// Print some node temperatures..
	for (int inode = 0; inode < my_mesh->GetNnodes(); ++inode)
	{
		if (my_mesh->GetNode(inode).IsType<ChNodeFEMxyzP>())
		{
			ChSharedPtr<ChNodeFEMxyzP> mnode ( my_mesh->GetNode(inode).DynamicCastTo<ChNodeFEMxyzP>() ); // downcast
			if (mnode->GetPos().x <0.01)
			{
				GetLog() << "Node at y=" << mnode->GetPos().y << " has T=" << mnode->GetP() << "\n"; 
			}
		}
	}

	return 0;
}


