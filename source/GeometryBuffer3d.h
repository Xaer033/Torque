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

namespace GG
{

	enum VertexProperty
	{
		POSITIONS	= 1 << 0,
		TEXCOORDS	= 1 << 1,
		NORMALS		= 1 << 2,
		COLORS		= 1 << 3,
		TANGENTS	= 1 << 4,
		BITANGENTS	= 1 << 5
	};

    enum DrawHint
    {
        D_STATIC    = GL_STATIC_DRAW,
        D_DYNAMIC   = GL_DYNAMIC_DRAW,
        D_STREAM    = GL_STREAM_DRAW
    };

	class GeometryBuffer3dImpl;



	class GeometryBuffer3d
	{
	public:

		GeometryBuffer3d(void);
		~GeometryBuffer3d(void);


		void setVertexProperties( int vertProps);
		int  getVertexProperties() const;
		
		void clearVertices();

		void addPosition( const Vector3 & p );
		void pushTexCoord( const Vector2 & t );
		void pushNormal( const Vector3 & n );
		void pushColor(	const Vector4 & t );
		void pushTangent( const Vector3 & t );
		void pushBitangent( const Vector3 & b );

		void build( DrawHint drawHint = D_STATIC );
        void build( Vertex * vertexList, uint vertCount, uint * indexList, uint indexCount, DrawHint drawHint = D_STATIC );

		void merge( const GeometryBuffer3d & buffer );

		uint getVertexHandle() const;
		uint getIndexHandle() const;


		uint getIndexCount() const;
		uint getVertexCount() const;

		uint	* getIndexArray() const;
		Vertex	* getVertexArray() const;

		//PositionList & getPosition() const;
		//NormalList & getNormals() const;
		//TexCoordList & getTexCoords() const;
		//ColorList & getColorList() const;
		 
	private:
		GeometryBuffer3dImpl * _impl;

	};
}

#endif

