#ifndef HASHTABLE_H_INCLUDED
#define HASHTABLE_H_INCLUDED

#include"allocator.h"
#include"iterator.h"
#include"vector.h"
#include"algorithm.h"
#include"pair.h"
#include"hash_functions.h"
static const int _stl_num_primes = 28;
static const unsigned long _stl_prime_list[_stl_num_primes] = {
53,         97,           193,         389,       769,
1543,       3079,         6151,        12289,     24593,
49157,      98317,        196613,      393241,    786433,
1572869,    3145739,      6291469,     12582917,  25165843,
50331653,   100663319,    201326611,   402653189, 805306457,
1610612741, 3221225473ul, 4294967291ul
};

inline unsigned long _stl_next_prime(unsigned long n){
    const unsigned long* first = _stl_prime_list;
    const unsigned long* last = _stl_prime_list + _stl_num_primes;
    const unsigned long* pos = lower_bound(first, last, n);
    return pos == last ? *(last - 1) : *pos;
}


template<class Value>
struct _hashtable_node{
    _hashtable_node* next;
    Value val;
};

template<class Value, class Key, class HashFcn,
         class ExtractKey, class EqualKey, class Alloc>
class hashtable;

template<class Value, class Key, class HashFcn,
         class ExtractKey, class EqualKey, class Alloc>
struct _hashtable_iterator;

template<class Value, class Key, class HashFcn,
         class ExtractKey, class EqualKey, class Alloc>
struct _hashtable_const_iterator;

template<class Value, class Key, class HashFcn,
         class ExtractKey, class EqualKey, class Alloc>
struct _hashtable_iterator{
    typedef hashtable<Value, Key, HashFcn,
                      ExtractKey, EqualKey, Alloc> hashtable;
    typedef _hashtable_iterator<Value, Key, HashFcn,
                                ExtractKey, EqualKey, Alloc> iterator;
    typedef _hashtable_const_iterator<Value, Key, HashFcn,
                                      ExtractKey, EqualKey, Alloc> const_iterator;
    typedef _hashtable_node<Value> node;

    typedef forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef Value& reference;
    typedef Value* pointer;
    node* cur;
    hashtable* ht;

    _hashtable_iterator(node* n, hashtable* tab):cur(n), ht(tab){}
    _hashtable_iterator(){}
    reference operator*()const {return cur->val;}
    pointer operator->()const{ return &(operator*());}

    iterator& operator++(){
        const node* old = cur;
        cur = cur->next;
        if(!cur){
            size_type bucket = ht->bkt_num(old->val);
            while(!cur && ++bucket < ht->buckets.size())
                cur = ht->buckets[bucket];
        }
        return *this;
    }

    iterator operator++(int){
        iterator tmp = *this;
        ++*this;
        return tmp;
    }

    bool operator==(const iterator& it)const{ return cur == it.cur;}
    bool operator!=(const iterator& it)const{ return cur != it.cur;}

};

template<class Value, class Key, class HashFcn,
         class ExtractKey, class EqualKey, class Alloc>
struct _hashtable_const_iterator{
    typedef hashtable<Value, Key, HashFcn,
                      ExtractKey, EqualKey, Alloc> hashtable;
    typedef _hashtable_iterator<Value, Key, HashFcn,
                                ExtractKey, EqualKey, Alloc> iterator;
    typedef _hashtable_const_iterator<Value, Key, HashFcn,
                                      ExtractKey, EqualKey, Alloc> const_iterator;
    typedef _hashtable_node<Value> node;

    typedef forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef const Value& reference;
    typedef const Value* pointer;
    const node* cur;
    const hashtable* ht;
    _hashtable_const_iterator(const node* n, const hashtable* tab):cur(n), ht(tab){}
    _hashtable_const_iterator(){}
    _hashtable_const_iterator(const iterator& it) : cur(it.cur), ht(it.ht) {}
    //一定要有iterator初始化const iterator的构造函数，不然unordered set会报错
    //因为涉及const和非const转化
    reference operator*()const {return cur->val;}
    pointer operator->()const{ return &(operator*());}

    const_iterator& operator++(){
        const node* old = cur;
        cur = cur->next;
        if(!cur){
            size_type bucket = ht->bkt_num(old->val);
            while(!cur && ++bucket < ht->buckets.size())
                cur = ht->buckets[bucket];
        }
        return *this;
    }

    const_iterator operator++(int){
        const_iterator tmp = *this;
        ++*this;
        return tmp;
    }

    bool operator==(const const_iterator& it)const{ return cur == it.cur;}
    bool operator!=(const const_iterator& it)const{ return cur != it.cur;}




};
template<class Value, class Key, class HashFcn,
         class ExtractKey, class EqualKey, class Alloc = alloc>
class hashtable{
    friend struct _hashtable_iterator<Value, Key, HashFcn,
                                    ExtractKey, EqualKey, Alloc>;
    friend struct _hashtable_const_iterator<Value, Key, HashFcn,
                                          ExtractKey, EqualKey, Alloc>;
    public:
        typedef HashFcn hasher;
        typedef EqualKey key_equal;
        typedef size_t size_type;
        typedef Key key_type;
        typedef Value value_type;
        typedef ptrdiff_t difference_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;

        typedef _hashtable_iterator<Value, Key, HashFcn,
                                    ExtractKey, EqualKey, Alloc> iterator;
        typedef _hashtable_const_iterator<Value, Key, HashFcn,
                                          ExtractKey, EqualKey, Alloc> const_iterator;

    private:
        hasher hash;
        key_equal equals;
        ExtractKey get_key;
        typedef _hashtable_node<Value> node;
        typedef simple_alloc<node, Alloc> node_allocator;
        vector<node*, Alloc> buckets;
        size_type num_elements;
        //一些定义，hash给出型别，get key给出键值,例如get key(obj) obj为value type的
        //node是每个节点他是一个单向链表节点结构
        //buckets是一个vector，他存放了一堆node，每个node都是链表头部


    public:
        hasher hash_funct() const { return hash; }
		key_equal key_eq() const { return equals; }

        size_type max_bucket_count() const{
            return _stl_prime_list[_stl_num_primes - 1];
        }
        size_type bucket_count()const{ return buckets.size();}
        size_type elems_in_bucket(size_type bucket)const{
            size_type result = 0;
            for(node* cur = buckets[bucket]; cur; cur = cur->next)
                ++result;
            return result;
        }

        size_type size() const {return num_elements;}
        size_type max_size() const { return size_type(-1);}
        bool empty() const{ return size() == 0;}

        iterator end(){
            return iterator(0, this);
        }

        const_iterator end() const {
            return const_iterator(0, this);
        }

        iterator begin(){
            for(size_type n = 0;n < buckets.size(); ++n)
                if(buckets[n])
                    return iterator(buckets[n], this);
            return end();
        }

        const_iterator begin() const {
            for(size_type n = 0;n < buckets.size(); ++n)
                if(buckets[n])
                    return const_iterator(buckets[n], this);
            return end();
        }

        //iterator里提供了构造函数
        //begin找到第一个不是null的node
        //end直接返回一个node(0)，这个node其实不在vector中

        void swap(hashtable& ht){
            std::swap(hash, ht.hash);
            std::swap(equals, ht.equals);
            std::swap(get_key, ht.get_key);
            buckets.swap(ht.buckets);
            std::swap(num_elements, ht.num_elements);
        }

    protected:
        node* new_node(const value_type& obj){
            node* n = node_allocator::allocate();
            n->next = 0;
            try{
                construct(&n->val, obj);
                return n;
            }
            catch(...){
                node_allocator::deallocate(n);
            }
        }

        void delete_node(node* n){
            destroy(&n->val);
            node_allocator::deallocate(n);
        }

        size_type next_size(size_type n)const{ return _stl_next_prime(n);}
        //这个函数用来查看是否需要扩容了

        void initialize_buckets(size_type n){
            const size_type n_buckets = next_size(n);
            buckets.reserve(n_buckets);//vector中的函数，保证vector够长
            buckets.insert(buckets.end(), n_buckets, (node*)0);
            //刚初始化，capacity够大，但是没有元素，insert值改变了finish
            num_elements = 0;
        }
    public:
        size_type bkt_num_key(const key_type& key)const{
            return bkt_num_key(key, buckets.size());
        }

        size_type bkt_num_key(const key_type& key, size_t n)const{
            return hash(key) % n;
        }

        size_type bkt_num(const value_type& obj, size_t n)const{
            return bkt_num_key(get_key(obj), n);
        }

        size_type bkt_num(const value_type& obj)const{
            return bkt_num_key(get_key(obj));
        }//通过同余得到key

    public:
        hashtable(size_type n, const HashFcn& hf, const EqualKey& eql):
        hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0){
            initialize_buckets(n);
        }
        ~hashtable(){clear();}


    protected:
        void resize(size_type num_elements_hint){
            const size_type old_n = buckets.size();
            if(num_elements_hint > old_n){
                const size_type n = next_size(num_elements_hint);
                if(n > old_n){
                    vector<node*, Alloc> tmp(n, (node*)0);
                    try{
                    for(size_type bucket = 0; bucket < old_n; ++bucket){
                        node* first = buckets[bucket];
                        while(first){
                            //如果需要扩容，key也会随之变化，因此需要重新获取key
                            //再插到不同的链表中
                            size_type new_bucket = bkt_num(first->val, n);
                            buckets[bucket] = first->next;
                            first->next = tmp[new_bucket];
                            //先给first->next赋值，再给first赋值
                            //这样，结果就是first变成了顶部，first->next是原来的顶部
                            //每个链表底部都是default(0)
                            tmp[new_bucket] = first;
                            first = buckets[bucket];
                        }
                    }
                    buckets.swap(tmp);

                    }
                    catch(...){
                        for(size_type bucket = 0; bucket < tmp.size(); ++bucket){
                            while(tmp[bucket]){
                                node* next = tmp[bucket]->next;
                                delete_node(tmp[bucket]);
                                tmp[bucket] = next;
                                //删除就没那么麻烦，直接全删就行
                            }
                        }
                    }
                }
            }
        }

        pair<iterator, bool> insert_unique_noresize(const value_type& obj){
            const size_type n = bkt_num(obj);
            node* first = buckets[n];
            for(node* cur = first; cur; cur = cur->next)
                if(equals(get_key(cur->val), get_key(obj)))//重复了就不添加
                    return pair<iterator, bool>(iterator(cur, this), false);
            node* tmp = new_node(obj);
            tmp->next = first;
            buckets[n] = tmp;//添加到顶端
            ++num_elements;
            return pair<iterator, bool>(iterator(tmp, this), true);
        }

        iterator insert_equal_noresize(const value_type& obj){
            const size_type n = bkt_num(obj);
            node* first = buckets[n];
            for(node* cur = first; cur; cur = cur->next)
                if(equals(get_key(cur->val), get_key(obj))){
                    node* tmp = new_node(obj);//重复了也添加，添加到第一个相同元素的下一个位置上
                    tmp->next = cur->next;
                    cur->next = tmp;
                    ++num_elements;
                    return iterator(tmp, this);
                }
            node* tmp = new_node(obj);
            tmp->next = first;
            buckets[n] = tmp;
            ++num_elements;
            return iterator(tmp, this);

        }
    public:
        //如果每个key都在一个链表下，会导致查找速度变成o(n)
        //因为resize判断只考虑总体大小，不考虑每个分支链表
        pair<iterator, bool> insert_unique(const value_type& obj){
            resize(num_elements + 1);
            return insert_unique_noresize(obj);
        }

        iterator insert_equal(const value_type& obj){
            resize(num_elements + 1);
            return insert_equal_noresize(obj);
        }

        void clear(){
            for(size_type i = 0;i < buckets.size(); ++i){
                node* cur = buckets[i];
                while(cur != 0){
                    node* next = cur->next;
                    delete_node(cur);
                    cur = next;
                }
                buckets[i] = 0;
            }
            num_elements = 0;
        }

        void copy_from(const hashtable& ht){
            buckets.clear();
            buckets.reserve(ht.buckets.size());
            buckets.insert(buckets.end(), ht.buckets.size(), (node*)0);
            try{
                for(size_type i = 0;i < ht.buckets.size(); ++i){
                    if(const node* cur = ht.buckets[i]){
                        node* copy = new_node(cur->val);
                        buckets[i] = copy;
                        for(node* next = cur->next; next; cur = next, next = cur->next){
                            copy->next = new_node(next->val);
                            copy = copy->next;
                        }
                    }
                }
                num_elements = ht.num_elements;
            }
            catch(...){
                clear();
            }

        }

    public:

        reference find_or_insert(const value_type& obj){
            resize(num_elements + 1);
            size_type n = bkt_num(obj);
            node* first = buckets[n];
            for(node* cur = first; cur; cur = cur->next)
                if(equals(get_key(cur->val), get_key(obj)))
                    return cur->val;//有的话返回那个值
            node* tmp = new_node(obj);
            tmp->next = first;
            buckets[n] = tmp;
            ++num_elements;
            //没有再插入
            return tmp->val;
        }

        void erase(const iterator& it){
            if(node* const p = it.cur){
                const size_type n = bkt_num(p->val);
                node* cur = buckets[n];
                if(cur == p){
                    buckets[n] = cur->next;
                    delete_node(cur);
                    --num_elements;
                }
                else{
                    node* next = cur->next;
                    while(next){
                        if(next == p){
                            cur->next = next->next;
                            delete_node(next);
                            --num_elements;
                            break;
                        }
                        else{
                            cur = next;
                            next = cur->next;
                        }
                    }
                }
            }
        }

        hashtable& operator=(const hashtable& ht){
            if(&ht != this){
                clear();
                hash = ht.hash;
                equals = ht.equals;
                get_key = ht.get_key;
                copy_from(ht);
            }
            return *this;
        }

        iterator find(const key_type& key){
            size_type n = bkt_num_key(key);
            node* first;
            for(first = buckets[n]; first && !equals(get_key(first->val), key); first = first->next){}
            return iterator(first, this);//如果没找到，会返回那个链表的尾部
        }

    friend bool operator==(const hashtable&, const hashtable&);
};
template<class Value, class Key, class HashFcn,
         class ExtractKey, class EqualKey, class Alloc>
bool operator==(const hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& ht1,
                const hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& ht2){
    typedef typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::node node;
    if(ht1.buckets.size() != ht2.buckets.size())
        return false;
    for(int n = 0;n < ht1.buckets.size(); ++n){
        node* cur1 = ht1.buckets[n];
        node* cur2 = ht2.buckets[n];
        for(; cur1 && cur2 && cur1->val == cur2->val; cur1 = cur1->next, cur2 = cur2->next){}
        if(cur1 || cur2)
            return false;
        return true;
    }
}

#endif // HASHTABLE_H_INCLUDED
