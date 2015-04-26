
#include "GmbFormat.h"

#include <fstream>
#include <vector>

#include "GeometryBuffer3d.h"
#include "Vertex.h"


namespace GG
{

	bool GmbFormat::loadFromFile( const char * fileLocation, GeometryBuffer3d & mesh, float importScale )
	{
	
		std::vector< Vertex >	vertList;
		std::vector< uint	>	indexList;
	
	
		std::ifstream myFile( fileLocation, std::ios::in | std::ios::binary );
		//DebugText debug;
		
		Vector3 min, max;
	
		uint versionID = 0;
	
		if( !myFile.is_open() ) 
		{
	
		//	debug << "Could Not load Binary Model from: " << fileLocation.c_str() << std::endl;
		//	debug.Print();
			return false;
		}
	
		//Loop through all the polydefs and read them in
		myFile.seekg( 0 );
	
	
		//Get Version Number
		myFile.read( (char*)&versionID, sizeof( uint ) );
	
		//Read in Chunk by chunck based off of version
		//for backwards compatibility
		if( versionID >= 1001 )
		{
		
			//Read the size of the vertex list and all of the verticies
			uint vertexListSize = 0;
			myFile.read( (char*)&vertexListSize, sizeof( uint ) );
	
			if( vertexListSize <= 0 )
			{
				//DebugText("Importer Issue: Vert Size is 0!!!");
				return false;
			}

			for( uint i = 0; i < vertexListSize; ++i )
			{
				Vertex vBuffer;
				myFile.read( (char*)&vBuffer, sizeof( Vertex ) );
	
				vBuffer.position *= importScale;
				vertList.push_back( vBuffer );
				Vector3 v = vertList.back().position;
	
				min.x = (v.x < min.x) ? v.x : min.x;
				min.y = (v.y < min.y) ? v.y : min.y;
				min.z = (v.z < min.z) ? v.z : min.z;
	
				max.x = (v.x > max.x) ? v.x : max.x;
				max.y = (v.y > max.y) ? v.y : max.y;
				max.z = (v.z > max.z) ? v.z : max.z;
			}
	
	
			//Do the same for the index list
			uint indexListSize = 0;
			myFile.read((char*)&indexListSize, sizeof( unsigned int ));
	
			if( indexListSize <= 0 )
			{
				//DebugText("Importer Issue: Index Size is 0!!!");
				return false;
			}
	
			for( uint j = 0; j < indexListSize; ++j )
			{
				uint iBuffer = 0;
				myFile.read((char*)&iBuffer, sizeof( uint ));
	
				indexList.push_back( iBuffer );
			}
		}
	
	
		
		mesh.build( &vertList[0], vertList.size(), &indexList[0], indexList.size() );

		//mesh.aaBoundingBox.SetBox(min, max);
	
		//aaBoundingBox.min = min;
		//aaBoundingBox.max = max;
		return true;
	}
	
	
	
	bool GmbFormat::loadFromMemory( char *info, uint size, GeometryBuffer3d & mesh )
	{
	
		return true;
	}
	
	
	bool GmbFormat::saveToFile( const char * fileLocation, GeometryBuffer3d & mesh)
	{
		std::ofstream myFile( fileLocation, std::ios::out | std::ios::binary );
		//DebugText debug;
		
		if(!myFile.is_open()) 
		{
	
		//	debug << "Could Not Save Binary Model from: " << fileLocation.c_str() << std::endl;
		//	debug.Print();
			return false;
		}
	
	
		//Loop through all the polydefs and save them out to a file
		myFile.seekp( 0 );
	
		//Save file Version
		uint fileVersion = (uint)VERSION_ID;
		myFile.write((char*)&fileVersion, sizeof(float));
	
		//Save the size of the vertex list and all of the verticies
		uint vertexListSize = mesh.getVertexCount();//.size();
		myFile.write((char*)&vertexListSize, sizeof( uint ));
	
		Vertex * vertexArray = mesh.getVertexArray();
		for( uint i = 0; i < vertexListSize; ++i)
		{
			//Last but not least, the body definition
			myFile.write((char*)( &vertexArray[i] ), sizeof( Vertex ));
		}
	
	
	
			//Loop through the point vector
		unsigned int indexListSize = mesh.getIndexCount();
		myFile.write((char*)&indexListSize, sizeof( uint ));
	
		uint * indexArray = mesh.getIndexArray();
		for(unsigned int j = 0; j < indexListSize; j++)
		{
			myFile.write( (char*)&indexArray[j], sizeof( uint ) );
		}
	
		return true;
	}
}