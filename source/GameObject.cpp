
#include "GameObject.h"

#include <map>
#include <vector>


#include "GameComponent.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "RenderFactory.h"


namespace GG
{
	typedef  std::map< const char *, GameComponent *> ComponentMap;

	class GameObjectImpl
	{
	public:
		
		ComponentMap componentMap;
		Transform transformCache;

		char * _name;

		int _layer;
		int _tag;

		bool _markedForRemoval;

		uint _entityID;

	public:
		GameObjectImpl( uint entityID,  const char * name );
		~GameObjectImpl( );
	
		void setName( const char * name );
		const char * getName( ) const;

		void setLayer( int layer );
		int	 getLayer( ) const;

		void setTag( int tag );
		int  getTag( ) const;

		
		void	markForRemoval();
		bool	doRemove() const;

		void update( float deltaTime );
		void fixedUpdate();

		GameComponent * addComponent( const char * name, const ComponentType cType);
		GameComponent * getComponent( const char * name );

		void removeComponent( const char * name );

		ComponentList getAllComponentsOfType( const ComponentType cType );


	};



//******************************* Interface ******************************//
	GameObject::GameObject( uint entityID, const char * name )
	{
		_impl = new GameObjectImpl( entityID, name );
	}


	GameObject::~GameObject( )
	{
		delete _impl;
	}


	void GameObject::setLayer( int layer )
	{
		_impl->setLayer( layer );
	}

	int GameObject::getLayer( ) const
	{
		return _impl->getLayer();
	}

	void GameObject::setTag( int tag )
	{
		_impl->setTag( tag );
	}

	int GameObject::getTag( ) const
	{
		return _impl->getTag();
	}

	
		
	void GameObject::markForRemoval()
	{
		_impl->markForRemoval();
	}

	bool GameObject::doRemove() const
	{
		return _impl->doRemove();
	}


	void GameObject::update( float deltaTime )
	{
		_impl->update( deltaTime );
	}


	void GameObject::fixedUpdate()
	{
		_impl->fixedUpdate();
	}


	GameComponent *  GameObject::addComponent( const char * name, const ComponentType cType )
	{
		return _impl->addComponent( name, cType );
	}

	GameComponent * GameObject::getComponent( const char * name )
	{
		return _impl->getComponent( name );
	}

	void GameObject::removeComponent( const char * name )
	{
		_impl->removeComponent( name );
	}

	
	ComponentList GameObject::getAllComponentsOfType( const ComponentType cType )
	{
		return _impl->getAllComponentsOfType( cType );
	}
	/****************************************************
		Game Object Implementation
	*****************************************************/
	GameObjectImpl::GameObjectImpl( uint entityID, const char * name )
	{
		setName( name );

		_layer	= 0;
		_tag	= 0;
	
		_markedForRemoval = false;
		_entityID = entityID;
	}

	GameObjectImpl::~GameObjectImpl()
	{
		ComponentMap::iterator  it = componentMap.begin();
		for(; it != componentMap.end(); ++it )
		{
			if( it->second )
				delete( it->second );
		}

		componentMap.clear();
	}


	void GameObjectImpl::setName( const char * name )
	{
		if( name != NULL )
			_name = const_cast< char * >( name );
	}

	const char * GameObjectImpl::getName( ) const
	{
		return _name;
	}

	void GameObjectImpl::setLayer( int layer )
	{
		_layer = layer;
	}

	int GameObjectImpl::getLayer( ) const
	{
		return _layer;
	}

	void GameObjectImpl::setTag( int tag )
	{
		_tag = tag;
	}

	int GameObjectImpl::getTag( ) const
	{
		return _tag;
	}

	
		
	void GameObjectImpl::markForRemoval()
	{
		_markedForRemoval = true;
	}

	bool GameObjectImpl::doRemove() const
	{
		return _markedForRemoval;
	}


	void GameObjectImpl::update( float deltaTime )
	{
		ComponentMap::iterator  it = componentMap.begin();
		for( ; it != componentMap.end(); ++it )
			it->second->update( deltaTime );
	}

	void GameObjectImpl::fixedUpdate()
	{
		ComponentMap::iterator  it = componentMap.begin();
		for( ; it != componentMap.end(); ++it )
		{
			it->second->fixedUpdate();
		}
	}

	GameComponent * GameObjectImpl::addComponent( const char * name, const ComponentType cType)
	{
		if( !name )
			return NULL;

		GameComponent * component = NULL;


		switch( cType )
		{
		case MESH_RENDERER:
			component = RenderFactory::getInstance().createMeshRenderer( _entityID );
			break;

		}

		component->init();
		componentMap[ name ] = component;

		return component;
	}

	GameComponent * GameObjectImpl::getComponent( const char * name )
	{
		if( !name )
			return NULL;

		ComponentMap::iterator it = componentMap.find( name );
		if( it == componentMap.end( ) )
			return NULL;

		return it->second;
	}

	void GameObjectImpl::removeComponent( const char * name )
	{
		if( !name )
			return;

		ComponentMap::iterator it = componentMap.find( name );
		if( it != componentMap.end( ) )
			componentMap.erase( it );
	}


	ComponentList GameObjectImpl::getAllComponentsOfType( const ComponentType cType )
	{
		ComponentList tempList;

		ComponentMap::iterator  it = componentMap.begin();
		for(; it != componentMap.end(); ++it)
		{
			if( it->second->getType() == (uint)cType )
				tempList.push_back( it->second );
		}


		return tempList;
	}

}