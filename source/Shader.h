
/*
	Module: Shaders
	Description:	Magic pixie pixel prettyfiers
	Author: Julian Williams
*/

#ifndef _SHADER_H_
#define _SHADER_H_


#include "Vector.h"

#include "IwTypes.h"
#include "glm/glm.hpp"
#include <GLES2\gl2.h>

namespace GG
{

	class ShaderImpl;
	class Transform;


	class Shader
	{
	public:

		Shader( );
		~Shader( );

		void bindAttribute( int tag, const char * name );

		uint getId() const;


		void bind( );

		void addDefinition(  const char * define );

		bool compile( const char * vertexShader, const char * pixelShader );

		void update( );

		
		void setParameter( const char * name, int x );
		void setParameter( const char * name, float x );
		void setParameter( const char * name, float x, float y );
		void setParameter( const char * name, float x, float y, float z );
		void setParameter( const char * name, float x, float y, float z, float w );
		void setParameter( const char * name, Vector2 & v );
		void setParameter( const char * name, Vector3 & v );
		void setParameter( const char * name, Vector4 & v );
		void setParameter( const char * name, glm::mat4 m );
		void setParameter( const char * name, Transform & t );
		void setParameterSampler( const char * name, uint samplerID );


	private:

		ShaderImpl * _impl;

	};
}

#endif
