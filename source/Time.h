/*
	Module: Time
	Description: Handle Time and Timers
	Author: Julian Williams
*/


#ifndef _TIMER_H_
#define _TIMER_H_

#include "s3eTypes.h"


namespace GG
{

	class Time
	{
	public:
		Time();
		~Time();

		static void		update();

		static float	getDeltaTime();
		static uint64	getTimeMs();


	private:
		static float	_maxDeltaTime;

		static uint64	_currentTime;
		static uint64	_newTime;
		static uint64	_frameTime;
	};

}

#endif
