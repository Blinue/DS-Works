// avl_tree : AVL 树
//

#pragma once

#include <iterator>
#include <cassert>

namespace ds
{

template <typename Ty>
class avl_tree;

// 迭代器
template <typename Ty>
class _avl_tree_const_iterator
{
	friend class avl_tree<Ty>;

public:
	using iterator_category = std::bidirectional_iterator_tag;

	using value_type = Ty;
	using difference_type = ptrdiff_t;
	using pointer = value_type *;
	using reference = value_type &;

private:
	using _node = typename avl_tree<Ty>::_node;

	_avl_tree_const_iterator(_node *node, bool is_end = false) : _ptr(node), _is_end(is_end) {}

public:
	// 解引用
	[[nodiscard]] value_type &operator*() const
	{
		assert(!_is_end);
		return _ptr->data;
	}
	[[nodiscard]] value_type *operator->() const
	{
		assert(!_is_end);
		return &_ptr->data;
	}

	// 自增
	_avl_tree_const_iterator operator++()
	{
		assert(!_is_end && _ptr);

		if (_ptr->right)
		{
			for (_ptr = _ptr->right; _ptr->left; _ptr = _ptr->left)
				;
		}
		else
		{
			_node *pt = _ptr;
			while (pt->parent && pt->parent->right == pt)
				pt = pt->parent;
			pt = pt->parent;

			if (pt)
				_ptr = pt;
			else
				_is_end = true; // 尾后
		}

		return *this;
	}
	_avl_tree_const_iterator operator++(int)
	{
		auto tmp = *this;
		return ++tmp;
	}

	// 自减
	_avl_tree_const_iterator operator--()
	{
		if (_is_end)
		{
			assert(_ptr);
			_is_end = false;
			return *this;
		}

		if (_ptr->left)
		{
			for (_ptr = _ptr->left; _ptr->right; _ptr = _ptr->right)
				;
		}
		else
		{
			while (_ptr->parent && _ptr->parent->left == _ptr)
				_ptr = _ptr->parent;
			_ptr = _ptr->parent;
			assert(_ptr);
		}

		return *this;
	}
	_avl_tree_const_iterator operator--(int)
	{
		auto tmp = *this;
		return --tmp;
	}

	template <typename Ty1>
	friend bool operator==(const _avl_tree_const_iterator<Ty1> &, const _avl_tree_const_iterator<Ty1> &);

private:
	_node *_ptr;
	bool _is_end = false;
}; // class _avl

template <typename Ty>
bool operator==(const _avl_tree_const_iterator<Ty> &left, const _avl_tree_const_iterator<Ty> &right)
{
	return left._ptr == right._ptr && left._is_end == right._is_end;
}

template <typename Ty>
bool operator!=(const _avl_tree_const_iterator<Ty> &left, const _avl_tree_const_iterator<Ty> &right)
{
	return !(left == right);
}

// AVL 树
template <typename Ty>
class avl_tree
{
	friend class _avl_tree_const_iterator<Ty>;

public:
	using value_type = Ty;

	using size_type = size_t;
	using difference_type = ptrdiff_t;

	using pointer = value_type *;
	using reference = value_type &;
	using const_pointer = const value_type *;
	using const_reference = const value_type &;

	using iterator = _avl_tree_const_iterator<Ty>;
	using const_iterator = iterator;

private:
	// 二叉树节点
	struct _node
	{
		value_type data;
		int bf{}; // 平衡因子

		_node *parent = nullptr; // 父节点
		_node *left = nullptr;	 // 左子树
		_node *right = nullptr;	 // 右子树
	};

public: // 构造函数
	avl_tree()
	{
	}

	template <typename InputIt>
	avl_tree(InputIt first, InputIt last)
	{
		for (; first != last; ++first)
			operator[](*first);
	}

	avl_tree(const avl_tree &) = delete;
	avl_tree(avl_tree &&) = delete;
	avl_tree &operator=(const avl_tree &) = delete;
	avl_tree &operator=(avl_tree &&) = delete;

private:
	// 用于在析构时释放内存
	void _free_node(_node *node)
	{
		if (!node)
			return;

		_free_node(node->left);
		_free_node(node->right);

		delete node;
	}

public:
	~avl_tree()
	{
		_free_node(_root);
	}

private:
	// 左旋
	void _left_rotate(_node *node)
	{
		//   |            |
		//   A <-         B
		//  / \          / \
		// 1   B  -->   A   3
		//    / \      / \
		//   2   3    1   2
		////////////////////////
		assert(node);
		_node *p = node->parent, *r = node->right;
		assert(r);
		(p ? (p->left == node ? p->left : p->right) : _root) = r;
		r->parent = p;
		node->right = r->left;
		if (r->left)
			r->left->parent = node;
		r->left = node;
		node->parent = r;
	}
	// 右旋
	void _right_rotate(_node *node)
	{
		//     |           |
		//     A <-        B
		//    / \         / \
		//   B   3  -->  1   A
		//  / \             / \
		// 1   2           2   3
		///////////////////////////
		assert(node);
		_node *p = node->parent, *l = node->left;
		assert(l);
		(p ? (p->left == node ? p->left : p->right) : _root) = l;
		l->parent = p;
		node->left = l->right;
		if (l->right)
			l->right->parent = node;
		l->right = node;
		node->parent = l;
	}

	// 插入后修正
	void _insert_fix_up(_node *node)
	{
		while (true)
		{
			_node *p = node->parent;
			if (!p)
				return;

			if (p->left == node)
			{
				switch (p->bf)
				{
				case -1:
				{
					p->bf = 0;
					return;
				}
				case 0:
				{
					p->bf = 1;
					node = p;
					continue;
				}
				case 1:
				{
					if (node->bf == -1)
						_left_rotate(node);

					_right_rotate(p);
					p->bf = 0;
					node->bf = 0;

					return;
				}
				default:
					throw std::logic_error("预料之外的平衡因子值");
				}
			}
			else
			{
				switch (p->bf)
				{
				case -1:
				{
					if (node->bf == 1)
						_right_rotate(node);

					_left_rotate(p);
					p->bf = 0;
					node->bf = 0;

					return;
				}
				case 0:
				{
					p->bf = -1;
					node = p;
					continue;
				}
				case 1:
				{
					p->bf = 0;
					return;
				}
				default:
					throw std::logic_error("预料之外的平衡因子值");
				}
			}
		}
	}

public:
	// 插入元素
	value_type &operator[](const value_type &e)
	{
		if (!_root)
		{
			_root = new _node{e}; // 树为空
			return _root->data;
		}

		_node *p = _root;
		while (true)
		{
			if (p->data == e)
				return p->data;
			else if (p->data < e)
			{
				if (p->right)
					p = p->right;
				else
				{
					p->right = new _node{e, 0, p};
					_insert_fix_up(p->right);

					return p->right->data;
				}
			}
			else
			{
				if (p->left)
					p = p->left;
				else
				{
					p->left = new _node{e, 0, p};
					_insert_fix_up(p->left);

					return p->left->data;
				}
			}
		}
	}

	// 查找元素
	[[nodiscard]] const_iterator search(const value_type &val)
	{
		if (!_root)
			return end();

		_node *p = _root;
		while (p->data != val)
		{
			if (p->data < val)
			{
				if (p->right)
					p = p->right;
				else
					return end();
			}
			else
			{
				if (p->left)
					p = p->left;
				else
					return end();
			}
		}
		return {p};
	}

private:
	// 删除后修正
	void _erase_fix_up(bool is_left_child, _node *parent)
	{
		while (true)
		{
			if (is_left_child)
			{
				switch (parent->bf)
				{
				case -1:
				{
					_node *right = parent->right;
					if (right->bf == 1)
						_right_rotate(right);

					_left_rotate(parent);
					parent->bf = -1;
					right->bf = 1;

					return;
				}
				case 0:
				{
					parent->bf = -1;
					return;
				}
				case 1:
				{
					parent->bf = 0;

					_node *gp = parent->parent;
					if (!gp)
						return;

					is_left_child = gp->left == parent;
					parent = gp;
					continue;
				}
				default:
					throw std::logic_error("预料之外的平衡因子值");
				}
			}
			else
			{
				switch (parent->bf)
				{
				case -1:
				{
					parent->bf = 0;

					_node *gp = parent->parent;
					if (!gp)
						return;

					is_left_child = gp->left == parent;
					parent = gp;
					continue;
				}
				case 0:
				{
					parent->bf = 1;
					return;
				}
				case 1:
				{
					_node *left = parent->left;
					assert(left);
					if (left->bf == -1)
						_left_rotate(left);

					_right_rotate(parent);
					parent->bf = 1;
					left->bf = -1;

					return;
				}
				default:
					throw std::logic_error("预料之外的平衡因子值");
				}
			}
		}
	}

public:
	// 删除元素
	bool erase(const value_type &val)
	{
		const_iterator it = search(val);
		if (it._ptr == nullptr)
			return false;

		if (it._ptr->left && it._ptr->right)
		{
			_node *p = it._ptr;
			p->data = std::move(*++it);
		}

		_node *parent = it._ptr->parent;
		if (_node *l = it._ptr->left)
		{
			(parent ? (parent->left == it._ptr ? parent->left : parent->right) : _root) = l;
			if (l)
				l->parent = parent;
			delete it._ptr;

			if (parent)
				_erase_fix_up(parent->left == l, parent);
		}
		else
		{
			_node *r = it._ptr->right;
			(parent ? (parent->left == it._ptr ? parent->left : parent->right) : _root) = r;
			if (r)
				r->parent = parent;
			delete it._ptr;

			if (parent)
				_erase_fix_up(parent->left == r, parent);
		}

		return true;
	}

	// 迭代器
	[[nodiscard]] iterator begin()
	{
		if (!_root)
			return {nullptr, true};

		_node *p = _root;
		for (; p->left; p = p->left)
			;
		return {p};
	}

	[[nodiscard]] iterator end()
	{
		if (!_root)
			return {nullptr, true};

		_node *p = _root;
		for (; p->right; p = p->right)
			;
		return {p, true};
	}

private:
	_node *_root = nullptr; // 根节点
};							// class avl_tree<>

} // namespace ds
