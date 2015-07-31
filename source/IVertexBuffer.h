#ifndef _I_VERTEX_BUFFER_H_
#define _I_VERTEX_BUFFER_H_

/*
Module: Vertex Buffer Interface
Description: Interface for Vertex Buffers
Author: Julian Williams
*/


#include <IwTypes.h>
#include <IwGL.h>
#include <Vector.h>

namespace GG
{
    enum DrawMode
    {
        POINTS          = GL_POINTS,
        LINES           = GL_LINES,
        LINE_LOOP       = GL_LINE_LOOP,
        LINE_STRIP      = GL_LINE_STRIP,
        TRIANGLE_LIST   = GL_TRIANGLES,
        TRIANGLE_STRIP  = GL_TRIANGLE_STRIP,
        TRIANGLE_FAN    = GL_TRIANGLE_FAN
    };


    enum VertexProperty
    {
        POSITIONS   = 1 << 0,
        TEXCOORDS   = 1 << 1,
        NORMALS     = 1 << 2,
        COLORS      = 1 << 3,
        TANGENTS    = 1 << 4,
        BITANGENTS  = 1 << 5
    };

    enum DrawHint
    {
        D_STATIC    = GL_STATIC_DRAW,
        D_DYNAMIC   = GL_DYNAMIC_DRAW,
        D_STREAM    = GL_STREAM_DRAW
    };

   


    class IVertexBuffer
    {
        public:
            IVertexBuffer();

            virtual         ~IVertexBuffer();

            void            setVertexProperties( int vertexProperties );
            int             getVertexProperties() const;

            virtual void    clearVertices()                        = 0;

            virtual void    addPosition( const Vector3 & p )       = 0;

            virtual void    pushTexCoord(  const Vector2 & t )     = 0;
            virtual void    pushNormal(    const Vector3 & n )     = 0;
            virtual void    pushColor(     const Vector4 & t )     = 0;
            virtual void    pushTangent(   const Vector3 & t )     = 0;
            virtual void    pushBitangent( const Vector3 & b )     = 0;

            virtual void    build( const DrawHint drawHint = D_STATIC ) = 0;
            virtual void    bind() const                                = 0;

            virtual void    render( const DrawMode & drawMode ) const   = 0;

        private:

            int             _vertexProperties;

    };

}

#endif
