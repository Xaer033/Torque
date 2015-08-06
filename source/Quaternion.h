
/*
	Module: Quaternions
	Description:	Represent Rotations in the game engine
				point on a 4D Sphere
	Author: Julian Williams
*/

#ifndef _QUATERNIONS_H_
#define _QUATERNIONS_H_


#define GLM_FORCE_COMPILER_UNKNOWN
#define GLM_FORCE_PURE
#define _GLIBCXX_USE_C99_MATH 1

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"


namespace GG
{
	typedef glm::quat Quaternion;
	
	extern Quaternion identity;
}


#endif

