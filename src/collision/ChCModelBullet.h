//
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2010-2011 Alessandro Tasora
// Copyright (c) 2013 Project Chrono
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be 
// found in the LICENSE file at the top level of the distribution
// and at http://projectchrono.org/license-chrono.txt.
//

#ifndef CHC_MODELBULLET_H
#define CHC_MODELBULLET_H
 
//////////////////////////////////////////////////
//  
//   ChCModelBullet.h
//
//   A wrapper to use the Bullet collision detection
//   library
//
//   HEADER file for CHRONO,
//	 Multibody dynamics engine
//
// ------------------------------------------------
//             www.deltaknowledge.com
// ------------------------------------------------
///////////////////////////////////////////////////


#include <vector>
#include "ChCCollisionModel.h" 
#include "core/ChSmartpointers.h"
#include "BulletCollision/CollisionShapes/btCollisionShape.h"

// forward references
class btCollisionObject;
//class btCollisionShape;

namespace chrono 
{

// forward references
class ChBody;
typedef ChSmartPtr<btCollisionShape> smartptrshapes;

namespace collision 
{

class ChConvexDecomposition;

///  A wrapper to use the Bullet collision detection
///  library

class ChApi ChModelBullet : public ChCollisionModel
{
protected:
			// The Bullet collision object containing Bullet geometries
	btCollisionObject* bt_collision_object;

			// Vector of shared pointers to geometric objects.
    std::vector<smartptrshapes> shapes; 

	short int	family_group;
	short int	family_mask;

public:

  ChModelBullet();
  virtual ~ChModelBullet();
  

		/// Deletes all inserted geometries.
		/// Also, if you begin the definition of a model, AFTER adding
		/// the geometric description, remember to call the ClearModel().
		/// MUST be inherited by child classes! (ex for resetting also BV hierarchies)
  virtual int ClearModel();

		/// Builds the BV hierarchy.
		/// Call this function AFTER adding the geometric description.
		/// MUST be inherited by child classes! (ex for bulding BV hierarchies)
  virtual int BuildModel();


	// 
	// GEOMETRY DESCRIPTION
	//
	//  The following functions must be called inbetween 
	//  the ClearModel() BuildModel() pair.

	/// Add a sphere shape to this model, for collision purposes
  virtual bool AddSphere (double            radius,
                          const ChVector<>& pos = ChVector<>());

	/// Add an ellipsoid shape to this model, for collision purposes
  virtual bool AddEllipsoid (double              rx,
                             double              ry,
                             double              rz,
                             const ChVector<>&   pos = ChVector<>(),
                             const ChMatrix33<>& rot = ChMatrix33<>(1));

	/// Add a box shape to this model, for collision purposes
  virtual bool AddBox (double              hx,
                       double              hy,
                       double              hz,
                       const ChVector<>&   pos = ChVector<>(),
                       const ChMatrix33<>& rot = ChMatrix33<>(1));

	/// Add a cylinder to this model (default axis on Y direction), for collision purposes
  virtual bool AddCylinder (double              rx,
                            double              rz,
                            double              hy,
                            const ChVector<>&   pos = ChVector<>(),
                            const ChMatrix33<>& rot = ChMatrix33<>(1));

	/// Add a cylinder to this model (default axis on Y direction), for collision purposes
  virtual bool AddCone (double              rx,
                        double              rz,
                        double              hy,
                        const ChVector<>&   pos = ChVector<>(),
                        const ChMatrix33<>& rot = ChMatrix33<>(1)) {return false;}

	/// Add a capsule to this model (default axis in Y direction), for collision purposes
  virtual bool AddCapsule(double              radius,
                          double              hlen,
                          const ChVector<>&   pos = ChVector<>(),
                          const ChMatrix33<>& rot = ChMatrix33<>(1)) {return false;}

  /// Add a rounded box shape to this model, for collision purposes
  virtual bool AddRoundedBox(double hx,
                             double hy,
                             double hz,
                             double sphere_r,
                             const ChVector<> &pos = ChVector<>(),
                             const ChMatrix33<> &rot = ChMatrix33<>(1)
                             ){return false;}


  /// Add a rounded cylinder to this model (default axis on Y direction), for collision purposes
  virtual bool AddRoundedCylinder(double rx,
                                  double rz,
                                  double hy,
                                  double sphere_r,
                                  const ChVector<> &pos = ChVector<>(),
                                  const ChMatrix33<> &rot = ChMatrix33<>(1)
                                  ){return false;}

  /// Add a rounded cone to this model (default axis on Y direction), for collision purposes
  virtual bool AddRoundedCone(double rx,
                              double rz,
                              double hy,
                              double sphere_r,
                              const ChVector<> &pos = ChVector<>(),
                              const ChMatrix33<> &rot = ChMatrix33<>(1)
                              ){return false;}

	/// Add a convex hull to this model. A convex hull is simply a point cloud that describe
	/// a convex polytope. Connectivity between the vertexes, as faces/edges in triangle meshes is not necessary.
	/// Points are passed as a list, that is instantly copied into the model.
  virtual bool AddConvexHull (std::vector<ChVector<double> >& pointlist,
                              const ChVector<>&               pos = ChVector<>(),
                              const ChMatrix33<>&             rot = ChMatrix33<>(1));

	/// Add a triangle mesh to this model, passing a triangle mesh (do not delete the triangle mesh
	/// until the collision model, because depending on the implementation of inherited ChCollisionModel
	/// classes, maybe the triangle is referenced via a striding interface or just copied)
	/// Note: if possible, in sake of high performance, avoid triangle meshes and prefer simplified 
	/// representations as compounds of convex shapes of boxes/spheres/etc.. type. 
  virtual bool AddTriangleMesh (const geometry::ChTriangleMesh& trimesh,
                                bool                            is_static,
                                bool                            is_convex,
                                const ChVector<>&               pos = ChVector<>(),
                                const ChMatrix33<>&             rot = ChMatrix33<>(1));

		/// CUSTOM for this class only: add a concave triangle mesh that will be managed
		/// by GImpact mesh-mesh algorithm. Note that, despite this can work with
		/// arbitrary meshes, there could be issues of robustness and precision, so 
		/// when possible, prefer simplified representations as compounds of convex 
		/// shapes of boxes/spheres/etc.. type.
    virtual bool AddTriangleMeshConcave (
                       const  geometry::ChTriangleMesh& trimesh,
                       const ChVector<>&                pos = ChVector<>(),
                       const ChMatrix33<>&              rot = ChMatrix33<>(1));

	/// CUSTOM for this class only: add a concave triangle mesh that will be decomposed
	/// into a compound of convex shapes. Decomposition could be more efficient than 
	/// AddTriangleMeshConcave(), but preprocessing decomposition might take a while, and
	/// decomposition result is often approximate. Therefore, despite this can work with
	/// arbitrary meshes, there could be issues of robustness and precision, so 
	/// when possible, prefer simplified representations as compounds of convex 
	/// shapes of boxes/spheres/etc.. type.
    virtual bool AddTriangleMeshConcaveDecomposed (
                       ChConvexDecomposition& mydecomposition,
                       const ChVector<>&      pos = ChVector<>(),
                       const ChMatrix33<>&    rot = ChMatrix33<>(1));

	/// Add a barrel-like shape to this model (main axis on Y direction), for collision purposes.
	/// The barrel shape is made by lathing an arc of an ellipse around the vertical Y axis.
	/// The center of the ellipse is on Y=0 level, and it is ofsetted by R_offset from 
	/// the Y axis in radial direction. The two radii of the ellipse are R_vert (for the 
	/// vertical direction, i.e. the axis parellel to Y) and R_hor (for the axis that
	/// is perpendicular to Y). Also, the solid is clamped with two discs on the top and
	/// the bottom, at levels Y_low and Y_high. 
  virtual bool AddBarrel (double              Y_low,
                          double              Y_high,
                          double              R_vert,
                          double              R_hor,
                          double              R_offset,
                          const ChVector<>&   pos = ChVector<>(),
                          const ChMatrix33<>& rot = ChMatrix33<>(1));

	/// Add all shapes already contained in another model.
	/// Thank to the adoption of shared pointers, underlying shapes are 
	/// shared (not copied) among the models; this will save memory when you must
	/// simulate thousands of objects with the same collision shape.
	/// The 'another' model must be of ChModelBullet subclass.
  virtual bool AddCopyOfAnotherModel (ChCollisionModel* another);


  virtual void SetFamily(int mfamily);
  virtual int  GetFamily();
  virtual void SetFamilyMaskNoCollisionWithFamily(int mfamily);
  virtual void SetFamilyMaskDoCollisionWithFamily(int mfamily);
  virtual bool GetFamilyMaskDoesCollisionWithFamily(int mfamily);

		/// Returns the axis aligned bounding box (AABB) of the collision model,
		/// i.e. max-min along the x,y,z world axes. Remember that SyncPosition()
		/// should be invoked before calling this.
  virtual void GetAABB(ChVector<>& bbmin, ChVector<>& bbmax) const;

	
			//
			// STREAMING
			//

		/// Method to allow deserializing a persistent binary archive (ex: a file)
		/// into transient data.
  virtual void StreamIN(ChStreamInBinary& mstream);

		/// Method to allow serializing transient data into a persistent
		/// binary archive (ex: a file).
  virtual void StreamOUT(ChStreamOutBinary& mstream);


	  //
	  // CUSTOM BULLET
	  //

		/// Return the pointer to the Bullet model
  btCollisionObject* GetBulletModel() {return this->bt_collision_object;}

  short int GetFamilyGroup() {return this->family_group;}
  short int GetFamilyMask() {return this->family_mask;}

private:
	void _injectShape(const ChVector<>& pos, const ChMatrix33<>& rot, btCollisionShape* mshape);
};






} // END_OF_NAMESPACE____
} // END_OF_NAMESPACE____


#endif
