#ifndef UNINITIALIZED_H_INCLUDED
#define UNINITIALIZED_H_INCLUDED

#include<new>
#include"construct.h"
//这个文件包含了uninitialized_fill_n uninitialized_copy 和 uninitialized_fill

//这uninitialized fill n的作用为从first赋值x进行n次
//顺序是uninitialized_fill_n -> _uninitialized_fill_n -> _uninitialized_fill_n_aux
//先判断型别，然后判断是否为POD，最后构造，true使用高阶函数，false用construct
//取得value type是为了能获得is POD
//这个函数的返回值是被输入内容的尾端(first + n)

//这个地方似乎不能做优化，因为first的value type也是一个template
template<class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator
_uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*){
    typedef typename _type_traits<T1>::is_POD_type is_POD;
    return _uninitialized_fill_n_aux(first, n, x, is_POD());

}

template<class ForwardIterator, class Size, class T>
inline ForwardIterator
uninitialized_fill_n(ForwardIterator first, Size n, const T& x){
    return _uninitialized_fill_n(first, n, x, value_type(first));
}



template<class ForwardIterator, class Size, class T>
inline ForwardIterator
_uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, _true_type){
    return fill_n(first, n, x);//这是一个来自algorithm的函数
}

template<class ForwardIterator, class Size, class T>
ForwardIterator
_uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, _false_type){
    ForwardIterator cur = first;
    try{//如果构造失败，删除之前所有构造的内容
        for(; n > 0; --n, ++cur){
            construct(&*cur, x);
        }
        return cur;
    }
    catch(...){
        destroy(first, cur);
        throw;
    }
}



//uninitialized copy的作用为复制first与last之间的内容到result
//顺序是uninitialized_copy -> _uninitialized_copy -> _uninitialized_copy_aux
//这个函数的返回值是被输入内容的尾端(result + n),(n = last - first)

template<class InputIterator, class ForwardIterator, class T>
inline ForwardIterator
_uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T*){
    typedef typename _type_traits<T>::is_POD_type is_POD;
    return _uninitialized_copy_aux(first, last, result, is_POD());
}

template<class InputIterator, class ForwardIterator>
inline ForwardIterator
uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result){
    return _uninitialized_copy(first, last, result, value_type(result));
}



template<class InputIterator, class ForwardIterator>
inline ForwardIterator
_uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, _true_type){
    return copy(first, last, result);//true可以调用高阶算法
}

template<class InputIterator, class ForwardIterator>
inline ForwardIterator
_uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, _false_type){
    ForwardIterator cur = result;
    try{
        for(; first != last; ++first, ++cur){
            construct(&*cur, *first);
        }
        return cur;
    }
    catch(...){
        destroy(result, cur);
        throw;
    }
}


//uninitialized fill的作用为从first到last赋值x
//顺序是uninitialized_fill -> _uninitialized_fill -> _uninitialized_fill_aux
//这个函数是void型，没有返回值
template<class ForwardIterator, class T>
inline void
_uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, _true_type){
    fill(first, last, x);//高阶函数
}

template<class ForwardIterator, class T>
inline void
_uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, _false_type){
    ForwardIterator cur = first;
    try{
        for(;cur != last; ++cur){
            construct(&*cur, x);
        }
    }
    catch(...){
        destroy(first, cur);
        throw;
    }

}

template<class ForwardIterator, class T, class T1>
inline void
_uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T1*){
    typedef typename _type_traits<T1>::is_POD_type is_POD;
    _uninitialized_fill_aux(first, last, x, is_POD());
}


template<class ForwardIterator, class T>
inline void
uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x){
    _uninitialized_fill(first, last, x, value_type(first));
}






#endif // UNINITIALIZED_H_INCLUDED
