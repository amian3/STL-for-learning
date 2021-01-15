#ifndef ALLOC_H
#define ALLOC_H

#include<string.h>//引入mempy 二级配置器的reallocate
#include<cstddef>//引入ptrdiff_t, size_t
#include<cstdlib>//引入exit()
#include<iostream>//引入cerr



//以下是一个第一级配置器
//这意味着它可以直接使用free，malloc等函数

template<int inst>
class _malloc_alloc_template{

    private:
        //oom i.e. out of memory
        //这三个函数用来处理内存不足的情况
        static void *oom_malloc(size_t);
        static void *oom_realloc(void *, size_t);
        static void (* _malloc_alloc_oom_handler)();

    public:

        static void* allocate(size_t n){//这个函数用来分配内存
        void *result = malloc(n);//一级配置器的权限
        if(0 == result)
            result = oom_malloc(n);//如果无法满足需求，调用private中的函数，即内存不足
        return result;
        }

        static void deallocate(void *p, size_t/* n */){//这个函数用来释放内存
            free(p);//一级配置器的权限
        }

        //sz i.e. size
        //这两个函数size_t未实例化参数是因为现在不需要使用这两个参数
        //但不保证以后的更新中不使用，因此现在先写上

        static void* reallocate(void *p, size_t/* old_sz */, size_t new_sz){
            void *result = realloc(p, new_sz);
            if(0 == result)
                result = oom_realloc(p, new_sz);//调用private中的函数
            return result;
        }
        //realloc的作用是修改第一个指针的所占的空间大小

        //这是一个通过函数指针来实例化的一个函数指针
        //需要指出的是，*f()并不能表达(*f)()
        //*f()等价于*(f())
        //这个函数用来仿真set_new_handler
        //它的作用是处理异常状态
        //输入的参数(*f)()是个指针，指向无法分配足够内存时应该被调用的函数
        //返回值是个指针，指向set_malloc_handler被调用前正在执行的函数
        //https://www.cnblogs.com/Forever-Road/p/6808875.html
        static void (* set_malloc_handler( void (*f)()))(){
            void (* old)() = _malloc_alloc_oom_handler;
            _malloc_alloc_oom_handler = f;
            return (old);
        }


};

//以下是三个内存不足的处理函数
template<int inst>
void (*_malloc_alloc_template<inst>::_malloc_alloc_oom_handler)() = 0;

template<int inst>
void *_malloc_alloc_template<inst>::oom_malloc(size_t n){
    void (* my_malloc_handler)();
    void *result;
    for(;;){//这个过程中，只有两个if可以终止，报错或成功处理
        my_malloc_handler = _malloc_alloc_oom_handler;
        if(0 == my_malloc_handler){
            std::cerr<<"out of memory"<<std::endl;//cerr用于输出错误到界面
            exit(1);
        }
        (*my_malloc_handler)();//调用第一个函数，想要释放内存（以使得再次分配时有空间）
        result = malloc(n);//再次分配内存
        if(result)
            return (result);
    }
}

template<int inst>
void *_malloc_alloc_template<inst>::oom_realloc(void *p, size_t n){
    void(* my_malloc_handler)();
    void *result;
    for(;;){
        my_malloc_handler = _malloc_alloc_oom_handler;
        if(0 == my_malloc_handler){
            std::cerr<<"out of memory"<<std::endl;
            exit(1);
        }
        result = realloc(p, n);
        if(result)
            return (result);
    }
}

typedef _malloc_alloc_template<0> malloc_alloc;//第一级适配器结束

//第二级适配器
template<bool threads, int inst>
class _default_alloc_template{

    private:

        static const int _ALIGN = 8;//保证每个区块都是八的倍数
        static const int _MAX_BYTES = 128;//小型区块的上限，如果大于128，就使用一级适配器
        static const int _NFREELISTS = 16;// i.e. nums of free list 自由链表的个数
        static size_t ROUND_UP(size_t bytes){
            return (((bytes) + _ALIGN -1) & ~(_ALIGN - 1));
            //&按位与 |按位或 ~按位取反
            //7 = 00000111 ~7 = 10001000
            //bytes + 00000111 & 10001000
            //由于是按位与，这个数值的最后三位必然为0，因此这个值必然是8的倍数
            //&源码实现？
        }
        //从这里是freelist的节点构造
        union obj{//free list的节点，每个节点都有下一个节点的指针
            //union本身占union中占空间最大的元素所占空间的空间
            //因此这个union本身占4字节
            //但由于相互覆盖的特性，这种定义可以让4字节的内容不仅指向下一个union而且指向data
            //观察后面obj的给出方式可以有更加直观的了解
            //需要明确的是obj并不能存储实际内容，它是指向新开辟的空间，并对新空间和自己的下一个节点做操作
            union obj* free_list_link;
            char client_data[1];
        };

        //从这里开始是对16个list的处理
        static obj* volatile free_list[_NFREELISTS];
        //volatile指示编译器“时刻观察”参数是否发生变化

        static size_t FREELIST_INDEX(size_t bytes){
            return (((bytes) + _ALIGN - 1)/ _ALIGN - 1);
            //这是说明bytes应该存在哪个list里
            //每个list的大小分别是(8,128)bytes的list
        }

        static void* refill(size_t n);
        //refill在allocate自由链表无可用空间的时候被调用
        //refill会调用chunk_alloc函数从内存池取得区块

        static char* chunk_alloc(size_t size, int& nobjs);
        //chunk_alloc函数会从内存池取得空间
        //nobjs个大小为size的区块

        static char* start_free;//内存池的开始位置，只在chunk_alloc中变化
        static char* end_free;//内存池的结束位置，只在chunk_alloc中变化
        static size_t heap_size;//大小

    public:

        //以下三个函数用来分配储存空间和析构
        static void* allocate(size_t n){
            obj * volatile *my_free_list;//*my_free_list是在n这个大小下的free_list索引
            //实际上，free list并不是一个二维数组，而是一个一维的，链表的头部的数组
            obj *result;
            //如果n大于128，那就应该调用第一层配置器（第二级配置器只能接受小于128的）
            if( n > (size_t) _MAX_BYTES)
                return (malloc_alloc::allocate(n));
            my_free_list = free_list + FREELIST_INDEX(n);
            //my_free_list表示数据应存放的那个的自由链表(一共有16个)
            //得到的my_free_list会通过170行的操作保证
            //若没有调用if，那么每次my_free_list获取的地址都是未存放数据的
            //这不是因为my_free_list不会被delete，而是因为在这个语句中
            //my_free_list的变化是"free_list[i]"的变化
            result = *my_free_list;
            if( result == 0 ){
                //如果分配失败，调用refill函数重新填充free list
                void *r = refill(ROUND_UP(n));
                return r;
            }
            //
            *my_free_list = result->free_list_link;
            //将my_free_list指到free_list[]的下一个元素,这个元素一定是空的，再返回上一个元素
            //这样一来，再次使用my free list时一定可以指向空区域，除非已经这个内存池已经用完了
            return (result);
        }//这说明自由链表是不需要赋予空间的，他本身就已经拥有空间了

        static void deallocate(void *p, size_t n){
            obj *q = (obj *)p;//现在q是要删除的那个union了
            obj * volatile * my_free_list;
            //我们的目的就是让n这个索引上的my_free_list指向q
            //这样q就是可以自由使用的空间了

            if(n > (size_t)_MAX_BYTES){
                //如果大于128，则要删除的内容并不是由二级配置的，因此应用第一级
                malloc_alloc::deallocate(p,n);
                return;
            }

            my_free_list = free_list + FREELIST_INDEX(n);
            //先找到正常的my_free_list
            q->free_list_link = *my_free_list;
            //让q指向正常的my_free_list的目的是，防止之后的allocate操作乱序
            *my_free_list = q;
            //最后，让正常的my_free_list变成q

        }

        static void* reallocate(void *p,size_t old_sz, size_t new_sz){
            void * result;
            size_t copy_sz;

            if(old_sz >(size_t)_MAX_BYTES && new_sz >(size_t)_MAX_BYTES)
                return (realloc(p, new_sz));//
            //如果老的和新的都大于128就用一级配置器
            if(ROUND_UP(old_sz) == ROUND_UP(new_sz))
                return (p);
            //不然如果老的和新的在一个级别上(8k,8k + 1)
            //那就放在原地即可
            result = allocate(new_sz);
            //如果都不满足，就创建new大小的空间
            //并把old塞进去，析构掉老的p
            //塞进去的原则是能塞多少塞多少，如果溢出了那也按new的大小来
            copy_sz = new_sz > old_sz ? old_sz : new_sz;
            memcpy(result, p, copy_sz);//从p中拷贝copy_sz个字节到result之中
            deallocate(p, old_sz);
            return (result);
        };
};


//以下是一些变量的初始化值
template<bool threads, int inst>
char *_default_alloc_template<threads, inst>:: start_free = 0;

template<bool threads, int inst>
char *_default_alloc_template<threads, inst>:: end_free = 0;

template<bool threads, int inst>
size_t _default_alloc_template<threads, inst>:: heap_size = 0;

template<bool threads, int inst>
typename _default_alloc_template<threads, inst>:: obj * volatile
_default_alloc_template<threads, inst>:: free_list[_NFREELISTS] =
{0, 0, 0, 0, 0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0};


template<bool threads, int inst>
void* _default_alloc_template<threads, inst>::refill(size_t n){
    int nobjs = 20;//如果在n这个索引满了，那默认开辟20个新的n大小的区域
    char * chunk = chunk_alloc(n, nobjs);//chunk是用于获取新节点的函数
    //它的返回值指向新空间的第一个内容
    obj * volatile * my_free_list;
    obj * result;
    //像前面一样，my_free_list是用来指向空区域的指针，result作为返回值
    obj * current_obj, * next_obj;//这两个值是用来连接新的union的
    //因为新的区域是无序的，因此需要这两个值把他们连接成为一个链表
    int i;
    if(1 == nobjs) return (chunk);
    my_free_list = free_list + FREELIST_INDEX(n);
    //和前面操作一致
    result = (obj *)chunk;
    //返回的是第一个新的union
    *my_free_list = next_obj = (obj *)(chunk + n);
    //obj每过n的长度指向下一个obj
    //my_free_list是第一个union的下一个，也就是next_obj
    //现在还需要把chunk空间串成一个链表
    //因为chunk得到的只是新的空间，这些空间没有储存任何信息，也没有合适的obj指向他们
    for(i = 1;;i++){
        current_obj = next_obj;
        next_obj = (obj *)((char *)next_obj + n);
        if(nobjs - 1 == i){
            current_obj -> free_list_link = 0;
            break;//这是暂停的标志，因为只获取了nobjs * n的空间
        }
        else{
            current_obj -> free_list_link = next_obj;
        }
    }
    return (result);
}


//chunk_alloc的工作是从内存池中获取空间并交付给free_list
//由于我们只是开辟空间，并需要返回新空间的首部，因此返回char*
template<bool threads, int inst>
char* _default_alloc_template<threads, inst>::
chunk_alloc(size_t size, int& nobjs){
    char* result;
    size_t total_bytes = size * nobjs;//要用的空间
    size_t bytes_left = end_free - start_free;//内存池中剩余的空间
    if(bytes_left >= total_bytes){
        //如果剩下的大于要用的，纳智捷变化start和end就可以返回了
        //具体怎要处理这些空间，refill函数有详细操作
        result = start_free;
        start_free += total_bytes;
        return (result);
    }
    else if(bytes_left >= size){
        //如果不够返回 total的大小，但是好歹能返回一些
        //那就把剩下的全用掉并返回
        nobjs = bytes_left / size;
        total_bytes = size * nobjs;
        result = start_free;
        start_free += total_bytes;
        return (result);
    }
    else{
        //剩下的空间连一个size都不能提供了，再去扩充内存池
        size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
        //右移四位，无所谓算术或是逻辑，正数都是添0

        //首先要看内存池剩下的区域是不是还有利用价值(bytes_left > 0)
        //如果是大于0的，那先把剩下的分配到和剩下的大小一致的free_list中
        if(bytes_left > 0){
            obj * volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
            ((obj *)start_free) -> free_list_link = *my_free_list;
            *my_free_list = (obj *)start_free;

        }
        start_free = (char *)malloc(bytes_to_get);//创建空间
        if(0 == start_free){//如果创建失败
            int i;
            obj * volatile * my_free_list, *p;
            //我们希望从其他的free_list中抽出一些空余的空间来用
            //my_free_list指示的是其他的free_list，即每个链表头部
            //p用来将my_free_list转换给所需的free_list
            //所以用p来规定start和end，而原本的my_free_list变成他指向的下一个元素
            for(i = size; i<= _MAX_BYTES; i += _ALIGN){
                my_free_list = free_list + FREELIST_INDEX(i);
                p = *my_free_list;
                if(0 != p){
                    //p != 0就说明当前p所在的free_list还有空间
                    //规定完变化量之后，就可以重新调用自己
                    //这样变化后的start和end会自动创建空间的
                    *my_free_list = p -> free_list_link;
                    start_free = (char *)p;
                    end_free = start_free + i;
                    return (chunk_alloc(size, nobjs));
                }
            }
            //以下代码除非所有的p都是0，否则不运行
            //这说明真没地方了，那就只能再去找一级配置器帮忙了
            //调用out-of-memory机制
            end_free = 0;
            start_free = (char *)malloc_alloc::allocate(bytes_to_get);
        }
        //以下代码是start不为0，也就是创建空间成功后调用
        //成功扩充了内存池，只需要更改start和end
        //并运行前面byte_left > 0的内容即可，调用自己就可以了
        heap_size += bytes_to_get;
        end_free = start_free + bytes_to_get;
        return (chunk_alloc(size, nobjs));
    }
}
//这是一个接口类，用来保证配置器符合STL规则
template<class T,class Alloc>
class simple_alloc{
    public:
        static T *allocate(size_t n){
            return 0 == n ? 0 : (T*) Alloc::allocate(n * sizeof(T));
        }
        static T *allocate(void){
            return (T*) Alloc::allocate(sizeof(T));
        }
        static void deallocate(T *p, size_t n){
            if(0 != n)Alloc::deallocate(p, n * sizeof(T));
        }
        static void deallocate(T *p){
            Alloc::deallocate(p, sizeof(T));
        }

};



typedef _malloc_alloc_template<0> malloc_alloc;
typedef malloc_alloc alloc;//这样vector等容器的默认调用就是二级配置器




//alloc结束
#endif // ALLOC_H
