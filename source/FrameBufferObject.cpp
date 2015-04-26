
#include "FrameBufferObject.h"

#include <IwGL.h>
#include <IwUtil.h>

namespace GG
{

	FrameBufferObject::FrameBufferObject()
	{
		_fboHandle			= 0;
		_colorTexHandle		= 0;
		_depthTexHandle		= 0;
		_depthRenderBuffer	= 0;
	}


	FrameBufferObject::~FrameBufferObject()
	{
		if( _fboHandle )
		{
			glDeleteFramebuffers( 1, &_fboHandle );
			_fboHandle = 0;
		}

		if( _depthRenderBuffer )
		{
			glDeleteRenderbuffers( 1, &_depthRenderBuffer );
			_depthRenderBuffer = 0;
		}
	}


	void FrameBufferObject::build(  int screenWidth, int screenHeight )
	{
		if( !_fboHandle )
			glGenFramebuffers( 1, &_fboHandle );

		if( !_colorTexHandle )
			glGenTextures( 1, &_colorTexHandle );

		if( !_depthRenderBuffer )
			glGenRenderbuffers( 1, &_depthRenderBuffer );

		if( !_depthTexHandle )
			glGenTextures( 1, &_depthTexHandle );


		glBindTexture( GL_TEXTURE_2D, _depthTexHandle );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );


		glBindTexture( GL_TEXTURE_2D, _colorTexHandle );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		
		glBindFramebuffer( GL_FRAMEBUFFER, _fboHandle );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,	GL_TEXTURE_2D, _colorTexHandle, 0 );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,	GL_TEXTURE_2D, _depthTexHandle, 0 );

		//glBindRenderbuffer( GL_RENDERBUFFER, _depthTexHandle );
		//glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, screenWidth, screenHeight );

		//glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthTexHandle );

		
		//glGenTextures(1, &_dTexHandle);
		//glBindTexture(GL_TEXTURE_2D, _dTexHandle);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, screenWidth, screenHeight, 
        //         0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		//
		//glFramebufferTexture2D( GL_FRAMEBUFFER, 
		//                  GL_DEPTH_ATTACHMENT, 
		//                  GL_TEXTURE_2D, 
		//                 _dTexHandle, 
		//                 0);




		glBindFramebuffer(	GL_FRAMEBUFFER,		0 );
		glBindRenderbuffer( GL_RENDERBUFFER,	0 );

		_colorTexture.setHandle( _colorTexHandle );
		_depthTexture.setHandle( _depthTexHandle );
	}


	Texture & FrameBufferObject::getColorTexture()
	{
		return _colorTexture;
	}

	Texture & FrameBufferObject::getDepthTexture()
	{
		return _depthTexture;
	}

	uint FrameBufferObject::getFrameBufferHandle() const
	{
		return _fboHandle;
	}
}
