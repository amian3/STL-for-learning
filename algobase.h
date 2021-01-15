#ifndef ALGOBASE_H_INCLUDED
#define ALGOBASE_H_INCLUDED

#include"pair.h"
#include "type_traits.h"
#include "iterator.h"
#include <string.h>
#include <climits>
#include <cstdlib>
#include <cstddef>
#include <new>


//这个文件包含了最重要的几个算法
//包括copy copy backward fill fill n
//max min swap等等
//需要注意的是value_type函数在iterator中的构造会将其强制转换为T*
//因此可以通过在template中添加class T 的方式引用
//全等
template<class InputIterator1, class InputIterator2>
inline bool equal(InputIterator1 first1, InputIterator1 last1,
                  InputIterator2 first2){
    for(; first1 != last1; ++first1, ++first2){
        if(*first1 != *first2){
            return false;
        }
    }
    return true;
}
//基于binary的映射
template<class InputIterator1, class InputIterator2,
         class BinaryPredicate>
inline bool equal(InputIterator1 first1, InputIterator1 last1,
                  InputIterator2 first2, BinaryPredicate binary_pred){
    for(; first1 != last1; ++first1, ++first2){
        if(!binary_pred(*first1, *first2)){
            return false;
        }
    }
    return true;
}
//这就是在uninitialized里用到的fill和fill_n
//使用他们的时候说明已经不需要construct了，因为已经有vector的内容了
//只是赋值而已
template<class ForwardIterator, class T>
void fill(ForwardIterator first, ForwardIterator last, const T& value){
    for(; first != last; ++first){
        *first = value;
    }
}
//返回赋值的尾端
template<class OutputIterator, class Size, class T>
OutputIterator fill_n(OutputIterator first, Size n, const T& value){
    for(; n > 0; --n, ++first){
        *first = value;
    }
    return first;
}
//对两个元素做交换
//需要test
//以下这两个swap参数是有区别的
//前者是通过迭代器交换，正是因为如此，迭代器并不要求一致，比如vector和map
//后者是直接通过地址进行交换
template<class ForwardIterator1, class ForwardIterator2>
inline void iter_swap(ForwardIterator1 a, ForwardIterator2 b){
    typename iterator_traits<ForwardIterator1>::value_type tmp = *a;
    *a = *b;
    *b = tmp;
}

template<class T>
inline void swap(T& a, T& b){
    T tmp = a;
    a = b;
    b = tmp;
}

//比大小, 且加入了仿函数
template<class T>
inline const T& max(const T& a, const T& b){
    return a < b ? b : a;
}

template<class T, class Compare>
inline const T& max(const T& a, const T& b, Compare comp){
    return comp(a, b) ? b : a;
}

template<class T>
inline const T& min(const T& a, const T& b){
    return a > b ? b : a;
}

template<class T, class Compare>
inline const T& min(const T& a, const T& b, Compare comp){
    return comp(b, a) ? b : a;
}

//字典比较
template<class InputIterator1, class InputIterator2>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                             InputIterator2 first2, InputIterator2 last2){
    for(; first1 != last1 && first2 != last2; ++first1, ++first2){
        if(*first1 < *first2){
            return true;
        }
        else if(*first1 > *first2){
            return false;
        }
    }
    return first1 == last1 && first2 != last2;
}

//基于仿函数的比较
template<class InputIterator1, class InputIterator2, class Compare>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                             InputIterator2 first2, InputIterator2 last2,
                             Compare comp){
    for(; first1 != last1 && first2 != last2; ++first1, ++first2){
        if(comp(*first1, *first2)){
            return true;
        }
        else if(comp(*first2, *first1)){
            return false;
        }
    }
    return first1 == last1 && first2 != last2;
}
//针对unsigned char*的特化版本
inline bool
lexicographical_compare(const unsigned char* first1,
                        const unsigned char* last1,
                        const unsigned char* first2,
                        const unsigned char* last2){
    const size_t len1 = last1 - first1;
    const size_t len2 = last2 - first2;
    const int result = memcmp(first1, first2, min(len1, len2));
    //在string.h中
    //作用是比较first1和frist2的前min个字节
    //返回大于0 则1 > 2
    //返回小于0 则1 < 2
    //返回等于0 则1 = 2
    return result != 0 ? result < 0 : len1 < len2;
}

//返回第一对不相等的两个Iterator
//pair在另一文件中
template<class InputIterator1, class InputIterator2>
pair<InputIterator1, InputIterator2>
mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2){
    while(first1 != last1 && *first1 == *first2){
        ++first1;++first2;
    }
    return pair<InputIterator1, InputIterator2>(first1, first2);
}

template<class InputIterator1,class InputIterator2,
         class BinaryPredicate>
pair<InputIterator1, InputIterator2>
mismatch(InputIterator1 first1, InputIterator1 last1,
         InputIterator2 first2, BinaryPredicate binary_pred){
    while(first1 != last1 && binary_pred(*first1, *first2)){
        ++first1;++first2;
    }
    return pair<InputIterator1, InputIterator2>(first1, first2);
}

//调用顺序是copy->_copy_dispatch->_copy_t或_copy
//_copy直接运行或者运行_copy_d
//_copy_t也是直接运行或运行_copy_d

//copy函数的作用是告诉copy_dispatch数据是什么类型，指针，迭代器还是const指针
//copy_dispatch

template<class RandomAccessIterator, class OutputIterator, class Distance>
inline OutputIterator
_copy_d(RandomAccessIterator first, RandomAccessIterator last,
        OutputIterator result, Distance*){
    for(Distance n = last - first; n > 0; --n, ++result, ++first){
        *result = *first;
    }
    return result;
}

template<class T>
inline T* _copy_t(const T* first,const T* last, T* result, _true_type){
    memmove(result, first, sizeof(T) * (last - first));
    return result + (last - first);
}

template<class T>
inline T* _copy_t(const T* first, const T* last, T* result, _false_type){
    return _copy_d(first, last, result,(ptrdiff_t*)0);
    //至于为什么这里直接用0作为参数
    //因为在copy d中会重新定义一遍，所以这里只要是ptrdiff_t的数据类型就可以了
}

template<class InputIterator, class OutputIterator>
inline OutputIterator
_copy(InputIterator first, InputIterator last,
      OutputIterator result, input_iterator_tag){
    for(; first != last; ++result, ++first){
        *result = *first;
    }
    return result;
}

template<class RandomAccessIterator, class OutputIterator>
inline OutputIterator
_copy(RandomAccessIterator first, RandomAccessIterator last,
      OutputIterator result, random_access_iterator_tag){
    return _copy_d(first, last, result, distance_type(first));
    //这和copy t->copy d是一个道理，并不需要给distance赋值
}

//下面是_copy_dispatch函数
template<class InputIterator, class OutputIterator>
struct _copy_dispatch{
    OutputIterator operator()
    (InputIterator first, InputIterator last, OutputIterator result){
    return _copy(first, last, result, iterator_category(first));
    }
};
//对于是否是指针这件事，如果不是看迭代器类型调用不同的函数
//至于为什么这么操作
//因为以n为for loop的条件速度比以迭代器为条件快
//如果是，那么考虑有没有trivial assignment operator以加速
template<class T>
struct _copy_dispatch<T*, T*>{
    T* operator()(T* first, T* last, T* result){
        typedef typename _type_traits<T>::has_trivial_assignment_operator t;
        return _copy_t(first, last, result, t());

    }
};

template<class T>
struct _copy_dispatch<const T*, T*>{
    T* operator()(const T* first, const T* last, T* result){
        typedef typename _type_traits<T>::has_trivial_assignment_operator t;
        return _copy_t(first, last, result, t());
    }
};

//以下是copy函数
template<class InputIterator, class OutputIterator>
inline OutputIterator
copy(InputIterator first, InputIterator last, OutputIterator result){
    return _copy_dispatch<InputIterator, OutputIterator>()
    (first, last, result);
}
//针对char*的特化版本
inline char* copy(const char* first, const char* last, char* result){
    memmove(result, first, last - first);
    //在string.h中
    //作用是从first复制last - first个元素到result中
    return result + (last - first);
}
//针对wchar_t*的特化版本
inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result){
    memmove(result, first, sizeof(wchar_t) * (last - first));
    return result + (last - first);
}



//copy backward是把first到last的内容复制到result - (last - first)到result之中
//所以它的迭代器必须是bidirectional的
//正因如此，不需要像copy一样判断是否是input或是random的
template<class BidirectionalIterator1, class BidirectionalIterator2, class Distance>
inline BidirectionalIterator2
_copy_backward_d(BidirectionalIterator1 first, BidirectionalIterator1 last,
        BidirectionalIterator2 result, Distance*){
    for(Distance n = last - first; n > 0; --n, --result, --last){
        *result = *last;
    }
    return result;
}

template<class T>
inline T* _copy_backward_t(const T* first,const T* last, T* result, _true_type){
    const ptrdiff_t n = last - first;
    memmove(result - n, first, sizeof(T) * n);
    return result - n;
}

template<class T>
inline T* _copy_backward_t(const T* first, const T* last, T* result, _false_type){
    return _copy_backward_d(first, last, result,(ptrdiff_t*)0);

}



template<class BidirectionalIterator1, class BidirectionalIterator2>
inline BidirectionalIterator2
_copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last,
      BidirectionalIterator2 result){
    for(; first != last; --result, --last){
        *result = *first;
    }
    return result;
}



template<class BidirectionalIterator1, class BidirectionalIterator2>
struct _copy_backward_dispatch{
     BidirectionalIterator2 operator()
    (BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result){
    return _copy_backward(first, last, result);
    }
};


template<class T>
struct _copy_backward_dispatch<T*, T*>{
    T* operator()(T* first, T* last, T* result){
        typedef typename _type_traits<T>::has_trivial_assignment_operator t;
        return _copy_backward_t(first, last, result, t());
    }
};

template<class T>
struct _copy_backward_dispatch<const T*, T*>{
    T* operator()(const T* first, const T* last, T* result){
        typedef typename _type_traits<T>::has_trivial_assignment_operator t;
        return _copy_backward_t(first, last, result, t());
    }
};


template<class BidirectionalIterator1, class BidirectionalIterator2>
inline BidirectionalIterator2
copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last,
              BidirectionalIterator2 result){
    return _copy_backward_dispatch<BidirectionalIterator1, BidirectionalIterator2>()
    (first, last, result);
}

#endif // ALGOBASE_H_INCLUDED
