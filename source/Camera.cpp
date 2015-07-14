
#include "Camera.h"

#include "IwGL.h"

#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"


namespace GG
{

	Camera::Camera( const char * name )
	{
		setName( name );
		setEnabled( true );
		setDepth( 0 );
		setLayer( 0 );
		setPerspective( 45, 920, 640, 0.1f, 100 );
		setViewport( 0, 0, 1, 1 );
		setClearColor( Vector4( 0, 0, 0, 0 ) );
	}
	
	Camera::~Camera()
	{

	}

	void Camera::setEnabled( bool enabled )
	{
		_enabled = enabled;
	}

	bool Camera::isEnabled( ) const
	{
		return _enabled;
	}

	void Camera::setName( const char * name )
	{
		_name = const_cast< char * >( name );
	}

	char * Camera::getName( ) const 
	{
		return _name;
	}

	void Camera::setLayer( int layer )
	{
		_layer = layer;
	}

	int Camera::getLayer( ) const
	{
		return _layer;
	}

	void Camera::setDepth( int depth )
	{
		_depth = depth;
	}

	void Camera::setClearColor( const Vector4 & clearColor )
	{
		if( clearColor == _cachedClearColor )
			return;

		_cachedClearColor = clearColor;
	}

	Vector4 Camera::getClearColor( ) const
	{
		return _cachedClearColor;
	}

	bool Camera::depthCompare( const Camera * a, const Camera * b )
	{
		if( a == NULL || b == NULL )
			return false;

		return a->getDepth() < b->getDepth();
	}

	int Camera::getDepth( ) const
	{
		return _depth;
	}


	void Camera::setClearMode( int clearMode ) 
	{
		_clearMode = clearMode;
	}

	int Camera::getClearMode( ) const
	{
		return _clearMode;
	}


	void Camera::setPerspective( float fovy, float width, float height, float zNear, float zFar )
	{
		_projectionMatrix = glm::perspectiveFov( fovy, width, height, zNear, zFar );
	}

	void Camera::setOrthogonal( float left, float right, float bottom, float top, float zNear, float zFar )
	{
		_projectionMatrix = glm::ortho( left, right, bottom, top, zNear, zFar  );
	}

	void Camera::setViewport( float x, float y, float width, float height )
	{
		_cachedViewport = Vector4( x, y, width, height );
	}

	Vector4 Camera::getViewport( ) const
	{
		return _cachedViewport;
	}

    Matrix4 Camera::getModelViewProjection( const Transform & model )
    {
        return getProjectionMatrix() * getViewMatrix() * model.getMatrix();
    }

	Matrix4 & Camera::getViewMatrix( ) 
	{
		_viewMatrix = glm::inverse( transform.getMatrix() );
		return _viewMatrix;
	}

	Matrix4 & Camera::getProjectionMatrix( ) 
	{
		return _projectionMatrix;
	}


}