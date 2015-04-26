
#include "Transform.h"

#include "DebugText.h"

#include <list>	

#include "Matrix.h"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"


namespace GG
{

	Transform::Transform()
	{
		_parent = NULL;
	}

	Transform::~Transform( )
	{
	}
	

	Vector3 Transform::forward()	const
	{
		return -back();
	}

	Vector3 Transform::back() const
	{
		return glm::normalize( Vector3( _worldMat[2][0], _worldMat[2][1], _worldMat[2][2] ) );
	}
	
	Vector3 Transform::right() const
	{
		return glm::normalize( Vector3( _worldMat[0][0], _worldMat[0][1], _worldMat[0][2] ) );
	}
	
	Vector3 Transform::left() const
	{
		return -right();
	}

	Vector3 Transform::up() const
	{
		return glm::normalize( Vector3( _worldMat[1][0], _worldMat[1][1], _worldMat[1][2] ) );
	}

	Vector3 Transform::down() const 
	{
		return -up();
	}


	glm::mat4 Transform::getMatrix() const
	{
		return _worldMat;
	}

	void Transform::setParent( Transform * parent )
	{
		if( _parent == parent )
			return; 

		Transform * oldp = _parent;
		_parent = parent;

		if( _parent != NULL )
		{
			glm::mat4 invParent = _parent->getMatrix();
			invParent = glm::inverse( invParent );

			_modelMat =  invParent * _worldMat;
			
			_parent->addChild( this );


		}else{

			if( oldp != NULL )
				oldp->removeChild( this );

			_modelMat = _worldMat;
		}

		_updateHierarchy( );

	}

	Transform * Transform::getParent()
	{
		return _parent;
	}


	void Transform::addChild( Transform * child )
	{
		_childrenList.push_back( child );
	}

	void Transform::removeChild( Transform * child )
	{
		_childrenList.remove( child );
	}


	void Transform::lookAt( const Vector3 & center, const Vector3 & up )
	{
		Vector3 pos		= getPosition();
		Vector3 delta	= pos - center;

		if(	( fabs( delta.x ) < 0.001f) &&
			( fabs( delta.y ) < 0.001f) &&
			( fabs( delta.z ) < 0.001f)) 
			return;


		_modelMat	= glm::lookAt( pos, center, up );
		_modelMat	= glm::inverse( _modelMat );

		//glm::mat4 & m		= _modelMat._m;
		//
		//m[0][0]	= invMat[0][0];
		//m[0][1] = invMat[0][1];
		//m[0][2] = invMat[0][2];
		//
		//m[1][0]	= invMat[1][0];
		//m[1][1] = invMat[1][1];
		//m[1][2] = invMat[1][2];
		//
		//m[2][0]	= invMat[2][0];
		//m[2][1] = invMat[2][1];
		//m[2][2] = invMat[2][2];
		
		_updateHierarchy( );
	}

	void Transform::lookAt( const Transform & center, const Vector3 & up )
	{
		lookAt( center.getPosition(), up );
	}

	Vector3 Transform::getPosition( Space space ) const
	{
		if( space == GG::LOCAL )
			return Vector3( _modelMat[3][0], _modelMat[3][1], _modelMat[3][2] );
		else
			return Vector3( _worldMat[3][0], _worldMat[3][1], _worldMat[3][2] );
	}

	void Transform::setPosition( const Vector3 & position )
	{
		_modelMat[3][0]	= position.x;
		_modelMat[3][1]	= position.y;
		_modelMat[3][2]	= position.z;

		_updateHierarchy( );
	}

	void Transform::setAxisAngle( float angle, const Vector3 & axis )
	{
		glm::mat4 rotateMat = glm::rotate( angle, axis );

		//Set rotation component only
		for( uint i = 0; i < 3; ++i )
		{
			_modelMat[0][i]	= rotateMat[0][i];
			_modelMat[1][i]	= rotateMat[1][i];
			_modelMat[2][i]	= rotateMat[2][i];
		}

		_updateHierarchy( );
	}


	void Transform::setScale( const Vector3 & scale )
	{
		_modelMat[0][3]	= scale.x;
		_modelMat[1][3]	= scale.y;
		_modelMat[2][3]	= scale.z;

		_updateHierarchy( );
	}

	void Transform::translate( const Vector3 & translation, Space moveSpace )
	{
		Vector4 pos( translation.x, translation.y, translation.z, 1 );
		
		if( moveSpace == LOCAL && _parent != NULL )
		{
			pos = _modelMat * pos;

			_modelMat[3][0]	= pos.x;
			_modelMat[3][1]	= pos.y;
			_modelMat[3][2]	= pos.z;
		}
		else
		{
			_modelMat[3][0]	+= pos.x;
			_modelMat[3][1]	+= pos.y;
			_modelMat[3][2]	+= pos.z;	
		}

		_updateHierarchy( );
	}

	void Transform::rotate( float angle, const Vector3 & axis, Space rotateSpace )
	{
		_modelMat = glm::rotate( _modelMat, angle, axis );

		_updateHierarchy( );
	}

	void Transform::scale( const Vector3 & scale, Space scaleSpace )
	{
		_modelMat = glm::scale( _modelMat, scale );

		_updateHierarchy( );
	}


	void Transform::inverse()
	{
		_modelMat = glm::inverse( _modelMat );
	}

	void Transform::_updateHierarchy( )
	{
		if( _parent != NULL )
			_worldMat =  _parent->getMatrix() * _modelMat;
		else
			_worldMat = _modelMat;

		_updateChildren();
	}

	void Transform::_updateChildren()
	{
		//Loop threw children to update them
		std::list<Transform*>::iterator it;
		for( it = _childrenList.begin(); it != _childrenList.end(); ++it )
		{
			if( *it )
				(*it)->_updateHierarchy( );
			else
				_childrenList.erase( it );
		}
	}
}
