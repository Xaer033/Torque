
/*
	Module: Textures
	Description: Wall paper for your world!
	Author: Julian Williams
*/

#ifndef _TEXTURES_H_
#define _TEXTURES_H_

#include "IwTypes.h"

class CIwImage;


namespace GG
{

	class Texture
	{
	public:

		Texture( );
		Texture( CIwImage & image );
		~Texture( );

		void setImage( CIwImage & image );
		bool loadFromFile( const char * location );

        uint getWidth()  const;
        uint getHeight() const;

		void setHandle( const uint handle );
		uint getHandle() const;

		

	private:
		bool _createVRAMTexture();

	private:
		unsigned int _handle;

		CIwImage * _image;
	};



}


#endif

