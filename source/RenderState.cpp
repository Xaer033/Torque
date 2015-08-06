
#include "RenderState.h"

#include <vector>
#include <hash_map>


#include <IwGL.h>
#include <IwGLExt.h>
#include <Shader.h>
#include <IVertexBuffer.h>

#include <Texture.h>
#include <FrameBufferObject.h>


//#define BUFFER_OFFSET(i) ((void*)(i))


namespace GG
{

	typedef std::hash_map< uint, uint > TextureCache;


	class RenderStateImpl
	{
	public:

		IVertexBuffer *	    _cachedVertexBuffer;
		uint				_cachedShader;

		TextureCache		_cachedTextures;
		
		BlendMode			_cachedBlendmode;

		bool				_cachedDepthTest;

		uint				_cachedFrameBuffer;

		CullMode			_cachedCullMode;

		uint				_renderWidth;
		uint				_renderHeight;

	public:

		RenderStateImpl()
		{
			clearIvars();
		}

		~RenderStateImpl()
		{
			clearIvars();
		}

		void clear( int mode )
		{
			glClear( mode );
		}

		void setClearColor( float r, float g, float b, float a ) const
		{
			glClearColor( r, g, b, a );
		}

		void setClearColor( const Vector4 & clearColor ) const
		{
			setClearColor( clearColor.r, clearColor.g, clearColor.b, clearColor.a );
		}

		void setDepthTesting( bool depthTest )
		{
			if( _cachedDepthTest == depthTest )
				return;

			if( depthTest )
				glEnable( GL_DEPTH_TEST );
			else
				glDisable( GL_DEPTH_TEST );

			_cachedDepthTest = depthTest;
		}

		void setDepthRange( float zNear, float zFar )
		{
			glDepthRangef( zNear, zFar );
		}

		void setCullMode( const CullMode cullMode )
		{
			if( _cachedCullMode == cullMode )
				return;

            if( cullMode == CULL_NONE )
			{
				glDisable( GL_CULL_FACE );
			}
			else
			{
				glEnable( GL_CULL_FACE );
				glCullFace( (GLenum)cullMode );
			}

			_cachedCullMode = cullMode;

		}

		void setBlendmode( const BlendMode mode )
		{
			if( _cachedBlendmode == mode )
				return;

			_cachedBlendmode = mode;


            if( mode == BM_NONE )
			{
				glDisable( GL_BLEND );
				return;
			}
				

			glEnable( GL_BLEND );

			switch( mode )
			{
                case BM_ALPHA:		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );	break;
				case BM_ADDITIVE:	glBlendFunc( GL_SRC_ALPHA, GL_ONE );					break;
                case BM_MULTIPLY:	glBlendFunc( GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA );	break;
                case BM_SCREEN:		glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_COLOR );			break;
			}
		}


		void setRenderSize( int width, int height )
		{
			_renderWidth	= width;
			_renderHeight	= height;
		}

		void setViewport( float x, float y, float width, float height )
		{
			Vector2 xVec = Vector2( x,  width ) * (float)_renderWidth;
			Vector2 yVec = Vector2( y , height ) * (float)_renderHeight;

			glViewport((GLint)xVec.x, (GLint)yVec.x, (GLsizei)xVec.y, (GLsizei)yVec.y );
		}


		void bindVertexBuffer( IVertexBuffer & buffer )
		{
            if( &buffer == _cachedVertexBuffer )
				return;

            buffer.bind();

            _cachedVertexBuffer = &buffer;
		}


		void bindTexture2d( uint samplerID, const Texture & texture )
		{
			TextureCache::iterator it =  _cachedTextures.find( samplerID );

			uint texHandle = texture.getHandle();

			if( it == _cachedTextures.end() )
				_setTexture( samplerID, texHandle );
			else if( it->second != texHandle )
				_setTexture( samplerID, texHandle );
		}


		void bindShader( const Shader & shader )
		{
			uint shaderId = shader.getId();

			if( shaderId == _cachedShader )
				return;

			glUseProgram( shaderId );

			_cachedShader = shaderId;
		}


		void bindFrameBuffer( const uint & frameBuffer )
		{
			glBindFramebuffer( GL_FRAMEBUFFER, frameBuffer );
		}


		void renderBoundGeometry( DrawMode mode )
		{
            if( _cachedVertexBuffer == NULL )
				return;

            _cachedVertexBuffer->render( mode );
		}


	private:

		void clearIvars()
		{
            _cachedVertexBuffer = NULL;
			_cachedShader		= 0;

			_cachedFrameBuffer	= 0;

			_cachedCullMode		= CULL_BACK;

			_renderWidth		= IwGLGetInt( IW_GL_WIDTH );
			_renderHeight		= IwGLGetInt( IW_GL_HEIGHT );

			_cachedTextures.clear();
		}

		void _setTexture( uint samplerId, uint texHandle )
		{
			glActiveTexture( GL_TEXTURE0 + samplerId );
			glBindTexture( GL_TEXTURE_2D, texHandle );

			_cachedTextures[ samplerId ] = texHandle;
		}

	};


//********************** Interface ******************************/

	RenderState::RenderState()
	{
		_impl = new RenderStateImpl();
	}

	RenderState::~RenderState()
	{
		delete( _impl );
	}


	void RenderState::clear( int mode )
	{
		_impl->clear( mode );
	}

	void RenderState::setClearColor( float r, float g, float b, float a ) const
	{
		_impl->setClearColor( r, g, b, a );
	}

	void RenderState::setClearColor( const Vector4 & clearColor ) const
	{
		_impl->setClearColor( clearColor.r, clearColor.g, clearColor.b, clearColor.a );
	}

	void RenderState::setDepthTesting( bool depthTest )
	{
		_impl->setDepthTesting( depthTest );
	}

	void RenderState::setDepthRange( float zNear, float zFar )
	{
		_impl->setDepthRange( zNear, zFar );
	}

	void RenderState::setCullMode( const CullMode cullMode )
	{
		_impl->setCullMode( cullMode );
	}

	void RenderState::setBlendmode( const BlendMode mode )
	{
		_impl->setBlendmode( mode );
	}

	void RenderState::setRenderSize( int width, int height )
	{
		_impl->setRenderSize( width, height );
	}

	void RenderState::setViewport( float x, float y, float width, float height )
	{
		_impl->setViewport( x, y, width, height );
	}

	void RenderState::bindVertexBuffer( IVertexBuffer & buffer )
	{
		_impl->bindVertexBuffer( buffer );
	}

	void RenderState::bindShader( const Shader & shader )
	{
		_impl->bindShader( shader );
	}

	void RenderState::bindTexture2d( uint samplerID, const Texture & texture )
	{
		_impl->bindTexture2d( samplerID, texture );
	}
	
	void RenderState::bindFrameBuffer( const uint & frameBuffer )
	{
		_impl->bindFrameBuffer( frameBuffer );
	}

	void RenderState::bindFrameBuffer( const FrameBufferObject & frameBuffer )
	{
		_impl->bindFrameBuffer( frameBuffer.getFrameBufferHandle() );
	}

	void RenderState::renderBoundGeometry( DrawMode mode )
	{
		_impl->renderBoundGeometry( mode );
	}


}
