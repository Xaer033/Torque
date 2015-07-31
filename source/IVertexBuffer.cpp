#include "IVertexBuffer.h"

namespace GG
{
    IVertexBuffer::IVertexBuffer()
    {
    }


    IVertexBuffer::~IVertexBuffer()
    {
    }

    void IVertexBuffer::setVertexProperties( int vertexProperties )
    {
        _vertexProperties = vertexProperties;
    }

    int IVertexBuffer::getVertexProperties( ) const
    {
        return _vertexProperties;
    }
}