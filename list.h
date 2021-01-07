#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED


#include"allocator.h"
#include"algorithm.h"
//������list�ĵ�����
template<class T>
struct _list_node{
    _list_node<T> * prev;
    _list_node<T> * next;
    T data;

};

//����ÿ���ڵ�Ľṹ��������һ������һ���Ľڵ��ַ�ͱ��ڵ������
//���ǵ����������ݣ����䵱ָ�������
//���list�Ƿ������� list -> _list_iterator -> _list_node
//iterator��node֮��Ӧ����.���ӣ���Ϊ����ĺ����õ�iterator������ָ��
//����iterator.node��prev��next֮��Ӧ����->����
//��Ϊlink_type��һ��ָ��

template<class T, class Ref, class Ptr>//Ref i.e. reference Ptr i.e. pointer
struct _list_iterator{
    typedef _list_iterator<T, T&, T*> iterator;
    typedef _list_iterator<T, Ref, Ptr> self;

    typedef bidirectional_iterator_tag iterator_category;
    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef _list_node<T>* link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    link_type node;
//���캯��                                                                                                                                                             ��
    _list_iterator(link_type x):node(x){}//��node����
    _list_iterator(){}//�޲ι��캯��
    _list_iterator(const iterator& x):node(x.node){}//����������



    bool operator == (const self& x) const{return node == x.node;}
    bool operator != (const self& x) const{return node != x.node;}
    reference operator*() const {return (*node).data;}
    pointer operator -> () const {return & (operator*());}
    self& operator++(){
        node = (link_type)((*node).next);
        return *this;
    }
    self operator++(int){
        self tmp = *this;
        ++*this;
        return tmp;
    }
    self& operator--(){
        node = (link_type)((*node).prev);
        return *this;

    }
    self operator--(int){
        self tmp = *this;
        --*this;
        return tmp;
    }
};

template<class T, class Alloc = alloc>
class list{
    protected:
        typedef _list_node<T> list_node;
        typedef simple_alloc<list_node, Alloc> list_node_allocator;

    public:
        typedef list_node* link_type;
        typedef _list_iterator<T, T&, T*> iterator;
        typedef size_t size_type;
        typedef T& reference;
        typedef T* pointer;


        iterator insert(iterator position, const T& x){
            link_type tmp = create_node(x);
            tmp->next = position.node;
            tmp->prev = position.node->prev;
            (link_type(position.node->prev))->next = tmp;
            position.node->prev = tmp;
            return tmp;
        }

        iterator insert(iterator position, size_type n, const T& x){
            n++;
            while(--n){
                link_type tmp = create_node(x);
                tmp->next = position.node;
                tmp->prev = position.node->prev;
                (link_type(position.node->prev))->next = tmp;
                position.node->prev = tmp;
            }
            return position.node->prev;
        }



    protected:
        link_type node;
        link_type get_node(){ return list_node_allocator::allocate();}//����ռ�
        //�ڽӿ�������һ���޲ι��캯��������һ������������ռ�ռ�Ŀռ�
        void put_node(link_type p){ list_node_allocator::deallocate(p);}

        link_type create_node(const T& x){
            link_type p = get_node();
            try{
                construct(&p->data, x);
            }
            catch(...){
                put_node(p);//���ﲻ��destroy
                //��ΪֻҪcatch�˾�˵��constructʧ����
            }
            return p;
        }
        void destroy_node(link_type p){
            destroy(&p->data);
            put_node(p);
        }

        void empty_initialize(){
            node = get_node();
            node->next = node;
            node->prev = node;
        }

        fill_initialize(size_type n, const T& value){
            empty_initialize();
            try{
                insert(begin(), n, value);
            }
            catch(...){
                clear();
                put_node(node);
                throw;
            }
        }

    public:
        iterator begin() {return (link_type)(node->next);}
        iterator end()   {return node;}
        bool empty() const {return node->next == node;}
        size_type size() const {
            size_type result = 0;
            distance(node, node->next, result);//��iterator�ļ���
            return result;
        }
        //�ṩ���ֹ��캯��
        //�ճ�ʼ������empty initialize
        //n��value��ʼ����n��default����˳��Ϊfill initialize->empty initialize + insert
        //����ֱ����insert����Ϊlist�����������ռ��ϴ洢��
        //insert��Ϊ�򵥣����÷���


        list(){empty_initialize();}

        list(size_type n, const T& value){fill_initialize(n, value);}
        list(int n, const T& value){      fill_initialize(n, value);}
        list(long n, const T& value){     fill_initialize(n, value);}
        explicit list(size_type n){       fill_initialize(n,T());}

        void clear(){
            link_type cur = (link_type) node->next;
            while(cur != node){
                link_type tmp = cur;
                cur = (link_type) cur->next;
                destroy_node(tmp);
            }
            node->next = node;
            node->prev = node;
        }

        ~list(){
            clear();
            put_node(node);
        }


        reference front() {return *begin();}
        reference back()  {return *(--end());}




        void push_front(const T& x){insert(begin(), x);}
        void push_back(const T& x){insert(end(), x);}


        iterator erase(iterator position){
            link_type next_node = link_type(position.node->next);
            link_type prev_node = linke_type(position.node->prev);
            prev_node->next = next_node;
            next_node->prev = prev_node;
            destroy_node(position.node);
            return iterator(next_node);
        }

        void pop_front(){erase(begin());}
        void pop_back(){erase(--end());}


        void remove(const T& value){//ɾ��data = value��Ԫ��
            iterator first = begin();
            iterator last = end();
            while(first != last()){
                if(*first == value){
                    iterator tmp = first;
                    ++first;
                    erase(tmp);
                }
                else ++first;
            }
        }

        void unique(){//ɾ�����ڽڵ�data�ظ���Ԫ��
            iterator first = begin();
            iterator last = end();
            if(first == last)
                return;
            iterator next = first;
            while(++first != last){
                if(*first == *next)
                    erase(next);
                else
                    first = next;
                next = first;
            }
        }
    protected:
        //(*temp).next �� temp->next��û��Ч���ϵ�����
        //transfer������[first,last)֮��������Ƶ�position֮ǰ
        //֮ǰ��FB->F->LB->L // PB->P
        //��������PB->F->LB->P // FB->L
        //�ȼ���position֮ǰ�������� first��last֮������ݱ�ɾ����
        void transfer(iterator position, iterator first, iterator last){
            if(position != last){
                (link_type(last.node->prev))->next = position.node;
                (link_type(first.node->prev))->next = last.node;
                (link_type(position.node)->prev)->next = first.node;
                link_type tmp = link_type(position.node)->prev;
                position.node->prev = last.node->prev;
                last.node->prev = first.node->prev;
                first.node->prev = tmp;
            }
        }
    public:
        //��x�����Ƶ�position֮ǰ
        //����Ϊʲô�����x����ָ������
        //��Ϊsplice��ԭ��:transfer�Ѿ�Ҫ����x�ĵ�����������iterator
        //��iterator��_list_iterator<T, T&, T*> iterator�͵�
        void splice(iterator position, list& x){
            if(!x.empty())
                transfer(position, x.begin(), x.end());
        }

        //��i�Ƶ�position֮ǰ
        void splice(iterator position, list&, iterator i){
            iterator j = i;
            ++j;
            if(position == i || position == j)
                return;
            transfer(position, i, j);
        }
        //��[first,last)�Ƶ�position֮ǰ
        void splice(iterator position, list&, iterator first, iterator last){
            if(first != last)
                transfer(position, first, last);
        }

        void merge(list& x){//��x�ϲ���this������Ҫ��x��this�������ź����
            iterator first1 = begin();
            iterator last1 = end();
            iterator first2 = x.begin();
            iterator last2 = x.end();
            while(first1 != last1 && first2 != last2){
                if(*first2 < *first1){
                    iterator next = first2;
                    transfer(first1, first2, ++next);
                    first2 = next;
                }
                else
                    ++first1;
            }
            if(first2 != last2)
                transfer(last1, first2, last2);
        }

        void reverse(){//��ת����
            if(node->next == node || link_type(node->next)->next == node)
                return;//�������ֻ��һ��Ԫ��
            iterator first = begin();
            ++first;
            while(first != end()){
                iterator old = first;
                ++first;
                transfer(begin(), old, first);
                //һ��һ��Ų��beginǰ�棬ÿ�ε���transfer�� begin����仯
            }
        }
        //list������ʹ��algorithm���sort����Ϊsortֻ����randomaccessiterator
/*
        void sort(){
            if(node->next == node || link_type(node->next)->next == node)
                return;
            list<T, Alloc> carry;
            list<T, Alloc> counter[64];
            int fill = 0;
            while(!empty()){
                carry.splice(carry.begin(), *this, begin());
                int i = 0;
                while(i < fill && !counter[i].empty()){
                    counter[i].merge(carry);
                    carry.swap(counter[i + 1]);
                }
                carry.swap(counter[i]);
                if(i == fill)
                    ++fill;
            }
            for(int i = 1;i < fill; ++i){
                counter[i].merge(counter[i - 1]);
            }
            swap(counter[fill - 1]);
        }

*/
//sort��������һ��swap��û���ҵ�����λ��
};

#endif // LIST_H_INCLUDED
