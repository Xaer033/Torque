/*
	Module: Light
	Description:	Let there be lights! Illuminate the world

	Author: Julian Williams
*/

#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "Vector.h"
#include "IwTypes.h"


namespace GG
{
	enum LightType 
	{
		LT_POINT = 0,
		LT_DIRECTIONAL,
		LT_SPOT
	};

	struct Light
	{
	public:
		Light()
		{
			Id		= 0;
			type	= LT_POINT;
			color	= Vector4( 1 );
			intensity = 1.0f;
			position	= Vector4( 0 );
		}

		uint		Id;
		LightType	type;

		float		intensity;
		
		Vector4		position;
		Vector4		color;
	};

	struct DirectionLight : public Light
	{

	};

	struct SpotLight : public Light
	{
		
		Vector3 lookVector;
	};

	struct PointLight : public Light
	{
		float radius;
	};

}

#endif
