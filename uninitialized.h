#ifndef UNINITIALIZED_H_INCLUDED
#define UNINITIALIZED_H_INCLUDED

#include<new>
#include"construct.h"
//����ļ�������uninitialized_fill_n uninitialized_copy �� uninitialized_fill

//��uninitialized fill n������Ϊ��first��ֵx����n��
//˳����uninitialized_fill_n -> _uninitialized_fill_n -> _uninitialized_fill_n_aux
//���ж��ͱ�Ȼ���ж��Ƿ�ΪPOD������죬trueʹ�ø߽׺�����false��construct
//ȡ��value type��Ϊ���ܻ��is POD
//��������ķ���ֵ�Ǳ��������ݵ�β��(first + n)

//����ط��ƺ��������Ż�����Ϊfirst��value typeҲ��һ��template
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
    return fill_n(first, n, x);//����һ������algorithm�ĺ���
}

template<class ForwardIterator, class Size, class T>
ForwardIterator
_uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, _false_type){
    ForwardIterator cur = first;
    try{//�������ʧ�ܣ�ɾ��֮ǰ���й��������
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



//uninitialized copy������Ϊ����first��last֮������ݵ�result
//˳����uninitialized_copy -> _uninitialized_copy -> _uninitialized_copy_aux
//��������ķ���ֵ�Ǳ��������ݵ�β��(result + n),(n = last - first)

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
    return copy(first, last, result);//true���Ե��ø߽��㷨
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


//uninitialized fill������Ϊ��first��last��ֵx
//˳����uninitialized_fill -> _uninitialized_fill -> _uninitialized_fill_aux
//���������void�ͣ�û�з���ֵ
template<class ForwardIterator, class T>
inline void
_uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, _true_type){
    fill(first, last, x);//�߽׺���
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
