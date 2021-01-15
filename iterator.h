#ifndef ITERATOR_H
#define ITERATOR_H

#include<cstddef>
//value_type 迭代器所指对象的型别
//difference_type两个迭代器之间的距离
//reference type 引用类型
//pointer type指针类型
//iterator category迭代器类型，共五种
//input iterator 只读
//output iterator 唯写
//forward iterator 允许写入型算法 在迭代器所形成区间上读写
//bidirectional iterator 可双向移动
//random access iterator涵盖所有指针算术能力(p++ p+n p[n] p1<p2)
//input  output -> forward -> bidirectional -> random access
//前者的算法会接受后者的迭代器，但不一定快
struct input_iterator_tag{};
struct output_iterator_tag{};
struct forward_iterator_tag : public input_iterator_tag{};
struct bidirectional_iterator_tag : public forward_iterator_tag{};
struct random_access_iterator_tag : public bidirectional_iterator_tag{};

template<class Category, class T, class Distance = ptrdiff_t,
         class Pointer = T*, class Reference = T&>
struct iterator{
    typedef Category iterator_category;
    typedef T value_type;
    typedef Distance difference_type;
    typedef Pointer pointer;
    typedef Reference reference;

};
//以下是traits萃取机
template<class Iterator>
struct iterator_traits{
    typedef typename Iterator::iterator_category iterator_category;
    typedef typename Iterator::value_type value_type;
    typedef typename Iterator::difference_type difference_type;
    typedef typename Iterator::pointer pointer;
    typedef typename Iterator::reference reference;
};
//针对原生指针的偏特化版本
template<class T>
struct iterator_traits<T*>{
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef T& reference;

};
//针对原生指针const型的特化版本
template<class T>
struct iterator_traits<const T*>{
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef const T* pointer;
    typedef const T& reference;
};
//这个函数用来决定迭代器类型
template<class Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&){
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return category();
}
//这个函数用来决定distance type的类型
template<class Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&){
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
    //将0强制转换成<>中的类型
}
//这个函数用来决定value type，在construct/destroy函数中会用到
template<class Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&){
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

//distance函数
//两个_distance函数用来处理不同的迭代器
//distance和advance主要是为了不同种类的迭代器可以用同样的算法来实现
//只不过一些是first + n 一些是advance(first, n)
//InputIterator是没有+n操作的因此要用到while
template<class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
_distance(InputIterator first, InputIterator last, input_iterator_tag){
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while(first != last){
        ++first;++n;
    }
    return n;
}

template<class RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
_distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag){
    return last - first;
}

template<class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last){
    typedef typename iterator_traits<InputIterator>::iterator_category category;
    return _distance(first, last, category());
}

//advance函数
//同上，不同的迭代器有自己的_advance版本
//InputIterator是单向的
//Bidirectional是双向的
//Random有+n操作
template<class InputIterator, class Distance>
inline void _advance(InputIterator& i, Distance n, input_iterator_tag){
    while(n--)i++;
}

template<class BidirectionalIterator, class Distance>
inline void _advance(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag){
    if(n >= 0){
        while(n--)++i;
    }
    else{
        while(n++)--i;
    }
}

template<class RandomAccessIterator, class Distance>
inline void _advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag){
    i += n;
}

template<class InputIterator, class Distance>
inline void advance(InputIterator& i, Distance n){
    _advance(i, n, iterator_category(i));
}
#endif // ITERATOR_H
