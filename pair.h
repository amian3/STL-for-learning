#ifndef PAIR_H_INCLUDED
#define PAIR_H_INCLUDED
//����һ����Ԫ���ԵĽṹ
//��algobase������mismatch�ķ���ֵ
//��ֻ��һ��������ʵ����iostream�����struct�������Լ�д
template<class T1, class T2>
struct pair{
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
