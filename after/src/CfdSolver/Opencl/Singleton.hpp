
#ifndef SOLEINSTANCE_HPP_
#define SOLEINSTANCE_HPP_

#include <memory>


template <class T>
class Singleton
{
public:
	virtual ~Singleton () { }
	inline static void deleteSoleInstance() { soleInstance = std::unique_ptr<T>(); }

	inline static T *get()
	{
		if (!soleInstance.get())
			soleInstance = std::unique_ptr<T>(new T);
		return soleInstance.get();
	}

protected:
	static std::unique_ptr<T> soleInstance;
};

template <class T>
std::unique_ptr<T> Singleton<T>::soleInstance;

#endif
