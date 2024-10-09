#ifndef _UTILS_H_
#define _UTILS_H_

//---------------------------------------------------------
// SwapType:
//---------------------------------------------------------
template <class T>
inline void SwapType( T &val1, T &val2 )
{
	T	tmp = val1;
	val1 = val2;
	val2 = tmp;
}

#endif