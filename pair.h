#ifndef PAIR_H_INCLUDED
#define PAIR_H_INCLUDED
//这是一个二元数对的结构
//在algobase中用于mismatch的返回值
//在algo文件中作为equal range的返回值
template<class T1, class T2>
struct pair{
    typedef T1 first_type;
    typedef T2 second_type;
    T1 first;
    T2 second;
    pair(): first(T1()), second(T2()){};
    pair(const T1& a, const T2& b): first(a),second(b){};

    template<class T3, class T4>
    pair(const pair<T3, T4>& temp):first(temp.first), second(temp.second){}
};

template<class T1, class T2>
inline bool operator == (const pair<T1, T2>& a, const pair<T1, T2>& b){
    return a.first = b.first && a.second == b.second;
}




#endif // PAIR_H_INCLUDED
