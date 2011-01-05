#ifndef CHIRRMESHTOOLS_H
#define CHIRRMESHTOOLS_H

//////////////////////////////////////////////////
//
//   ChIrrMeshTools.h
//
//   FOR IRRLICHT USERS ONLY!
//
//   Some functions to allow easy creation of
//   meshes for Irrlicht visualization
//
//   HEADER file for CHRONO,
//	 Multibody dynamics engine
//
// ------------------------------------------------
// 	 Copyright:Alessandro Tasora / DeltaKnowledge
//             www.deltaknowledge.com
// ------------------------------------------------
///////////////////////////////////////////////////


#include <irrlicht.h>

namespace irr
{
namespace scene
{



static IAnimatedMesh* createEllipticalMesh(f32 radiusH, f32 radiusV, f32 Ylow, f32 Yhigh, f32 offset, u32 polyCountX, u32 polyCountY)
{
	SMeshBuffer* buffer = new SMeshBuffer();

	// The following code is based on a modified version of the 
	// Irrlicht createSphereMesh function, that is also based on the
	// work by Alfaz93 
	bool disc_high = false;
	bool disc_low  = false;
	if (Yhigh <  radiusV) 
		disc_high  = true;
	if (Ylow  > -radiusV) 
		disc_low   = true;

	if (polyCountX < 2)
		polyCountX = 2;
	if (polyCountY < 2)
		polyCountY = 2;
	if (polyCountX * polyCountY > 32767) // prevent u16 overflow
		if (polyCountX > polyCountY) // prevent u16 overflow
			polyCountX = 32767/polyCountY-1;
		else
			polyCountY = 32767/(polyCountX+1);

	u32 polyCountXPitch = polyCountX+1; // get to same vertex on next level
	u32 n_tot_verts = (polyCountXPitch * polyCountY) + 2;
	u32 n_tot_verts_withoutdiscs = n_tot_verts;
	if (disc_high) n_tot_verts += polyCountXPitch;
	if (disc_low)  n_tot_verts += polyCountXPitch;
	u32 n_tot_indeces = (polyCountX * polyCountY) * 6;
	buffer->Vertices.set_used(n_tot_verts);
	buffer->Indices.set_used(n_tot_indeces);

	video::SColor clr(100, 255,255,255);

	u32 i=0;
	u32 i_disc=0;
	u32 level = 0;

	for (u32 p1 = 0; p1 < polyCountY-1; ++p1)
	{
		//main quads, top to bottom
		for (u32 p2 = 0; p2 < polyCountX - 1; ++p2)
		{
			const u32 curr = level + p2;
			buffer->Indices[i] = curr + polyCountXPitch;
			buffer->Indices[++i] = curr;
			buffer->Indices[++i] = curr + 1;
			buffer->Indices[++i] = curr + polyCountXPitch;
			buffer->Indices[++i] = curr+1;
			buffer->Indices[++i] = curr + 1 + polyCountXPitch;
			++i;
		}

		// the connectors from front to end
		buffer->Indices[i] = level + polyCountX - 1 + polyCountXPitch;
		buffer->Indices[++i] = level + polyCountX - 1;
		buffer->Indices[++i] = level + polyCountX;
		++i;

		buffer->Indices[i] = level + polyCountX - 1 + polyCountXPitch;
		buffer->Indices[++i] = level + polyCountX;
		buffer->Indices[++i] = level + polyCountX + polyCountXPitch;
		++i;
		level += polyCountXPitch;
	}

	u32 polyCountSq = polyCountXPitch * polyCountY; // top point
	u32 polyCountSq1 = polyCountSq + 1; // bottom point
	u32 polyCountSqM  = 0; // first row's first vertex
	u32 polyCountSqM1 = (polyCountY - 1) * polyCountXPitch; // last row's first vertex

	if (disc_high && ! disc_low)
		polyCountSqM = n_tot_verts_withoutdiscs;
	if (disc_low && ! disc_high)
		polyCountSqM1 = n_tot_verts_withoutdiscs;
	if (disc_low && disc_high)
	{
		polyCountSqM  = n_tot_verts_withoutdiscs;
		polyCountSqM1 = polyCountSqM + polyCountX+1;
	}

	for (u32 p2 = 0; p2 < polyCountX - 1; ++p2)
	{
		// create triangles which are at the top of the sphere
		buffer->Indices[i]   = polyCountSq;
		buffer->Indices[++i] = polyCountSqM + p2 + 1;
		buffer->Indices[++i] = polyCountSqM + p2;
		++i;

		// create triangles which are at the bottom of the sphere
		buffer->Indices[i]   = polyCountSqM1 + p2;
		buffer->Indices[++i] = polyCountSqM1 + p2 + 1;
		buffer->Indices[++i] = polyCountSq1;
		++i;
	}

	// create final triangle which is at the top of the sphere
	buffer->Indices[i]   = polyCountSq;
	buffer->Indices[++i] = polyCountSqM + polyCountX;
	buffer->Indices[++i] = polyCountSqM + polyCountX-1;
	++i;

	// create final triangle which is at the bottom of the sphere
	buffer->Indices[i]   = polyCountSqM1 + polyCountX - 1;
	buffer->Indices[++i] = polyCountSqM1;
	buffer->Indices[++i] = polyCountSq1;

	// calculate the angle which separates all points in a circle
	f64 r_low, alpha_low, r_high, alpha_high;
	if (disc_high)
	{
		r_high = radiusH* sqrt( 1- pow ( Yhigh/radiusV , 2) );
		alpha_high = atan( (radiusH/radiusV)*(Yhigh/r_high) );
	}
	else { r_high = 0; alpha_high = core::PI/2;}

	if (disc_low)
	{
		r_low  = radiusH* sqrt( 1- pow ( Ylow /radiusV , 2) );
		alpha_low  = atan( (radiusH/radiusV)*(Ylow /r_low) );
	} 
	else { r_low = 0; alpha_low = -(core::PI/2);}

	const f64 AngleX = 2 * core::PI / polyCountX;
	f64  borderslice = polyCountY; 
	if (disc_high) borderslice--;
	if (disc_low)  borderslice--;
	const f64 AngleY = (alpha_high-alpha_low) / borderslice; //= core::PI / polyCountY;

	i = 0;
	i_disc = n_tot_verts_withoutdiscs;
	f64 axz;

	f64 ay = (core::PI/2)-alpha_high; //=0   //=AngleY / 2;  // meant to work in 0..PI, building from top
	if (!disc_high) 
		ay += AngleY;

	for (u32 y = 0; y < polyCountY; ++y)
	{
		const f64 sinay = sin(ay);
		const f64 cosay = cos(ay);
		axz = 0;

		// calculate the necessary vertices without the doubled one
		for (u32 xz = 0;xz < polyCountX; ++xz)
		{
			// calculate points position

			core::vector3df pos((f32)(radiusH * cos(axz) * sinay),
									  (f32)(radiusV * cosay),
									  (f32)(radiusH * sin(axz) * sinay));
			// for spheres the normal is the position
			core::vector3df normal(pos);
			normal.normalize();
			
			// add the offset
			core::vector3df Roffset((f32)(offset * cos(axz)), 0, (f32)(offset * sin(axz)) );
			pos += Roffset;

			// calculate texture coordinates via sphere mapping
			// tu is the same on each level, so only calculate once
			f32 tu = 0.5f;
			if (y==0)
			{
				if (normal.Y != -1.0f && normal.Y != 1.0f)
					tu = (f32)(acos(core::clamp(normal.X/sinay, -1.0, 1.0)) * 0.5 *core::RECIPROCAL_PI64);
				if (normal.Z < 0.0f)
					tu=1-tu;
			}
			else
				tu = buffer->Vertices[i-polyCountXPitch].TCoords.X;

			buffer->Vertices[i] = video::S3DVertex(pos.X, pos.Y, pos.Z,
						normal.X, normal.Y, normal.Z,
						clr, tu,
						(f32)(ay*core::RECIPROCAL_PI64));
			++i;

			axz += AngleX;
		}
		// This is the doubled vertex on the initial position
		buffer->Vertices[i] = video::S3DVertex(buffer->Vertices[i-polyCountX]);
		buffer->Vertices[i].TCoords.X=1.0f;
		++i;
		ay += AngleY;
	}

	// the vertex at the top of the sphere
	if (disc_high)
		buffer->Vertices[i] = video::S3DVertex(0.0f,Yhigh,0.0f, 0.0f,1.0f,0.0f, clr, 0.5f, 0.0f);
	else
		buffer->Vertices[i] = video::S3DVertex(0.0f,radiusV,0.0f, 0.0f,1.0f,0.0f, clr, 0.5f, 0.0f);

	// the vertex at the bottom of the sphere
	++i;
	if (disc_low)
		buffer->Vertices[i] = video::S3DVertex(0.0f,Ylow,0.0f, 0.0f,-1.0f,0.0f, clr, 0.5f, 1.0f);
	else
		buffer->Vertices[i] = video::S3DVertex(0.0f,-radiusV,0.0f, 0.0f,-1.0f,0.0f, clr, 0.5f, 1.0f);

	i_disc = n_tot_verts_withoutdiscs;
	if (disc_high) 
	{
		for (u32 xz = 0;xz < (polyCountX+1); ++xz)
		{
			// duplicate points for top disc
			buffer->Vertices[i_disc] = video::S3DVertex(buffer->Vertices[i_disc-n_tot_verts_withoutdiscs]);
			buffer->Vertices[i_disc].Normal.set(0,1,0);
			i_disc++;
		}
	}
	int ifrom = 0;
	int mshift = (polyCountY - 1) * polyCountXPitch; // last row's first vertex
	if (disc_low) 
	{
		for (u32 xz = 0;xz < (polyCountX+1); ++xz)
		{
			// duplicate points for low disc   
			buffer->Vertices[i_disc] = video::S3DVertex(buffer->Vertices[ifrom + mshift]);
			buffer->Vertices[i_disc].Normal.set(0,-1,0);
			ifrom ++;
			i_disc++;
		}
	}
	

	// recalculate bounding box

	buffer->BoundingBox.reset(buffer->Vertices[i].Pos);
	buffer->BoundingBox.addInternalPoint(buffer->Vertices[i-1].Pos);
	buffer->BoundingBox.addInternalPoint(radiusH,0.0f,0.0f);
	buffer->BoundingBox.addInternalPoint(-radiusH,0.0f,0.0f);
	buffer->BoundingBox.addInternalPoint(0.0f,0.0f,radiusH);
	buffer->BoundingBox.addInternalPoint(0.0f,0.0f,-radiusH);

	
	SMesh* mmesh = new SMesh();
	mmesh->addMeshBuffer(buffer);
	mmesh->recalculateBoundingBox();
	
	SAnimatedMesh* Amesh = new SAnimatedMesh();
	Amesh->addMesh(mmesh);

	buffer->drop();
	mmesh->drop();
	
	return Amesh;
}


// This function is based on a modified version of the irrlicht_bullet demo,
// see  http://www.continuousphysics.com
// It is used to convert an Irrlicht mesh into a ChTriangleMesh, which is used
// for collision detection in Chrono::Engine.
static void fillChTrimeshFromIrlichtMesh(chrono::geometry::ChTriangleMesh* chTrimesh, scene::IMesh* pMesh)
{	
	chrono::ChVector<> vertices[3];
	u32 i,j,k,index,numVertices;
	u16* mb_indices;

	for(i=0;i<pMesh->getMeshBufferCount();i++){
		irr::scene::IMeshBuffer* mb=pMesh->getMeshBuffer(i);

		//extract vertex data
		//because the vertices are stored as structs with no common base class,
		// we need to handle each type separately
		if(mb->getVertexType()==irr::video::EVT_STANDARD){
			irr::video::S3DVertex* mb_vertices=(irr::video::S3DVertex*)mb->getVertices();
			mb_indices = mb->getIndices();
			numVertices = mb->getVertexCount();
			for(j=0;j<mb->getIndexCount();j+=3){ //get index into vertex list
				for (k=0;k<3;k++) { //three verts per triangle
					index = mb_indices[j+k];
					if (index > numVertices) 
						throw (chrono::ChException("Cannot convert corrupted Irrlicht mesh in ChronoEngine ChTriangleMesh."));
					vertices[k] = chrono::ChVector<>(mb_vertices[index].Pos.X,mb_vertices[index].Pos.Y,mb_vertices[index].Pos.Z);		
				}
				chTrimesh->addTriangle(vertices[0], vertices[1], vertices[2]);
			}
		}
		else if(mb->getVertexType()==irr::video::EVT_2TCOORDS){
			//same but for S3DVertex2TCoords data
			irr::video::S3DVertex2TCoords* mb_vertices=(irr::video::S3DVertex2TCoords*)mb->getVertices();
			u16* mb_indices = mb->getIndices();
			s32 numVertices = mb->getVertexCount();
			for(j=0;j<mb->getIndexCount();j+=3){ //index into irrlicht data
				for (k=0;k<3;k++) {
					s32 index = mb_indices[j+k];
					if (index > numVertices)  
						throw (chrono::ChException("Cannot convert corrupted Irrlicht mesh in ChronoEngine ChTriangleMesh."));
					vertices[k] = chrono::ChVector<>(mb_vertices[index].Pos.X,mb_vertices[index].Pos.Y,mb_vertices[index].Pos.Z);		
				}
				chTrimesh->addTriangle(vertices[0], vertices[1], vertices[2]);
			}
		}	
		// for EVT_TANGENTS vertex type, do nothing
	}

}


// Given a ChTriangleMesh object, computes an Irrlicht mesh. 
// Note: the ChTriangleMesh is a 'triangle soup', so no connectivity is
// used. As a consequence, mo Gourad/Phong shading is possible and all
// triangles will look flat.


static void fillIrlichtMeshFromChTrimesh(scene::IMesh* pMesh, chrono::geometry::ChTriangleMesh* chTrimesh, video::SColor clr = video::SColor(255, 255,255,255))
{
	SMeshBuffer* buffer = new SMeshBuffer();
	
	// From index-less triangle soup 
	buffer->Vertices.set_used( chTrimesh->getNumTriangles()*3);
	buffer->Indices.set_used ( chTrimesh->getNumTriangles()*3);
	
	for (int i = 0; i < chTrimesh->getNumTriangles(); i++)
	{
		chrono::ChVector<> normal = chTrimesh->getTriangle(i).GetNormal();
		chrono::ChVector<> pos;
		pos = chTrimesh->getTriangle(i).p1;
		buffer->Vertices[i*3+0] = video::S3DVertex((irr::f32)pos.x, (irr::f32)pos.y, (irr::f32)pos.z,
						(irr::f32)normal.x, (irr::f32)normal.y, (irr::f32)normal.z,
						clr, 0, 0);
		pos = chTrimesh->getTriangle(i).p2;
		buffer->Vertices[i*3+1] = video::S3DVertex((irr::f32)pos.x, (irr::f32)pos.y, (irr::f32)pos.z,
						(irr::f32)normal.x, (irr::f32)normal.y, (irr::f32)normal.z,
						clr, 0, 0);
		pos = chTrimesh->getTriangle(i).p3;
		buffer->Vertices[i*3+2] = video::S3DVertex((irr::f32)pos.x, (irr::f32)pos.y, (irr::f32)pos.z,
						(irr::f32)normal.x, (irr::f32)normal.y, (irr::f32)normal.z,
						clr, 0, 0);
		buffer->Indices[i*3+0]=i*3+0;
		buffer->Indices[i*3+1]=i*3+1;
		buffer->Indices[i*3+2]=i*3+2;
	}

	SMesh* mmesh = dynamic_cast<SMesh*>(pMesh);
	mmesh->addMeshBuffer(buffer);
	mmesh->recalculateBoundingBox();
}



} // END_OF_NAMESPACE____
} // END_OF_NAMESPACE____

#endif // END of ChIrrMeshTools.h
