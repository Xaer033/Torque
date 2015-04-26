/*
	Module: DebugText
	Description: Print info that we need to know!
	Author: Julian Williams
*/


#ifndef _DEBUG_TEXT_H_
#define _DEBUG_TEXT_H_


#include <sstream>

#include "s3eDebug.h"

namespace GG
{

	class DebugText : public std::stringstream
	{
	public:
		DebugText() {}
		DebugText( const char * text )
		{
			str( text );
			print();
		}

		void print()
		{
		//OutputDebugString( this->str().c_str() );
			s3eDebugOutputString( this->str().c_str() );
			this->str("");
		}
	};

}

#endif
