
#include "RenderState.h"

#include <vector>
#include <hash_map>


#include "IwGL.h"
#include "IwGLExt.h"
#include "Shader.h"
#include "GeometryBuffer3d.h"
#include "Texture.h"
#include "FrameBufferObject.h"


#define BUFFER_OFFSET(i) ((void*)(i))


namespace GG
{

	typedef std::hash_map< uint, uint > TextureCache;


	class RenderStateImpl
	{
	public:

		GeometryBuffer3d *	_cachedGeoBuffer3d;
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

		void setClearColor( float r, float g, float b, float a )
		{
			glClearColor( r, g, b, a );
		}

		void setClearColor( Vector4 clearColor )
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
				glCullFace( cullMode );
			}

			_cachedCullMode = cullMode;

		}

		void setBlendmode( BlendMode mode )
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
			case BM_ALPHA:
				glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
				break;

			case BM_ADDITIVE:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				break;

			case BM_MULTIPLY:
				glBlendFunc( GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA );
				break;

			case BM_SCREEN:
				glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_COLOR );
				break;
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

			glViewport( xVec.x, yVec.x, xVec.y, yVec.y );
		}


		void bindGeometry3d( GeometryBuffer3d & buffer )
		{
			if( &buffer == _cachedGeoBuffer3d )
				return;

			glBindBuffer( GL_ARRAY_BUFFER, buffer.getVertexHandle() );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffer.getIndexHandle() );


			glEnableVertexAttribArray( PositionTag );
			glVertexAttribPointer( PositionTag, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), BUFFER_OFFSET( 0 ) );
			
			if( buffer.getVertexProperties() & GG::TEXCOORDS )
			{
				glEnableVertexAttribArray( TexcoordTag );
				glVertexAttribPointer( TexcoordTag, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), BUFFER_OFFSET( 3 * sizeof( float ) ) );
			}
			else
			{
				glDisableVertexAttribArray( TexcoordTag );
			}
			
			if( buffer.getVertexProperties() & GG::NORMALS )
			{
				glEnableVertexAttribArray( NormalTag );
				glVertexAttribPointer( NormalTag, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), BUFFER_OFFSET( 5 * sizeof( float ) ) );
			}
			else
			{
				glDisableVertexAttribArray( NormalTag );
			}
			
			if( buffer.getVertexProperties() & GG::TANGENTS )
			{
				glEnableVertexAttribArray( TangentTag );
				glVertexAttribPointer( TangentTag, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), BUFFER_OFFSET( 8 * sizeof( float ) ) );
			}
			else
			{
				glDisableVertexAttribArray( TangentTag );
			}

			if( buffer.getVertexProperties() & GG::BITANGENTS )
			{
				glEnableVertexAttribArray( BitangentTag );
				glVertexAttribPointer( BitangentTag, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), BUFFER_OFFSET( 11 * sizeof( float ) ) );
			}
			else
			{
				glDisableVertexAttribArray( BitangentTag );
			}

			if( buffer.getVertexProperties() & GG::COLORS )
			{
				glEnableVertexAttribArray( ColorTag );
				glVertexAttribPointer( ColorTag, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ), BUFFER_OFFSET( 14 * sizeof( float ) ) );
			}
			else
			{
				glDisableVertexAttribArray( ColorTag );
			}

			_cachedGeoBuffer3d = &buffer;
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
			if( _cachedGeoBuffer3d == NULL )
				return;


			int glMode = 0;

			switch( mode )
			{
			case TRIANGLE_LIST:
				glMode = GL_TRIANGLES;
				break;
			case TRIANGLE_STRIP:
				glMode = GL_TRIANGLE_STRIP;
				break;
			case LINES:
				glMode = GL_LINES;
				break;
			case LINE_STRIP:
				glMode = GL_LINE_STRIP;
				break;
			case LINE_LOOP:
				glMode = GL_LINE_LOOP;
				break;
			}

			glDrawElements( glMode, _cachedGeoBuffer3d->getIndexCount(), GL_UNSIGNED_INT, NULL );
		}


	private:

		void clearIvars()
		{
			_cachedGeoBuffer3d	= NULL;
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

	void RenderState::setClearColor( float r, float g, float b, float a )
	{
		_impl->setClearColor( r, g, b, a );
	}

	void RenderState::setClearColor( Vector4 clearColor )
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

	void RenderState::setBlendmode( BlendMode mode )
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

	void RenderState::bindGeometry3d( GeometryBuffer3d & buffer )
	{
		_impl->bindGeometry3d( buffer );
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
