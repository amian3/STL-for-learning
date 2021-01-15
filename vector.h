#ifndef VECTOR_H
#define VECTOR_H

#include"allocator.h"
#include"algorithm.h"
//初始化用allocate负责分配内存
//而construct只能给内存以内容

template<class T, class Alloc = alloc>
class vector{
    public:
        typedef T value_type;
        typedef value_type* pointer;
        typedef value_type* iterator;
        typedef value_type& reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    protected:
        typedef simple_alloc<value_type, Alloc> data_allocator;

        iterator start;
        iterator finish;
        iterator end_of_storage;
        //这个iterator并不是iterator文件中的struct,而就是之前刚刚typedef的iterator
        void insert_aux(iterator position, const T& x){
            if(finish != end_of_storage){
                construct(finish, *(finish - 1));
                ++finish;//先增加finish的位置
                T x_copy = x;
                copy_backward(position, finish - 2, finish - 1);//算法内函数
                //其作用是将position以后的内容依次后移
                *position = x_copy;
                //空出来的position放置x
            }
            else{
                const size_type old_size = size();
                const size_type len = old_size != 0 ? 2 * old_size : 1;
                //扩大两倍，如果是空的就变成1(因为空的扩大两倍还是空的)

                iterator new_start = data_allocator::allocate(len);//整体移动vector
                iterator new_finish = new_start;
                try{
                    new_finish = uninitialized_copy(start, position, new_start);
                    //将start到position的内容复制到new_start为起点的空间中

                    construct(new_finish, x);
                    ++new_finish;
                    //再单独赋值position
                    new_finish = uninitialized_copy(position, finish, new_finish);
                    //最后赋值其余position到finish的内容
                }
                catch(...){
                    destroy(new_start, new_finish);
                    data_allocator::deallocate(new_start, len);
                    throw;//发现问题就删掉之前创建的内容，然后throw
                }
                //删除之前的vector
                destroy(begin(),end());
                deallocate();
                start = new_start;
                finish = new_finish;
                end_of_storage = new_start + len;
            }
        }

        void deallocate(){
            if(start)
                data_allocator::deallocate(start, end_of_storage - start);
        }

        void fill_initialize(size_type n, const T& value){
            start = allocate_and_fill(n, value);//尚未找到这个函数的位置
            finish = start + n;
            end_of_storage = finish;
        }

    public:
        iterator begin(){return start;}
        iterator end(){return finish;}
        size_type size() const{return size_type(finish - start);}
        size_type capacity() const{return size_type(end_of_storage - start);}
        bool empty() const{return begin() == end();}
        reference operator[](size_type n){return *(begin() + n);}//这就是能用[]索引的原因

        //以下是构造函数
        vector(): start(0), finish(0), end_of_storage(0){}
        vector(size_type n, const T& value){fill_initialize(n, value);}
        vector(int n, const T& value){      fill_initialize(n, value);}
        vector(long n, const T& value){     fill_initialize(n, value);}
        explicit vector(size_type n){       fill_initialize(n,T());}
        //explicit(显式)取消了隐式转换
        //因此只能通过vector<int> a(10)来定义
        //不能通过vector<int> a = 10定义

        ~vector(){
            destroy(start, finish);//全局函数
            deallocate();//vector本身的内容
        }

        reference front(){return *start;}
        reference back(){return *(finish - 1);}
        //第一和最后一个元素，返回的并不是迭代器，而是元素本身

        void push_back(const T& x){
            if(finish != end_of_storage){
                construct(finish, x);//全局函数
                ++finish;}
            else
                insert_aux(finish, x);//vector本身的内容
        }

        void pop_back(){
            --finish;
            destroy(finish);//全局函数
        }

        //erase应实现两个版本
        //在position删除和删除first到last的所有内容
        iterator erase(iterator position){//
            if(position + 1 != end())
                copy(position + 1, finish, position);
            --finish;
            destroy(finish);
            return position;
        }

        iterator erase(iterator first, iterator last){
            iterator i = copy(last, finish, first);//两个全局函数
            destroy(i, finish);
            finish = finish - (last - first);//也许是因为没有加法》
            return first;
        }
        //以上的两个erase都可以为client所调用，可以直接a.erase(a.begin() + i, a.begin() + j)
        //当然也可以 vector<int>::iterator iter = a.erase(a.begin() + i, a.begin() + j)

        void resize(size_type new_size, const T& x){
            if(new_size < size())
                erase(begin() + new_size, end());
            else
                insert(end(), new_size - size(), x);
        }

        void resize(size_type new_size){resize(new_size, T());}//T()即T的默认值

        void clear(){erase(begin(),end());}

        //insert 应实现两个版本
        //在position插入x和在position插入n个x
        iterator insert(iterator position, const T& x){
            if(position == finish)//finish确实特殊，因为要保证不会调用copy backward
                push_back(x);
            else
            insert_aux(position, x);
        }
        //这个insert的写法是为了加快速度
        //因此并没有使用正常的先把后面的过去再从前面赋值的方法
        //因为以前的方法要遍历一遍以上，而这个方法实际上对指针只遍历了一遍
        //寻址操作是由stze_type来承担的
        void insert(iterator position, size_type n, const T& x){
            if(n == 0)
                return;
            if(size_type(end_of_storage - finish) >= n){
                T x_copy = x;
                const size_type elems_after = finish - position;
                iterator old_finish = finish;
                if(elems_after > n){
                    uninitialized_copy(finish - n, finish, finish);
                    finish += n;
                    copy_backward(position, old_finish - n, old_finish);
                    fill(position, position + n, x_copy);
                }
                else{
                    uninitialized_fill_n(finish, n - elems_after, x_copy);
                    finish += n - elems_after;
                    uninitialized_copy(position, old_finish, finish);
                    finish += elems_after;
                    fill(position, old_finish, x_copy);
                }
            }
            else{
                const size_type old_size = size();
                const size_type len = old_size + max(old_size, n);
                iterator new_start = data_allocator::allocate(len);
                iterator new_finish = new_start;
                try{
                    new_finish = uninitialized_copy(start, position, new_start);
                    new_finish = uninitialized_fill_n(new_finish, n, x);
                    new_finish = uninitialized_copy(position, finish, new_finish);
                }
                catch(...){
                    destroy(new_start, new_finish);
                    data_allocator::deallocate(new_start, len);
                    throw;
                }
                destroy(start, finish);
                deallocate();
                start = new_start;
                finish = new_finish;
                end_of_storage = new_start + len;
            }

        }

    protected:
        iterator allocate_and_fill(size_type n, const T& x){
            iterator result = data_allocator::allocate(n);
            uninitialized_fill_n(result, n, x);//全局函数
            return result;
        }
};

#endif // VECTOR_H
