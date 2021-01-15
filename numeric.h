#ifndef NUMERIC_H_INCLUDED
#define NUMERIC_H_INCLUDED
//这个文件的内容是一些数值算法

//这个算法用来得到整个集合值的和
//只能得到first到last与init的和
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

//加法规则：按binary_op的约定做加法
template<class InputIterator, class T, class BinaryOperation>
T accumulate(InputIterator first, InputIterator last, T init, BinaryOperation binary_op){
    for(; first != last; ++first){
        init = binary_op(init, * first);
    }
    return init;
}

//output[i] = input[i] - input[i - 1]// i > 0
//output[0] = input[0]// i == 0
//这也是自行编写的函数
//返回值是result的尾端
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

//加法规则：按binary_op的约定做加法
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

//内积
//两个向量的直积 (first1, last1)和(first2, first2 + last1 - first1)
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

//规则1:乘法规则，两个向量用binary_op2的约定做乘法
//规则2:加法规则，每个维度的积按按binary_op1的约定做加法
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

//这个函数的作用是output[i] = sum[0,i];
//需要test，是自行编写的函数
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
//加法规则：按binary_op的约定做加法
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

//补充：partial sum 和 adjacent difference互为逆运算
//这意味着 adjacent difference( partial sum( x ) ) == x
/*
template<class T, class Integer>
inline T power(T x, Integer n){
    return power(x, n, multiplies<T>());//仿函数
}
//monoid i.e. 独异点
//这个操作应该满足结合律
template<class T, class Integer, class MonoidOperation>
T power(T x, Integer n, MonoidOperation op){
    if(n == 0)
        return identity_element(op);仿函数
    else{
        while(( n & 1) == 0){//折半相乘
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
//为[first, last]赋值，按value的值递增
//比如value = 2， [first，last] 长度为3
//那么这部分的数据就会变成[2,3,4]
//比较类似于python中的numpy
template<class ForwardIterator, class T>
void iota(ForwardIterator first, ForwardIterator last, T value){
    while(first != last){
        *first++ = value++;
    }
}
#endif // NUMERIC_H_INCLUDED
