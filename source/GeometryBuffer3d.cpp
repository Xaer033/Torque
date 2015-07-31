/*
	Module: Geometry Buffer
	Description: Geometry that we send to the graphics card
	Author: Julian Williams
*/

#include "GeometryBuffer3d.h"

#include <IVertexBuffer.h>

#include <vector>
#include <cmath>
#include <sstream>

#include <IwGL.h>

#include "Vertex.h"
#include "Vector.h"
#include "DebugText.h"


#define BUFFER_OFFSET(i) ((void*)(i))


namespace GG
{
    GeometryBuffer3d::GeometryBuffer3d() : 
        IVertexBuffer(),        _vertexBufferHandle(0), 
        _indexBufferHandle(0),  _arrayBufferHandle(0 )
	{
		_vertexList.clear();
		_indexList.clear();
	}

    GeometryBuffer3d::~GeometryBuffer3d()
	{
		if( _vertexBufferHandle != 0 )
            glDeleteBuffers( 1, &_vertexBufferHandle );

        if( _indexBufferHandle != 0 )
            glDeleteBuffers( 1, &_indexBufferHandle );
	}


    void GeometryBuffer3d::merge( const GeometryBuffer3d & buffer )
	{
		_vertexList.insert	( _vertexList.end(),	buffer._vertexList.begin(),	buffer._vertexList.end()	);
		_indexList.insert	( _indexList.end(),		buffer._indexList.begin(),	buffer._indexList.end()		);
	}


    void GeometryBuffer3d::clearVertices()
	{
		_vertexList.clear();
		_indexList.clear();
	}

    void GeometryBuffer3d::addPosition( const Vector3 & p )
	{
		Vertex v = 
		{ 
			p, 
			_currentTexCoord, 
			_currentNormal,  
			_currentTangent, 
			_currentBitangent,
			_currentColor
		};
	
		_vertexList.push_back( v );

	}

    void GeometryBuffer3d::pushTexCoord( const Vector2 & t )
	{
		_currentTexCoord = t;
	}

    void GeometryBuffer3d::pushNormal( const Vector3 & n )
	{
		_currentNormal = n;
	}

    void GeometryBuffer3d::pushColor( const Vector4 & c )
	{
		_currentColor = c;
	}

    void GeometryBuffer3d::pushTangent( const Vector3 & t )
	{
		_currentTangent = t;
	}

    void GeometryBuffer3d::pushBitangent( const Vector3 & b )
	{
		_currentBitangent = b;
	}


    void GeometryBuffer3d::_generateTangents()
	{
		Vertex v[3];

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


			
			if( fabs( tangent.x ) < 0.01f && fabs( tangent.y ) < 0.01f && fabs( tangent.z ) )
			{
				//text << "Zero tangent!";
				//text.print();
			}

			//v[0].tangent	= v[1].tangent	 = v[2].tangent		=  tangent;
			//v[0].biTangent	= v[1].biTangent = v[2].biTangent	=  biTangent;

	
			for( uint j = 0; j < 3; ++j )
			{
				_vertexList[i + j].tangent		= tangent;
				_vertexList[i + j].biTangent	= biTangent;
			}
		}
	}

    


	

    void GeometryBuffer3d::build( DrawHint drawHint )
	{

		_generateIndices();
		
		if( getVertexProperties() & TANGENTS )
			_generateTangents();

        if( _vertexBufferHandle == 0 )
            glGenBuffers( 1, &_vertexBufferHandle );

        if( _indexBufferHandle == 0 )
            glGenBuffers( 1, &_indexBufferHandle );
		
        int vCount = getVertexCount();
        int iCount = getIndexCount();

        glBindBuffer( GL_ARRAY_BUFFER,          _vertexBufferHandle );
        glBufferData( GL_ARRAY_BUFFER,          sizeof( Vertex ) * _vertexList.size(), &_vertexList[ 0 ], drawHint );
        //glBufferSubData( GL_ARRAY_BUFFER, 0,    sizeof( Vertex ) * _vertexList.size(), &_vertexList[ 0 ] );


        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,          _indexBufferHandle );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER,          sizeof( GLuint ) * _indexList.size(), &_indexList[ 0 ] , drawHint );
       // glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 0,    sizeof( GLuint ) * _indexList.size(), &_indexList[ 0 ] );
	}

    void GeometryBuffer3d::build( Vertex    * vertexList,   uint vertCount,
                                  uint      * indexList,    uint indexCount,
                                  DrawHint drawHint )
	{

		_vertexList.clear();

		for( uint i = 0; i < vertCount; ++i )
			_vertexList.push_back( vertexList[ i ] );


        if( indexList == NULL || indexCount == 0 )
        {
            _generateIndices();
        }
		else
		{
			_indexList.clear();

			for( uint i = 0; i < indexCount; ++i )
				_indexList.push_back( indexList[ i ] );
		}

		
		if( getVertexProperties() & TANGENTS )
			_generateTangents();

        if( _vertexBufferHandle == 0 )
            glGenBuffers( 1, &_vertexBufferHandle );

        if( _indexBufferHandle == 0 )
            glGenBuffers( 1, &_indexBufferHandle );
		

        glBindBuffer( GL_ARRAY_BUFFER,          _vertexBufferHandle );
        glBufferData( GL_ARRAY_BUFFER,          sizeof( Vertex ) * _vertexList.size(), NULL, drawHint   );
		glBufferSubData( GL_ARRAY_BUFFER, 0,    sizeof( Vertex ) * _vertexList.size(), &_vertexList[0]  );


        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _indexBufferHandle );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER,          sizeof( GLuint ) * _indexList.size(), NULL, drawHint    );
        glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 0,    sizeof( GLuint ) * _indexList.size(), &_indexList[ 0 ]  );
	}

    void GeometryBuffer3d::bind() const
    {

        glBindBuffer( GL_ARRAY_BUFFER,          _vertexBufferHandle );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,  _indexBufferHandle  );


        glEnableVertexAttribArray( PositionTag );
        glVertexAttribPointer( PositionTag, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), BUFFER_OFFSET( 0 ) );

        int vertexProperties = getVertexProperties();

        if( vertexProperties & GG::TEXCOORDS )
        {
            glEnableVertexAttribArray( TexcoordTag );
            glVertexAttribPointer( TexcoordTag, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), BUFFER_OFFSET( 3 * sizeof( float ) ) );
        }
        else
        {
            glDisableVertexAttribArray( TexcoordTag );
        }

        if( vertexProperties & GG::NORMALS )
        {
            glEnableVertexAttribArray( NormalTag );
            glVertexAttribPointer( NormalTag, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), BUFFER_OFFSET( 5 * sizeof( float ) ) );
        }
        else
        {
            glDisableVertexAttribArray( NormalTag );
        }

        if( vertexProperties & GG::TANGENTS )
        {
            glEnableVertexAttribArray( TangentTag );
            glVertexAttribPointer( TangentTag, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), BUFFER_OFFSET( 8 * sizeof( float ) ) );
        }
        else
        {
            glDisableVertexAttribArray( TangentTag );
        }

        if( vertexProperties & GG::BITANGENTS )
        {
            glEnableVertexAttribArray( BitangentTag );
            glVertexAttribPointer( BitangentTag, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), BUFFER_OFFSET( 11 * sizeof( float ) ) );
        }
        else
        {
            glDisableVertexAttribArray( BitangentTag );
        }

        if( vertexProperties & GG::COLORS )
        {
            glEnableVertexAttribArray( ColorTag );
            glVertexAttribPointer( ColorTag, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ), BUFFER_OFFSET( 14 * sizeof( float ) ) );
        }
        else
        {
            glDisableVertexAttribArray( ColorTag );
        }
    }

    void GeometryBuffer3d::render( const DrawMode & drawMode ) const
    {
        glDrawElements( ( GLenum )drawMode, _indexList.size(), GL_UNSIGNED_INT, NULL );
    }


    Vertex *   GeometryBuffer3d::getVertexArray()
    {
        return &_vertexList[ 0 ];
    }

    uint *    GeometryBuffer3d::getIndexArray()
    {
        return &_indexList[ 0 ];
    }

    uint  GeometryBuffer3d::getVertexCount() const
    {
        return _vertexList.size();
    }

    uint  GeometryBuffer3d::getIndexCount() const
    {
        return _indexList.size();
    }
//---------------------------- Private Implementation -------------------------------------

	///check 2 verticies to see if they're the same
    bool GeometryBuffer3d::_isSameVertex( const Vertex & v1, const Vertex & v2 ) const
	{
		const float limit = 0.001f;

		Vector3 deltaPos	= v1.position - v2.position;
		bool posResult		= (	fabs( deltaPos.x ) < limit && 
								fabs( deltaPos.y ) < limit &&
								fabs( deltaPos.z ) < limit	);

        int vertexProperties = getVertexProperties();

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

    void GeometryBuffer3d::_generateIndices()
    {
        _indexList.clear();

        VertexList  newVertList;
        //DebugText   debugText;

        for( uint i = 0; i < _vertexList.size(); ++i )
        {
            bool repeated = false;

            ///FLip uv's for openGL
           // _vertexList[ i ].texCoord.y = 1 - _vertexList[ i ].texCoord.y;

            for( uint j = 0; j < newVertList.size(); ++j )
            {

                repeated = _isSameVertex( _vertexList[ i ], newVertList[ j ] );

                if( repeated )
                {
                    _indexList.push_back( j );
                    break;
                }
            }


            if( !repeated )
            {
                newVertList.push_back( _vertexList[ i ] );
                _indexList.push_back( newVertList.size() - 1 );
            }
        }

        _vertexList = newVertList;
    }
}