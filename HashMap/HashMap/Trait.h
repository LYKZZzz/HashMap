#ifndef __TRAIT__
#define __TRAIT__


#include<algorithm>
#include<iostream>
template<typename T>
struct is_pair {
	typedef T Type;
	static const Type& ExtractKey(const T& Data){
		return Data;
	}
};

template<typename _Key, typename _Val>
struct is_pair<std::pair<_Key, _Val>> {	
	typedef _Key Type;
	static const Type& ExtractKey(const std::pair<_Key, _Val>& Data) {
		return Data.first;
	}
};

template<class T>
static auto has_less_or_equal_operator(int)-> decltype(std::declval<T>() > std::declval<T>());

template<class T>
static auto has_less_or_equal_operator(int)-> decltype(std::declval<T>().operator<=(std::declval<T>()));

template<class T>
static void has_less_or_equal_operator(...);


template<class T, class U>
struct _notGreater{
	static bool compare(const T&x, const T&y) {
		return x <= y;
	}
};

template<class T>
struct _notGreater<T, void> {
	static bool compare(const T&x, const T&y) {
		size_t length = sizeof(T) / sizeof(char);
		char* ptx = (char*)&x, *pty = (char*)&y;
		for (size_t i = 0; i < length; i++) {
			if (ptx[i] > pty[i])
				return false;
			else if (ptx[i] < pty[i])
				return true;
		}
		return true;
	}
};

#endif // !__TRAIT__
