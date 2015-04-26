/*
	Module: RenderFactory
	Description: Factory that creates and manages renderable components 
				for various gameobjects
	Author: Julian Williams
*/


#ifndef _RENDER_FACTORY_H_
#define _RENDER_FACTORY_H_

#include <map>
#include <IwTypes.h>

#include "Singleton.h"


namespace GG
{
	class MeshRenderer;

	class RenderFactory : public Singleton< RenderFactory >
	{
	public:
		typedef std::map< uint, MeshRenderer* > RenderMap;

	public:

		MeshRenderer * createMeshRenderer( uint entityID );
		MeshRenderer * getMeshRenderer( uint entityID );

		RenderMap & getRenderMap()
		{
			return _renderMap;
		}
	private:
		
		RenderMap _renderMap;
	};

}

#endif