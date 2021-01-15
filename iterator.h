#ifndef ITERATOR_H
#define ITERATOR_H

#include<cstddef>
//value_type ��������ָ������ͱ�
//difference_type����������֮��ľ���
//reference type ��������
//pointer typeָ������
//iterator category���������ͣ�������
//input iterator ֻ��
//output iterator Ψд
//forward iterator ����д�����㷨 �ڵ��������γ������϶�д
//bidirectional iterator ��˫���ƶ�
//random access iterator��������ָ����������(p++ p+n p[n] p1<p2)
//input  output -> forward -> bidirectional -> random access
//ǰ�ߵ��㷨����ܺ��ߵĵ�����������һ����
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
//������traits��ȡ��
template<class Iterator>
struct iterator_traits{
    typedef typename Iterator::iterator_category iterator_category;
    typedef typename Iterator::value_type value_type;
    typedef typename Iterator::difference_type difference_type;
    typedef typename Iterator::pointer pointer;
    typedef typename Iterator::reference reference;
};
//���ԭ��ָ���ƫ�ػ��汾
template<class T>
struct iterator_traits<T*>{
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef T& reference;

};
//���ԭ��ָ��const�͵��ػ��汾
template<class T>
struct iterator_traits<const T*>{
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef const T* pointer;
    typedef const T& reference;
};
//�������������������������
template<class Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&){
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return category();
}
//���������������distance type������
template<class Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&){
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
    //��0ǿ��ת����<>�е�����
}
//���������������value type����construct/destroy�����л��õ�
template<class Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&){
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

//distance����
//����_distance������������ͬ�ĵ�����
//distance��advance��Ҫ��Ϊ�˲�ͬ����ĵ�����������ͬ�����㷨��ʵ��
//ֻ����һЩ��first + n һЩ��advance(first, n)
//InputIterator��û��+n���������Ҫ�õ�while
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

//advance����
//ͬ�ϣ���ͬ�ĵ��������Լ���_advance�汾
//InputIterator�ǵ����
//Bidirectional��˫���
//Random��+n����
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
