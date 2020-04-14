// rb_tree.hpp : ºìºÚÊ÷
//

#pragma once

#include <iterator>
#include <memory>
#include <cassert>

namespace ds
{

template <typename Ty>
class rb_tree;

template <typename Ty>
class _rb_tree_const_iterator
{
	friend class rb_tree<Ty>;

public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = Ty;

	using difference_type = size_t;

	using pointer = value_type *;
	using reference = value_type &;

private:
	using _node = typename rb_tree<Ty>::_node;

	_rb_tree_const_iterator(_node *ptr, bool is_end = false) : _ptr(ptr), _is_end(is_end) {}

public:
	const value_type &operator*()
	{
		assert(!_is_end);
		return _ptr->data;
	}

	const value_type *operator->()
	{
		assert(!_is_end);
		return &_ptr->data;
	}

	_rb_tree_const_iterator &operator++()
	{
		assert(!_is_end && _ptr != _null);

		if (_ptr->right != _null)
		{
			for (_ptr = _ptr->right; _ptr->left != _null; _ptr = _ptr->left)
				;
		}
		else
		{
			_node *pt = _ptr;
			while (pt->parent != _null && pt->parent->right == pt)
				pt = pt->parent;
			pt = pt->parent;
			if (pt != _null)
				_ptr = pt;
			else
				_is_end = true;
		}

		return *this;
	}

	_rb_tree_const_iterator operator++(int)
	{
		_rb_tree_const_iterator tmp(*this);
		return ++tmp;
	}

	_rb_tree_const_iterator &operator--()
	{
		if (_is_end)
		{
			_is_end = false;
			assert(_ptr != _null);
			return *this;
		}

		if (_ptr->left != _null)
		{
			for (_ptr = _ptr->left; _ptr->right != _null; _ptr = _ptr->right)
				;
		}
		else
		{
			while (_ptr->parent != _null && _ptr->parent->left == _ptr)
				_ptr = _ptr->parent;
			_ptr = _ptr->parent;
			assert(_ptr != _null);
		}

		return *this;
	}

	_rb_tree_const_iterator operator--(int)
	{
		_rb_tree_const_iterator tmp(*this);
		return --tmp;
	}

	template <typename Ty1>
	friend bool operator==(const _rb_tree_const_iterator<Ty1> &, const _rb_tree_const_iterator<Ty1> &);

private:
	_node *_ptr;
	bool _is_end = false;
	static _node *_null;
}; // class _rb_tree_const_iterator<>

template <typename Ty>
typename _rb_tree_const_iterator<Ty>::_node *_rb_tree_const_iterator<Ty>::_null = rb_tree<Ty>::_null;

template <typename Ty>
bool operator==(const _rb_tree_const_iterator<Ty> &left, const _rb_tree_const_iterator<Ty> &right)
{
	return left._ptr == right._ptr && left._is_end == right._is_end;
}

template <typename Ty>
bool operator!=(const _rb_tree_const_iterator<Ty> &left, const _rb_tree_const_iterator<Ty> &right)
{
	return !(left == right);
}

template <typename Ty>
class rb_tree
{
	friend class _rb_tree_const_iterator<Ty>;

public:
	using value_type = Ty;

	using pointer = value_type *;
	using const_pointer = const value_type *;
	using reference = value_type &;
	using const_reference = const value_type &;

	using const_iterator = _rb_tree_const_iterator<Ty>;

private:
	struct _node
	{
		value_type data;
		char color; // ÑÕÉ«£º'R'-ºì£¬'B'-ºÚ
		_node *parent = _null;
		_node *left = _null;
		_node *right = _null;
	};

public:
	template <typename InputIt>
	rb_tree(InputIt first, InputIt last)
	{
		for (; first != last; ++first)
			insert(*first);
	}

	rb_tree(const rb_tree &) = delete;
	rb_tree(rb_tree &&) = delete;

	rb_tree operator=(const rb_tree &) = delete;
	rb_tree operator=(rb_tree &&) = delete;

private:
	void _free_node(_node *node)
	{
		if (node == _null)
			return;
		_free_node(node->left);
		_free_node(node->right);

		delete node;
	}

public:
	~rb_tree()
	{
		_free_node(_root);
	}

private:
	void _left_rotate(_node *node)
	{
		//   |            |
		//   A <-         B
		//  / \          / \
		// 1   B  -->   A   3
		//    / \      / \
		//   2   3    1   2
		////////////////////////
		_node *right = node->right;
		assert(right != _null);
		if (_node *parent = node->parent; parent == _null)
		{
			_root = right;
			right->parent = _null;
		}
		else
		{
			(parent->left == node ? parent->left : parent->right) = right;
			right->parent = parent;
		}

		node->right = right->left;
		right->left->parent = node;
		right->left = node;
		node->parent = right;
	}

	void _right_rotate(_node *node)
	{
		//     |           |
		//     A <-        B
		//    / \         / \
		//   B   3  -->  1   A
		//  / \             / \
		// 1   2           2   3
		///////////////////////////
		_node *left = node->left;
		assert(left != _null);
		if (_node *parent = node->parent; parent == _null)
		{
			_root = left;
			left->parent = _null;
		}
		else
		{
			(parent->left == node ? parent->left : parent->right) = left;
			left->parent = parent;
		}

		node->left = left->right;
		left->right->parent = node;
		left->right = node;
		node->parent = left;
	}

	void _insert_fix_up(_node *node)
	{
		assert(node && node->color == 'R');

		while (true)
		{
			_node *parent = node->parent;
			if (parent == _null)
			{
				node->color = 'B';
				return;
			}

			_node *grandparent = parent->parent;
			if (grandparent == _null)
				return;
			assert(grandparent->color == 'B');

			if (grandparent->left == parent)
			{
				if (grandparent->right->color == 'R')
				{
					//     B            R <- |   B           R <-
					//    / \          / \   |  / \         / \
					//   R   R  -->   B   B  | R   R  -->  B   B
					//  /            /       |  \           \
					// R <-         R        |   R <-        R
					//////////////////////////////////////////////
					parent->color = 'B';
					grandparent->color = 'R';
					grandparent->right->color = 'B';

					node = grandparent;
				}
				else
				{
					//   B            B           B
					//  / \          / \         / \
					// R   B  -->   R   B  -->  R   R
					//  \          /                 \
					//   R        R <-                B
					//////////////////////////////////////
					if (parent->right == node)
					{
						_left_rotate(parent);

						node = parent;
						parent = node->parent;
					}

					parent->color = 'B';
					grandparent->color = 'R';
					_right_rotate(grandparent);

					break;
				}
			}
			else
			{
				if (grandparent->left->color == 'R')
				{
					parent->color = 'B';
					grandparent->color = 'R';
					grandparent->left->color = 'B';

					node = grandparent;
				}
				else
				{
					if (parent->left == node)
					{
						_right_rotate(parent);

						node = parent;
						parent = node->parent;
					}

					parent->color = 'B';
					grandparent->color = 'R';
					_left_rotate(grandparent);

					break;
				}
			}
		}

		_root->color = 'B';
	}

public:
	const_iterator insert(const value_type &e)
	{
		if (_root == _null)
		{
			_root = new _node{e, 'B'};
			return {_root};
		}

		_node *p = _root;
		while (true)
		{
			if (p->data == e)
				return {p};
			else if (p->data < e)
			{
				if (p->right != _null)
					p = p->right;
				else
				{
					p->right = new _node{e, 'R', p};
					if (p->color == 'R')
						_insert_fix_up(p->right);
					return {p->right};
				}
			}
			else
			{
				if (p->left != _null)
					p = p->left;
				else
				{
					p->left = new _node{e, 'R', p};
					if (p->color == 'R')
						_insert_fix_up(p->left);
					return {p->left};
				}
			}
		}
	}

private:
	void _erase_fix_up(bool is_left, _node *parent)
	{
		while (parent != _null)
		{
			if (is_left)
			{
				_node *left = parent->left;
				if (left->color == 'R')
				{
					//  |
					// (B)
					//  |    -->  |
					//  R <-      B
					///////////////////
					left->color = 'B';
					return;
				}

				if (parent->color == 'R')
					//     R            B
					//    / \          /
					//  (B)  B  -->   R
					//  /            /
					// B <-         B
					/////////////////////////
					return _left_rotate(parent);

				if (_node *brother = parent->right; brother->color == 'R')
				{
					//     B            B
					//    / \          /
					//  (B)  R  -->   R
					//  /            /
					// B <-         B <-
					/////////////////////////
					_left_rotate(parent);

					parent->color = 'R';
					brother->color = 'B';
				}
				else
				{
					//     B            B <-
					//    / \          /
					//  (B)  B  -->   R
					//  /            /
					// B <-         B
					///////////////////////////
					_left_rotate(parent);

					parent = brother->parent;
					is_left = (parent->left == brother);
				}
				continue;
			}
			else
			{
				_node *right = parent->right;
				if (right->color == 'R')
				{
					right->color = 'B';
					return;
				}

				if (parent->color == 'R')
					return _right_rotate(parent);

				if (_node *brother = parent->left; brother->color == 'R')
				{
					_right_rotate(parent);

					parent->color = 'R';
					brother->color = 'B';
				}
				else
				{
					_right_rotate(parent);

					parent = brother->parent;
					is_left = (parent->left == brother);
				}
				continue;
			}
		}
	}

	void _erase(const_iterator it)
	{
		_node *parent = it._ptr->parent, *child = it._ptr->left != _null ? it._ptr->left : it._ptr->right;

		if (parent != _null)
		{
			if (parent->left == it._ptr)
			{
				parent->left = child;
				child->parent = parent;

				if (it._ptr->color == 'B')
					_erase_fix_up(true, parent);
			}
			else
			{
				parent->right = child;
				child->parent = parent;

				if (it._ptr->color == 'B')
					_erase_fix_up(false, parent);
			}
		}
		else
		{
			_root = child;
			_root->parent = _null;
			_root->color = 'B';
		}

		delete it._ptr;
	}

public:
	const_iterator erase(const_iterator it)
	{
		assert(!it._is_end);

		auto next = std::next(it);
		if (it._ptr->left != _null && it._ptr->right != _null)
		{
			it._ptr->data = std::move(next._ptr->data);
			_erase(next);
			return it;
		}

		_erase(it);
		return next;
	}

	const_iterator find(const value_type &e)
	{
		_node *p = _root;
		while (p != _null)
		{
			if (p->data == e)
				return {p};
			else if (p->data < e)
				p = p->right;
			else
				p = p->left;
		}
		return end();
	}

	const_iterator begin()
	{
		if (_root == _null)
			return {_null, true};

		_node *p = _root;
		while (p->left != _null)
			p = p->left;
		return {p};
	}

	const_iterator end()
	{
		if (_root == _null)
			return {_null, true};

		_node *p = _root;
		while (p->right != _null)
			p = p->right;
		return {p, true};
	}

private:
	_node *_root = _null;

	static _node _null_node, *_null;
}; // class rb_tree<>

template <typename Ty>
typename rb_tree<Ty>::_node rb_tree<Ty>::_null_node = {{}, 'B', nullptr, nullptr, nullptr};
template <typename Ty>
typename rb_tree<Ty>::_node *rb_tree<Ty>::_null = &_null_node;

template <typename InputIt>
rb_tree(InputIt, InputIt)->rb_tree<typename std::iterator_traits<InputIt>::value_type>;

} // namespace ds
