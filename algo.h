#ifndef ALGO_H_INCLUDED
#define ALGO_H_INCLUDED

#include"iterator.h"
#include"type_traits.h"
#include"pair.h"
//���ҵ�һ���ظ�Ԫ�أ����û�л᷵��last
template<class ForwardIterator>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last){
    if(first == last)return last;
    ForwardIterator next = first;
    while(++next != last){
        if(*first == *next)
            return first;
        first = next;
    }
    return last;
}

template<class ForwardIterator, class BinaryPredicate>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last,
                              BinaryPredicate binary_pred){
    if(first == last)return last;
    ForwardIterator next = first;
    while(++next != last){
        if(binary_pred(*first, *next))
            return first;
        first = next;
    }
    return last;
}
//��n�ظ�������
template<class InputIterator, class T>
typename iterator_traits<InputIterator>::difference_type
count(InputIterator first, InputIterator last, const T& value){
    typename iterator_traits<InputIterator>::difference_type n = 0;
    for(; first != last; ++first){
        if(*first == value)
            ++n;
    }
    return n;
}

template<class InputIterator, class Predicate>
typename iterator_traits<InputIterator>::difference_type
count_if(InputIterator first, InputIterator last,Predicate pred){
    typename iterator_traits<InputIterator>::difference_type n = 0;
    for(; first != last; ++first){
        if(pred(*first))
            ++n;
    }
    return n;
}

//����value�ĵ�һ������
template<class InputIterator, class T>
InputIterator find(InputIterator first, InputIterator last,
                   const T& value){
    while(first != last && *first != value)++first;
    return first;
}

template<class InputIterator, class Predicate>
InputIterator find_if(InputIterator first, InputIterator last,
                      Predicate pred){
    while(first != last && !pred(*first))++first;
    return first;
    }


//����[first2,last2)��[first1,last1)���һ�γ��ֵĵص�
//�ֳ�ForwardIterator��BidirectionalIterator����
//��������ȱ�ٷ��������
/*
template<class ForwardIterator1, class ForwardIterator2>
ForwardIterator1 _find_end(ForwardIterator1 first1, ForwardIterator1 last1,
                           ForwardIterator2 first2, ForwardIterator2 last2,
                           forward_iterator_tag, forward_iterator_tag){
    if(first2 == last2)return last1;
    else{
        ForwardIterator1 result = last1;
        while(true){
            ForwardIterator1 new_result = search(first1, last1, first2, last2);
            if(new_result == last1)
                return result;
            else{
                result = new_result;
                first1 = new_result;
                ++first1;
            }
        }
    }
}

template<class BidirectionalIterator1, class BidirectionalIterator2>
BidirectionalIterator1 _find_end(
    BidirectionalIterator1 first1, BidirectionalIterator1 last1,
    BidirectionalIterator2 first2, BidirectionalIterator2 last2,
    bidirectional_iterator_tag, bidirectional_iterator_tag){
    typedef reverse_iterator<BidirectionalIterator1> reviter1;
    typedef reverse_iterator<BidirectionalIterator2> reviter2;
    reviter1 rlast1(first1);
    reviter2 rlast2(first2);
    reviter1 rresult = search(reviter1(last1), rlast1,
                              reviter2(last2), rlast2);
    if(rresult == rlast1)
        return last1;
    else{
        BidirectionalIterator1 result = rresult.base();
        advance(result, -distance(first2, last2));
        return result;
    }
}

template<class ForwardIterator1, class ForwardIterator2>
inline ForwardIterator1
find_end(ForwardIterator1 first1, ForwardIterator1 last1,
         ForwardIterator2 first2, ForwardIterator2 last2){
    typedef typename iterator_traits<ForwardIterator1>::iterator_category
    category1;
    typedef typename iterator_traits<ForwardIterator2>::iterator_category
    category2;
    return _find_end(first1, last1, first2, last2, category1(), category2());
}
*/
//�ҳ�[first2,last2)���׸���[first1,last1)���ֵ�����
//�����synesthesia�в���aeiou���ͻ᷵��synesthesia�е�e
template<class InputIterator, class ForwardIterator>
InputIterator find_first_of(InputIterator first1, InputIterator last1,
                            ForwardIterator first2, ForwardIterator last2){
    for(; first1 != last1; ++first1){
        for(ForwardIterator iter = first2; iter != last2; ++iter){
            if(*first1 == *iter)
                return first1;
        }
    }
    return last1;
}

template<class InputIterator, class ForwardIterator, class BinaryPredicate>
InputIterator find_first_of(InputIterator first1, InputIterator last1,
                            ForwardIterator first2, ForwardIterator last2,
                            BinaryPredicate comp){
    for(; first1 != last1; ++first1){
        for(ForwardIterator iter = first2; iter != last2; ++iter){
            if(comp(*first1, *iter))
                return first1;
        }
    }
    return last1;
}
//��first last�е�Ԫ�ؽ���function��������Щ������transform
template<class InputIterator, class Function>
Function for_each(InputIterator first, InputIterator last, Function f){
    for(; first != last; ++first)
        f(*first);
    return f;
}
//����һ�����ƣ�����ǰ�generator�Ľ����ֵ��first last��
template<class ForwardIterator, class Generator>
void generate(ForwardIterator first, ForwardIterator last, Generator gen){
    for(; first != last; ++first)
        *first = gen();
}

template<class OutputIterator, class Size, class Generator>
OutputIterator generate_n(OutputIterator first, Size n, Generator gen){
    for(; n > 0; --n, ++first)
        *first = gen();
    return first;
}

//�ж�b�Ƿ���a��Ҫ���������������ź����
template<class InputIterator1, class InputIterator2>
bool includes(InputIterator1 first1, InputIterator1 last1,
              InputIterator2 first2, InputIterator2 last2){
    while(first1 != last1 && first2 != last2){
        if(*first2 < * first1)
            return false;
        else if(*first1 < *first2)
            ++first1;
        else{
            ++first1, ++first2;
        }
    }
    return first2 == last2;
}

template<class InputIterator1, class InputIterator2, class Compare>
bool includes(InputIterator1 first1, InputIterator1 last1,
              InputIterator2 first2, InputIterator2 last2,
              Compare comp){
    while(first1 != last1 && first2 != last2){
        if(comp(*first2, *first1))
            return false;
        else if(comp(*first1, *first2))
            ++first1;
        else{
            ++first1, ++first2;
        }
    }
    return first2 == last2;
}

//�ҳ����Ԫ��
template<class ForwardIterator>
ForwardIterator max_element(ForwardIterator first, ForwardIterator last){
    if(first == last)
        return first;
    ForwardIterator result = first;
    while(++first != last){
        if(*result < *first)
            result = first;
    }
    return result;
}

template<class ForwardIterator, class Compare>
ForwardIterator max_element(ForwardIterator first, ForwardIterator last,
                            Compare comp){
    if(first == last)
        return first;
    ForwardIterator result = first;
    while(++first != last){
        if(comp(*result, *first))
            result = first;
    }
    return result;
}

//�ҳ���СԪ��
template<class ForwardIterator>
ForwardIterator min_element(ForwardIterator first, ForwardIterator last){
    if(first == last)
        return first;
    ForwardIterator result = first;
    while(++first != last){
        if(*first < *result)//ԭ��ָ������ĵ�������û������<����Ĳ�������
            result = first;
    }
    return result;
}

template<class ForwardIterator, class Compare>
ForwardIterator min_element(ForwardIterator first, ForwardIterator last,
                            Compare comp){
    if(first == last)
        return first;
    ForwardIterator result = first;
    while(++first != last){
        if(comp(*first, *result))
            result = first;
    }
    return result;
}

//����������iterator�ϲ�Ϊ�µ�����iterator
template<class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
                     InputIterator2 first2, InputIterator2 last2,
                     OutputIterator result){
    while(first1 != last1 && first2 != last2){
        if(*first2 < *first1){
            *result = *first2;
            ++first2;
        }
        else{
            *result = *first1;
            ++first1;
        }
        ++result;
    }
    return copy(first2, last2, copy(first1, last1, result));
    //����������һ��Ϊ�յ�
}

template<class InputIterator1, class InputIterator2, class OutputIterator,
         class Compare>
OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
                     InputIterator2 first2, InputIterator2 last2,
                     OutputIterator result, Compare comp){
    while(first1 != last1 && first2 != last2){
        if(comp(*first2, *first1)){
            *result = *first2;
            ++first2;
        }
        else{
            *result = *first1;
            ++first1;
        }
        ++result;
    }
    return copy(first2, last2, copy(first1, last1, result));
}

//����pred�����ķ���ǰ�棬������ں���
template<class BidirectionalIterator, class Predicate>
BidirectionalIterator partition(BidirectionalIterator first,
                                BidirectionalIterator last,
                                Predicate pred){
    while(true){
            while(true){
                if(first == last)
                    return first;
                else if(pred(*first))
                    ++first;
                else
                    break;
            }
            --last;
            while(true){
                if(first == last)
                    return first;
                else if(!pred(*last))
                    --last;
                else
                    break;
            }
            iter_swap(first, last);
            ++first;
    }
}

//ɾ��ֵΪvalue��Ԫ��
//���ⲻ��������ɾ��
//0 1 0 2 0 3 0 4ɾ��0���� 1 2 3 4 0 3 0 4
//ֻ�ǰ�0�����Ԫ�ظ��Ƶ�ǰ�棬���治��
template<class InputIterator, class OutputIterator, class T>
OutputIterator remove_copy(InputIterator first, InputIterator last,
                           OutputIterator result, const T& value){
    for(; first != last; ++first){
        if(*first != value){
            *result = *first;
            ++result;
        }
        return result;
    }
}

template<class ForwardIterator, class T>
ForwardIterator remove(ForwardIterator first, ForwardIterator last,
                       const T& value){
    first = find(first, last, value);
    ForwardIterator next = first;
    return first == last ? first : remove_copy(++next, last, first, value);
}

template<class InputIterator, class OutputIterator, class Predicate>
OutputIterator remove_copy_if(InputIterator first, InputIterator last,
                              OutputIterator result, Predicate pred){
    for(; first != last; ++first){
        if(!pred(*first)){
            *result = *first;
            ++result;
        }
        return result;
    }
}

template<class ForwardIterator, class Predicate>
ForwardIterator remove_if(ForwardIterator first, ForwardIterator last,
                          Predicate pred){
    first = find_if(first, last, pred);
    ForwardIterator next = first;
    return first == last ? first : remove_copy_if(++next, last, first, pred);
}

//�����е�old value�滻��new value
template<class ForwardIterator, class T>
void replace(ForwardIterator first, ForwardIterator last,
             const T& old_value, const T& new_value){
    for(; first != last; ++first){
        if(*first == old_value)
            *first = new_value;
    }
}
//ԭ���в��䣬�����е�old value��Ϊnew value
template<class InputIterator, class OutputIterator, class T>
OutputIterator replace_copy(InputIterator first, InputIterator last,
                            OutputIterator result, const T& old_value,
                            const T& new_value){
    for(; first != last; ++first, ++result)
        *result = *first == old_value ? new_value : *first;
    return result;
}
//����pred�ı�׼ת��Ϊnew value
template<class ForwardIterator, class Predicate, class T>
void replace_if(ForwardIterator first, ForwardIterator last,
                Predicate pred, const T& new_value){
    for(; first != last; ++first){
        if(pred(*first))
            *first = new_value;
    }
}
//����pred�ı�׼����������
template<class InputIterator, class OutputIterator, class Predicate, class T>
OutputIterator replace_copy_if(InputIterator first, InputIterator last,
                               OutputIterator result, Predicate pred,
                               const T& new_value){
    for(; first != last; ++first, ++result)
        *result = pred(*first) ? new_value : *first;
    return result;
}
//��ת
template<class BidirectionalIterator>
void _reverse(BidirectionalIterator first, BidirectionalIterator last,
              bidirectional_iterator_tag){
    while(true){
        if(first == last || first == --last)
            return;
        else
            iter_swap(first++, last);
    }
}

template<class RandomAccessIterator>
void _reverse(RandomAccessIterator first, RandomAccessIterator last,
              random_access_iterator_tag){
    while(first < last)
        iter_swap(first++, --last);
}

template<class BidirectionalIterator>
inline void reverse(BidirectionalIterator first, BidirectionalIterator last){
    _reverse(first, last, iterator_category(first));
}

//��ת����������
template<class BidirectionalIterator, class OutputIterator>
OutputIterator reverse_copy(BidirectionalIterator first,
                            BidirectionalIterator last,
                            OutputIterator result){
    while(first != last){
        --last;
        *result = *last;
        ++result;
    }
    return result;
}
//ABCDEFG HIGKLMNOP
//HIGKLMN ABCDEFGOP
//HIGKLMN OPCDEFGAB
//HIGKLMN OPABEFGCD
//HIGKLMN OPABCDGEF
//HIGKLMN OPABCDEFG
template<class ForwardIterator, class Distance>
void _rotate(ForwardIterator first, ForwardIterator middle,
             ForwardIterator last, Distance*, forward_iterator_tag){
    for(ForwardIterator i = middle; ;){
        iter_swap(first, i);
        ++first;
        ++i;
        if(first == middle){
            if( i == last)
                return;
            middle = i;
        }
        else if(i == last)
            i = middle;
    }
}

template<class BidirectionalIterator, class Distance>
void _rotate(BidirectionalIterator first, BidirectionalIterator middle,
             BidirectionalIterator last, Distance*,
             bidirectional_iterator_tag){
    reverse(first, middle);
    reverse(middle, last);
    reverse(first, last);

}

template<class EuclideanRingElement>
EuclideanRingElement _gcd(EuclideanRingElement m, EuclideanRingElement n){
    while( n != 0){
        EuclideanRingElement t = m % n;
        m = n;
        n = t;
    }
    return m;
}

template<class RandomAccessIterator, class Distance, class T>
void _rotate_cycle(RandomAccessIterator first, RandomAccessIterator last,
                   RandomAccessIterator initial, Distance shift, T*){
    T value = *initial;
    RandomAccessIterator ptr1 = initial;
    RandomAccessIterator ptr2 = ptr1 + shift;
    while(ptr2 != initial){
        *ptr1 = *ptr2;//��ֵ����
        ptr1 = ptr2;//ptr1ָ��ת��
        if(last - ptr2 > shift)
            ptr2 += shift;
        else
            ptr2 = first + (shift - (last - ptr2));
        //ptr2ָ��ת��
    }
    *ptr1 = value;//���һ��ֵ�ĸ�ֵ����
//�ѷ�ת�ֳ�n�����֣�n��gcd����,��1
//ABCDEF GHI
//GBCDEF GHI
//GBCDEF DHI
//GBCAEF DHI��һ��rotate cycle����
//GHCAEF DHI
//GHCAEF DEI
//GHCABF DEI�ڶ���rotate cycle����
//GHIABF DEI
//GHIABF DEF
//GHIABC DEF������rotate cycle����

//��2
//AB CDE
//CB CDE
//CB EDE
//CB EDB
//CD EDB
//CD EAB ��������ֻ��һ��rotate cycle
}

template<class RandomAccessIterator, class Distance>
void _rotate(RandomAccessIterator first, RandomAccessIterator middle,
             RandomAccessIterator last, Distance*,
             random_access_iterator_tag){
    Distance n = _gcd(last - first, middle - first);
    while(n--)
        _rotate_cycle(first, last, first + n, middle - first, value_type(first));
}


template<class ForwardIterator>
inline void rotate(ForwardIterator first, ForwardIterator middle,
                   ForwardIterator last){
    if(first == middle || middle == last)
        return;
    _rotate(first, middle, last, distance_type(first),
            iterator_category(first));
}

//rotate��result��
template<class ForwardIterator, class OutputIterator>
OutputIterator rotate_copy(ForwardIterator first, ForwardIterator middle,
                           ForwardIterator last, OutputIterator result){
    return copy(first, middle, copy(middle, last, result));
}

//���ҵڶ��������״��ڵ�һ�������г��ֵ�����(iterator)
template<class ForwardIterator1, class ForwardIterator2>
inline ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1,
                               ForwardIterator2 first2, ForwardIterator2 last2){
    typename iterator_traits<ForwardIterator1>::difference_type d1 = distance(first1, last1);
    typename iterator_traits<ForwardIterator1>::difference_type d2 = distance(first2, last2);
    if(d1 < d2)return last1;
    ForwardIterator1 current1 = first1;
    ForwardIterator2 current2 = first2;
    while(current2 != last2){
        if(*current1 == *current2){
            ++current1;
            ++current2;
        }
        else{
            if(d1 == d2)
                return last1;
            else{
                current1 = ++first1;
                current2 = first2;
                --d1;
            }
        }
    }
    return first1;
}

template<class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
inline ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1,
                               ForwardIterator2 first2, ForwardIterator2 last2,
                               BinaryPredicate binary_pred){
    typename iterator_traits<ForwardIterator1>::difference_type d1 = distance(first1, last1);
    typename iterator_traits<ForwardIterator1>::difference_type d2 = distance(first2, last2);
    if(d1 < d2)return last1;
    ForwardIterator1 current1 = first1;
    ForwardIterator2 current2 = first2;
    while(current2 != last2){
        if(binary_pred(*current1, *current2)){
            ++current1;
            ++current2;
        }
        else{
            if(d1 == d2)
                return last1;
            else{
                current1 = ++first1;
                current2 = first2;
                --d1;
            }
        }
    }
    return first1;
}

//����value��������n�εĵ�һ������
template<class ForwardIterator, class Integer, class T>
ForwardIterator search_n(ForwardIterator first, ForwardIterator last,
                         Integer count, const T& value){
    if(count <= 0)
        return first;
    else{
        first = find(first, last, value);
        while(first != last){
            Integer n = count - 1;
            ForwardIterator i = first;
            ++i;
            while(i != last && n != 0 && *i == value){
                ++i;
                --n;
            }
            if(n == 0)
                return first;
            else
                first = find(i, last, value);
        }
        return last;
    }
}

template<class ForwardIterator, class Integer, class T, class BinaryPredicate>
ForwardIterator search_n(ForwardIterator first, ForwardIterator last,
                         Integer count, const T& value,
                         BinaryPredicate binary_pred){
    if(count <= 0)
        return first;
    else{
        while(first != last){//�൱��find
            if(binary_pred(*first, value))
                break;
            ++first;
        }
        while(first != last){
            Integer n = count - 1;
            ForwardIterator i = first;
            ++i;
            while(i != last && n != 0 && binary_pred(*i, value)){
                ++i;
                --n;
            }
            if(n == 0)
                return first;
            else{
                while(i != last){
                    if(binary_pred(*i, value))
                        break;
                    ++i;
                }
                first = i;
            }
        }
        return last;
    }
}

//���������г���Ϊlast1 - first1������
template<class ForwardIterator1, class ForwardIterator2>
ForwardIterator2 swap_ranges(ForwardIterator1 first1, ForwardIterator1 last1,
                             ForwardIterator2 first2){
    for(; first1 != last1; ++first1, ++first2){
        iter_swap(first1, first2);
    }
    return first2;
}

//�������Էº������ã����ص�result�У���Щ������for each
template<class InputIterator, class OutputIterator, class UnaryOperation>//unary i.e. һԪ��
OutputIterator transform(InputIterator first, InputIterator last,
                         OutputIterator result, UnaryOperation op){
    for(; first != last; ++first, ++result){
        *result = op(*first);
    }
    return result;
}

template<class InputIterator1, class InputIterator2,
         class OutputIterator, class BinaryOperation>
OutputIterator transform(InputIterator1 first1, InputIterator1 last1,
                         InputIterator2 first2, OutputIterator result,
                         BinaryOperation binary_op){
    for(; first1 != last1; ++first1, ++ first2, ++ result){
        *result = binary_op(*first1, *first2);
    }
    return result;
}

//ɾ�����������е��ظ�Ԫ�أ����ı�size���в���
template<class InputIterator, class ForwardIterator>
ForwardIterator _unique_copy(InputIterator first, InputIterator last,
                             ForwardIterator result, forward_iterator_tag){
    *result = *first;
    while(++first != last){
        if(*result != *first)
            *++result = *first;
    }
    return ++result;
}

template<class InputIterator, class OutputIterator, class T>
OutputIterator _unique_copy(InputIterator first, InputIterator last,
                            OutputIterator result, T*){
    T value = *first;
    *result = value;
    while(++first != last){
        if(value != *first){
            value = *first;
            *++result = value;
        }
    }
    return ++result;
}

template<class InputIterator, class OutputIterator>
inline OutputIterator _unique_copy(InputIterator first, InputIterator last,
                                   OutputIterator result, output_iterator_tag){
    return _unique_copy(first, last, result, value_type(first));
}


template<class InputIterator, class OutputIterator>
inline OutputIterator unique_copy(InputIterator first, InputIterator last,
                                  OutputIterator result){
    if(first == last)return result;
    return _unique_copy(first, last, result, iterator_category(result));
}

template<class ForwardIterator>
ForwardIterator unique(ForwardIterator first, ForwardIterator last){
    first = adjacent_find(first, last);
    return unique_copy(first, last, first);
}

//���ҵ�һ����С��value��Ԫ��
//�����汾�Ĳ������random����������� + n��������������ֻ��ͨ��distance��advanceʵ��
//��������λ�Ʋ������ж��ֲ��ҵķ�ʽ��ֵ��ѧϰ
template<class ForwardIterator, class T, class Distance>
ForwardIterator _lower_bound(ForwardIterator first, ForwardIterator last,
                             const T& value, Distance*, forward_iterator_tag){
    Distance len = distance(first, last);
    Distance half;
    ForwardIterator middle;
    while(len > 0){
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if(*middle < value){
            first = middle;
            ++first;
            len = len - half - 1;
        }
        else{
            len = half;
        }
    }
    return first;
}

template<class RandomAccessIterator, class T, class Distance>
RandomAccessIterator _lower_bound(RandomAccessIterator first, RandomAccessIterator last,
                                  const T& value, Distance*, random_access_iterator_tag){
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle;
    while(len > 0){
        half = len >> 1;
        middle = first + half;
        if(*middle < value){
            first = middle + 1;
            len = len - half - 1;
        }
        else{
            len = half;
        }
    }
    return first;

}

template<class ForwardIterator, class T>
inline ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last,
                                   const T& value){
    return _lower_bound(first, last, value, distance_type(first),
                        iterator_category(first));
}


template<class ForwardIterator, class T, class Compare, class Distance>
ForwardIterator _lower_bound(ForwardIterator first, ForwardIterator last, const T& value,
                             Compare comp, Distance*, forward_iterator_tag){
    Distance len = distance(first, last);
    Distance half;
    ForwardIterator middle;
    while(len > 0){
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if(comp(*middle, value)){
            first = middle;
            ++first;
            len = len - half - 1;
        }
        else{
            len = half;
        }
    }
    return first;
}

template<class RandomAccessIterator, class T, class Compare, class Distance>
RandomAccessIterator _lower_bound(RandomAccessIterator first, RandomAccessIterator last, const T& value,
                                  Compare comp, Distance*, random_access_iterator_tag){
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle;
    while(len > 0){
        half = len >> 1;
        middle = first + half;
        if(comp(*middle, value)){
            first = middle + 1;
            len = len - half - 1;
        }
        else{
            len = half;
        }
    }
    return first;

}

template<class ForwardIterator, class T, class Compare>
inline ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last,
                                   const T& value, Compare comp){
    return _lower_bound(first, last, value, comp, distance_type(first),
                        iterator_category(first));
}


//���Ҵ���value�ĵ�һ��������(�п�����last)
template<class ForwardIterator, class T, class Distance>
ForwardIterator _upper_bound(ForwardIterator first, ForwardIterator last,
                             const T& value, Distance*, forward_iterator_tag){
    Distance len = distance(first, last);
    Distance half;
    ForwardIterator middle;
    while(len > 0){
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if(value < *middle)
            len = half;
        else{
            first = middle;
            ++first;
            len = len - half - 1;
        }
    }
    return first;
}

template<class RandomAccessIterator, class T, class Distance>
RandomAccessIterator _upper_bound(RandomAccessIterator first, RandomAccessIterator last,
                                  const T& value, Distance*, random_access_iterator_tag){
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle;
    while(len > 0){
        half = len >> 1;
        middle = first;
        middle = first + half;
        if(value < *middle)
            len = half;
        else{
            first = middle + 1;
            len = len - half - 1;
        }
    }
    return first;
}

template<class ForwardIterator, class T>
inline ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last,
                                   const T& value){
    return _upper_bound(first, last, value, distance_type(first),
                        iterator_category(first));
}


template<class ForwardIterator, class T, class Compare, class Distance>
ForwardIterator _upper_bound(ForwardIterator first, ForwardIterator last, const T& value,
                             Compare comp, Distance*, forward_iterator_tag){
    Distance len = distance(first, last);
    Distance half;
    ForwardIterator middle;
    while(len > 0){
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if(comp(value, *middle))
            len = half;
        else{
            first = middle;
            ++first;
            len = len - half - 1;
        }
    }
    return first;
}

template<class RandomAccessIterator, class T, class Compare, class Distance>
RandomAccessIterator _upper_bound(RandomAccessIterator first, RandomAccessIterator last, const T& value,
                                  Compare comp, Distance*, random_access_iterator_tag){
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle;
    while(len > 0){
        half = len >> 1;
        middle = first;
        middle = first + half;
        if(comp(value, *middle))
            len = half;
        else{
            first = middle + 1;
            len = len - half - 1;
        }
    }
    return first;
}

template<class ForwardIterator, class T, class Compare>
inline ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last,
                                   const T& value, Compare comp){
    return _upper_bound(first, last, value, comp, distance_type(first),
                        iterator_category(first));
}


//��Ҫ!(value < *i)����Ϊ���ܳ���13579�����6
template<class ForwardIterator, class T>
bool binary_search(ForwardIterator first, ForwardIterator last,
                   const T& value){
    ForwardIterator i = lower_bound(first, last, value);
    return i != last && !(value < *i);
}

template<class ForwardIterator, class T, class Compare>
bool binary_search(ForwardIterator first, ForwardIterator last,
                   const T& value, Compare comp){
    ForwardIterator i = lower_bound(first, last, value, comp);
    return i != last && !(value < *i);
}

template<class BidirectionalIterator>
bool next_permutation(BidirectionalIterator first,
                      BidirectionalIterator last){
    if(first == last)
        return false;
    BidirectionalIterator i = first;
    ++i;
    if(i == last)
        return false;
    i = last;
    --i;
    for(;;){
        BidirectionalIterator ii = i;
        --i;
        if(*i < *ii){
            BidirectionalIterator j = last;
            while(!(*i < *--j));
            iter_swap(i, j);
            reverse(ii, last);
            return true;
        }
        if(i == first){
            reverse(first, last);
            return false;
        }
    }
}

template<class BidirectionalIterator>
bool prev_permutation(BidirectionalIterator first,
                      BidirectionalIterator last){
    if(first == last)
        return false;
    BidirectionalIterator i = first;
    ++i;
    if(i == last)
        return false;
    i = last;
    --i;
    for(;;){
        BidirectionalIterator ii = i;
        --i;
        if(*ii < *i){
            BidirectionalIterator j = last;
            while(!(*--j < *i));
            iter_swap(i, j);
            reverse(ii, last);
            return true;
        }
        if(i == first){
            reverse(first, last);
            return false;
        }
    }

}

//equal rangeָ��:firstָ���һ��value��secondָ�����һ��value����ĵ�һ��Ԫ��
//����������Ҳ������ͷ��ص�һ������value��Ԫ��
//equal range������Ӧ���������
template<class RandomAccessIterator, class T, class Distance>
pair<RandomAccessIterator, RandomAccessIterator>
_equal_range(RandomAccessIterator first, RandomAccessIterator last,
             const T& value, Distance*, random_access_iterator_tag){
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle, left, right;
    while(len > 0){
        half = len >> 1;
        middle = first + half;
        if(*middle < value){
            first = middle + 1;
            len = len - half - 1;
        }
        else if(value < *middle){
            len = half;
        }
        else{
            left = lower_bound(first, middle, value);
            right = upper_bound(++middle, first + len, value);
            return pair<RandomAccessIterator, RandomAccessIterator>(left, right);
        }
    }

    return pair<RandomAccessIterator, RandomAccessIterator>(first, first);
}

template<class ForwardIterator, class T, class Distance>
pair<ForwardIterator, ForwardIterator>
_equal_range(ForwardIterator first, ForwardIterator last,
             const T& value, Distance*, forward_iterator_tag){
    Distance len = distance(first, last);
    Distance half;
    ForwardIterator middle, left, right;
    while(len > 0){
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if(*middle < value){
            first = middle;
            ++first;
            len = len - half - 1;
        }
        else if(value < *middle){
            len = half;
        }
        else{
            left = lower_bound(first, middle, value);
            advance(first, len);
            right = upper_bound(++middle, first, value);
            return pair<ForwardIterator, ForwardIterator>(left, right);
        }
    }
    return pair<ForwardIterator, ForwardIterator>(first, first);
}

template<class ForwardIterator, class T>
inline pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first, ForwardIterator last, const T& value){
    return _equal_range(first, last, value, distance_type(first), iterator_category(first));
}


template<class RandomAccessIterator, class T, class Compare, class Distance>
pair<RandomAccessIterator, RandomAccessIterator>
_equal_range(RandomAccessIterator first, RandomAccessIterator last,
             const T& value, Compare comp, Distance*, random_access_iterator_tag){
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle, left, right;
    while(len > 0){
        half = len >> 1;
        middle = first + half;
        if(comp(*middle, value)){
            first = middle + 1;
            len = len - half - 1;
        }
        else if(comp(value, *middle)){
            len = half;
        }
        else{
            left = lower_bound(first, middle, value, comp);
            right = upper_bound(++middle, first + len, value, comp);
            return pair<RandomAccessIterator, RandomAccessIterator>(left, right);
        }
    }

    return pair<RandomAccessIterator, RandomAccessIterator>(first, first);
}

template<class ForwardIterator, class T, class Compare, class Distance>
pair<ForwardIterator, ForwardIterator>
_equal_range(ForwardIterator first, ForwardIterator last, const T& value,
             Compare comp, Distance*, forward_iterator_tag){
    Distance len = distance(first, last);
    Distance half;
    ForwardIterator middle, left, right;
    while(len > 0){
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if(comp(*middle, value)){
            first = middle;
            ++first;
            len = len - half - 1;
        }
        else if(comp(value, *middle)){
            len = half;
        }
        else{
            left = lower_bound(first, middle, value, comp);
            advance(first, len);
            right = upper_bound(++middle, first, value, comp);
            return pair<ForwardIterator, ForwardIterator>(left, right);
        }
    }
    return pair<ForwardIterator, ForwardIterator>(first, first);
}

template<class ForwardIterator, class T, class Compare>
inline pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first, ForwardIterator last,
            const T& value, Compare comp){
    return _equal_range(first, last, value, comp, distance_type(first),
                        iterator_category(first));
}



#endif // ALGO_H_INCLUDED
