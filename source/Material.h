/*
	Module: Materialz
	Description: Describe how stuff looks / is rendered
	Author: Julian Williams
*/


#ifndef __MATERIALS_H__
#define __MATERIALS_H__

#include "IwTypes.h"
#include "Vector.h"

#define MAX_TEXTURE_LAYERS 3


namespace GG
{
	class Texture;
	class Shader;


	enum 
	{
		INVALID_ID     = 0,
        ShadowRecv     = 0x20,
        ShadowCast     = 0x40,
        LightRecv      = 0x80,
	};

	struct TexureLayer
	{
		Texture * diffuseName;
		Texture * specularName;
		Texture * normalName;

	};

	struct Material
	{
	public:

		uint	Id;
		
		uint	flags;

		Vector4 tintColor;

		Vector4 bumbScale;
		Vector4 specularScale;

		Vector4 diffuseColor;
		Vector4 specularColor;

		uint	usedLayers;

		TexureLayer textureLayers[ MAX_TEXTURE_LAYERS ];

		Shader * shader;
	};
}

#endif
