

#ifndef _SINGLETON_H_
#define _SINGLETON_H_

namespace GG
{
	
	template <typename T>
	class Singleton
	{
	public:

	    static T & getInstance()
		{
			if( _instance == NULL )
				_instance = new T();

			return *_instance;
		}
	
	protected:

	    virtual ~Singleton()
		{
			if( _instance != NULL )
				delete Singleton::_instance;

			_instance = NULL;
		}

	    inline explicit Singleton()
		{
		}
	
	private:
	    static T * _instance;
	};
	

	template<typename T>
	T * Singleton<T>::_instance = NULL;
}

#endif