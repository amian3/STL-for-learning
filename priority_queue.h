#ifndef PRIORITY_QUEUE_H_INCLUDED
#define PRIORITY_QUEUE_H_INCLUDED

#include"allocator.h"
#include"algorithm.h"
#include"vector.h"
#include"functional.h"
template<class T, class Sequence = vector<T>,
         class Compare = less<typename Sequence::value_type> >
class priority_queue{
public:
    typedef typename Sequence::value_type value_type;
    typedef typename Sequence::size_type size_type;
    typedef typename Sequence::reference reference;
   // typedef typename Sequence::const_reference const_reference;

protected:
    Sequence c;
    Compare comp;

public:
    priority_queue():c(){}
    explicit priority_queue(const Compare& x):c(),comp(x){}

    template<class InputIterator>
    priority_queue(InputIterator first, InputIterator last, const Compare& x)
    : c(first, last), comp(x){make_heap(c.begin(), c.end(), comp);}
    template<class InputIterator>
    priority_queue(InputIterator first, InputIterator last)
    : c(first, last){make_heap(c.begin(), c.end(), comp);}

    bool empty(){ return c.empty();}
    size_type size() const{ return c.size();}
    reference top() { return c.front();}
    void push(const value_type& x){
        try{
             c.push_back(x);
             push_heap(c.begin(), c.end(), comp);
        }
        catch(...){
            c.clear();
            throw;
        }
    }
    void pop(){
        try{
            pop_heap(c.begin(), c.end(), comp);
            c.pop_back();
        }
        catch(...){
            c.clear();
            throw;
        }
    }
};

#endif // PRIORITY_QUEUE_H_INCLUDED
