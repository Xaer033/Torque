/*
	Module: Mesh Renderer
	Description:	Component that represents the visible geometry 
					for a gameObject
	Author: Julian Williams
*/

#ifndef __MESH_RENDERER_H__
#define __MESH_RENDERER_H__

#include "GameComponent.h"

namespace GG
{
	class GeometryBuffer3d;
	struct Material;

	class MeshRenderer : public GameComponent
	{
	public:
		MeshRenderer( ComponentType cType, uint cEntityID );

		void setMesh( GeometryBuffer3d & mesh );
		GeometryBuffer3d & getMesh();

		void setMaterial( Material & material );
		Material & getMaterial();


	private:
		GeometryBuffer3d * _mesh;
		Material * _material;

	};
}

#endif
