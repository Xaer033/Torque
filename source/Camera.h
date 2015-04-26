/*
	Module: lights, *CAMERA*, action!
	Description:	Camera object to see the world through and apply
				camera view matrix to world geometry
	Author: Julian Williams
*/

#ifndef _CAMERA_H_
#define _CAMERA_H_


#include "Transform.h"
#include "Matrix.h"
#include "Vector.h"


namespace GG
{

	class Camera
	{
	public:
		
		Camera( const char * name = NULL );
		~Camera();

		void	setEnabled( bool enabled );
		bool	isEnabled() const;

		void	setName( const char * name );
		char *	getName( ) const;

		void	setLayer( int layer );
		int		getLayer() const;

		void	setDepth( int depth );
		int		getDepth( ) const;
		static	bool depthCompare( const Camera * a, const Camera * b );

		void	setClearMode( int clearMode );
		int		getClearMode( ) const;

		void	setClearColor( const Vector4 & clearColor );
		Vector4	getClearColor( ) const;

		void	setOrthogonal( float left, float right, float bottom, float top, float zNear, float zFar );
		void	setPerspective( float fovy, float width, float height, float zNear, float zFar );
		
		void	setViewport( float x, float y, float width, float height );
		Vector4 getViewport( ) const;


		Matrix4	&	getViewMatrix();
		Matrix4 &	getProjectionMatrix();


		Transform transform;

	private:

		bool		_enabled;

		int			_depth;

		int			_layer;

		int			_clearMode;

		char *		_name;
		Matrix4		_projectionMatrix;
		Matrix4		_viewMatrix;

		Vector4		_cachedClearColor;
		Vector4		_cachedViewport;
	};
}

#endif