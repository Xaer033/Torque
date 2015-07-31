/*
Module: Stream Vertex Buffer
Description:	Vertex Buffer for objects that need to be constructed per frame
camera view matrix to world geometry
Author: Julian Williams
*/

#ifndef _STREAM_VERTEX_BUFFER_H
#define _STREAM_VERTEX_BUFFER_H

#include <IVertexBuffer.h>

#include <Vector.h>

#include <vector>


namespace GG
{
    typedef std::vector< Vector3 > PositionList;
    typedef std::vector< Vector2 > TexCoordList;
    typedef std::vector< Vector3 > NormalList;
    typedef std::vector< Vector3 > TangentList;
    typedef std::vector< Vector3 > BiTangentList;
    typedef std::vector< Vector4 > ColorList;



    class StreamVertexBuffer : public IVertexBuffer
    {
        public:

        StreamVertexBuffer();
        ~StreamVertexBuffer();

        virtual void        clearVertices();

        virtual void        addPosition( const Vector3 & p );

        virtual void        pushTexCoord( const Vector2 & t );
        virtual void        pushNormal( const Vector3 & n );
        virtual void        pushColor( const Vector4 & t );
        virtual void        pushTangent( const Vector3 & t );
        virtual void        pushBitangent( const Vector3 & b );

        virtual void        build( const DrawHint drawHint = D_STATIC );
        virtual void        bind() const;

        virtual void        render( const DrawMode & drawMode ) const;


        void                merge( const StreamVertexBuffer & buffer );


        private:

        Vector2             _currentTexCoord;
        Vector3             _currentNormal;
        Vector3             _currentTangent;
        Vector3             _currentBitangent;
        Vector4             _currentColor;

        PositionList        _positionList;
        TexCoordList        _texCoordList;
        NormalList          _normalList;
        TangentList         _tangentList;
        BiTangentList       _biTangentList;
        ColorList           _colorList;

        void                _destroy();
    };
}

#endif
