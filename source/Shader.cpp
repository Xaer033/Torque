
#include "Shader.h"


#include <map>
#include <sstream>
#include <vector>
#include <string>

#include "glm/gtc/type_ptr.hpp"

#include "s3e.h"
#include "IwGL.h"

#include "Matrix.h"

#include "Vertex.h"
#include "Texture.h"
#include "Transform.h"

#include "DebugText.h"


namespace GG
{

//******************************** Implementation ************************************
//************************************************************************************
	typedef std::map< std::string, int >	ParameterMap;
	typedef std::vector< std::string >		AttributeVector;
	typedef std::vector< const char * >		DefineList;


	class ShaderImpl
	{
	public:
		ParameterMap		parameterHandles;  
		AttributeVector		attributeHandles;

		DefineList			defineList;


		uint vertexHandle;
		uint pixelHandle;
		uint programHandle;


		ShaderImpl()
		{
			vertexHandle	= 0;
			pixelHandle		= 0;

			programHandle	= 0;

			for( int i = 0; i < VertexTagMax; ++i )
				attributeHandles.push_back();

		}

		~ShaderImpl()
		{
			if( vertexHandle )
				glDeleteShader( vertexHandle );

			if( pixelHandle )
				glDeleteShader( pixelHandle );

			if( programHandle )
				glDeleteProgram( programHandle );

		}


		uint getId() const
		{
			return programHandle;
		}

		void bindAttribute( int tag, const char * name )
		{
			if( (uint)tag < attributeHandles.size() )
				attributeHandles[ tag ] = name;
		}

		void bind( )
		{
			glUseProgram( programHandle );
		}


	// Create a shader object, load the shader source, and
// compile the shader.
		void validateProgram( ) 
		{
			DebugText debugText;

			const uint BUFFER_SIZE = 1024;
			char buffer[ BUFFER_SIZE ];
			memset( buffer, 0, BUFFER_SIZE );

			GLsizei length = 0;
			memset( buffer, 0, BUFFER_SIZE );

			glGetProgramInfoLog( programHandle, BUFFER_SIZE, &length, buffer);

			if (length > 0)
			{
				debugText << "Program " << programHandle << " linker: " << buffer;
				debugText.print();
			}

			glValidateProgram( programHandle );

			GLint status = 0;

			glGetProgramiv( programHandle, GL_VALIDATE_STATUS, &status );
			if (status == GL_FALSE)
			{
				debugText << "Error validating shader : returned false" << programHandle;
				debugText.print();
			}
		}


		bool loadShader( GLuint * shader, const char * shaderSrc, GLenum type )
		{
			GLint compiled;
			// Create the shader object
            if( *shader == 0 )
			    *shader = glCreateShader( type );

			// Load the shader source
			std::vector< const char * > shaderStrings;
			shaderStrings.insert( shaderStrings.end(), defineList.begin(), defineList.end() );
			shaderStrings.push_back( shaderSrc );

			glShaderSource( *shader, shaderStrings.size(), &shaderStrings[0], NULL );
			

			// Compile the shader
			glCompileShader( *shader );
			// Check the compile status
			glGetShaderiv( *shader, GL_COMPILE_STATUS, &compiled );

			if( !compiled )
			{
				GLint infoLen = 0;
				glGetShaderiv( *shader, GL_INFO_LOG_LENGTH, &infoLen );

				if( infoLen > 1 )
				{
					char * infoLog = (char*)malloc( sizeof(char) * infoLen );
					glGetShaderInfoLog(*shader, infoLen, NULL, infoLog);
					
					std::stringstream stream;
					stream << "Error Compiling shader: " << infoLog;
					s3eDebugOutputString( stream.str().c_str() );

					free( infoLog );
				}

				glDeleteShader( *shader );
				return false;
			}
            return true;
		}


		void bindAllAttributes()
		{
			for( uint i = 0; i < attributeHandles.size(); ++i )
				glBindAttribLocation( programHandle, i, attributeHandles[i].c_str() );
		}


		void addDefinition( const char * define )
		{
			//std::stringstream stream;
			//stream << "#define " << define << "\0";
			
			defineList.push_back( define );
			
		}


		bool compile( const char * vertexShader, const char * pixelShader )
		{
			int linked	= 0;

			loadShader( &vertexHandle,  vertexShader,   GL_VERTEX_SHADER    );
			loadShader( &pixelHandle,   pixelShader,    GL_FRAGMENT_SHADER  );
			
			if( programHandle == 0 )
            { 
				programHandle	= glCreateProgram();
            }

			glAttachShader( programHandle, vertexHandle );
			glAttachShader( programHandle, pixelHandle );

			bindAllAttributes();

			// Link the program
			glLinkProgram( programHandle );
			// Check the link status
			glGetProgramiv( programHandle, GL_LINK_STATUS, &linked );

			if( !linked )
			{
				GLint infoLen = 0;
				glGetProgramiv( programHandle, GL_INFO_LOG_LENGTH, &infoLen );

				if( infoLen > 1 )
				{
					char* infoLog = (char*)malloc(sizeof(char) * infoLen);
					glGetProgramInfoLog( programHandle, infoLen, NULL, infoLog );

					std::stringstream stream;
					stream << "Error Compiling shader: " << infoLog;
					s3eDebugOutputString( stream.str().c_str() );
					
					free( infoLog );
				}
			
				glDeleteProgram( programHandle );
				return false;
			}

			
			validateProgram();

			return true;
		}

		void update( )
		{

		}

		
		int getParameterID( const char * name )
		{
			ParameterMap::iterator it = parameterHandles.find(  name  );
			if( it == parameterHandles.end() )
			{
				int ID = glGetUniformLocation( programHandle, name );
				parameterHandles[ name ] = ID;

				return ID;
			}

			return it->second;
		}

		void setParameter( const char * name, int x )
		{
			glUniform1i( getParameterID( name ), x );
		}

		void setParameter( const char * name, float x )
		{
			glUniform1f( getParameterID( name ), x );
		}

		void setParameter( const char * name, float x, float y )
		{
			glUniform2f( getParameterID( name ), x, y );
		}

		void setParameter( const char * name, float x, float y, float z )
		{
			glUniform3f( getParameterID( name ), x, y, z );
		}

		void setParameter( const char * name, float x, float y, float z, float w )
		{
			int id = getParameterID( name );
			glUniform4f( id, x, y, z, w );
		}


		void setParameter( const char * name, Vector2 & v )
		{
			glUniform2f( getParameterID( name ), v.x, v.y );
		}

		void setParameter( const char * name, Vector3 & v )
		{
			glUniform3f( getParameterID( name ), v.x, v.y, v.z );
		}

		void setParameter( const char * name, Vector4 & v )
		{
			glUniform4f( getParameterID( name ), v.x, v.y, v.z, v.w );
		}

		void setParameterSampler( const char * name, uint samplerID )
		{
		// Set the sampler texture unit to 0
			glUniform1i( getParameterID( name ) , samplerID );
		}


		void setParameter( const char * name, glm::mat4 m )
		{
			glUniformMatrix4fv( getParameterID( name ), 1, GL_FALSE, (float*)glm::value_ptr( m ) );
		}

		void setParameter( const char * name, Transform & t )
		{
			setParameter( name, t.getMatrix() );
		}
	};

	

//************************ Interface ********************************
//*******************************************************************

	Shader::Shader()
	{
		_impl = new ShaderImpl( );
	}

	Shader::~Shader()
	{
		delete( _impl );
	}


	uint Shader::getId() const
	{
		return _impl->getId();
	}

	void Shader::bindAttribute( int tag, const char * name )
	{
		_impl->bindAttribute( tag, name );
	}

	void Shader::bind( )
	{
		_impl->bind( );
	}

	void Shader::addDefinition( const char * define )
	{
		_impl->addDefinition( define );
	}

	bool Shader::compile( const char * vertexShader, const char * pixelShader )
	{
		return _impl->compile( vertexShader, pixelShader );
	}

	void Shader::update( )
	{
		_impl->update( );
	}


	void Shader::setParameter( const char * name, int x )
	{
		_impl->setParameter( name, (int)x );
	}

	void Shader::setParameter( const char * name, float x )
	{
		_impl->setParameter( name, x );
	}

	void Shader::setParameter( const char * name, float x, float y )
	{
		_impl->setParameter( name, x, y );
	}

	void Shader::setParameter( const char * name, float x, float y, float z )
	{
		_impl->setParameter( name, x, y, z );
	}

	void Shader::setParameter( const char * name, float x, float y, float z, float w )
	{
		_impl->setParameter( name, x, y, z, w );
	}

	void Shader::setParameter( const char * name, Vector2 & v)
	{
		_impl->setParameter( name, v );
	}

	void Shader::setParameter( const char * name, Vector3 & v)
	{
		_impl->setParameter( name, v );
	}

	void Shader::setParameter( const char * name, Vector4 & v)
	{
		_impl->setParameter( name, v );
	}

	void Shader::setParameterSampler( const char * name, uint samplerID )
	{
		_impl->setParameterSampler( name, samplerID );
	}

	void Shader::setParameter( const char * name, glm::mat4 m )
	{
		_impl->setParameter( name, m );
	}

	void Shader::setParameter( const char * name, Transform & t )
	{
		_impl->setParameter( name, t.getMatrix() );
	}
}