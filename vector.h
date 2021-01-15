#ifndef VECTOR_H
#define VECTOR_H

#include"allocator.h"
#include"algorithm.h"
//��ʼ����allocate��������ڴ�
//��constructֻ�ܸ��ڴ�������

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
        //���iterator������iterator�ļ��е�struct,������֮ǰ�ո�typedef��iterator
        void insert_aux(iterator position, const T& x){
            if(finish != end_of_storage){
                construct(finish, *(finish - 1));
                ++finish;//������finish��λ��
                T x_copy = x;
                copy_backward(position, finish - 2, finish - 1);//�㷨�ں���
                //�������ǽ�position�Ժ���������κ���
                *position = x_copy;
                //�ճ�����position����x
            }
            else{
                const size_type old_size = size();
                const size_type len = old_size != 0 ? 2 * old_size : 1;
                //��������������ǿյľͱ��1(��Ϊ�յ������������ǿյ�)

                iterator new_start = data_allocator::allocate(len);//�����ƶ�vector
                iterator new_finish = new_start;
                try{
                    new_finish = uninitialized_copy(start, position, new_start);
                    //��start��position�����ݸ��Ƶ�new_startΪ���Ŀռ���

                    construct(new_finish, x);
                    ++new_finish;
                    //�ٵ�����ֵposition
                    new_finish = uninitialized_copy(position, finish, new_finish);
                    //���ֵ����position��finish������
                }
                catch(...){
                    destroy(new_start, new_finish);
                    data_allocator::deallocate(new_start, len);
                    throw;//���������ɾ��֮ǰ���������ݣ�Ȼ��throw
                }
                //ɾ��֮ǰ��vector
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
            start = allocate_and_fill(n, value);//��δ�ҵ����������λ��
            finish = start + n;
            end_of_storage = finish;
        }

    public:
        iterator begin(){return start;}
        iterator end(){return finish;}
        size_type size() const{return size_type(finish - start);}
        size_type capacity() const{return size_type(end_of_storage - start);}
        bool empty() const{return begin() == end();}
        reference operator[](size_type n){return *(begin() + n);}//���������[]������ԭ��

        //�����ǹ��캯��
        vector(): start(0), finish(0), end_of_storage(0){}
        vector(size_type n, const T& value){fill_initialize(n, value);}
        vector(int n, const T& value){      fill_initialize(n, value);}
        vector(long n, const T& value){     fill_initialize(n, value);}
        explicit vector(size_type n){       fill_initialize(n,T());}
        //explicit(��ʽ)ȡ������ʽת��
        //���ֻ��ͨ��vector<int> a(10)������
        //����ͨ��vector<int> a = 10����

        ~vector(){
            destroy(start, finish);//ȫ�ֺ���
            deallocate();//vector���������
        }

        reference front(){return *start;}
        reference back(){return *(finish - 1);}
        //��һ�����һ��Ԫ�أ����صĲ����ǵ�����������Ԫ�ر���

        void push_back(const T& x){
            if(finish != end_of_storage){
                construct(finish, x);//ȫ�ֺ���
                ++finish;}
            else
                insert_aux(finish, x);//vector���������
        }

        void pop_back(){
            --finish;
            destroy(finish);//ȫ�ֺ���
        }

        //eraseӦʵ�������汾
        //��positionɾ����ɾ��first��last����������
        iterator erase(iterator position){//
            if(position + 1 != end())
                copy(position + 1, finish, position);
            --finish;
            destroy(finish);
            return position;
        }

        iterator erase(iterator first, iterator last){
            iterator i = copy(last, finish, first);//����ȫ�ֺ���
            destroy(i, finish);
            finish = finish - (last - first);//Ҳ������Ϊû�мӷ���
            return first;
        }
        //���ϵ�����erase������Ϊclient�����ã�����ֱ��a.erase(a.begin() + i, a.begin() + j)
        //��ȻҲ���� vector<int>::iterator iter = a.erase(a.begin() + i, a.begin() + j)

        void resize(size_type new_size, const T& x){
            if(new_size < size())
                erase(begin() + new_size, end());
            else
                insert(end(), new_size - size(), x);
        }

        void resize(size_type new_size){resize(new_size, T());}//T()��T��Ĭ��ֵ

        void clear(){erase(begin(),end());}

        //insert Ӧʵ�������汾
        //��position����x����position����n��x
        iterator insert(iterator position, const T& x){
            if(position == finish)//finishȷʵ���⣬��ΪҪ��֤�������copy backward
                push_back(x);
            else
            insert_aux(position, x);
        }
        //���insert��д����Ϊ�˼ӿ��ٶ�
        //��˲�û��ʹ���������ȰѺ���Ĺ�ȥ�ٴ�ǰ�渳ֵ�ķ���
        //��Ϊ��ǰ�ķ���Ҫ����һ�����ϣ����������ʵ���϶�ָ��ֻ������һ��
        //Ѱַ��������stze_type���е���
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
            uninitialized_fill_n(result, n, x);//ȫ�ֺ���
            return result;
        }
};

#endif // VECTOR_H
