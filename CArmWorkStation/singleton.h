#ifndef SINGLETON_H_
#define SINGLETON_H_

template<typename T> 
class Singleton {
public:
	static T& getInstance();
protected:
	Singleton();
	virtual ~Singleton();
private:
	static T& instance;
};
/**
* The method return the instance of the object
* @return reference of T
*/
template<typename T> T& Singleton<T>::getInstance() {
	static T instance;
	return instance;
}

#endif /* SINGLETON_H_ */
