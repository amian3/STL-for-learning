#ifndef DEQUE_H_INCLUDED
#define DEQUE_H_INCLUDED
#include"allocator.h"
#include"algorithm.h"

//deque也支持random访问
//但是它的的结构和vector有很大不同
//因为他是通过组连续，而不同组之间用链表连接的
//因此它的随机访问属实没vector快
inline size_t _deque_buf_size(size_t n, size_t sz){//size_t就是 long long unsigned int
    return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
    //如果n != 0返回n
    //如果n = 0但sz < 512 返回 512/sz
    //如果n = 0而且sz > 512 那返回1
}
template<class T, class Ref, class Ptr, size_t BufSiz>//BufSize在deque上有default参数
struct _deque_iterator{
    typedef _deque_iterator<T, T&, T*, BufSiz> iterator;
    typedef _deque_iterator<T, const T&, const T&, BufSiz> const_iterator;
    static size_t buffer_size(){return _deque_buf_size(BufSiz, sizeof(T));}
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T** map_pointer;

    typedef _deque_iterator self;
    T* cur;
    T* first;
    T* last;
    map_pointer node;

    void set_node(map_pointer new_node){
        node = new_node;
        first = *new_node;
        last = first + difference_type(buffer_size());
    }


    reference operator*() const{return *cur;}

    pointer operator->() const{return &(operator*());}

    difference_type operator-(const self& x)const{
        return difference_type(buffer_size()) * (node - x.node - 1)
        + (cur - first) + (x.last - x.cur);
        //x是在this之前的，因此顺序是
        //x.cur -> x.last -> other buffers -> this->first -> this->cur
    }

    self& operator++(){
        ++cur;
        if(cur == last){//跳到另一个缓冲区
            set_node(node + 1);
            cur = first;
        }
        return *this;
    }

    self& operator++(int){
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--(){
        if(cur == first){//跳到另一个缓冲区
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }

    self operator--(int){
        self tmp = *this;
        --*this;
        return tmp;
    }
    //这几个函数很好的体现出了vector随机访问的优势
    self& operator += (difference_type n){
        difference_type offset = n + (cur - first);
        if(offset >= 0 && offset < difference_type(buffer_size()))
            cur += n;
        else{
            difference_type node_offset =
            offset > 0 ? offset / difference_type (buffer_size())
            : -difference_type((-offset - 1) / buffer_size()) - 1;
            set_node(node + node_offset);
            cur = first + (offset - node_offset * difference_type(buffer_size()));
        }
        return *this;
    }

    self operator +(difference_type n)const{
        self tmp = *this;
        return tmp += n;
    }

    self& operator -= (difference_type n){
        return *this += -n;
    }

    self operator - (difference_type n)const{
        self tmp = *this;
        return tmp -= n;
    }

    reference operator[] (difference_type n)const{return *(*this + n);}

    bool operator==(const self& x)const{return cur == x.cur;}
    bool operator!=(const self& x)const{return !(*this == x);}
    bool operator<(const self* x)const{
        return (node == x.node) ? (cur < x.cur) :(node < x.node);
    }

};


template<class T, class Alloc = alloc, size_t BufSiz = 0>
class deque{
    public:
        typedef T value_type;
        typedef value_type* pointer;
        typedef T& reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        static size_t buffer_size(){return _deque_buf_size(BufSiz, sizeof(T));}
    public:
        typedef _deque_iterator<T, T&, T*, BufSiz> iterator;

    protected:
        typedef pointer* map_pointer;
        iterator start;
        iterator finish;
        map_pointer map;
        size_type map_size;
        //配置器应该分成缓冲区的和缓冲区指针的两类
        typedef simple_alloc<value_type, Alloc> data_allocator;
        typedef simple_alloc<pointer, Alloc> map_allocator;
        //注意map_allocator的数据类型并不是value type

        static size_type initial_map_size(){return size_type(8);}
        //两个default大小的函数
        //buffer size代表缓冲区大小
        //initial map size代表map 的大小


        pointer allocate_node() { return data_allocator::allocate(buffer_size()); }
        //例如，deque是int型的，他就会allocate 512/4 = 128个int的位置
        void deallocate_node(pointer n){ data_allocator::deallocate(n, buffer_size());}

        void create_map_and_nodes(size_type num_elements){
            size_type num_nodes = num_elements / buffer_size() + 1;
            //这是map的nodes
            map_size = max(initial_map_size(), num_nodes + 2);
            map = map_allocator::allocate(map_size);//给出分配空间的头部
            map_pointer nstart = map + (map_size - num_nodes)/2;
            //nstart = map + 1或4 - num nodes / 2
            //无论如何pointer都会剩下头部的一点空间
            map_pointer nfinish = nstart + num_nodes - 1;
            //map + 1时，nfinish = map + num nodes
            //4 - num nodes / 2时 nfinish = 3 + num nodes/2
            map_pointer cur;
            try{
                for(cur = nstart; cur < nfinish; ++cur){//没有重载<=
                    *cur = allocate_node();
                }
                *cur = allocate_node();
            }
            catch(...){
                for(cur = nstart; cur < nfinish; ++cur){
                    deallocate_node(*cur);
                }
                deallocate_node(*cur);
			    map_allocator::deallocate(map, map_size);
            }//这是将map allocate
            start.set_node(nstart);
            //这是iterator中的函数
            //类似于二维数组的结构
            finish.set_node(nfinish);
            start.cur = start.first;
            finish.cur = finish.first + num_elements % buffer_size();
        }

        void fill_initialize(size_type n, const value_type& value){
            create_map_and_nodes(n);
            map_pointer cur;
            try{
                for(cur = start.node;cur < finish.node; ++cur){
                    uninitialized_fill(*cur, *cur + buffer_size(), value);
                }
                //create给出了map的构造，cur根据map里给出的地址在连续空间中构造
                uninitialized_fill(finish.first, finish.cur, value);
            }
            catch(...){
                for(map_pointer tmp = start.node; tmp < cur; ++tmp){
                    destroy(*tmp, *tmp + buffer_size());
                }
                for(map_pointer cur = start.node;  cur <= finish.node; ++cur){
                    deallocate_node(*cur);
                }//两个循环分别deallocate缓冲区和map
                map_allocator::deallocate(map, map_size);
                throw;
            }
        }
        //map和每一个缓冲区的构造起点是完全随机的
        //但是map的内容是这些随机节点的头部
        //缓冲区的内容是真正的data

    public:
        //构造函数
        //和list，vector一样，提供三个版本
        //n个value，空的和n个默认值

        //空初始化调用create map and node
        //其余两个初始化调用fill initialize-> create map and nodes + uninitialize fill
        deque():start(),  finish(), map(0), map_size(0){
            create_map_and_nodes(0);
        }

        deque(size_type n, const value_type& value):
        start(),  finish(), map(0), map_size(0){
            fill_initialize(n, value);
        }

        deque(int n, const value_type& value):
        start(),  finish(), map(0), map_size(0){
            fill_initialize(n, value);
        }

        deque(long n, const value_type& value):
        start(),  finish(), map(0), map_size(0){
            fill_initialize(n, value);
        }

        explicit deque(size_type n):
        start(), finish(), map(0), map_size(0){
		    fill_initialize(n,value_type());
        }

        ~deque(){
            destroy(start, finish);
            map_pointer tmp = start.node;
            for(; tmp <= finish.node; ++tmp){
                deallocate_node(*tmp);
            }
            map_allocator::deallocate(map, map_size);
          //  std::cout<<"sign"<<std::endl;
        }
        //基本函数
        iterator begin(){return start;}
        iterator end(){return finish;}
        reference operator[](size_type n){
            return start[difference_type(n)];
            //这是使用了iterator重载的[]
            //实际上这个行为在主函数里也可以实现
        }
        reference front(){return *start;}
        reference back(){//待测试函数
//            iterator tmp = finish;
//            --tmp;
//            return *tmp;
              return *(finish - 1);//这个写法并没有发现什么问题
        }
        size_type size()const{return finish - start;}
        size_type max_size()const{return size_type(-1);}
        bool empty(){return finish == start;}

        //头，尾插入函数
        //虽然这个函数很复杂，但是通常的int型几乎用不到全部函数
        //因为int才占了4个字节
        //初始化的每个缓冲区都能存128个int
        //而且默认有八个缓冲区（算上必须剩下的end）
    protected:

        iterator reallocate_map(size_type nodes_to_add, bool add_at_front){
            size_type old_num_nodes = finish.node - start.node + 1;
            size_type new_num_nodes = old_num_nodes + nodes_to_add;
            map_pointer new_nstart;
            if(map_size > 2 * new_num_nodes){//map size的default值为8
                new_nstart = map + (map_size - new_num_nodes) / 2
                            + (add_at_front ? nodes_to_add : 0);
                if(new_nstart < start.node)
                    copy(start.node, finish.node + 1, new_nstart);
                else//这样做的目的是防止数据互相覆盖
                    copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
            }//map本身指向区域的头部，它比start还靠前
            else{//扩大map size并将map复制到新的区域中
                size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;
                map_pointer new_map = map_allocator::allocate(new_map_size);
                new_nstart = new_map + (new_map_size - new_num_nodes) / 2
                             + (add_at_front ? nodes_to_add : 0);
                copy(start.node, finish.node + 1, new_nstart);
                map_allocator::deallocate(map, map_size);//原来的空间被释放
                map = new_map;
                map_size = new_map_size;
            }
            start.set_node(new_nstart);
            finish.set_node(new_nstart + old_num_nodes - 1);

            return add_at_front ?  start : finish;

        }

        iterator reserve_map_at_back(size_type nodes_to_add = 1){
            if(nodes_to_add + 1 > map_size - (finish.node - map))//要保证尾部剩一个
                return reallocate_map(nodes_to_add, false);
        }

        iterator reserve_map_at_front(size_type nodes_to_add = 1){
            if(nodes_to_add > start.node - map)//新的start已经越过了map
                return reallocate_map(nodes_to_add, true);
        }

        void push_back_aux(const value_type& t){
            value_type t_copy = t;
            reserve_map_at_back();
            *(finish.node + 1) = allocate_node();
            //这一步是对map的变化
            //finish.node是map指针
            //allocate finish.node的下一个连续空间

            try{
                construct(finish.cur, t_copy);
                finish.set_node(finish.node + 1);
                //finish本身是个迭代器
                //这句的效果是让finish这个迭代器的first变成finish.node + 1
                //last变成新的first + buffer size
                //这样就达到了更改finish的值
                finish.cur = finish.first;
                //最后再把cur更改到first上
            }
            catch(...){
                deallocate_node(*(finish.node + 1));
                //如果出现问题了也不用改变finish，因为一定是construct出现的问题
                //在那个时候finish还没有变化
                throw;
            }
        }

        void push_front_aux(const value_type& t){
            value_type t_copy = t;
            reserve_map_at_front();
            *(start.node - 1) = allocate_node();
            try{
                start.set_node(start.node - 1);
                start.cur = start.last - 1;
                construct(start.cur, t_copy);
            }
            catch(...){
                start.set_node(start.node + 1);
                start.cur = start.first;
                deallocate_node(*(start.node - 1));
                throw;
            }
        }

    public:
        //如果finish或start没满，则直接添加
        //push back只会遇到有两个以上和有一个备用空间的情况
        //push front是可能遇到没有备用空间的
        //这也是两个aux函数中construct位置不同的原因
        void push_front(const value_type& t){
            if(start.cur != start.first){
                construct(start.cur - 1, t);
                --start.cur;
            }
            else
                push_front_aux(t);
        }

        void push_back(const value_type& t){
            if(finish.cur != finish.last - 1){
                construct(finish.cur, t);
                ++finish.cur;
            }
            else
                push_back_aux(t);
        }

    protected:
        void pop_back_aux(){
            deallocate_node(finish.first);
            finish.set_node(finish.first);
            finish.cur = finish.last - 1;
            destroy(finish.cur);//和pop front aux同理
        }

        void pop_front_aux(){
            destroy(start.cur);//能这么做是因为cur是这个缓冲区唯一的元素
            deallocate_node(start.first);
            start.set_node(start.node + 1);
            start.cur = start.first;
        }



    public:
        void pop_back(){
            if(finish.cur != finish.first){
                --finish.cur;
                destroy(finish.cur);
            }
            else
                pop_back_aux();
        }

        void pop_front(){
            if(start.cur != start.last - 1){
                destroy(start.cur);
                ++start.cur;
            }
            else
                pop_front_aux();
        }

        void clear(){
            for(map_pointer node = start.node + 1; node < finish.node; ++node){
                destroy(*node, *node + buffer_size());
                data_allocator::deallocate(*node, buffer_size());
            }//析构start和finish之间的内容
            if(start.node != finish.node){
                destroy(start.cur, start.last);
                destroy(finish.first, finish.cur);
                data_allocator::deallocate(finish.first, buffer_size());
            }//析构start和finish
            else
                destroy(start.cur, finish.cur);
                //析构但是保留空间
                //这也体现出destroy负责析构，deallocate负责释放
            finish = start;
        }

        iterator erase(iterator pos){
            iterator next = pos;
            ++next;
            difference_type index = pos - start;
            if(index < (size() >> 1)){
                copy_backward(start, pos, next);
                //能这么做是因为已经重载了++
                //所以copy函数可以在不同的缓冲区之间作用
                pop_front();
            }
            else{
                copy(next, finish, pos);
                pop_back();
            }
            return start + index;
        }

        erase(iterator first, iterator last){
            if(first == start && last == finish){
                clear();//全删
                return finish;
            }
            else{
                difference_type n = last - first;
                difference_type elems_before = first - start;
                if(elems_before < (size() - n) / 2){
                    copy_backward(start, first, last);
                    iterator new_start = start + n;
                    destroy(start, new_start);
                    for(map_pointer cur = start.node; cur < new_start.node; ++cur){
                        data_allocator::deallocate(*cur, buffer_size());
                    }//原理和前面一样
                    start = new_start;
                }
                else{
                    copy(last, finish, first);
                    iterator new_finish = finish - n;
                    destroy(new_finish, finish);
                    for(map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur){
                        data_allocator::deallocate(*cur, buffer_size());
                    }//map pointer是有<=的
                    finish = new_finish;
                }
                return start + elems_before;

            }
        }
    protected:
        iterator insert_aux(iterator pos, const value_type& x){
            //算法的核心是把扩容的任务交给push front&&back函数处理
            //自己只管复制
            difference_type index = pos - start;
            value_type x_copy = x;
            if(index < size() / 2){
                push_front(front());
                iterator front1 = start;
                ++front1;
                iterator front2 = front1;
                ++front2;
                pos = start + index;
                iterator pos1 = pos;
                ++pos1;
                copy(front2, pos1, front1);
            }
            else{
                push_back(back());
                iterator back1 = finish;
                --back1;
                iterator back2 = back1;
                --back2;
                pos = start + index;
                copy_backward(pos, back2, back1);
            }
            *pos = x_copy;
            return pos;
        }
        iterator insert_aux(iterator pos, size_type n, const value_type& x){
            //这样处理主要是因为有些内容需要初始化
            //有些内容已经初始化了
            //有些内容是复制的
            //有些内容是赋值的
            //有些内容要从后往前复制
            value_type x_copy = x;
            difference_type index = pos - start;
            if(index < size() / 2){
                iterator new_start = reserve_map_at_front(n);
                iterator old_start = start;//这么做主要是为了安全
                try{
                    if(index >= difference_type(n)){
                        iterator start_n = start + difference_type(n);
                        uninitialized_copy(start, start_n, new_start);
                        start = new_start;
                        copy(start_n, pos, old_start);
                        fill(pos - difference_type(n), pos, x_copy);
                    }//如果复制的内容数量小于pos的索引则应该分两步
                    else{
                        uninitialized_copy(start, pos, new_start);
                        start = new_start;
                        uninitialized_fill(old_start - difference_type(n), old_start, x_copy);
                        fill(old_start, pos, x_copy);
                    }
                }
                catch(...){
                    for(map_pointer n = new_start.node; n < start.node; ++n){
                        deallocate_node(*n);
                    }
                    throw;
                }

            }
            else{//待测试函数，整体思路和上一致
                index = finish - pos;
                iterator new_finish = reserve_map_at_back(n);
                iterator old_finish = finish;
                try{
                    if(index >= difference_type(n)){
                        iterator finish_n = finish - difference_type(n);
                        uninitialized_copy(finish_n, finish, finish);
                        finish = new_finish;
                        copy_backward(pos, finish_n, old_finish);
                        fill(pos, pos + difference_type(n), x_copy);
                    }
                    else{
                        uninitialized_copy(pos, finish, pos + difference_type(n));
                        finish = new_finish;
                        uninitialized_fill(old_finish, pos + difference_type(n), x_copy);
                        fill(pos, old_finish, x_copy);
                    }
                }
                catch(...){
                    for(map_pointer n = new_finish.node; n > finish.node; --n){
                        deallocate_node(*n);
                    }
                    throw;
                }

            }
            *pos = x_copy;
            return pos;
        }




    public:
        iterator insert(iterator position, const value_type& x){
            if(position.cur == start.cur){
                push_front(x);
                return start;
            }
            else if(position.cur == finish.cur){
                push_back(x);
                iterator tmp = finish;
                --tmp;
                return tmp;
            }
            else{
                return insert_aux(position, x);
            }
        }

        iterator insert(iterator position, size_type n, const value_type& x){
            if(position.cur == start.cur){
                iterator new_start = reserve_map_at_front(n);
                uninitialized_fill(new_start, start, x);
                start = new_start;
                return start;
            }
            else if(position.cur == finish.cur){
                iterator new_finish = reserve_map_at_back(n);
                uninitialized_fill(finish, new_finish, x);
                finish = new_finish;
                iterator tmp = finish;
                --tmp;
                return tmp;
            }
            else
                return insert_aux(position, n, x);

        }

};



#endif // DEQUE_H_INCLUDED
