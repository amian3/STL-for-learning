#ifndef TYPE_TRAITS_H_INCLUDED
#define TYPE_TRAITS_H_INCLUDED

//这个文件放置了type的特征萃取器，而不是iterator的萃取器
//它们将会在construct和destroy等函数中起到重要作用

struct _true_type{};
struct _false_type{};

template<class type>
struct _type_traits{
    typedef _true_type this_dummy_member_must_be_first;
    typedef _false_type has_trivial_default_constructor;
    typedef _false_type has_trivial_copy_constructor;
    typedef _false_type has_trivial_assignment_operator;
    typedef _false_type has_trivial_destructor;
    typedef _false_type is_POD_type;//Plain Old Data
};

template<>
struct _type_traits<char>{
    typedef _true_type has_trivial_default_constructor;
    typedef _true_type has_trivial_copy_constructor;
    typedef _true_type has_trivial_assignment_operator;
    typedef _true_type has_trivial_destructor;
    typedef _true_type is_POD_type;

};

template<>
struct _type_traits<signed char>{
    typedef _true_type has_trivial_default_constructor;
    typedef _true_type has_trivial_copy_constructor;
    typedef _true_type has_trivial_assignment_operator;
    typedef _true_type has_trivial_destructor;
    typedef _true_type is_POD_type;

};

template<>
struct _type_traits<unsigned char>{
    typedef _true_type has_trivial_default_constructor;
    typedef _true_type has_trivial_copy_constructor;
    typedef _true_type has_trivial_assignment_operator;
    typedef _true_type has_trivial_destructor;
    typedef _true_type is_POD_type;

};

template<>
struct _type_traits<short>{
    typedef _true_type has_trivial_default_constructor;
    typedef _true_type has_trivial_copy_constructor;
    typedef _true_type has_trivial_assignment_operator;
    typedef _true_type has_trivial_destructor;
    typedef _true_type is_POD_type;

};

template<>
struct _type_traits<unsigned short>{
    typedef _true_type has_trivial_default_constructor;
    typedef _true_type has_trivial_copy_constructor;
    typedef _true_type has_trivial_assignment_operator;
    typedef _true_type has_trivial_destructor;
    typedef _true_type is_POD_type;

};

template<>
struct _type_traits<int>{
    typedef _true_type has_trivial_default_constructor;
    typedef _true_type has_trivial_copy_constructor;
    typedef _true_type has_trivial_assignment_operator;
    typedef _true_type has_trivial_destructor;
    typedef _true_type is_POD_type;

};

template<>
struct _type_traits<unsigned int>{
    typedef _true_type has_trivial_default_constructor;
    typedef _true_type has_trivial_copy_constructor;
    typedef _true_type has_trivial_assignment_operator;
    typedef _true_type has_trivial_destructor;
    typedef _true_type is_POD_type;

};

template<>
struct _type_traits<long>{
    typedef _true_type has_trivial_default_constructor;
    typedef _true_type has_trivial_copy_constructor;
    typedef _true_type has_trivial_assignment_operator;
    typedef _true_type has_trivial_destructor;
    typedef _true_type is_POD_type;

};

template<>
struct _type_traits<unsigned long>{
    typedef _true_type has_trivial_default_constructor;
    typedef _true_type has_trivial_copy_constructor;
    typedef _true_type has_trivial_assignment_operator;
    typedef _true_type has_trivial_destructor;
    typedef _true_type is_POD_type;

};

template<>
struct _type_traits<float>{
    typedef _true_type has_trivial_default_constructor;
    typedef _true_type has_trivial_copy_constructor;
    typedef _true_type has_trivial_assignment_operator;
    typedef _true_type has_trivial_destructor;
    typedef _true_type is_POD_type;

};

template<>
struct _type_traits<double>{
    typedef _true_type has_trivial_default_constructor;
    typedef _true_type has_trivial_copy_constructor;
    typedef _true_type has_trivial_assignment_operator;
    typedef _true_type has_trivial_destructor;
    typedef _true_type is_POD_type;

};

template<>
struct _type_traits<long double>{
    typedef _true_type has_trivial_default_constructor;
    typedef _true_type has_trivial_copy_constructor;
    typedef _true_type has_trivial_assignment_operator;
    typedef _true_type has_trivial_destructor;
    typedef _true_type is_POD_type;

};

template<class T>
struct _type_traits<T*>{
    typedef _true_type has_trivial_default_constructor;
    typedef _true_type has_trivial_copy_constructor;
    typedef _true_type has_trivial_assignment_operator;
    typedef _true_type has_trivial_destructor;
    typedef _true_type is_POD_type;

};


#endif // TYPE_TRAITS_H_INCLUDED
