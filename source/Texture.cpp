
#include "Texture.h"

#include "IwGL.h"
#include "IwImage.h"

namespace GG
{

	Texture::Texture()
	{
		_image	= NULL;
		_handle = 0;
	}

	Texture::Texture( CIwImage & image )
	{
		_handle	= 0;
		setImage( image );
	}

	Texture::~Texture()
	{
		//we dont own image, so dont free
		_image = NULL;

		if( _handle )
			glDeleteTextures( 1, &_handle );
	}


	void Texture::setImage( CIwImage & image )
	{
		_image = &image;
		
		_createVRAMTexture();
	}

	bool Texture::loadFromFile( const char * location )
	{
		if( _image )
			delete( _image );

		_image = new CIwImage();
		_image->LoadFromFile( location );
		
		_createVRAMTexture();
		return true;
	}


	void Texture::setHandle( const uint handle )
	{
		//if( _handle )
		//	glDeleteTextures( 1, &_handle );

		_handle = handle;
	}


	uint Texture::getHandle() const
	{
		return _handle;
	}


	bool Texture::_createVRAMTexture()
	{
		if( _image == NULL )
			return false;

		
		int interalFormat = GL_RGB;

		if( _image->HasAlpha() == 1 )
			interalFormat = GL_RGBA;

		if( !_handle )
			glGenTextures( 1, &_handle );

		glBindTexture( GL_TEXTURE_2D, _handle );

	//gluBuild2DMipmaps( GL_TEXTURE_2D, 4, texture.getSize().x, texture.getSize().y, GL_RGBA, GL_UNSIGNED_BYTE, texture.getPixelsPtr() );
		glTexImage2D( GL_TEXTURE_2D, 0, interalFormat, _image->GetWidth(), _image->GetHeight(), 0, interalFormat, GL_UNSIGNED_BYTE, _image->GetTexels() );
		glGenerateMipmap( GL_TEXTURE_2D );

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		return true;
	}


	







}

