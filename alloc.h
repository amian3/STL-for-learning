#ifndef ALLOC_H
#define ALLOC_H

#include<string.h>//����mempy ������������reallocate
#include<cstddef>//����ptrdiff_t, size_t
#include<cstdlib>//����exit()
#include<iostream>//����cerr



//������һ����һ��������
//����ζ��������ֱ��ʹ��free��malloc�Ⱥ���

template<int inst>
class _malloc_alloc_template{

    private:
        //oom i.e. out of memory
        //�������������������ڴ治������
        static void *oom_malloc(size_t);
        static void *oom_realloc(void *, size_t);
        static void (* _malloc_alloc_oom_handler)();

    public:

        static void* allocate(size_t n){//����������������ڴ�
        void *result = malloc(n);//һ����������Ȩ��
        if(0 == result)
            result = oom_malloc(n);//����޷��������󣬵���private�еĺ��������ڴ治��
        return result;
        }

        static void deallocate(void *p, size_t/* n */){//������������ͷ��ڴ�
            free(p);//һ����������Ȩ��
        }

        //sz i.e. size
        //����������size_tδʵ������������Ϊ���ڲ���Ҫʹ������������
        //������֤�Ժ�ĸ����в�ʹ�ã����������д��

        static void* reallocate(void *p, size_t/* old_sz */, size_t new_sz){
            void *result = realloc(p, new_sz);
            if(0 == result)
                result = oom_realloc(p, new_sz);//����private�еĺ���
            return result;
        }
        //realloc���������޸ĵ�һ��ָ�����ռ�Ŀռ��С

        //����һ��ͨ������ָ����ʵ������һ������ָ��
        //��Ҫָ�����ǣ�*f()�����ܱ��(*f)()
        //*f()�ȼ���*(f())
        //���������������set_new_handler
        //���������Ǵ����쳣״̬
        //����Ĳ���(*f)()�Ǹ�ָ�룬ָ���޷������㹻�ڴ�ʱӦ�ñ����õĺ���
        //����ֵ�Ǹ�ָ�룬ָ��set_malloc_handler������ǰ����ִ�еĺ���
        //https://www.cnblogs.com/Forever-Road/p/6808875.html
        static void (* set_malloc_handler( void (*f)()))(){
            void (* old)() = _malloc_alloc_oom_handler;
            _malloc_alloc_oom_handler = f;
            return (old);
        }


};

//�����������ڴ治��Ĵ�����
template<int inst>
void (*_malloc_alloc_template<inst>::_malloc_alloc_oom_handler)() = 0;

template<int inst>
void *_malloc_alloc_template<inst>::oom_malloc(size_t n){
    void (* my_malloc_handler)();
    void *result;
    for(;;){//��������У�ֻ������if������ֹ�������ɹ�����
        my_malloc_handler = _malloc_alloc_oom_handler;
        if(0 == my_malloc_handler){
            std::cerr<<"out of memory"<<std::endl;//cerr����������󵽽���
            exit(1);
        }
        (*my_malloc_handler)();//���õ�һ����������Ҫ�ͷ��ڴ棨��ʹ���ٴη���ʱ�пռ䣩
        result = malloc(n);//�ٴη����ڴ�
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

typedef _malloc_alloc_template<0> malloc_alloc;//��һ������������

//�ڶ���������
template<bool threads, int inst>
class _default_alloc_template{

    private:

        static const int _ALIGN = 8;//��֤ÿ�����鶼�ǰ˵ı���
        static const int _MAX_BYTES = 128;//С����������ޣ��������128����ʹ��һ��������
        static const int _NFREELISTS = 16;// i.e. nums of free list ��������ĸ���
        static size_t ROUND_UP(size_t bytes){
            return (((bytes) + _ALIGN -1) & ~(_ALIGN - 1));
            //&��λ�� |��λ�� ~��λȡ��
            //7 = 00000111 ~7 = 10001000
            //bytes + 00000111 & 10001000
            //�����ǰ�λ�룬�����ֵ�������λ��ȻΪ0��������ֵ��Ȼ��8�ı���
            //&Դ��ʵ�֣�
        }
        //��������freelist�Ľڵ㹹��
        union obj{//free list�Ľڵ㣬ÿ���ڵ㶼����һ���ڵ��ָ��
            //union����ռunion��ռ�ռ�����Ԫ����ռ�ռ�Ŀռ�
            //������union����ռ4�ֽ�
            //�������໥���ǵ����ԣ����ֶ��������4�ֽڵ����ݲ���ָ����һ��union����ָ��data
            //�۲����obj�ĸ�����ʽ�����и���ֱ�۵��˽�
            //��Ҫ��ȷ����obj�����ܴ洢ʵ�����ݣ�����ָ���¿��ٵĿռ䣬�����¿ռ���Լ�����һ���ڵ�������
            union obj* free_list_link;
            char client_data[1];
        };

        //�����￪ʼ�Ƕ�16��list�Ĵ���
        static obj* volatile free_list[_NFREELISTS];
        //volatileָʾ��������ʱ�̹۲족�����Ƿ����仯

        static size_t FREELIST_INDEX(size_t bytes){
            return (((bytes) + _ALIGN - 1)/ _ALIGN - 1);
            //����˵��bytesӦ�ô����ĸ�list��
            //ÿ��list�Ĵ�С�ֱ���(8,128)bytes��list
        }

        static void* refill(size_t n);
        //refill��allocate���������޿��ÿռ��ʱ�򱻵���
        //refill�����chunk_alloc�������ڴ��ȡ������

        static char* chunk_alloc(size_t size, int& nobjs);
        //chunk_alloc��������ڴ��ȡ�ÿռ�
        //nobjs����СΪsize������

        static char* start_free;//�ڴ�صĿ�ʼλ�ã�ֻ��chunk_alloc�б仯
        static char* end_free;//�ڴ�صĽ���λ�ã�ֻ��chunk_alloc�б仯
        static size_t heap_size;//��С

    public:

        //�������������������䴢��ռ������
        static void* allocate(size_t n){
            obj * volatile *my_free_list;//*my_free_list����n�����С�µ�free_list����
            //ʵ���ϣ�free list������һ����ά���飬����һ��һά�ģ������ͷ��������
            obj *result;
            //���n����128���Ǿ�Ӧ�õ��õ�һ�����������ڶ���������ֻ�ܽ���С��128�ģ�
            if( n > (size_t) _MAX_BYTES)
                return (malloc_alloc::allocate(n));
            my_free_list = free_list + FREELIST_INDEX(n);
            //my_free_list��ʾ����Ӧ��ŵ��Ǹ�����������(һ����16��)
            //�õ���my_free_list��ͨ��170�еĲ�����֤
            //��û�е���if����ôÿ��my_free_list��ȡ�ĵ�ַ����δ������ݵ�
            //�ⲻ����Ϊmy_free_list���ᱻdelete��������Ϊ����������
            //my_free_list�ı仯��"free_list[i]"�ı仯
            result = *my_free_list;
            if( result == 0 ){
                //�������ʧ�ܣ�����refill�����������free list
                void *r = refill(ROUND_UP(n));
                return r;
            }
            //
            *my_free_list = result->free_list_link;
            //��my_free_listָ��free_list[]����һ��Ԫ��,���Ԫ��һ���ǿյģ��ٷ�����һ��Ԫ��
            //����һ�����ٴ�ʹ��my free listʱһ������ָ������򣬳����Ѿ�����ڴ���Ѿ�������
            return (result);
        }//��˵�����������ǲ���Ҫ����ռ�ģ���������Ѿ�ӵ�пռ���

        static void deallocate(void *p, size_t n){
            obj *q = (obj *)p;//����q��Ҫɾ�����Ǹ�union��
            obj * volatile * my_free_list;
            //���ǵ�Ŀ�ľ�����n��������ϵ�my_free_listָ��q
            //����q���ǿ�������ʹ�õĿռ���

            if(n > (size_t)_MAX_BYTES){
                //�������128����Ҫɾ�������ݲ������ɶ������õģ����Ӧ�õ�һ��
                malloc_alloc::deallocate(p,n);
                return;
            }

            my_free_list = free_list + FREELIST_INDEX(n);
            //���ҵ�������my_free_list
            q->free_list_link = *my_free_list;
            //��qָ��������my_free_list��Ŀ���ǣ���ֹ֮���allocate��������
            *my_free_list = q;
            //�����������my_free_list���q

        }

        static void* reallocate(void *p,size_t old_sz, size_t new_sz){
            void * result;
            size_t copy_sz;

            if(old_sz >(size_t)_MAX_BYTES && new_sz >(size_t)_MAX_BYTES)
                return (realloc(p, new_sz));//
            //����ϵĺ��µĶ�����128����һ��������
            if(ROUND_UP(old_sz) == ROUND_UP(new_sz))
                return (p);
            //��Ȼ����ϵĺ��µ���һ��������(8k,8k + 1)
            //�Ǿͷ���ԭ�ؼ���
            result = allocate(new_sz);
            //����������㣬�ʹ���new��С�Ŀռ�
            //����old����ȥ���������ϵ�p
            //����ȥ��ԭ�����������������٣�����������Ҳ��new�Ĵ�С��
            copy_sz = new_sz > old_sz ? old_sz : new_sz;
            memcpy(result, p, copy_sz);//��p�п���copy_sz���ֽڵ�result֮��
            deallocate(p, old_sz);
            return (result);
        };
};


//������һЩ�����ĳ�ʼ��ֵ
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
    int nobjs = 20;//�����n����������ˣ���Ĭ�Ͽ���20���µ�n��С������
    char * chunk = chunk_alloc(n, nobjs);//chunk�����ڻ�ȡ�½ڵ�ĺ���
    //���ķ���ֵָ���¿ռ�ĵ�һ������
    obj * volatile * my_free_list;
    obj * result;
    //��ǰ��һ����my_free_list������ָ��������ָ�룬result��Ϊ����ֵ
    obj * current_obj, * next_obj;//������ֵ�����������µ�union��
    //��Ϊ�µ�����������ģ������Ҫ������ֵ���������ӳ�Ϊһ������
    int i;
    if(1 == nobjs) return (chunk);
    my_free_list = free_list + FREELIST_INDEX(n);
    //��ǰ�����һ��
    result = (obj *)chunk;
    //���ص��ǵ�һ���µ�union
    *my_free_list = next_obj = (obj *)(chunk + n);
    //objÿ��n�ĳ���ָ����һ��obj
    //my_free_list�ǵ�һ��union����һ����Ҳ����next_obj
    //���ڻ���Ҫ��chunk�ռ䴮��һ������
    //��Ϊchunk�õ���ֻ���µĿռ䣬��Щ�ռ�û�д����κ���Ϣ��Ҳû�к��ʵ�objָ������
    for(i = 1;;i++){
        current_obj = next_obj;
        next_obj = (obj *)((char *)next_obj + n);
        if(nobjs - 1 == i){
            current_obj -> free_list_link = 0;
            break;//������ͣ�ı�־����Ϊֻ��ȡ��nobjs * n�Ŀռ�
        }
        else{
            current_obj -> free_list_link = next_obj;
        }
    }
    return (result);
}


//chunk_alloc�Ĺ����Ǵ��ڴ���л�ȡ�ռ䲢������free_list
//��������ֻ�ǿ��ٿռ䣬����Ҫ�����¿ռ���ײ�����˷���char*
template<bool threads, int inst>
char* _default_alloc_template<threads, inst>::
chunk_alloc(size_t size, int& nobjs){
    char* result;
    size_t total_bytes = size * nobjs;//Ҫ�õĿռ�
    size_t bytes_left = end_free - start_free;//�ڴ����ʣ��Ŀռ�
    if(bytes_left >= total_bytes){
        //���ʣ�µĴ���Ҫ�õģ����ǽݱ仯start��end�Ϳ��Է�����
        //������Ҫ������Щ�ռ䣬refill��������ϸ����
        result = start_free;
        start_free += total_bytes;
        return (result);
    }
    else if(bytes_left >= size){
        //����������� total�Ĵ�С�����Ǻô��ܷ���һЩ
        //�ǾͰ�ʣ�µ�ȫ�õ�������
        nobjs = bytes_left / size;
        total_bytes = size * nobjs;
        result = start_free;
        start_free += total_bytes;
        return (result);
    }
    else{
        //ʣ�µĿռ���һ��size�������ṩ�ˣ���ȥ�����ڴ��
        size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
        //������λ������ν���������߼�������������0

        //����Ҫ���ڴ��ʣ�µ������ǲ��ǻ������ü�ֵ(bytes_left > 0)
        //����Ǵ���0�ģ����Ȱ�ʣ�µķ��䵽��ʣ�µĴ�Сһ�µ�free_list��
        if(bytes_left > 0){
            obj * volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
            ((obj *)start_free) -> free_list_link = *my_free_list;
            *my_free_list = (obj *)start_free;

        }
        start_free = (char *)malloc(bytes_to_get);//�����ռ�
        if(0 == start_free){//�������ʧ��
            int i;
            obj * volatile * my_free_list, *p;
            //����ϣ����������free_list�г��һЩ����Ŀռ�����
            //my_free_listָʾ����������free_list����ÿ������ͷ��
            //p������my_free_listת���������free_list
            //������p���涨start��end����ԭ����my_free_list�����ָ�����һ��Ԫ��
            for(i = size; i<= _MAX_BYTES; i += _ALIGN){
                my_free_list = free_list + FREELIST_INDEX(i);
                p = *my_free_list;
                if(0 != p){
                    //p != 0��˵����ǰp���ڵ�free_list���пռ�
                    //�涨��仯��֮�󣬾Ϳ������µ����Լ�
                    //�����仯���start��end���Զ������ռ��
                    *my_free_list = p -> free_list_link;
                    start_free = (char *)p;
                    end_free = start_free + i;
                    return (chunk_alloc(size, nobjs));
                }
            }
            //���´���������е�p����0����������
            //��˵����û�ط��ˣ��Ǿ�ֻ����ȥ��һ����������æ��
            //����out-of-memory����
            end_free = 0;
            start_free = (char *)malloc_alloc::allocate(bytes_to_get);
        }
        //���´�����start��Ϊ0��Ҳ���Ǵ����ռ�ɹ������
        //�ɹ��������ڴ�أ�ֻ��Ҫ����start��end
        //������ǰ��byte_left > 0�����ݼ��ɣ������Լ��Ϳ�����
        heap_size += bytes_to_get;
        end_free = start_free + bytes_to_get;
        return (chunk_alloc(size, nobjs));
    }
}
//����һ���ӿ��࣬������֤����������STL����
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
typedef malloc_alloc alloc;//����vector��������Ĭ�ϵ��þ��Ƕ���������




//alloc����
#endif // ALLOC_H
