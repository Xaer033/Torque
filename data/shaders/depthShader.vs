
uniform mat4 modelViewProj; 

attribute vec4 vPosition;


varying vec4 depthPos;


void main() 
{ 
	vec4 pos	= modelViewProj * vPosition; 
	gl_Position = pos;
 
	depthPos = pos;

} 