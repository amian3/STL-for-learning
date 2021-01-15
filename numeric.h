#ifndef NUMERIC_H_INCLUDED
#define NUMERIC_H_INCLUDED
//����ļ���������һЩ��ֵ�㷨

//����㷨�����õ���������ֵ�ĺ�
//ֻ�ܵõ�first��last��init�ĺ�
#include"iterator.h"
#include"type_traits.h"

template<class InputIterator, class T>
T accumulate(InputIterator first, InputIterator last, T init){
    for(;first != last; ++first){
        init = init + *first;
    }
    return init;
}


template<class InputIterator>
typename iterator_traits<InputIterator>::difference_type
accumulate(InputIterator first, InputIterator last){
    typename iterator_traits<InputIterator>::difference_type init = 0;
    for(;first != last; ++first){
        init = init + *first;
    }
    return init;
}

//�ӷ����򣺰�binary_op��Լ�����ӷ�
template<class InputIterator, class T, class BinaryOperation>
T accumulate(InputIterator first, InputIterator last, T init, BinaryOperation binary_op){
    for(; first != last; ++first){
        init = binary_op(init, * first);
    }
    return init;
}

//output[i] = input[i] - input[i - 1]// i > 0
//output[0] = input[0]// i == 0
//��Ҳ�����б�д�ĺ���
//����ֵ��result��β��
template<class InputIterator, class OutputIterator>
OutputIterator adjacent_difference(InputIterator first, InputIterator last,
                                   OutputIterator result){
    if(first == last)
        return result;
    *result = *first;
    typename iterator_traits<InputIterator>::value_type value = *first;
   // std::cout<<"a"<<std::endl;
    while(++first != last){
        typename iterator_traits<InputIterator>::value_type tmp = *first;
        *++result = tmp - value;
        value = tmp;
    }
    return ++result;
}

//�ӷ����򣺰�binary_op��Լ�����ӷ�
template<class InputIterator, class OutputIterator, class BinaryOperation>
OutputIterator adjacent_difference(InputIterator first, InputIterator last,
                                   OutputIterator result, BinaryOperation binary_op){
    if(first == last)
        return result;
    *result = *first;
    typename iterator_traits<InputIterator>::value_type value = *first;
    while(++first != last){
        typename iterator_traits<InputIterator>::value_type tmp = *first;
        *++result = binary_op(tmp, value);
        value = tmp;
    }
    return ++result;
}

//�ڻ�
//����������ֱ�� (first1, last1)��(first2, first2 + last1 - first1)
template<class InputIterator1, class InputIterator2, class T>
T inner_product(InputIterator1 first1, InputIterator1 last1,
                InputIterator2 first2, T init){
    for(; first1 != last1; ++first1, ++first2){
        init = init + (*first1 * *first2);
    }
    return init;
}

template<class InputIterator1, class InputIterator2>
typename iterator_traits<InputIterator1>::difference_type
inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2){
    typename iterator_traits<InputIterator1>::difference_type init = 0;
    for(; first1 != last1; ++first1, ++first2){
        init = init + (*first1 * *first2);
    }
    return init;
}

//����1:�˷���������������binary_op2��Լ�����˷�
//����2:�ӷ�����ÿ��ά�ȵĻ�����binary_op1��Լ�����ӷ�
template<class InputIterator1, class InputIterator2, class T,
         class BinaryOperation1, class BinaryOperation2>
T inner_product(InputIterator1 first1, InputIterator1 last1,
                InputIterator2 first2, T init, BinaryOperation1 binary_op1,
                BinaryOperation2 binary_op2){
    for(; first1 != last1; ++first1, ++first2){
        init = binary_op1(init, binary_op2(*first1, *first2));
    }
    return init;
}

//���������������output[i] = sum[0,i];
//��Ҫtest�������б�д�ĺ���
template<class InputIterator, class OutputIterator>
OutputIterator partial_sum(InputIterator first, InputIterator last,
                           OutputIterator result){
    if(first == last)
        result;
    *result = *first;
    typename iterator_traits<InputIterator>::value_type value = *first;
    while(++first != last){
        value = value + *first;
        *++result = value;
    }
    return ++result;
}
//�ӷ����򣺰�binary_op��Լ�����ӷ�
template<class InputIterator, class OutputIterator, class BinaryOperation>
OutputIterator partial_sum(InputIterator first, InputIterator last,
                           OutputIterator result, BinaryOperation binary_op){
    if(first == last)
        return result;
    *result = *first;
    typename iterator_traits<InputIterator>::value_type value = *first;
    while(++first != last){
        value = binary_op(value, *first);
        *++result = value;
    }
    return ++result;
}

//���䣺partial sum �� adjacent difference��Ϊ������
//����ζ�� adjacent difference( partial sum( x ) ) == x
/*
template<class T, class Integer>
inline T power(T x, Integer n){
    return power(x, n, multiplies<T>());//�º���
}
//monoid i.e. �����
//�������Ӧ����������
template<class T, class Integer, class MonoidOperation>
T power(T x, Integer n, MonoidOperation op){
    if(n == 0)
        return identity_element(op);�º���
    else{
        while(( n & 1) == 0){//�۰����
            n >>= 1;
            x = op(x, x);
        }
        T result = x;
        n >>= x;
        while(n != 0){
            x = op(x, x);
            if((n & 1) != 0)
                result = op(result, x);
            n >>= 1;
        }
        return result;
    }
}*/
//Ϊ[first, last]��ֵ����value��ֵ����
//����value = 2�� [first��last] ����Ϊ3
//��ô�ⲿ�ֵ����ݾͻ���[2,3,4]
//�Ƚ�������python�е�numpy
template<class ForwardIterator, class T>
void iota(ForwardIterator first, ForwardIterator last, T value){
    while(first != last){
        *first++ = value++;
    }
}
#endif // NUMERIC_H_INCLUDED
