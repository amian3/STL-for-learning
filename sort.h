#ifndef SORT_H_INCLUDED
#define SORT_H_INCLUDED

#include"iterator.h"
#include"type_traits.h"
#include"algobase.h"
//��������ʵ����̫��Ҫ�ˣ�����Ұ��������ֳ�һ���ļ���д

//������
//push�����ã���hole��ʼ������hole�ķ�֧�е���������
//����

template<class RandomAccessIterator, class Distance, class T>
void _push_heap(RandomAccessIterator first, Distance holeIndex,
                Distance topIndex, T value){
    Distance parent = (holeIndex - 1) / 2;
    while(holeIndex > topIndex && *(first + parent) < value){
        *(first + holeIndex) = *(first + parent);
        holeIndex = parent;
        parent = (holeIndex - 1) / 2;
    }
    *(first + holeIndex) = value;

}

template<class RandomAccessIterator, class Distance, class T>
inline void _push_heap_aux(RandomAccessIterator first, RandomAccessIterator last,
                           Distance*, T*){
    _push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)));
}


template<class RandomAccessIterator>
inline void push_heap(RandomAccessIterator first,
                      RandomAccessIterator last){
//����������ôд����Ϊpop heap��ʱ��Ҳ��push heap
//������д��inline����
//    typename iterator_traits<RandomAccessIterator>::difference_type holeIndex = (last - first) - 1;
//    typename iterator_traits<RandomAccessIterator>::difference_type topIndex = 0;
//    typename iterator_traits<RandomAccessIterator>::value_type value = *(last - 1);
//    typename iterator_traits<RandomAccessIterator>::difference_type parent = (holeIndex - 1) / 2;
    _push_heap_aux(first, last, distance_type(first), value_type(first));

}
//adjust���������������Լ��Ĳ��֣��ٵ���push
//�Լ��Ĳ����Ǹ����ݹ��̣�����ȫ���ҵ��׶˵Ľϴ�Ԫ��
//��push���value�����׶ˣ�Ȼ�������ݿ�value�Ƿ�̫����
//������̱Ƚ���֣���Ϊԭʼ���㷨һ�����ݵ�ָ��λ�þ͹���
//�����뵽��ԭ�������ȡ�����ݹ��̣��Ǿ�Ҫ��ÿ�������м���ÿ������value���ж�
//����Ҳ�������ʱ�䣿
template<class RandomAccessIterator, class Distance, class T>
void _adjust_heap(RandomAccessIterator first, Distance holeIndex,
                  Distance len, T value){
    Distance topIndex = holeIndex;
    Distance secondChild = 2 * holeIndex + 2;//������
    while(secondChild < len){
        //���ǴӶ����������ݹ飬����һ��pop��Ӧ��������
        if(*(first + secondChild) < *(first + (secondChild - 1)))
            secondChild--;//��������child�Ĵ�С���������������¼�������
        *(first + holeIndex) = *(first + secondChild);
        holeIndex = secondChild;
        secondChild = 2 * (secondChild + 1);
    }
    if(secondChild == len){//������Ϊlen�������������β�ˣ�β��û��Ԫ��
        *(first + holeIndex) = *(first + (secondChild - 1));
        holeIndex = secondChild - 1;
    }

    _push_heap(first, holeIndex, topIndex, value);

}

template<class RandomAccessIterator, class T, class Distance>
inline void _pop_heap(RandomAccessIterator first, RandomAccessIterator last,
                      RandomAccessIterator result, T value, Distance*){
    *result = *first;//�Ѷ���Ԫ���ƶ���β��
    _adjust_heap(first, Distance(0), Distance(last - first), value);
}

template<class RandomAccessIterator, class T>
inline void _pop_heap_aux(RandomAccessIterator first,
                          RandomAccessIterator last, T*){
    _pop_heap(first, last - 1, last - 1, T(*(last - 1)), distance_type(first));
}

template<class RandomAccessIterator>
inline void pop_heap(RandomAccessIterator first,
                     RandomAccessIterator last){
    _pop_heap_aux(first, last, value_type(first));

}

template<class RandomAccessIterator>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last){
    while(last - first > 1)
        pop_heap(first, last--);
}

template<class RandomAccessIterator, class T, class Distance>
void _make_heap(RandomAccessIterator first, RandomAccessIterator last,
                T*, Distance*){
    if(last - first < 2)
        return;
    Distance len = last - first;
    Distance parent = (len - 2) / 2;
    while(true){//���ѭ����ʵ���Ǵ�0��n/2�ı�����ʵ�ֽ��ѹ���
        _adjust_heap(first, parent, len, T(*(first + parent)));
        if(parent == 0)
            return;
        parent--;
    }
}

template<class RandomAccessIterator>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last){
    _make_heap(first, last, value_type(first), distance_type(first));
}

//pop���𽨺ö��Ժ�ĵ�������
//push��������
//make��sort���û��ӿ�

//�������Լ��༭��heap sort�汾
//����汾��������Ҫ���������ݣ�ֻ��Ҫһ�����ݾ͹���
/*
template<class RandomAccessIterator, class Distance, class T>
void _adjust_heap(RandomAccessIterator first, Distance parent,
                  Distance len, T*){
    T value = *(first + parent);
    Distance child = 2 * parent + 1;
    while(child < len){
        if(child + 1 < len && *(first + child) < *(first + child + 1))
            child++;
        if(value >= *(first +child))
            break;
        *(first + parent) = *(first + child);
        parent = child;
        child = 2 * child + 1;
    }
    *(first + parent) = value;
}

template<class RandomAccessIterator, class T, class Distance>
void _make_heap(RandomAccessIterator first, RandomAccessIterator last,
                T*, Distance*){

    Distance len = last - first;
    Distance parent = len / 2;
    while(parent >= 0){
        _adjust_heap(first, parent, len, value_type(first));
        parent--;
    }
}

template<class RandomAccessIterator>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last){
    _make_heap(first, last, value_type(first), distance_type(first));
}

template<class RandomAccessIterator, class T, class Distance>
void _sort_heap(RandomAccessIterator first, RandomAccessIterator last,
                T*, Distance*){
    for(Distance i = last - first - 1;i > 0; i--){
        T value = *(first + i);
        *(first + i) = *(first);
        *(first) = value;
        _adjust_heap(first, Distance(0), i,value_type(first));
    }
}

template<class RandomAccessIterator>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last){
    _sort_heap(first, last, value_type(first), distance_type(first));

}
*/

template<class RandomAccessIterator, class Compare, class Distance, class T>
void _push_heap(RandomAccessIterator first, Compare comp, Distance holeIndex,
                Distance topIndex, T value){
    Distance parent = (holeIndex - 1) / 2;
    while(holeIndex > topIndex && comp(*(first + parent), value)){
        *(first + holeIndex) = *(first + parent);
        holeIndex = parent;
        parent = (holeIndex - 1) / 2;
    }
    *(first + holeIndex) = value;

}

template<class RandomAccessIterator, class Compare, class Distance, class T>
inline void _push_heap_aux(RandomAccessIterator first, RandomAccessIterator last,
                           Compare comp, Distance*, T*){
    _push_heap(first, comp, Distance((last - first) - 1), Distance(0), T(*(last - 1)));
}


template<class RandomAccessIterator, class Compare>
inline void push_heap(RandomAccessIterator first,
                      RandomAccessIterator last, Compare comp){
    _push_heap_aux(first, last, comp, distance_type(first), value_type(first));

}

template<class RandomAccessIterator, class Compare, class Distance, class T>
void _adjust_heap(RandomAccessIterator first, Compare comp, Distance holeIndex,
                  Distance len, T value){
    Distance topIndex = holeIndex;
    Distance secondChild = 2 * holeIndex + 2;
    while(secondChild < len){
        if(comp(*(first + secondChild), *(first + (secondChild - 1))))
            secondChild--;
        *(first + holeIndex) = *(first + secondChild);
        holeIndex = secondChild;
        secondChild = 2 * (secondChild + 1);
    }
    if(secondChild == len){
        *(first + holeIndex) = *(first + (secondChild - 1));
        holeIndex = secondChild - 1;
    }

    _push_heap(first, comp, holeIndex, topIndex, value);

}

template<class RandomAccessIterator, class Compare, class T, class Distance>
inline void _pop_heap(RandomAccessIterator first, RandomAccessIterator last,
                      RandomAccessIterator result, Compare comp, T value, Distance*){
    *result = *first;
    _adjust_heap(first, comp, Distance(0), Distance(last - first), value);
}

template<class RandomAccessIterator, class Compare, class T>
inline void _pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last,
                          Compare comp, T*){
    _pop_heap(first, last - 1, last - 1, comp, T(*(last - 1)), distance_type(first));
}

template<class RandomAccessIterator, class Compare>
inline void pop_heap(RandomAccessIterator first,
                     RandomAccessIterator last, Compare comp){
    _pop_heap_aux(first, last, comp, value_type(first));

}

template<class RandomAccessIterator, class Compare>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp){
    while(last - first > 1)
        pop_heap(first, last--, comp);
}

template<class RandomAccessIterator, class Compare, class T, class Distance>
void _make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp,
                T*, Distance*){
    if(last - first < 2)
        return;
    Distance len = last - first;
    Distance parent = (len - 2) / 2;
    while(true){
        _adjust_heap(first, comp, parent, len, T(*(first + parent)));
        if(parent == 0)
            return;
        parent--;
    }
}

template<class RandomAccessIterator, class Compare>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp){
    _make_heap(first, last, comp, value_type(first), distance_type(first));
}





//��������,N^2���Ӷ�
//��value���뵽lastΪβ�������У�����˳��
template<class RandomAccessIterator, class T>
void _unguarded_linear_insert(RandomAccessIterator last, T value){
    RandomAccessIterator next = last;
    --next;
    while(value < *next){
        *last = *next;
        last = next;
        --next;
    }
    *last = value;
}

template<class RandomAccessIterator, class T>
inline void _linear_insert(RandomAccessIterator first, RandomAccessIterator last, T*){
    T value = *last;
    if(value < *first){
        copy_backward(first, last, last + 1);
        *first = value;
    }
    else
        _unguarded_linear_insert(last, value);
}

template<class RandomAccessIterator>
void _insertion_sort(RandomAccessIterator first, RandomAccessIterator last){
    if(first == last)
        return;
    for(RandomAccessIterator i = first + 1; i != last; ++i){
        _linear_insert(first, i, value_type(first));
    }
}

template<class RandomAccessIterator, class Compare, class T>
void _unguarded_linear_insert(RandomAccessIterator last, Compare comp, T value){
    RandomAccessIterator next = last;
    --next;
    while(comp(value,*next)){
        *last = *next;
        last = next;
        --next;
    }
    *last = value;
}

template<class RandomAccessIterator, class Compare, class T>
inline void _linear_insert(RandomAccessIterator first, RandomAccessIterator last,
                           Compare comp, T*){
    T value = *last;
    if(comp(value,*first)){
        copy_backward(first, last, last + 1);
        *first = value;
    }
    else
        _unguarded_linear_insert(last, comp, value);
}

template<class RandomAccessIterator, class Compare>
void _insertion_sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp){
    if(first == last)
        return;
    for(RandomAccessIterator i = first + 1; i != last; ++i){
        _linear_insert(first, i, comp, value_type(first));
    }
}



const int _stl_threshold = 16;

//partial sortӦ�ö����򣬰���СԪ�طŵ�first��middle֮�䣬����Ĳ���֤˳��
//���pop��Ƶĺܾ�����i����Ҫ��first��middle֮��Ҳ���Խ���
//���������Ҫ��Ϊsort���ö�����ʱ�Ľӿ�
template<class RandomAccessIterator, class T>
void _partial_sort(RandomAccessIterator first, RandomAccessIterator middle,
                   RandomAccessIterator last, T*){
    make_heap(first, middle);
    for(RandomAccessIterator i = middle; i < last; ++i){
        if(*i < *first)
            _pop_heap(first, middle, i, T(*i), distance_type(first));
    }
    sort_heap(first, middle);
}

template<class RandomAccessIterator>
inline void partial_sort(RandomAccessIterator first, RandomAccessIterator middle,
                         RandomAccessIterator last){
    _partial_sort(first, middle, last, value_type(first));
}


template<class RandomAccessIterator, class Compare, class T>
void _partial_sort(RandomAccessIterator first, RandomAccessIterator middle,
                   RandomAccessIterator last, Compare comp, T*){
    make_heap(first, middle, comp);
    for(RandomAccessIterator i = middle; i < last; ++i){
        if(*i < *first)
            _pop_heap(first, middle, i, comp, T(*i), distance_type(first));
    }
    sort_heap(first, middle, comp);
}

template<class RandomAccessIterator, class Compare>
inline void partial_sort(RandomAccessIterator first, RandomAccessIterator middle,
                         RandomAccessIterator last, Compare comp){
    _partial_sort(first, middle, last, comp, value_type(first));
}



template<class T>
inline const T& _median(const T& a, const T& b, const T& c){
    if(a < b)
        if(b < c)
            return b;
        else if(a < c)
            return c;
        else
            return a;
    else if(a < c)
        return a;
    else if(b < c)
        return c;
    else
        return b;
}
//�ָ�ɴ���pivot��С�ڵ�������
//�û���Ҫ���ã�Ҫ���������û����Խ�紦��
//��Ҳ��sort���õĺ���
template<class RandomAccessIterator, class T>
RandomAccessIterator _unguarded_partition(RandomAccessIterator first,
                                          RandomAccessIterator last,
                                          T pivot){
    while(true){
        while(*first < pivot)
            ++first;
        --last;
        while(pivot < *last)
            --last;
        if(!(first < last))
            return first;//���ش�����ĵ�һ������
        iter_swap(first, last);
        ++first;
    }
}

template<class RandomAccessIterator, class T, class Size>
void _introsort_loop(RandomAccessIterator first, RandomAccessIterator last,
                     T*, Size depth_limit){
    while(last - first > _stl_threshold){
        if(depth_limit == 0){
            //����ָ���࣬���ö�����
            partial_sort(first, last, last);
           // make_heap(first, last);
           // sort_heap(first, last);
            return;
        }
        --depth_limit;
        RandomAccessIterator cut = _unguarded_partition
        (first, last, T(_median(*first, *(first + (last - first) / 2), *(last - 1))));
        //������̺ʹ�ͳ��quicksort����̫һ��
        //�����Ŀ����õ���_unguarded_partition(first, last, *(first))
        _introsort_loop(cut, last, value_type(first), depth_limit);
        last = cut;//������ֻ��cut��last���Եݹ�
        //��������first��cut�ĵݹ�ŵ�����һ��ѭ��
        //��������һ����first, cut��depth����ԭ��cut last��ͬ
        //����һ����������Ҳ�����׵���heap sort
    }
}

template<class RandomAccessIterator, class T>
void _unguarded_insertion_sort_aux(RandomAccessIterator first,
                                   RandomAccessIterator last, T*){
    for(RandomAccessIterator i = first;i != last; ++i){
        _unguarded_linear_insert(i, T(*i));
        //����д��ȫ������N^2���Ӷȣ���ΪС�鶼��Ԥ���źöӵ���
        //���insert�ƶ���Զ�ľ���Ҳ���ᳬ��16
    }
}

template<class RandomAccessIterator>
inline void _unguarded_insertion_sort(RandomAccessIterator first,
                                      RandomAccessIterator last){
    _unguarded_insertion_sort_aux(first, last, value_type(first));

}

template<class RandomAccessIterator>
void _final_insertion_sort(RandomAccessIterator first,
                           RandomAccessIterator last){
    if(last - first > _stl_threshold){
        _insertion_sort(first, first + _stl_threshold);
        //ע��introsort���룬�����������last - first > 16
        //��ô��һ��Ԥ���鳤��һ���ǵ���16��(��Ϊ��ݹ�ŵ���һ��ѭ�������˼��)
        //���Կ��������ֿ�����
        _unguarded_insertion_sort(first + _stl_threshold, last);
        //
    }
    else
        _insertion_sort(first, last);
        //����С��16������ֱ�Ӳ�������
}

//�ҳ�n����������λ��������n = 20 = 16 + 4 = 0001 0100 k = 4
template<class Size>
inline Size _lg(Size n){
    Size k;
    for(k = 0; n > 1; n >>= 1) ++k;
    return k;
}
//��Ԥ����������ÿ��С��
template<class RandomAccessIterator>
inline void sort(RandomAccessIterator first, RandomAccessIterator last){
    if(first != last){
        _introsort_loop(first, last, value_type(first), _lg(last - first) * 2);
        _final_insertion_sort(first, last);
    }
}

//�����ṩһ��ֻʹ�ÿ�������İ汾

/*
template<class RandomAccessIterator, class T>
void _quick_sort(RandomAccessIterator first, RandomAccessIterator last, T*){
    if(last - first <= 0)
        return;
    T key = *first;
    RandomAccessIterator i = first + 1, j = last - 1;
    while(true){
        //ע�����൱�ڴ�first + 1��unguarded partition
        //��Ҫ��first��ʼ������Ȼ�Ͳ�֪��*first�ᱻ���ڵ�������
        //Ҳ��û����first�ŵ��м���
        while(i < last && *i < key)++i;
        while(j > first && *j > key)--j;
        if(j - i <= 0)
            break;
        iter_swap(i, j);
    }
    iter_swap(first, j);
    _quick_sort(first, j, value_type(first));
    _quick_sort(j + 1, last, value_type(first));
}

template<class RandomAccessIterator>
inline void quick_sort(RandomAccessIterator first, RandomAccessIterator last){
    _quick_sort(first, last, value_type(first));

}
*/

template<class T, class Compare>
inline const T& _median(const T& a, const T& b, const T& c, Compare comp){
    if(comp(a, b))
        if(comp(b, c))
            return b;
        else if(comp(a, c))
            return c;
        else
            return a;
    else if(comp(a, c))
        return a;
    else if(comp(b, c))
        return c;
    else
        return b;
}

template<class RandomAccessIterator, class Compare, class T>
RandomAccessIterator _unguarded_partition(RandomAccessIterator first,
                                          RandomAccessIterator last,
                                          Compare comp, T pivot){
    while(true){
        while(comp(*first, pivot))
            ++first;
        --last;
        while(comp(pivot, *last))
            --last;
        if(!(first < last))
            return first;
        iter_swap(first, last);
        ++first;
    }
}

template<class RandomAccessIterator, class Compare, class T, class Size>
void _introsort_loop(RandomAccessIterator first, RandomAccessIterator last,
                     Compare comp, T*, Size depth_limit){
    while(last - first > _stl_threshold){
        if(depth_limit == 0){
            partial_sort(first, last, last, comp);
            return;
        }
        --depth_limit;
        RandomAccessIterator cut = _unguarded_partition
        (first, last, comp, T(_median(*first, *(first + (last - first) / 2), *(last - 1))));
        _introsort_loop(cut, last, comp, value_type(first), depth_limit);
        last = cut;
    }
}

template<class RandomAccessIterator, class Compare, class T>
void _unguarded_insertion_sort_aux(RandomAccessIterator first, RandomAccessIterator last,
                                   Compare comp, T*){
    for(RandomAccessIterator i = first;i != last; ++i){
        _unguarded_linear_insert(i, comp, T(*i));
    }
}

template<class RandomAccessIterator, class Compare>
inline void _unguarded_insertion_sort(RandomAccessIterator first, RandomAccessIterator last,
                                      Compare comp){
    _unguarded_insertion_sort_aux(first, last, comp, value_type(first));

}

template<class RandomAccessIterator, class Compare>
void _final_insertion_sort(RandomAccessIterator first, RandomAccessIterator last,
                           Compare comp){
    if(last - first > _stl_threshold){
        _insertion_sort(first, first + _stl_threshold, comp);
        _unguarded_insertion_sort(first + _stl_threshold, last, comp);
    }
    else
        _insertion_sort(first, last, comp);
}

template<class RandomAccessIterator, class Compare>
inline void sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp){
    if(first != last){
        _introsort_loop(first, last, comp, value_type(first), _lg(last - first) * 2);
        _final_insertion_sort(first, last, comp);
    }
}

template<class RandomAccessIterator, class T>
void _nth_element(RandomAccessIterator first, RandomAccessIterator nth,
                  RandomAccessIterator last, T*){
    while(last - first > 3){
        RandomAccessIterator cut = _unguarded_partition
        (first, last, T(_median(*first, *(first + (last - first) / 2), *(last - 1))));
        if(cut <= nth)
            first = cut;
        else
            last = cut;
    }
    _insertion_sort(first, last);
}

template<class RandomAccessIterator>
inline void nth_element(RandomAccessIterator first, RandomAccessIterator nth,
                        RandomAccessIterator last){
    _nth_element(first, nth, last, value_type(first));
}

template<class RandomAccessIterator, class T, class Compare>
void _nth_element(RandomAccessIterator first, RandomAccessIterator nth,
                  RandomAccessIterator last, Compare comp, T*){
    while(last - first > 3){
        RandomAccessIterator cut = _unguarded_partition
        (first, last, comp, T(_median(*first, *(first + (last - first) / 2), *(last - 1))));
        if(cut <= nth)
            first = cut;
        else
            last = cut;
    }
    _insertion_sort(first, last);
}

template<class RandomAccessIterator, class Compare>
inline void nth_element(RandomAccessIterator first, RandomAccessIterator nth,
                        RandomAccessIterator last, Compare comp){
    _nth_element(first, nth, last, comp, value_type(first));
}
#endif // SORT_H_INCLUDED
