#ifndef CONSTRUCT_H_
#define CONSTRUCT_H_
#include<new>
#include"type_traits.h"
#include"iterator.h"
//����ļ�������construct��destroy
//construct�ܼ�̣��������new�����Ǹ����ռ�
//����������realloc�������пռ�����T1�Ĺ��캯������T1
template<class T1, class T2>
inline void construct(T1* p, const T2& value){
    new(p) T1(value);
}
//destroy���������������汾
//����ֱ������pointer
template<class T>
inline void destroy(T* pointer){
    pointer->~T();
}
//����������first��last���е�����
//���ĵ���˳����destroy -> _destroy -> _destroy_aux
//�ȸ���Ҫ�������ͱ�֮���ж��Ƿ���trivial destructor���������

template<class ForwardIterator>
inline void _destroy_aux(ForwardIterator first, ForwardIterator last, _false_type){
    for(;first < last; ++first){
        destroy(&*first);
    }
}

template<class ForwardIterator, class T>
inline void _destroy(ForwardIterator first, ForwardIterator last, T*){
    typedef typename _type_traits<T>::has_trivial_destructor trivial_destructor;
    //��ȡT�Ƿ���trivial destructor
    _destroy_aux(first, last, trivial_destructor());
}

template<class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last){
    _destroy(first, last, value_type(first));//value type������iterator�ļ���
}





template<class ForwardIterator>
inline void _destroy_aux(ForwardIterator, ForwardIterator, _true_type){}
//�����ػ��汾
inline void destroy(char*, char*){}
inline void destroy(wchar_t*, wchar_t*){}
#endif // CONSTRUCT_H
