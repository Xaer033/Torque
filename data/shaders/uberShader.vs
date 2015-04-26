
uniform mat4 modelViewProj; 
uniform mat4 modelViewMat;
uniform mat4 shadowMat[4];

uniform mat4 modelMat;

attribute vec4 vPosition;
attribute vec2 vTexcoord;
attribute vec3 vNormal;
attribute vec3 vTangent;
attribute vec3 vBitangent;


varying vec4 vPos;
varying vec2 uv; 
varying vec4 normal;

varying vec4 shadowCoord[4];
varying mat3 tbnMat;



void main() 
{ 
	gl_Position =	modelViewProj * vPosition; 

	
	vPos		=	modelViewMat * vPosition;
	uv			=	vTexcoord; 
	normal			=	normalize( modelViewMat * vec4( vNormal, 0 ) );
	vec4 tangent	=	normalize( modelViewMat * vec4( vTangent, 0 ) );
	vec4 biTangent	=	normalize( modelViewMat * vec4( vBitangent, 0 ) );

	//for( int i = 0; i < 4; ++i )
	//{
		shadowCoord[0]		=	shadowMat[0] * vPosition;
	//}

	tbnMat		= mat3( tangent.xyz, biTangent.xyz, normal.xyz );
} 