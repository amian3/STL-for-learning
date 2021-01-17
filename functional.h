#ifndef FUNCTIONAL_H_INCLUDED
#define FUNCTIONAL_H_INCLUDED
//这个文件存放了一些常用的仿函数

//一元函数
template<class Arg, class Result>
struct unary_function{
    typedef Arg argument_type;
    typedef Result result_type;
};

//二元函数
template<class Arg1, class Arg2, class Result>
struct binary_function{
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Result result_type;
};
//算术类仿函数
template<class T>
struct plus : public binary_function<T, T, T>{
    T operator()(const T& x, const T& y) const { return x + y;}
};

template<class T>
struct minus : public binary_function<T, T, T>{
    T operator()(const T& x, const T& y) const { return x - y;}
};

template<class T>
struct multiplies : public binary_function<T, T, T>{
    T operator()(const T& x, const T& y) const { return x * y;}
};

template<class T>
struct divides : public binary_function<T, T, T>{
    T operator()(const T& x, const T& y) const { return x / y;}
};

template<class T>
struct modulus : public binary_function<T, T, T>{
    T operator()(const T& x, const T& y) const { return x % y;}
};

template<class T>
struct negate : public unary_function<T, T>{
    T operator()(const T& x) const { return -x;}
};

template<class T>
inline T identity_element(plus<T>){
    return T(0);
}

template<class T>
inline T identity_element(multiplies<T>){
    return T(1);
}

//关系类仿函数
template<class T>
struct equal_to : public binary_function<T, T, bool>{
    bool operator()(const T& x, const T& y)const{ return x == y;}
};

template<class T>
struct not_equal_to : public binary_function<T, T, bool>{
    bool operator()(const T& x, const T& y)const{ return x != y;}
};

template<class T>
struct greater : public binary_function<T, T, bool>{
    bool operator()(const T& x, const T& y)const{ return x > y;}
};

template<class T>
struct greater_equal : public binary_function<T, T, bool>{
    bool operator()(const T& x, const T& y)const{ return x >= y;}
};

template<class T>
struct less : public binary_function<T, T, bool>{
    bool operator()(const T& x, const T& y)const{ return x < y;}
};

template<class T>
struct less_equal : public binary_function<T, T, bool>{
    bool operator()(const T& x, const T& y)const{ return x <= y;}
};

//逻辑类仿函数
template<class T>
struct logical_and : public binary_function<T, T, bool>{
    bool operator()(const T& x, const T& y)const{ return x && y;}
};

template<class T>
struct logical_or : public binary_function<T, T, bool>{
    bool operator()(const T& x, const T& y)const{ return x || y;}
};

template<class T>
struct logical_not : public unary_function<T, bool>{
    bool operator()(const T& x)const{ return !x;}
};


#endif // FUNCTIONAL_H_INCLUDED
