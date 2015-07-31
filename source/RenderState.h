/*
	Module: Object to handle gl code for binding / rendering
	Description:	Camera object to see the world through and apply
				camera view matrix to world geometry
	Author: Julian Williams
*/

#ifndef _RENDER_STATES_H_
#define _RENDER_STATES_H_

#include "IwTypes.h"
#include "IwGL.h"

#include "Vector.h"
#include <IVertexBuffer.h>

namespace GG
{



    enum class BlendMode
	{
		BM_NONE     = 0,
		BM_ALPHA,
		BM_ADDITIVE,
		BM_MULTIPLY,
		BM_SCREEN
	};

	enum ClearMode
	{
		CM_COLOR	= GL_COLOR_BUFFER_BIT,
		CM_DEPTH	= GL_DEPTH_BUFFER_BIT,
		CM_STENCIL	= GL_STENCIL_BUFFER_BIT
	};

    enum class CullMode
	{
		CULL_NONE   = 0,
		CULL_FRONT	= GL_FRONT,
		CULL_BACK	= GL_BACK,
		CULL_FRONT_AND_BACK = GL_FRONT_AND_BACK
	};

	class RenderStateImpl;

	class Texture;
	class IVertexBuffer;
	class Shader;
	class FrameBufferObject;

	class RenderState
	{
	public:
		RenderState();
		~RenderState();

		void clear( int mode );
		void setClearColor( float r, float g, float b, float a ) const;
		void setClearColor( const Vector4 & clearColor ) const;

		void setBlendmode( const BlendMode mode );

		void setRenderSize( int width, int height );
		void setViewport( float x, float y, float width, float height );

		void setDepthTesting( bool depthTest );
		void setDepthRange( float zNear, float zFar );

		void setCullMode( const CullMode cullMode );

		void bindVertexBuffer( IVertexBuffer & geoBuffer );
		void bindTexture2d( uint samplerID, const Texture & texture );
		void bindShader( const Shader & shader );

		void bindFrameBuffer( const uint & frameBuffer  );
		void bindFrameBuffer( const FrameBufferObject & frameBuffer  );

		void renderBoundGeometry( DrawMode mode );

	private:
		RenderStateImpl * _impl;

	};
}

#endif
