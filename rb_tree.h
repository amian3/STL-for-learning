#ifndef RB_TREE_H_INCLUDED
#define RB_TREE_H_INCLUDED
//红黑树
#include"allocator.h"
#include"algorithm.h"
#include"functional.h"
#include"pair.h"
typedef bool _rb_tree_color_type;
const _rb_tree_color_type _rb_tree_red = false;
const _rb_tree_color_type _rb_tree_black = true;
struct _rb_tree_node_base{
    typedef _rb_tree_color_type color_type;
    typedef _rb_tree_node_base* base_ptr;

    color_type color;
    base_ptr parent;
    base_ptr left;
    base_ptr right;

    static base_ptr minimum(base_ptr x){
        while(x->left != 0)x = x->left;
        return x;
    }

    static base_ptr maximum(base_ptr x){
        while(x->right != 0)x = x->right;
        return x;
    }
};

template<class Value>
struct _rb_tree_node : public _rb_tree_node_base{
    typedef _rb_tree_node<Value>* link_type;
    Value value_field;
};
//树的分层结构，节点和颜色用node base处理，value用node处理
struct _rb_tree_base_iterator{
    typedef _rb_tree_node_base::base_ptr base_ptr;
    typedef bidirectional_iterator_tag iterator_category;
    typedef ptrdiff_t difference_type;

    base_ptr node;

    void increment(){//如果有右节点，increment就是右节点
        if(node->right != 0){
            node = node->right;
            while(node->left != 0)
                node = node->left;
        }
        else{//如果没有，看自己是不是parent的左节点
             //如果是，那就是parent
             //如果不是就持续往上查找到是parent的左节点的节点为止
            base_ptr y = node->parent;
            while(node == y->right){
                node = y;
                y = y->parent;
            }
            if(node->right != y)//这只可能在node已经是最大元素的时候出现
                node = y;
        }
    }

    void decrement(){
        if(node->color == _rb_tree_red && node->parent->parent == node)
            node = node->right;//这是一种特殊情况处理
        else if(node->left != 0){
            base_ptr y = node->left;
            while(y->right != 0)
                y = y->right;
            node = y;
        }
        else{
            base_ptr y = node->parent;
            while(node == y->left){
                node = y;
                y = y->parent;
            }
            node = y;
        }
    }
};

template<class Value, class Ref, class Ptr>
struct _rb_tree_iterator : public _rb_tree_base_iterator{
    typedef Value value_type;
    typedef Ref reference;
    typedef Ptr pointer;
    typedef _rb_tree_iterator<Value, Value&, Value*> iterator;
    typedef _rb_tree_iterator<Value, const Value&, const Value*> const_iterator;
    typedef _rb_tree_iterator<Value, Ref, Ptr> self;
    typedef _rb_tree_node<Value>* link_type;

    _rb_tree_iterator(){}
    _rb_tree_iterator(link_type x){node = x;}
    _rb_tree_iterator(const iterator& it){node = it.node;}

    reference operator*() const {return link_type(node)->value_field;}
    pointer operator->() const { return &(operator*()); }

    self& operator++(){increment();return *this;}
    self operator++(int){
        self tmp = *this;
        increment();
        return tmp;
    }

    self& operator--(){decrement();return *this;}
    self operator--(int){
        self tmp = *this;
        decrement();
        return tmp;
    }
};

inline bool operator==(const _rb_tree_base_iterator & x, const _rb_tree_base_iterator & y){
    return x.node == y.node;
}
//迭代器用两层处理，base层只存一个节点，剩下都是一些基本定义
//iterator层增加*和->，和一些简单的构造函数

//红黑树的性质：
//节点是红色或黑色，根节点是黑色，叶子是黑色，红节点的子节点都是黑色
//任一节点到每个叶子的路径都有同数量的黑节点
        inline void _rb_tree_rotate_left(_rb_tree_node_base* x, _rb_tree_node_base*& root){
            //         x
            //             y
            //           k
            //变成
            //         y
            //     x
            //       k
            _rb_tree_node_base* y = x->right;
            x->right = y->left;
            if(y->left != 0)
                y->left->parent = x;
            y->parent = x->parent;
            if(x == root)//如果x是root就不能执行else语句，不然right/leftmost就被覆盖了
                root = y;
            else if(x == x->parent->left)
                x->parent->left = y;
            else
                x->parent->right = y;
            y->left = x;
            x->parent = y;
        }

        inline void _rb_tree_rotate_right(_rb_tree_node_base* x, _rb_tree_node_base*& root){
            //          x
            //      y
            //        k
            //变成
            //         y
            //             x
            //           k
            _rb_tree_node_base* y = x->left;
            x->left = y->right;
            if(y->right != 0)
                y->right->parent = x;
            y->parent = x->parent;
            if(x == root)
                root = y;
            else if(x == x->parent->right)
                x->parent->right = y;
            else
                x->parent->left = y;
            y->right = x;
            x->parent = y;
        }

        inline _rb_tree_node_base* _rb_tree_rebalance_for_erase(_rb_tree_node_base* _z, _rb_tree_node_base*& _root,
			_rb_tree_node_base*& _leftmost, _rb_tree_node_base*& _rightmost){
		_rb_tree_node_base* y = _z;
		_rb_tree_node_base* _x = 0;
		_rb_tree_node_base* _x_parent = 0;
		if (y->left == 0)
			_x = y->left;
		else
			if (y->right == 0)
				_x = y->left;
			else {
				y = y->right;
				while (y->left != 0)
					y = y->left;
				_x = y->right;
			}
			if (y != _z) {
				_z->left->parent = y;
				y->left = _z->left;
				if (y != _z->right) {
					_x_parent = y->parent;
					if (_x) _x->parent = y->parent;
					y->parent->left = _x;
					y->right = _z->right;
					_z->right->parent = y;
				}
				else
					_x_parent = y;
				if (_root == _z)
					_root = y;
				else if (_z->parent->left == _z)
					_z->parent->left = y;
				else
					_z->parent->right = y;
				y->parent = _z->parent;
				std::swap(y->color, _z->color);
				y = _z;
			}
			else {
				_x_parent = y->parent;
				if (_x) _x->parent = y->parent;
				if (_root == _z)
					_root = _x;
				else
					if (_z->parent->left == _z)
						_z->parent->left = _x;
					else
						_z->parent->right = _x;
				if (_leftmost == _z)
					if (_z->right == 0)
						_leftmost = _z->parent;
					else
						_leftmost = _rb_tree_node_base::minimum(_x);
				if (_rightmost == _z)
					if (_z->left == 0)
						_rightmost = _z->parent;
					else
						_rightmost = _rb_tree_node_base::maximum(_x);
			}
			if (y->color != _rb_tree_red) {
				while (_x != _root && (_x == 0 || _x->color == _rb_tree_black))
					if (_x == _x_parent->left) {
						_rb_tree_node_base* w = _x_parent->right;
						if (w->color == _rb_tree_red) {
							w->color = _rb_tree_black;
							_x_parent->color = _rb_tree_red;
							_rb_tree_rotate_left(_x_parent, _root);
							w = _x_parent->right;
						}
						if ((w->left == 0 || w->left->color == _rb_tree_black) &&
							(w->right == 0 || w->right->color == _rb_tree_black)) {
							w->color = _rb_tree_red;
							_x = _x_parent;
							_x_parent = _x_parent->parent;
						}
						else {
							if (w->right == 0 || w->right->color == _rb_tree_black) {
								if (w->left) w->left->color = _rb_tree_black;
								w->color = _rb_tree_red;
								_rb_tree_rotate_right(w, _root);
								w = _x_parent->right;
							}
							w->color = _x_parent->color;
							_x_parent->color = _rb_tree_black;
							if (w->right) w->right->color = _rb_tree_black;
							_rb_tree_rotate_left(_x_parent, _root);
							break;
						}
					}
					else {
						_rb_tree_node_base* w = _x_parent->left;
						if (w->color == _rb_tree_red) {
							w->color = _rb_tree_black;
							_x_parent->color = _rb_tree_red;
							_rb_tree_rotate_right(_x_parent, _root);
							w = _x_parent->left;
						}
						if ((w->right == 0 || w->right->color == _rb_tree_black) &&
							(w->left == 0 || w->left->color == _rb_tree_black)) {
							w->color = _rb_tree_red;
							_x = _x_parent;
							_x_parent = _x_parent->parent;
						}
						else {
							if (w->left == 0 || w->left->color == _rb_tree_black) {
								if (w->right) w->right->color = _rb_tree_black;
								w->color = _rb_tree_red;
								_rb_tree_rotate_left(w, _root);
								w = _x_parent->left;
							}
							w->color = _x_parent->color;
							_x_parent->color = _rb_tree_black;
							if (w->left) w->left->color = _rb_tree_black;
							_rb_tree_rotate_right(_x_parent, _root);
							break;
						}
					}
					if (_x) _x->color = _rb_tree_black;
			}
			return y;
	}



        inline void _rb_tree_rebalance(_rb_tree_node_base* x, _rb_tree_node_base*& root){
            x->color = _rb_tree_red;
            while(x != root && x->parent->color == _rb_tree_red){
                if(x->parent == x->parent->parent->left){
                    _rb_tree_node_base* y = x->parent->parent->right;
                    if(y && y->color == _rb_tree_red){
                        x->parent->color = _rb_tree_black;
                        y->color = _rb_tree_black;
                        x->parent->parent->color = _rb_tree_red;
                        x = x->parent->parent;
                    }
                    else{
                        if(x == x->parent->right){
                            x = x->parent;
                            _rb_tree_rotate_left(x, root);
                        }
                        x->parent->color = _rb_tree_black;
                        x->parent->parent->color = _rb_tree_red;
                        _rb_tree_rotate_right(x->parent->parent, root);
                    }
                    //两个一组改色，如果不用改，就说明需要转了
                }
                else{
                    _rb_tree_node_base* y = x->parent->parent->left;
                    if(y && y->color == _rb_tree_red){
                        x->parent->color = _rb_tree_black;
                        y->color = _rb_tree_black;
                        x->parent->parent->color = _rb_tree_red;
                        x = x->parent->parent;
                    }
                    else{
                        if(x == x->parent->left){
                            x = x->parent;
                            _rb_tree_rotate_right(x, root);
                        }
                        x->parent->color = _rb_tree_black;
                        x->parent->parent->color = _rb_tree_red;
                        _rb_tree_rotate_left(x->parent->parent, root);
                    }
                }
            }
            root->color = _rb_tree_black;
        }

//value是值的类型，key是键值，是比较的值，compare是比较的标准
//keyofvalue是value的键值，key = keyofvalue()(value)
//相当于两个value在比较的时候先要变成key，再做compare比较
//比如插入一个元素v，那么本身的x要调用key函数，返回一个keyofvalue()(x)
//再和keyofvalue(v)传入compare，由compare比较
template<class Key, class Value, class KeyOfValue, class Compare,
         class Alloc = alloc>
class rb_tree{
    protected:
        typedef void* void_pointer;
        typedef _rb_tree_node_base* base_ptr;
        typedef _rb_tree_node<Value> rb_tree_node;
        typedef simple_alloc<rb_tree_node, Alloc> rb_tree_node_allocator;
        typedef _rb_tree_color_type color_type;

    public:
        typedef Key key_type;
        typedef Value value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef rb_tree_node* link_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    protected:
        link_type get_node(){
            return rb_tree_node_allocator::allocate();
        }
        void put_node(link_type p){
            rb_tree_node_allocator::deallocate(p);
        }
        link_type create_node(const value_type& x){
            link_type tmp = get_node();
            try{
                construct(&tmp->value_field, x);
            }
            catch(...){
                put_node(tmp);
            }
            return tmp;
        }
        link_type clone_type(link_type x){
            link_type tmp = create_node(tmp->value_field);
            tmp->color = x->color;
            tmp->left = 0;
            tmp->right = 0;
            return tmp;
        }
        void destroy_node(link_type p){
            destroy(&p->value_field);
            put_node(p);
        }

    protected:
        size_type node_count;
        link_type header;
        //这个header不是树里的内容，只不过是用于方便访问root，leftmost和rightmost
        Compare key_compare;

        link_type& root() const {
            return (link_type&) header->parent;
        }
        link_type& leftmost() const {
            return (link_type&) header->left;
        }
        link_type& rightmost() const {
            return (link_type&) header->right;
        }

        static link_type& left(link_type x){
            return (link_type&)(x->left);
        }
        static link_type& right(link_type x){
            return (link_type&)(x->right);
        }
        static link_type& parent(link_type x){
            return (link_type&)(x->parent);
        }

        static reference value(link_type x){
            return x->value_field;
        }
        static const Key& key(link_type x){
            return KeyOfValue()(value(x));
        }
        static color_type& color(link_type x){
            return (color_type&)(x->color);
        }

        static link_type& left(base_ptr x){
            return (link_type&)(x->left);
        }
        static link_type& right(base_ptr x){
            return (link_type&)(x->right);
        }
        static link_type& parent(base_ptr x){
            return (link_type&)(x->parent);
        }
        static reference value(base_ptr x){
            return ((link_type)x)->value_field;
        }
        static const Key& key(base_ptr x){
            return KeyOfValue()(value(link_type(x)));
        }
        static color_type& color(base_ptr x){
            return (color_type&)(link_type(x)->color);
        }

        static link_type minimum(link_type x){
            return (link_type) _rb_tree_node_base::minimum(x);
        }
        static link_type maximum(link_type x){
            return (link_type) _rb_tree_node_base::maximum(x);
        }

    public:
        typedef _rb_tree_iterator<value_type, reference, pointer> iterator;
        typedef _rb_tree_iterator<value_type, const_reference, const_pointer> const_iterator;
        void swap(rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& t){
			std::swap(header, t.header);
			std::swap(node_count, t.node_count);
			std::swap(key_compare, t.key_compare);
		}

        template <class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	    inline
		void swap(rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>& x,
			rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>& y){
		    x.swap(y);
	}

    public:
        Compare key_comp()const{
            return key_compare;
        }
        iterator begin(){
            return leftmost();
        }
        iterator end(){
            return header;
        }
        bool empty() const{
            return node_count == 0;
        }
        size_type size() const{
            return node_count;
        }
        size_type max_size() const{
            return size_type(-1);
        }
//insert模块
    private:
        iterator _insert(base_ptr x_, base_ptr y_, const Value& v){
            link_type x = (link_type) x_;
            link_type y = (link_type) y_;
            link_type z;
            if(y == header || x != 0 || key_compare(KeyOfValue()(v), key(y))){
                z = create_node(v);
                left(y) = z;
                if(y == header){
                    root() = z;
                    rightmost() = z;
                }
                else if(y == leftmost())
                    leftmost() = z;
            }
            else{
                z = create_node(v);
                right(y) = z;
                if(y == rightmost())
                    rightmost() = z;
            }
            parent(z) = y;
            left(z) = 0;
            right(z) = 0;
            //直接插入，然后再靠rebalance平衡

            _rb_tree_rebalance(z, header->parent);
            ++node_count;
            return iterator(z);
        }

    public:

        pair<iterator, bool> insert_unique(const value_type& v){
            link_type y = header;
            link_type x = root();
            bool comp = true;
            while(x != 0){
                y = x;
                comp = key_compare(KeyOfValue()(v), key(x));
                x = comp ? left(x) : right(x);
            }
            iterator j = iterator(y);
            if(comp)
                if(j == begin())
                    return pair<iterator, bool>(_insert(x, y, v), true);
                //v < begin(),就说明v小于所有元素，因此不重复
                //能走到这一步一定是一直left下来的
                else
                    --j;

            if(key_compare(key(j.node), KeyOfValue()(v)))//j < v
                return pair<iterator, bool>(_insert(x, y, v), true);
            //如果comp == false，那么说明不应该插入到j左侧，这个判断说明了应该插入到j的右侧（不相等）
            //如果comp == true，那么一定经过了--j操作
            //这个操作涉及三个单元v ，y和y->parent,y->parent->right == y
            //如果是true，则说明了v介于y和y->parent之间
            //v > y,这说明 v > y->parent->parent以及y->parent->parent的左侧内容
            //v < y->parent说明y < y->parent以及y->parent的右侧内容
            //这说明不可能有元素和v重复了
            //如果是false,且进行了--j，必须满足y->parent < v < y，这说明重复了
            //如果是false但没有--j，说明这一定是插入在右子树的下面，false说明j >= v,v >= j，重复

            return pair<iterator, bool>(j, false);
        }

        iterator insert_equal(const value_type& v){
            link_type y = header;
            link_type x = root();
            while(x != 0){
                y = x;
                x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
            }
            return _insert(x, y, v);
        }

//erase模块
    private:
        //删掉x以及x以下的所有内容
        void _erase(link_type x){
            while (x != 0) {
			    _erase(right(x));
			    link_type y = left(x);
			    destroy_node(x);
			    x = y;
		    }
        }
    public:
        //只删除position这个位置的内容
        void erase(iterator position){
		    link_type y = (link_type)_rb_tree_rebalance_for_erase(position._base_node,
			header->parent, header->left, header->right);
		    destroy_node(y);
		    --node_count;
	    }

        void erase(iterator first, iterator last){
		    if (first == begin() && last == end())
		   	    clear();
		    else
			    while (first != last) erase(first++);
        }

    private:
        link_type _copy(link_type x, link_type p){
            link_type top = _clone_node(x);
		    top->parent = p;
            try{
			    if (x->right)
				    top->right = _copy(right(x), top);//right交给下一个递归，left自己完成
			    p = top;
			    x = left(x);
			    while (x != 0){
				    link_type y = _clone_node(x);
				    p->left = y;
				    y->parent = p;
				    if (x->right)
					    y->right = _copy(right(x), y);
				    p = y;
				    x = left(x);
                    }
                }
		    catch (...){
			    _erase(top);
		    }
		    return top;
	    }


        void init(){//default的内容，左右边界都是header
            header = get_node();
            color(header) = _rb_tree_red;
            root() = 0;
            leftmost() = header;
            rightmost() = header;
        }

    public:
        void clear(){//应用erase函数
			if (node_count != 0){
				_erase(root());
				leftmost() = header;
				root() = 0;
				rightmost() = header;
				node_count = 0;
			}
		}

    public:
        rb_tree(const Compare& comp = Compare()): node_count(0),
        key_compare(comp){init();}

        ~rb_tree(){
            clear();
            put_node(header);
        }


        //赋值操作
        rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& operator =
        (const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x){
            if(this != &x){
			    clear();
			    node_count = 0;
			    key_compare = x.key_compare;
			    if(x.root() == 0){
				    root() = 0;
				    leftmost() = header;
				    rightmost() = header;
			    }
			    else{
				    root() = _copy(x.root(), header);
				    leftmost() = minimum(root());
				    rightmost() = maximum(root());
				    node_count = x.node_count;
			    }
		    }
		    return *this;
        }


    public:

        //二分查找
        iterator find(const Key& k){
            link_type y = header;
            link_type x = root();
            while(x != 0){
                if(!key_compare(key(x), k)){
                    y = x;
                    x = left(x);
                }
                else
                    x = right(x);
            }
            iterator j = iterator(y);
            return(j == end() || key_compare(k, key(j.node))) ? end() : j;
        }



};


#endif // RB_TREE_H_INCLUDED
