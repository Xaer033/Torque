/*
	Module: GameObject
	Description: Entity that represents something in the game world
	Author: Julian Williams
*/


#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__



#include "GameComponent.h"

#include "Transform.h"
#include <vector>


namespace GG
{
	
	typedef std::vector< GameComponent * > ComponentList;

	
	class GameComponent;
	class GameObjectImpl;




	class GameObject
	{
	public:
		GameObject ( uint entityID, const char * name = NULL );
		~GameObject( void );
	
		void			setName( const char * name );
		const char *	getName( ) const;

		void	setLayer( int layer );
		int		getLayer( ) const;

		void	setTag( int tag );
		int		getTag( ) const;

		void	markForRemoval();
		bool	doRemove() const;

		void	update( float deltaTime );
		void	fixedUpdate();

		GameComponent * addComponent( const char * name, const ComponentType cType );
		GameComponent * getComponent( const char * name );
		void	removeComponent( const char * name );

		///All GameObjects will have a Transform
		Transform transform;

		///This returns an array of all the components of this type,
		///attached to the game object
		ComponentList getAllComponentsOfType( const ComponentType cType );

	private:
		GameObjectImpl * _impl;

	};

}



#endif

