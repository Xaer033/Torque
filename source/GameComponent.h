/*
	Module: Mesh Renderer
	Description:	Base Game Component class to inherit from 
	Author: Julian Williams
*/

#ifndef __GAME_COMPONENT_H__
#define __GAME_COMPONENT_H__


#include <IwTypes.h>

namespace GG
{
	
	enum ComponentType
	{
		TRANSFORM,
		PLAYER_INPUT,
		MESH_RENDERER,
		VIDEO_RENDERER,
		PARTICLE_RENDERER,
		SKINNED_MESH_RENDERER,
		SCRIPT
	};

	class Transform;


	class GameComponent
	{
	public:
		GameComponent( ComponentType cType, uint cEntityID ) : 
			type( cType ), _entityID( cEntityID )
		{}

		virtual ~GameComponent()
		{}

		virtual bool registerScriptEngine( /*const ScriptManager & scriptManager*/ )
		{
			return true;
		}

		virtual void init()
		{}

		virtual void update( float deltaTime )
		{}

		virtual void fixedUpdate( )
		{}


		ComponentType getType() const
		{
			return type;
		}

		uint getEntityID() const
		{
			return _entityID;
		}

	private:
		ComponentType type;
		uint _entityID;

	};
}
#endif


