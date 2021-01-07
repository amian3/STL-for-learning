#ifndef DEQUE_H_INCLUDED
#define DEQUE_H_INCLUDED
#include"allocator.h"
#include"algorithm.h"

//dequeҲ֧��random����
//�������ĵĽṹ��vector�кܴ�ͬ
//��Ϊ����ͨ��������������ͬ��֮�����������ӵ�
//����������������ʵûvector��
inline size_t _deque_buf_size(size_t n, size_t sz){//size_t���� long long unsigned int
    return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
    //���n != 0����n
    //���n = 0��sz < 512 ���� 512/sz
    //���n = 0����sz > 512 �Ƿ���1
}
template<class T, class Ref, class Ptr, size_t BufSiz>//BufSize��deque����default����
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
        //x����this֮ǰ�ģ����˳����
        //x.cur -> x.last -> other buffers -> this->first -> this->cur
    }

    self& operator++(){
        ++cur;
        if(cur == last){//������һ��������
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
        if(cur == first){//������һ��������
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
    //�⼸�������ܺõ����ֳ���vector������ʵ�����
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
        //������Ӧ�÷ֳɻ������ĺͻ�����ָ�������
        typedef simple_alloc<value_type, Alloc> data_allocator;
        typedef simple_alloc<pointer, Alloc> map_allocator;
        //ע��map_allocator���������Ͳ�����value type

        static size_type initial_map_size(){return size_type(8);}
        //����default��С�ĺ���
        //buffer size����������С
        //initial map size����map �Ĵ�С


        pointer allocate_node() { return data_allocator::allocate(buffer_size()); }
        //���磬deque��int�͵ģ����ͻ�allocate 512/4 = 128��int��λ��
        void deallocate_node(pointer n){ data_allocator::deallocate(n, buffer_size());}

        void create_map_and_nodes(size_type num_elements){
            size_type num_nodes = num_elements / buffer_size() + 1;
            //����map��nodes
            map_size = max(initial_map_size(), num_nodes + 2);
            map = map_allocator::allocate(map_size);//��������ռ��ͷ��
            map_pointer nstart = map + (map_size - num_nodes)/2;
            //nstart = map + 1��4 - num nodes / 2
            //�������pointer����ʣ��ͷ����һ��ռ�
            map_pointer nfinish = nstart + num_nodes - 1;
            //map + 1ʱ��nfinish = map + num nodes
            //4 - num nodes / 2ʱ nfinish = 3 + num nodes/2
            map_pointer cur;
            try{
                for(cur = nstart; cur < nfinish; ++cur){//û������<=
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
            }//���ǽ�map allocate
            start.set_node(nstart);
            //����iterator�еĺ���
            //�����ڶ�ά����Ľṹ
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
                //create������map�Ĺ��죬cur����map������ĵ�ַ�������ռ��й���
                uninitialized_fill(finish.first, finish.cur, value);
            }
            catch(...){
                for(map_pointer tmp = start.node; tmp < cur; ++tmp){
                    destroy(*tmp, *tmp + buffer_size());
                }
                for(map_pointer cur = start.node;  cur <= finish.node; ++cur){
                    deallocate_node(*cur);
                }//����ѭ���ֱ�deallocate��������map
                map_allocator::deallocate(map, map_size);
                throw;
            }
        }
        //map��ÿһ���������Ĺ����������ȫ�����
        //����map����������Щ����ڵ��ͷ��
        //��������������������data

    public:
        //���캯��
        //��list��vectorһ�����ṩ�����汾
        //n��value���յĺ�n��Ĭ��ֵ

        //�ճ�ʼ������create map and node
        //����������ʼ������fill initialize-> create map and nodes + uninitialize fill
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
        //��������
        iterator begin(){return start;}
        iterator end(){return finish;}
        reference operator[](size_type n){
            return start[difference_type(n)];
            //����ʹ����iterator���ص�[]
            //ʵ���������Ϊ����������Ҳ����ʵ��
        }
        reference front(){return *start;}
        reference back(){//�����Ժ���
//            iterator tmp = finish;
//            --tmp;
//            return *tmp;
              return *(finish - 1);//���д����û�з���ʲô����
        }
        size_type size()const{return finish - start;}
        size_type max_size()const{return size_type(-1);}
        bool empty(){return finish == start;}

        //ͷ��β���뺯��
        //��Ȼ��������ܸ��ӣ�����ͨ����int�ͼ����ò���ȫ������
        //��Ϊint��ռ��4���ֽ�
        //��ʼ����ÿ�����������ܴ�128��int
        //����Ĭ���а˸������������ϱ���ʣ�µ�end��
    protected:

        iterator reallocate_map(size_type nodes_to_add, bool add_at_front){
            size_type old_num_nodes = finish.node - start.node + 1;
            size_type new_num_nodes = old_num_nodes + nodes_to_add;
            map_pointer new_nstart;
            if(map_size > 2 * new_num_nodes){//map size��defaultֵΪ8
                new_nstart = map + (map_size - new_num_nodes) / 2
                            + (add_at_front ? nodes_to_add : 0);
                if(new_nstart < start.node)
                    copy(start.node, finish.node + 1, new_nstart);
                else//��������Ŀ���Ƿ�ֹ���ݻ��า��
                    copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
            }//map����ָ�������ͷ��������start����ǰ
            else{//����map size����map���Ƶ��µ�������
                size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;
                map_pointer new_map = map_allocator::allocate(new_map_size);
                new_nstart = new_map + (new_map_size - new_num_nodes) / 2
                             + (add_at_front ? nodes_to_add : 0);
                copy(start.node, finish.node + 1, new_nstart);
                map_allocator::deallocate(map, map_size);//ԭ���Ŀռ䱻�ͷ�
                map = new_map;
                map_size = new_map_size;
            }
            start.set_node(new_nstart);
            finish.set_node(new_nstart + old_num_nodes - 1);

            return add_at_front ?  start : finish;

        }

        iterator reserve_map_at_back(size_type nodes_to_add = 1){
            if(nodes_to_add + 1 > map_size - (finish.node - map))//Ҫ��֤β��ʣһ��
                return reallocate_map(nodes_to_add, false);
        }

        iterator reserve_map_at_front(size_type nodes_to_add = 1){
            if(nodes_to_add > start.node - map)//�µ�start�Ѿ�Խ����map
                return reallocate_map(nodes_to_add, true);
        }

        void push_back_aux(const value_type& t){
            value_type t_copy = t;
            reserve_map_at_back();
            *(finish.node + 1) = allocate_node();
            //��һ���Ƕ�map�ı仯
            //finish.node��mapָ��
            //allocate finish.node����һ�������ռ�

            try{
                construct(finish.cur, t_copy);
                finish.set_node(finish.node + 1);
                //finish�����Ǹ�������
                //����Ч������finish�����������first���finish.node + 1
                //last����µ�first + buffer size
                //�����ʹﵽ�˸���finish��ֵ
                finish.cur = finish.first;
                //����ٰ�cur���ĵ�first��
            }
            catch(...){
                deallocate_node(*(finish.node + 1));
                //�������������Ҳ���øı�finish����Ϊһ����construct���ֵ�����
                //���Ǹ�ʱ��finish��û�б仯
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
        //���finish��startû������ֱ�����
        //push backֻ���������������Ϻ���һ�����ÿռ�����
        //push front�ǿ�������û�б��ÿռ��
        //��Ҳ������aux������constructλ�ò�ͬ��ԭ��
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
            destroy(finish.cur);//��pop front auxͬ��
        }

        void pop_front_aux(){
            destroy(start.cur);//����ô������Ϊcur�����������Ψһ��Ԫ��
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
            }//����start��finish֮�������
            if(start.node != finish.node){
                destroy(start.cur, start.last);
                destroy(finish.first, finish.cur);
                data_allocator::deallocate(finish.first, buffer_size());
            }//����start��finish
            else
                destroy(start.cur, finish.cur);
                //�������Ǳ����ռ�
                //��Ҳ���ֳ�destroy����������deallocate�����ͷ�
            finish = start;
        }

        iterator erase(iterator pos){
            iterator next = pos;
            ++next;
            difference_type index = pos - start;
            if(index < (size() >> 1)){
                copy_backward(start, pos, next);
                //����ô������Ϊ�Ѿ�������++
                //����copy���������ڲ�ͬ�Ļ�����֮������
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
                clear();//ȫɾ
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
                    }//ԭ���ǰ��һ��
                    start = new_start;
                }
                else{
                    copy(last, finish, first);
                    iterator new_finish = finish - n;
                    destroy(new_finish, finish);
                    for(map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur){
                        data_allocator::deallocate(*cur, buffer_size());
                    }//map pointer����<=��
                    finish = new_finish;
                }
                return start + elems_before;

            }
        }
    protected:
        iterator insert_aux(iterator pos, const value_type& x){
            //�㷨�ĺ����ǰ����ݵ����񽻸�push front&&back��������
            //�Լ�ֻ�ܸ���
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
            //����������Ҫ����Ϊ��Щ������Ҫ��ʼ��
            //��Щ�����Ѿ���ʼ����
            //��Щ�����Ǹ��Ƶ�
            //��Щ�����Ǹ�ֵ��
            //��Щ����Ҫ�Ӻ���ǰ����
            value_type x_copy = x;
            difference_type index = pos - start;
            if(index < size() / 2){
                iterator new_start = reserve_map_at_front(n);
                iterator old_start = start;//��ô����Ҫ��Ϊ�˰�ȫ
                try{
                    if(index >= difference_type(n)){
                        iterator start_n = start + difference_type(n);
                        uninitialized_copy(start, start_n, new_start);
                        start = new_start;
                        copy(start_n, pos, old_start);
                        fill(pos - difference_type(n), pos, x_copy);
                    }//������Ƶ���������С��pos��������Ӧ�÷�����
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
            else{//�����Ժ���������˼·����һ��
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
