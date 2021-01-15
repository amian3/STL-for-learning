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


//����ļ�����������Ҫ�ļ����㷨
//����copy copy backward fill fill n
//max min swap�ȵ�
//��Ҫע�����value_type������iterator�еĹ���Ὣ��ǿ��ת��ΪT*
//��˿���ͨ����template�����class T �ķ�ʽ����
//ȫ��
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
//����binary��ӳ��
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
//�������uninitialized���õ���fill��fill_n
//ʹ�����ǵ�ʱ��˵���Ѿ�����Ҫconstruct�ˣ���Ϊ�Ѿ���vector��������
//ֻ�Ǹ�ֵ����
template<class ForwardIterator, class T>
void fill(ForwardIterator first, ForwardIterator last, const T& value){
    for(; first != last; ++first){
        *first = value;
    }
}
//���ظ�ֵ��β��
template<class OutputIterator, class Size, class T>
OutputIterator fill_n(OutputIterator first, Size n, const T& value){
    for(; n > 0; --n, ++first){
        *first = value;
    }
    return first;
}
//������Ԫ��������
//��Ҫtest
//����������swap�������������
//ǰ����ͨ��������������������Ϊ��ˣ�����������Ҫ��һ�£�����vector��map
//������ֱ��ͨ����ַ���н���
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

//�ȴ�С, �Ҽ����˷º���
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

//�ֵ�Ƚ�
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

//���ڷº����ıȽ�
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
//���unsigned char*���ػ��汾
inline bool
lexicographical_compare(const unsigned char* first1,
                        const unsigned char* last1,
                        const unsigned char* first2,
                        const unsigned char* last2){
    const size_t len1 = last1 - first1;
    const size_t len2 = last2 - first2;
    const int result = memcmp(first1, first2, min(len1, len2));
    //��string.h��
    //�����ǱȽ�first1��frist2��ǰmin���ֽ�
    //���ش���0 ��1 > 2
    //����С��0 ��1 < 2
    //���ص���0 ��1 = 2
    return result != 0 ? result < 0 : len1 < len2;
}

//���ص�һ�Բ���ȵ�����Iterator
//pair����һ�ļ���
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

//����˳����copy->_copy_dispatch->_copy_t��_copy
//_copyֱ�����л�������_copy_d
//_copy_tҲ��ֱ�����л�����_copy_d

//copy�����������Ǹ���copy_dispatch������ʲô���ͣ�ָ�룬����������constָ��
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
    //����Ϊʲô����ֱ����0��Ϊ����
    //��Ϊ��copy d�л����¶���һ�飬��������ֻҪ��ptrdiff_t���������;Ϳ�����
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
    //���copy t->copy d��һ������������Ҫ��distance��ֵ
}

//������_copy_dispatch����
template<class InputIterator, class OutputIterator>
struct _copy_dispatch{
    OutputIterator operator()
    (InputIterator first, InputIterator last, OutputIterator result){
    return _copy(first, last, result, iterator_category(first));
    }
};
//�����Ƿ���ָ������£�������ǿ����������͵��ò�ͬ�ĺ���
//����Ϊʲô��ô����
//��Ϊ��nΪfor loop�������ٶȱ��Ե�����Ϊ������
//����ǣ���ô������û��trivial assignment operator�Լ���
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

//������copy����
template<class InputIterator, class OutputIterator>
inline OutputIterator
copy(InputIterator first, InputIterator last, OutputIterator result){
    return _copy_dispatch<InputIterator, OutputIterator>()
    (first, last, result);
}
//���char*���ػ��汾
inline char* copy(const char* first, const char* last, char* result){
    memmove(result, first, last - first);
    //��string.h��
    //�����Ǵ�first����last - first��Ԫ�ص�result��
    return result + (last - first);
}
//���wchar_t*���ػ��汾
inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result){
    memmove(result, first, sizeof(wchar_t) * (last - first));
    return result + (last - first);
}



//copy backward�ǰ�first��last�����ݸ��Ƶ�result - (last - first)��result֮��
//�������ĵ�����������bidirectional��
//������ˣ�����Ҫ��copyһ���ж��Ƿ���input����random��
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
