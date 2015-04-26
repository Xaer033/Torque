#include "RenderFactory.h"

#include "Transform.h"
#include "MeshRenderer.h"


namespace GG
{
	MeshRenderer * RenderFactory::createMeshRenderer( uint entityID )
	{
		const ComponentType type = MESH_RENDERER;
		_renderMap[ entityID ] =  new MeshRenderer( type, entityID );
		
		return _renderMap[ entityID ];
	}

	MeshRenderer * RenderFactory::getMeshRenderer( uint entityID )
	{
		RenderMap::iterator it = _renderMap.find( entityID );
		if( it != _renderMap.end() )
			return it->second;

		return NULL;
	}
}