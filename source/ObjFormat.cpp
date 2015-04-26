
#include "ObjFormat.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <stdio.h>

#include "Vertex.h"
#include "GeometryBuffer3d.h"
#include "DebugText.h"


namespace GG
{

	bool ObjFormat::loadFromFile( const char * fileLocation, GeometryBuffer3d & mesh)
	{
		mesh.clearVertices();

		const uint kMaxLineSize = 256;


		std::vector< uint	>	indexList;
	
		std::vector< Vector3 >	vertPosList;
		std::vector< Vector2 >	texCoordList;
		std::vector< Vector3 >	normalList;
	
		DebugText debugText;


		FILE * gFile = fopen( fileLocation, "r" );
	
		if ( gFile == NULL ) 
		{ 
			//std::cout << "File: " << filename.c_str() << " - Failed to load sucka." << std::endl;
			return false; 
		}

		char line[ kMaxLineSize ]; 
	 
	
		int v  = 0; //vert pos
		int vt = 0; //vert texCoord
		int vn = 0; //vert Normal
	
	
		Vector3 min, max;
	//Pass 1
		int lineCount = 0;
	
		while( fgets( line, 128, gFile ) != NULL )
	    {
			lineCount++;
	
			
	        switch( line[0] )
	        {
			case '#':
			case 's':
				continue;

			case 'm':
			{

			char mat[128];

			sscanf(line, "mtllib %s", &mat);
			//matLib = mat;
            //std::cout << matLib.c_str() << std::endl;
			}
			break;

		case 'u':
			{
				char mat[128];
				sscanf(line, "usemtl %s", &mat);
				//matName = mat;
				//std::cout << matName.c_str() << std::endl;

			}
			break;

	//Handle Vertices
	        case 'v':   // v, vt, or vn
	            switch( line[1] )
	            {
			//Position
	            case ' ':
					{
					Vector3 v;
	                sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z);
	                
					min.x = (v.x < min.x) ? v.x : min.x;
					min.y = (v.y < min.y) ? v.y : min.y;
					min.z = (v.z < min.z) ? v.z : min.z;
	
					max.x = (v.x > max.x) ? v.x : max.x;
					max.y = (v.y > max.y) ? v.y : max.y;
					max.z = (v.z > max.z) ? v.z : max.z;
	
					vertPosList.push_back( v );
	
					}
	                break;
			//Normals
	            case 'n':
				//	std::cout << lineCount << ": " << line << std::endl;
					{
	                Vector3 vn;
	                sscanf(line, "vn %f %f %f", &vn.x, &vn.y, &vn.z);
	               // std::cout << "Normal: " << vn << std::endl;
	
					normalList.push_back( vn );
					}
	                break;
			//UV Tex Coords
	            case 't':
					{
	                Vector2 vt;
	                sscanf(line, "vt %f %f", &vt.x, &vt.y);
	                
					texCoordList.push_back( vt );
					}
	
	            default:
	                break;
	            }
	            break;
	
	
		//Handle Faces
			case 'f':
				{
				//std::cout << line << std::endl;
					unsigned int posIndex[3];
					unsigned int texIndex[3];
					unsigned int normalIndex[3];
	
					//memset( posIndex,		0,	3 );
					//memset( texIndex,		0,	3 );
					//memset( normalIndex,	0 ,	3 );
	
				//Position
					sscanf( line, "f %d/%d/%d %d/%d/%d %d/%d/%d",	&posIndex[0], &texIndex[0], &normalIndex[0],
																	&posIndex[1], &texIndex[1], &normalIndex[1],
																	&posIndex[2], &texIndex[2], &normalIndex[2] );
				
					
					Vertex v;
					for( uint i = 0; i < 3; ++i )
					{
						//std::cout << "Size: " << vertPosList.size() << " - " << posIndex[0] -1 << std::endl;
						
						mesh.pushTexCoord( texCoordList[ texIndex[i] - 1 ]	);
						mesh.pushNormal( normalList[ normalIndex[i]	- 1 ]	);
						mesh.addPosition( vertPosList[ posIndex[i] - 1	]	);

						

					}
				}
	            break;

	        default:
	            break;
			}
		}

		fclose ( gFile );
	
	
	
		//mesh.aaBoundingBox.min = min;
		//mesh.aaBoundingBox.max = max;
	
		
		mesh.build( );
	
		return true;
	}
	
	
	
	bool ObjFormat::loadFromMemory(char * info, uint size,  GeometryBuffer3d & mesh)
	{
	
		return true;
	}
	
	
	bool ObjFormat::saveToFile( const char * fileLocation, GeometryBuffer3d & mesh)
	{
	
		return true;
	}

}