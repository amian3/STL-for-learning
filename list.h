#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED


#include"allocator.h"
#include"algorithm.h"
//以下是list的迭代器
template<class T>
struct _list_node{
    _list_node<T> * prev;
    _list_node<T> * next;
    T data;

};

//这是每个节点的结构，存有上一个，下一个的节点地址和本节点的内容
//这是迭代器的内容，他充当指针的作用
//因此list是分三级的 list -> _list_iterator -> _list_node
//iterator和node之间应该用.连接，因为下面的函数用的iterator都不是指针
//但是iterator.node和prev和next之间应该用->连接
//因为link_type是一个指针

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
//构造函数                                                                                                                                                             吗
    _list_iterator(link_type x):node(x){}//用node构造
    _list_iterator(){}//无参构造函数
    _list_iterator(const iterator& x):node(x.node){}//迭代器构造



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
        link_type get_node(){ return list_node_allocator::allocate();}//分配空间
        //在接口类里有一个无参构造函数，构造一个数据类型所占空间的空间
        void put_node(link_type p){ list_node_allocator::deallocate(p);}

        link_type create_node(const T& x){
            link_type p = get_node();
            try{
                construct(&p->data, x);
            }
            catch(...){
                put_node(p);//这里不用destroy
                //因为只要catch了就说明construct失败了
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
            distance(node, node->next, result);//在iterator文件中
            return result;
        }
        //提供三种构造函数
        //空初始化调用empty initialize
        //n个value初始化和n个default调用顺序为fill initialize->empty initialize + insert
        //这里直接用insert是因为list并不是连续空间上存储的
        //insert较为简单，调用方便


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


        void remove(const T& value){//删除data = value的元素
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

        void unique(){//删除相邻节点data重复的元素
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
        //(*temp).next 和 temp->next并没有效果上的区别
        //transfer函数将[first,last)之间的内容移到position之前
        //之前是FB->F->LB->L // PB->P
        //操作后变成PB->F->LB->P // FB->L
        //等价于position之前加了内容 first到last之间的内容被删除了
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
        //把x整体移到position之前
        //关于为什么这里的x不用指定类型
        //因为splice的原型:transfer已经要求了x的迭代器类型是iterator
        //而iterator是_list_iterator<T, T&, T*> iterator型的
        void splice(iterator position, list& x){
            if(!x.empty())
                transfer(position, x.begin(), x.end());
        }

        //把i移到position之前
        void splice(iterator position, list&, iterator i){
            iterator j = i;
            ++j;
            if(position == i || position == j)
                return;
            transfer(position, i, j);
        }
        //把[first,last)移到position之前
        void splice(iterator position, list&, iterator first, iterator last){
            if(first != last)
                transfer(position, first, last);
        }

        void merge(list& x){//把x合并到this并排序，要求x和this本身都是排好序的
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

        void reverse(){//反转链表
            if(node->next == node || link_type(node->next)->next == node)
                return;//空链表或只有一个元素
            iterator first = begin();
            ++first;
            while(first != end()){
                iterator old = first;
                ++first;
                transfer(begin(), old, first);
                //一个一个挪到begin前面，每次调用transfer， begin都会变化
            }
        }
        //list本身不能使用algorithm里的sort，因为sort只接受randomaccessiterator
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
//sort函数里有一个swap还没有找到具体位置
};

#endif // LIST_H_INCLUDED
