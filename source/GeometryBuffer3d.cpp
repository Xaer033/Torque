/*
	Module: Geometry Buffer
	Description: Geometry that we send to the graphics card
	Author: Julian Williams
*/

#include "GeometryBuffer3d.h"

#include <vector>
#include <cmath>
#include <sstream>

#include <GLES2\gl2.h>
#include <GLES2\gl2ext.h>

#include "Vertex.h"
#include "Vector.h"
#include "DebugText.h"


#define BUFFER_OFFSET(i) ((void*)(i))


namespace GG
{


	typedef std::vector<Vertex> VertexList;
	typedef std::vector<GLuint> IndexList;




	class GeometryBuffer3dImpl
	{
	public:
		GeometryBuffer3dImpl();
		~GeometryBuffer3dImpl();
		
		void clearVertices();

		void addPosition( const Vector3 & p );
		void pushTexCoord( const Vector2 & t );
		void pushNormal( const Vector3 & n );
		void pushColor(	const Vector4 & t );
		void pushTangent( const Vector3 & t );
		void pushBiTangent( const Vector3 & b );

		
		void merge( const GeometryBuffer3dImpl & buffer );
		
		
		void build( );
		void build( Vertex * vertexList, uint vertCount, uint * indexList, uint indexCount );
		
		void _generateTangents();

		void _generateIndices();
		bool _isSameVertex(const Vertex & v1, const Vertex & v2) const;

		VertexList		_vertexList;
		IndexList		_indexList;

		GLuint indexBuffer;
		GLuint vertexBuffer;
		GLuint arrayHandle;

		int vertexProperties;

		Vector2 currentTexCoord;
		Vector3 currentNormal;
		Vector4 currentColor;
		Vector3 currentTangent;
		Vector3 currentBitangent;
		 
	};



	GeometryBuffer3d::GeometryBuffer3d(void)
	{
		_impl = new GeometryBuffer3dImpl();
	}


	GeometryBuffer3d::~GeometryBuffer3d(void)
	{
		delete _impl;
	}

	
	void GeometryBuffer3d::setVertexProperties( int vertProps)
	{
		_impl->vertexProperties = vertProps;
	}

	int GeometryBuffer3d::getVertexProperties() const
	{
		return _impl->vertexProperties;
	}
		

	void GeometryBuffer3d::clearVertices()
	{
		_impl->clearVertices();
	}

	void GeometryBuffer3d::addPosition( const Vector3 & p )
	{
		_impl->addPosition( p );
	}

	void GeometryBuffer3d::pushTexCoord( const Vector2 & t )
	{
		_impl->pushTexCoord( t );
	}

	void GeometryBuffer3d::pushNormal( const Vector3 & n )
	{
		_impl->pushNormal( n );
	}

	void GeometryBuffer3d::pushColor(	const Vector4 & c )
	{
		_impl->pushColor( c );
	}
	void GeometryBuffer3d::pushTangent( const Vector3 & t )
	{
		_impl->pushTangent( t );
	}




	void GeometryBuffer3d::merge( const GeometryBuffer3d & rhb )
	{
		_impl->merge( *rhb._impl );
	}

				    

	uint GeometryBuffer3d::getVertexHandle() const
	{
		return _impl->vertexBuffer;
	}

	uint GeometryBuffer3d::getIndexHandle() const 
	{
		return _impl->indexBuffer;
	}


	void GeometryBuffer3d::build()
	{
		_impl->build();
	}

	void GeometryBuffer3d::build( Vertex * vertexList, uint vertCount, uint * indexList, uint indexCount )
	{
		_impl->build( vertexList, vertCount, indexList, indexCount );
	}

	uint * GeometryBuffer3d::getIndexArray() const
	{
		return &_impl->_indexList[0];
	}

	Vertex * GeometryBuffer3d::getVertexArray() const
	{
		return &_impl->_vertexList[0];
	}

	uint GeometryBuffer3d::getIndexCount() const
	{
		return _impl->_indexList.size();
	}

	uint GeometryBuffer3d::getVertexCount() const
	{
		return _impl->_vertexList.size();
	}
	/********************************************
			Geometry Buffer Implementation
	*********************************************/

	GeometryBuffer3dImpl::GeometryBuffer3dImpl()
	{
		_vertexList.clear();
		_indexList.clear();

		vertexProperties = POSITIONS;

		//currentTexCoord = Vector2;
		//currentNormal	= Vector3;
		//currentColor	= Vector4;
		//currentTangent	= Vector3;
		
		vertexBuffer	= 0;
		indexBuffer		= 0;
	}

	GeometryBuffer3dImpl::~GeometryBuffer3dImpl()
	{
		if( vertexBuffer != 0 )
			glDeleteBuffers( 1, &vertexBuffer );

		if( indexBuffer != 0 )
			glDeleteBuffers( 1, &indexBuffer );
	}


	void GeometryBuffer3dImpl::merge( const GeometryBuffer3dImpl & buffer )
	{
		_vertexList.insert	( _vertexList.end(),	buffer._vertexList.begin(),	buffer._vertexList.end()	);
		_indexList.insert	( _indexList.end(),		buffer._indexList.begin(),	buffer._indexList.end()		);
	}


	void GeometryBuffer3dImpl::clearVertices()
	{
		_vertexList.clear();
		_indexList.clear();
	}

	void GeometryBuffer3dImpl::addPosition( const Vector3 & p )
	{
		Vertex v = 
		{ 
			p, 
			currentTexCoord, 
			currentNormal,  
			currentTangent, 
			currentBitangent,
			currentColor
		};
	
		_vertexList.push_back( v );

	}

	void GeometryBuffer3dImpl::pushTexCoord( const Vector2 & t )
	{
		currentTexCoord = t;
	}

	void GeometryBuffer3dImpl::pushNormal( const Vector3 & n )
	{
		currentNormal = n;
	}

	void GeometryBuffer3dImpl::pushColor(	const Vector4 & c )
	{
		currentColor = c;
	}

	void GeometryBuffer3dImpl::pushTangent( const Vector3 & t )
	{
		currentTangent = t;
	}

	void GeometryBuffer3dImpl::pushBiTangent( const Vector3 & b )
	{
		currentBitangent = b;
	}


	void GeometryBuffer3dImpl::_generateTangents( )
	{
		Vertex v[3];
		DebugText text("boom");

		for( uint i = 0; i < _vertexList.size(); i += 3 )
		{
			if( i >= _vertexList.size() - 3 )
				break;
	
		
			v[0] = _vertexList[i];
			v[1] = _vertexList[i+1];
			v[2] = _vertexList[i+2];

			Vector3 tangent, biTangent;

	//Make Tangent/bitangent building
			Vector3 edge1	= v[1].position - v[0].position;
			Vector3 edge2	= v[2].position - v[0].position;


			Vector2 edge1uv = v[1].texCoord - v[0].texCoord;
			Vector2 edge2uv = v[2].texCoord - v[0].texCoord;

			//Determ
			float det = edge1uv.x * edge2uv.y - edge1uv.y * edge2uv.x;

			if( det != 0.0f) 
			{
				float mul = 1.0f / det;
				tangent	  = (edge1 * -edge2uv.y + edge2 * edge1uv.y) * mul;
				biTangent = (edge1 * -edge2uv.x + edge2 * edge1uv.x) * mul;

				/*tangent = Vector3((edge2uv.y * edge1.x - edge1uv.y * edge2.x) * mul,
								  (edge2uv.y * edge1.y - edge1uv.y * edge2.y) * mul,
								  (edge2uv.y * edge1.z - edge1uv.y * edge2.z) * mul);*/
			}


			
			if( abs( tangent.x ) < 0.01f && abs( tangent.y ) < 0.01f && abs( tangent.z ) )
			{
				text << "Zero tangent!";
				text.print();
			}

			v[0].tangent	= v[1].tangent	 = v[2].tangent		=  tangent;
			v[0].biTangent	= v[1].biTangent = v[2].biTangent	=  biTangent;

	
			for( uint j = 0; j < 3; ++j )
			{
				_vertexList[i + j].tangent		= v[j].tangent;
				_vertexList[i + j].biTangent	= v[j].biTangent;
			}
		}
	}

	void GeometryBuffer3dImpl::_generateIndices()
	{
		_indexList.clear();

		std::vector< Vertex > newVertList;
		DebugText debugText;

		for( uint i = 0; i < _vertexList.size(); ++i )
		{
			bool repeated = false;

			///FLip uv's for openGL
			_vertexList[i].texCoord.y = 1 - _vertexList[i].texCoord.y;

			for( uint j = 0; j < newVertList.size(); ++j )
			{
			
				repeated = _isSameVertex( _vertexList[i], newVertList[j] );

				if( repeated ) 
				{ 
					_indexList.push_back( j );
					break; 
				}
			}


			if( !repeated ) 
			{
				newVertList.push_back( _vertexList[i] );
				_indexList.push_back( newVertList.size() - 1 );
			}
		}

		_vertexList = newVertList;
	}


	

	void GeometryBuffer3dImpl::build()
	{

		_generateIndices();
		
		if( vertexProperties & TANGENTS )
			_generateTangents();

		glGenBuffers( 1, &vertexBuffer );
		glGenBuffers( 1, &indexBuffer );
		

		glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData( GL_ARRAY_BUFFER, sizeof( Vertex ) * _vertexList.size(), &_vertexList[0], GL_STATIC_DRAW );


		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( GLuint ) * _indexList.size(), &_indexList[0], GL_STATIC_DRAW );

	}

	void GeometryBuffer3dImpl::build( Vertex * vertexList, uint vertCount, uint * indexList, uint indexCount )
	{

		_vertexList.clear();

		for( uint i = 0; i < vertCount; ++i )
			_vertexList.push_back( vertexList[ i ] );


		if( indexList == NULL || indexCount == 0 )
			_generateIndices();
		else
		{
			_indexList.clear();
			for( uint i = 0; i < indexCount; ++i )
				_indexList.push_back( indexList[ i ] );
		}

		
		if( vertexProperties & TANGENTS )
			_generateTangents();


		glGenBuffers( 1, &vertexBuffer );
		glGenBuffers( 1, &indexBuffer );
		

		glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData( GL_ARRAY_BUFFER, sizeof( Vertex ) * _vertexList.size(), &_vertexList[0], GL_STATIC_DRAW );


		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( GLuint ) * _indexList.size(), &_indexList[0], GL_STATIC_DRAW );

	}


	///check 2 verticies to see if they're the same
	bool GeometryBuffer3dImpl::_isSameVertex(const Vertex & v1, const Vertex & v2) const
	{
		const float limit = 0.001f;

		Vector3 deltaPos	= v1.position - v2.position;
		bool posResult		= (	fabs( deltaPos.x ) < limit && 
								fabs( deltaPos.y ) < limit &&
								fabs( deltaPos.z ) < limit	);


		bool texResult = true;
		if( vertexProperties & GG::TEXCOORDS )
		{
			Vector2 deltaTex = v1.texCoord - v2.texCoord;
			texResult = (fabs(deltaTex.x) < limit &&
						 fabs(deltaTex.y) < limit);
		}

		bool normalResult = true;
		if( vertexProperties & GG::NORMALS )
		{
			Vector3 deltaNormal = v1.normal - v2.normal;
			normalResult = (fabs(deltaNormal.x) < limit &&
							fabs(deltaNormal.y) < limit &&
							fabs(deltaNormal.z) < limit);
		}
		
		bool colorResult = true;
		if( vertexProperties & GG::COLORS )
		{
			Vector4 deltaColor = v1.color - v2.color;
			colorResult =  (fabs(deltaColor.x) < limit &&
							fabs(deltaColor.y) < limit &&
							fabs(deltaColor.z) < limit &&
							fabs(deltaColor.w) < limit);
		}

		bool tangentResult = true;
		if( vertexProperties & GG::TANGENTS )
		{
			Vector3 deltaTangent = v1.tangent - v2.tangent;
			tangentResult = (fabs(deltaTangent.x) < limit &&
							 fabs(deltaTangent.y) < limit &&
							 fabs(deltaTangent.z) < limit);
		}

		return posResult && texResult && normalResult && colorResult && tangentResult;
	}

}