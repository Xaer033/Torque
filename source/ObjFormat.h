/*
	Module: OBJ model format
	Description: Functions to handle obj models from the device
	Author: Julian Williams
*/

#ifndef _OBJ_IMPORTER_H
#define _OBJ_IMPORTER_H




namespace GG
{
	class GeometryBuffer3d;

	class ObjFormat
	{
	public: 

		static bool loadFromFile( const char * fileLocation, GeometryBuffer3d & mesh );
		static bool loadFromMemory( char * info, unsigned int size, GeometryBuffer3d & mesh );
		static bool saveToFile( const char * fileLocation, GeometryBuffer3d & mesh );

	};
}
#endif