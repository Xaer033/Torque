/*
	Module: Transform
	Description:	Transform for game objects, essentially there worldMatrix
	Author: Julian Williams
*/

#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_


#define GLM_FORCE_COMPILER_UNKNOWN
#define GLM_FORCE_PURE
#define _GLIBCXX_USE_C99_MATH 1

#include <list>

#include "glm\glm.hpp"

#include "GameComponent.h"
#include "Vector.h"


namespace GG
{

	typedef enum Space
	{
		WORLD = 0,
		LOCAL
	} Space;


	class Transform;

	typedef std::list< Transform * > ChildrenList;


	class Transform
	{
	public:

		Transform();
		virtual ~Transform( );


		Vector3		    forward()	const;
		Vector3		    back()		const;
		Vector3		    right()		const;
		Vector3		    left()		const;
		Vector3		    up()		const;
		Vector3		    down()		const;
	
	
	
		glm::mat4	    getMatrix() const;

		void		    setParent( Transform * parent );
		Transform *     getParent();

		void	        addChild( Transform * child );
		void	        removeChild( Transform * child );


		glm::mat4 	    inverse() const;
		void	        lookAt( const Vector3 & center, const Vector3 & up = Vector3( 0, 1, 0 ) );
		void	        lookAt( const Transform & center, const Vector3 & up = Vector3( 0, 1, 0 )  );

		void	        setPosition( const Vector3 & position );
		Vector3         getPosition( Space space = LOCAL ) const;

		void	        setAxisAngle( float angle, const Vector3 & axis );
		void	        setScale(const Vector3 & scale );
	
		void	        translate( const Vector3 & translation, Space moveSpace = LOCAL );
		void	        rotate( float angle, const Vector3 & axis, Space rotateSpace = LOCAL );
		void	        scale( const Vector3 & scale, Space scaleSpace = LOCAL );


        Vector3         transformPoint( const Vector3 & point );
        Vector3         transformDirection( const Vector3 & direction );
        Vector3         inverseTransformPoint( const Vector3 & point );
        Vector3         inverseTransformDirection( const Vector3 & direction );

	
	private:

		void	        _updateHierarchy( );
		void	        _updateChildren( );

	private:
		
		glm::mat4       _modelMat;
		glm::mat4       _worldMat;
        glm::mat4       _inverseMat;

		Transform *     _parent;

		ChildrenList    _childrenList;
	
	};


}

#endif
