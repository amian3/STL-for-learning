#ifndef CONSTRUCT_H_
#define CONSTRUCT_H_
#include<new>
#include"type_traits.h"
#include"iterator.h"
//这个文件包含了construct和destroy
//construct很简短，但是这个new并不是给出空间
//而是类似于realloc，在已有空间上用T1的构造函数给出T1
template<class T1, class T2>
inline void construct(T1* p, const T2& value){
    new(p) T1(value);
}
//destroy函数有两个参数版本
//这是直接析构pointer
template<class T>
inline void destroy(T* pointer){
    pointer->~T();
}
//这是析构从first到last所有的内容
//它的调用顺序是destroy -> _destroy -> _destroy_aux
//先给出要析构的型别，之后判断是否有trivial destructor，最后析构

template<class ForwardIterator>
inline void _destroy_aux(ForwardIterator first, ForwardIterator last, _false_type){
    for(;first < last; ++first){
        destroy(&*first);
    }
}

template<class ForwardIterator, class T>
inline void _destroy(ForwardIterator first, ForwardIterator last, T*){
    typedef typename _type_traits<T>::has_trivial_destructor trivial_destructor;
    //获取T是否有trivial destructor
    _destroy_aux(first, last, trivial_destructor());
}

template<class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last){
    _destroy(first, last, value_type(first));//value type函数在iterator文件中
}





template<class ForwardIterator>
inline void _destroy_aux(ForwardIterator, ForwardIterator, _true_type){}
//两个特化版本
inline void destroy(char*, char*){}
inline void destroy(wchar_t*, wchar_t*){}
#endif // CONSTRUCT_H
