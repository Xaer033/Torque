
#include "Time.h"

#include "s3eTimer.h"


namespace GG
{

	uint64 Time::_currentTime	= 0;
	uint64 Time::_newTime		= 0;
	uint64 Time::_frameTime		= 0;
	
	float Time::_maxDeltaTime	= 0.1f;


	Time::Time()
	{
		_currentTime = s3eTimerGetMs();
	}

	Time::~Time()
	{
	}

	void Time::update()
	{
		_newTime		= s3eTimerGetMs();
		_frameTime		= _newTime - _currentTime;

		_currentTime	= _newTime;
	}

	float Time::getDeltaTime()
	{
		float dt = (float)_frameTime / 1000.0f;
		return ( dt > _maxDeltaTime ) ? _maxDeltaTime : dt;
	}

	uint64 Time::getTimeMs()
	{
		return s3eTimerGetMs();
	}

}
