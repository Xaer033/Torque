/*
	Module: Vertex
	Description: Most basic geometric entity
	Author: Julian Williams
*/

#ifndef __VERTEX_H__
#define __VERTEX_H__

#include "Vector.h"

namespace GG
{
	enum VertexTags
	{
		PositionTag = 0,
		TexcoordTag,
		NormalTag,
		TangentTag,
		BitangentTag,
		ColorTag,

		VertexTagMax
	};

	struct Vertex
	{
		Vector3 position;
		Vector2 texCoord;
		Vector3 normal;
		Vector3 tangent;
		Vector3 biTangent;
		Vector4 color;
	};

}


#endif
