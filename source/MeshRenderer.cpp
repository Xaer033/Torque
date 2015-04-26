
#include "MeshRenderer.h"

#include "GeometryBuffer3d.h"


namespace GG
{

	MeshRenderer::MeshRenderer( ComponentType cType, uint cEntityID ) : 
		GameComponent( cType, cEntityID )
	{
		_mesh		= NULL;
		_material	= NULL;
	}


	void MeshRenderer::setMesh( GeometryBuffer3d & mesh )
	{
		_mesh = &mesh;
	}

	GeometryBuffer3d & MeshRenderer::getMesh()
	{
		return *_mesh;
	}


	void MeshRenderer::setMaterial( Material & material )
	{
		_material = &material;
	}

	Material & MeshRenderer::getMaterial()
	{
		return *_material;
	}
}
