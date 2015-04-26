/*
	Module: Ghost Model Binary Format
	Description: Functions to load binary models from the device
	Author: Julian Williams
*/


#ifndef _GMB_FORMAT_H_
#define _GMB_FORMAT_H_

#include "IwTypes.h"


namespace GG
{
	class GeometryBuffer3d;

	class GmbFormat
	{
	public: 
		static const uint VERSION_ID = 1001;

		static bool loadFromFile( const char * fileLocation, GeometryBuffer3d & mesh, float importScale = 1.0f );
		static bool loadFromMemory( char * info, unsigned int size,	GeometryBuffer3d & mesh );
		static bool saveToFile( const char * fileLocation, GeometryBuffer3d & mesh );

	};
}

#endif
