/*
	Module: Geometry Buffer
	Description: Geometry that we send to the graphics card
	Author: Julian Williams
*/


#ifndef __GEOMETRY_BUFFER_3D_H__
#define __GEOMETRY_BUFFER_3D_H__

#include "Vector.h"
#include "Vertex.h"
#include "IwTypes.h"
#include "IwGL.h"

#include <IVertexBuffer.h>
#include <vector>

namespace GG
{

	class GeometryBuffer3d : public IVertexBuffer
	{
	public:
        typedef std::vector<Vertex>     VertexList;
        typedef std::vector<GLuint>     IndexList;


		GeometryBuffer3d( );
		~GeometryBuffer3d( );

		virtual void        clearVertices();

		virtual void        addPosition(    const Vector3 & p );
		virtual void        pushTexCoord(   const Vector2 & t );
		virtual void        pushNormal(     const Vector3 & n );
		virtual void        pushColor(	    const Vector4 & t );
		virtual void        pushTangent(    const Vector3 & t );
		virtual void        pushBitangent(  const Vector3 & b );

		virtual void        build( const DrawHint drawHint = D_STATIC );

        virtual void        render( const DrawMode & drawMode ) const;

        virtual void        bind() const;


        void                merge( const   GeometryBuffer3d &  buffer );

        void                build( Vertex   *   vertexList, uint vertCount,
                                   uint     *   indexList, uint indexCount,
                                   DrawHint     drawHint = D_STATIC );


        Vertex  *           getVertexArray();
        uint    *           getIndexArray();


        uint                getIndexCount()     const;
        uint                getVertexCount()    const;

	private:

        VertexList		    _vertexList;
        IndexList		    _indexList;

        GLuint              _indexBufferHandle;
        GLuint              _vertexBufferHandle;
        GLuint              _arrayBufferHandle;


        Vector2             _currentTexCoord;
        Vector3             _currentNormal;
        Vector4             _currentColor;
        Vector3             _currentTangent;
        Vector3             _currentBitangent;


        void                _generateTangents();
        void                _generateIndices();

        bool                _isSameVertex( const Vertex &   v1, const Vertex &  v2 )    const;
	};
}

#endif

