/*
	Module: Frame Buffer Object
	Description: Frame Buffer for offscreen rendering and post processing
	Author: Julian Williams
*/
#ifndef _FRAME_BUFFER_OBJECT_H_
#define _FRAME_BUFFER_OBJECT_H_

#include <IwTypes.h>

#include "Texture.h"


namespace GG
{


	class FrameBufferObject
	{
	public:
		FrameBufferObject();
		~FrameBufferObject();

		void build( int screenWidth, int screenHeight );

		uint getFrameBufferHandle() const;
		Texture & getColorTexture();
		Texture & getDepthTexture();

	private:

		uint _fboHandle;
		uint _colorTexHandle;
		uint _depthRenderBuffer;
		uint _depthTexHandle;

		Texture _depthTexture;
		Texture _colorTexture;

	};

}

#endif

