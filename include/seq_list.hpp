// seq_list.hpp : 顺序表
//

#pragma once

#include <algorithm>
#include <limits>
#include <memory>

#include "_common.hpp"

namespace ds
{

// 类型定义
template <typename Ty, typename Alloc>
struct _seq_list_typename
{
	using allocator_type = Alloc;

	using value_type = Ty;
	static_assert(std::is_same_v<value_type,
								 typename std::allocator_traits<allocator_type>::value_type>,
				  "未定义行为：allocator_type::value_type 与 Ty 不同");
	using pointer = typename std::allocator_traits<allocator_type>::pointer;
	using reference = value_type &;
	using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;
	using const_reference = const value_type &;

	using size_type = size_t;
	using difference_type = ptrdiff_t;

	using _data_type = struct
	{
		value_type *base;	  // 存储空间基址
		size_type size;		  // 当前元素数量
		size_type alloc_size; // 预分配空间容量
	};
};

// 顺序表的 const 迭代器
template <typename Ty, typename Alloc>
struct _seq_list_const_iterator
{
	// 类型定义
	using iterator_category = std::random_access_iterator_tag;

	using value_type = typename _seq_list_typename<Ty, Alloc>::value_type;
	using pointer = typename _seq_list_typename<Ty, Alloc>::pointer;
	using reference = typename _seq_list_typename<Ty, Alloc>::reference;
	using difference_type = typename _seq_list_typename<Ty, Alloc>::difference_type;

	using _data_type = typename _seq_list_typename<Ty, Alloc>::_data_type;

	// 构造操作
	_seq_list_const_iterator(value_type *cur, const _data_type *data) : _cur(cur), _data(data)
	{
		assert(_data != nullptr);
		assert(_verify_valid(_cur));
	}

	// 非成员比较操作
	template <typename Ty_, typename Alloc_>
	[[nodiscard]] friend bool operator<(
		const _seq_list_const_iterator<Ty_, Alloc_> &left,
		const _seq_list_const_iterator<Ty_, Alloc_> &right)
	{
		assert(left._verify_compare(right));

		return left._cur < right._cur;
	}
	template <typename Ty_, typename Alloc_>
	[[nodiscard]] friend bool operator<=(
		const _seq_list_const_iterator<Ty_, Alloc_> &left,
		const _seq_list_const_iterator<Ty_, Alloc_> &right)
	{
		assert(left._verify_compare(right));

		return !(right < left);
	}

	template <typename Ty_, typename Alloc_>
	[[nodiscard]] friend bool operator>(
		const _seq_list_const_iterator<Ty_, Alloc_> &left,
		const _seq_list_const_iterator<Ty_, Alloc_> &right)
	{
		assert(left._verify_compare(right));

		return right < left;
	}
	template <typename Ty_, typename Alloc_>
	[[nodiscard]] friend bool operator>=(
		const _seq_list_const_iterator<Ty_, Alloc_> &left,
		const _seq_list_const_iterator<Ty_, Alloc_> &right)
	{
		assert(left._verify_compare(right));

		return !(left < right);
	}

	template <typename Ty_, typename Alloc_>
	[[nodiscard]] friend bool operator==(
		const _seq_list_const_iterator<Ty_, Alloc_> &left,
		const _seq_list_const_iterator<Ty_, Alloc_> &right)
	{
		assert(left._verify_compare(right));

		return left._cur == right._cur;
	}
	template <typename Ty_, typename Alloc_>
	[[nodiscard]] friend bool operator!=(
		const _seq_list_const_iterator<Ty_, Alloc_> &left,
		const _seq_list_const_iterator<Ty_, Alloc_> &right)
	{
		assert(left._verify_compare(right));

		return !(left == right);
	}

	// 移动操作
	_seq_list_const_iterator &operator++()
	{
		assert(_verify_valid(_cur + 1)); // 非尾后迭代器

		++_cur;
		return *this;
	}
	[[nodiscard]] _seq_list_const_iterator operator++(int)
	{
		assert(_verify_valid(_cur + 1)); // 非尾后迭代器

		return _seq_list_const_iterator(_cur++, _data);
	}

	_seq_list_const_iterator &operator--()
	{
		assert(_verify_valid(_cur - 1)); // 非首元素的迭代器

		--_cur;
		return *this;
	}
	[[nodiscard]] _seq_list_const_iterator operator--(int)
	{
		assert(_verify_valid(_cur - 1)); // 非首元素的迭代器

		return _seq_list_const_iterator(_cur--, _data);
	}

	_seq_list_const_iterator &operator+=(difference_type off)
	{
		assert(_verify_valid(_cur + off));

		_cur += off;
		return *this;
	}

	_seq_list_const_iterator &operator-=(difference_type off)
	{
		assert(_verify_valid(_cur - off));

		return operator+=(-off);
	}

	[[nodiscard]] _seq_list_const_iterator operator-(difference_type off) const
	{
		assert(_verify_valid(_cur - off));

		return *this + (-off);
	}

	// 非成员移动操作
	template <typename Ty_, typename Alloc_>
	[[nodiscard]] friend _seq_list_const_iterator<Ty_, Alloc_> operator+(
		_seq_list_const_iterator<Ty_, Alloc_> it,
		typename _seq_list_const_iterator<Ty_, Alloc_>::difference_type step)
	{
		assert(it._verify_valid(it._cur + step));

		return _seq_list_const_iterator(it._cur + step, it._data);
	}

	template <typename Ty_, typename Alloc_>
	[[nodiscard]] friend _seq_list_const_iterator<Ty_, Alloc_> operator+(
		typename _seq_list_const_iterator<Ty_, Alloc_>::difference_type step,
		_seq_list_const_iterator<Ty_, Alloc_> it)
	{
		assert(it._verify_valid(it._cur + step));

		return it + step;
	}

	template <typename Ty_, typename Alloc_>
	[[nodiscard]] friend
		typename _seq_list_typename<Ty_, Alloc_>::difference_type
		operator-(
			const _seq_list_const_iterator<Ty_, Alloc_> &left,
			const _seq_list_const_iterator<Ty_, Alloc_> &right)
	{
		assert(left._verify_compare(right));

		return left._cur - right._cur;
	}

	// 访问元素操作
	[[nodiscard]] const value_type &operator*() const
	{
		return *_cur;
	}

	[[nodiscard]] const value_type *operator->() const
	{
		return _cur;
	}

	// 指向元素的指针
	value_type *_cur;
	const _data_type *_data;

	// 辅助函数
	// 判断指针是否合法
	[[nodiscard]] bool _verify_valid(value_type *off) const
	{
		return off >= _data->base && off <= _data->base + _data->size;
	}
	// 判断两迭代器是否指向同一容器
	[[nodiscard]] bool _verify_compare(const _seq_list_const_iterator &other) const
	{
		return _data == other._data;
	}

}; // class _SeqList_const_iterator<>

// 顺序表迭代器
template <typename Ty, typename Alloc>
struct _seq_list_iterator : public _seq_list_const_iterator<Ty, Alloc>
{
	// 类型定义
	using iterator_category = std::random_access_iterator_tag;

	using value_type = typename _seq_list_typename<Ty, Alloc>::value_type;
	using pointer = typename _seq_list_typename<Ty, Alloc>::pointer;
	using reference = typename _seq_list_typename<Ty, Alloc>::reference;
	using difference_type = typename _seq_list_typename<Ty, Alloc>::difference_type;

	using _data_type = typename _seq_list_typename<Ty, Alloc>::_data_type;

	// 构造操作
	_seq_list_iterator(value_type *cur, const _data_type *data) : _seq_list_const_iterator<Ty, Alloc>(cur, data) {}

	// 移动操作
	_seq_list_iterator &operator++()
	{
		assert((_seq_list_const_iterator<Ty, Alloc>::_verify_valid(_seq_list_const_iterator<Ty, Alloc>::_cur + 1))); // 非尾后迭代器

		++_seq_list_const_iterator<Ty, Alloc>::_cur;
		return *this;
	}
	[[nodiscard]] _seq_list_iterator operator++(int)
	{
		assert((_seq_list_const_iterator<Ty, Alloc>::_verify_valid(_seq_list_const_iterator<Ty, Alloc>::_cur + 1))); // 非尾后迭代器

		return _seq_list_iterator(_seq_list_const_iterator<Ty, Alloc>::_cur++, _seq_list_const_iterator<Ty, Alloc>::_data);
	}

	_seq_list_iterator &operator--()
	{
		assert((_seq_list_const_iterator<Ty, Alloc>::_verify_valid(_seq_list_const_iterator<Ty, Alloc>::_cur - 1))); // 非首元素迭代器

		--_seq_list_const_iterator<Ty, Alloc>::_cur;
		return *this;
	}
	[[nodiscard]] _seq_list_iterator operator--(int)
	{
		assert((_seq_list_const_iterator<Ty, Alloc>::_verify_valid(_seq_list_const_iterator<Ty, Alloc>::_cur - 1))); // 非首元素迭代器

		return _seq_list_iterator(_seq_list_const_iterator<Ty, Alloc>::_cur--, _seq_list_const_iterator<Ty, Alloc>::_data);
	}

	[[nodiscard]] _seq_list_iterator operator-(difference_type off)
	{
		assert((_seq_list_const_iterator<Ty, Alloc>::_verify_valid(_seq_list_const_iterator<Ty, Alloc>::_cur - off)));

		return *this + (-off);
	}

	_seq_list_iterator &operator+=(difference_type off)
	{
		assert((_seq_list_const_iterator<Ty, Alloc>::_verify_valid(_seq_list_const_iterator<Ty, Alloc>::_cur + off)));

		_seq_list_const_iterator<Ty, Alloc>::_cur += off;
		return *this;
	}

	_seq_list_iterator &operator-=(difference_type off)
	{
		assert((_seq_list_const_iterator<Ty, Alloc>::_verify_valid(_seq_list_const_iterator<Ty, Alloc>::_cur - off)));

		return operator+=(-off);
	}

	// 非成员移动操作
	template <typename Ty_, typename Alloc_>
	[[nodiscard]] friend _seq_list_iterator<Ty_, Alloc_> operator+(
		_seq_list_iterator<Ty_, Alloc_> it,
		typename _seq_list_iterator<Ty_, Alloc_>::difference_type off)
	{
		assert((it._verify_valid(it._cur + off)));

		return _seq_list_iterator<Ty_, Alloc_>(it._cur + off, it._data);
	}

	template <typename Ty_, typename Alloc_>
	[[nodiscard]] friend _seq_list_iterator<Ty_, Alloc_> operator+(
		typename _seq_list_iterator<Ty_, Alloc_>::difference_type off,
		_seq_list_iterator<Ty_, Alloc_> it)
	{
		assert((it._verify_valid(it._cur + off)));

		return it + off;
	}

	// 访问元素操作
	[[nodiscard]] Ty &operator*() const
	{
		return *_seq_list_const_iterator<Ty, Alloc>::_cur;
	}
	[[nodiscard]] Ty *operator->() const
	{
		return _seq_list_const_iterator<Ty, Alloc>::_cur;
	}

}; // class _SeqList_iterator<>

// 顺序表
template <typename Ty, typename Alloc = std::allocator<Ty>>
class seq_list
{
public: // 类型定义
	using allocator_type = typename _seq_list_typename<Ty, Alloc>::allocator_type;

	using value_type = typename _seq_list_typename<Ty, Alloc>::value_type;
	using pointer = typename _seq_list_typename<Ty, Alloc>::pointer;
	using reference = typename _seq_list_typename<Ty, Alloc>::reference;
	using const_pointer = typename _seq_list_typename<Ty, Alloc>::const_pointer;
	using const_reference = typename _seq_list_typename<Ty, Alloc>::const_reference;

	using size_type = typename _seq_list_typename<Ty, Alloc>::size_type;
	using difference_type = typename _seq_list_typename<Ty, Alloc>::difference_type;

	using iterator = _seq_list_iterator<Ty, Alloc>;
	using const_iterator = _seq_list_const_iterator<Ty, Alloc>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	using _data_type = typename _seq_list_typename<Ty, Alloc>::_data_type;

public: // 构造操作
	seq_list() noexcept(noexcept(allocator_type()))
	{
		_init_alloc();
	}

	explicit seq_list(const allocator_type &alloc) noexcept : _allocator(alloc)
	{
		_init_alloc();
	}

	seq_list(size_type count, const value_type &value, const allocator_type &alloc = allocator_type()) : _allocator(alloc)
	{
		insert(begin(), count, value);
	}

	explicit seq_list(size_type count, const allocator_type &alloc = allocator_type()) : _allocator(alloc)
	{
		if (count == 0)
			_init_alloc();
		else
			resize(count);
	}

	template <typename InputIt, typename = std::enable_if_t<std::is_same_v<
									typename std::iterator_traits<InputIt>::value_type,
									value_type>>>
	seq_list(InputIt first, InputIt last, const allocator_type &alloc = allocator_type()) : _allocator(alloc)
	{
		insert(begin(), first, last);
	}

	seq_list(const seq_list &other)
	{
		operator=(other);
	}
	seq_list(const seq_list &other, const allocator_type &alloc) : _allocator(alloc)
	{
		operator=(other);
	}

	seq_list(seq_list &&other) noexcept
	{
		operator=(std::move(other));
	}
	seq_list(seq_list &&other, const allocator_type &alloc) : _allocator(alloc)
	{
		operator=(std::move(other));
	}

	seq_list(std::initializer_list<Ty> ilist, const allocator_type &alloc = allocator_type()) : _allocator(alloc)
	{
		insert(begin(), ilist);
	}

	seq_list &operator=(const seq_list &other)
	{
		// 分配器
		std::allocator_traits<allocator_type>::select_on_container_copy_construction(other.get_allocator());

		insert(begin(), other.begin(), other.end());

		return *this;
	}
	seq_list &operator=(seq_list &&other)
	{
		_data = std::move(other._data);

		other._data.base = nullptr;
		other._data.size = other._data.alloc_size = 0;

		return *this;
	}

	~seq_list()
	{
		clear();
		std::allocator_traits<allocator_type>::deallocate(_allocator, _data.base, _data.alloc_size);
		_data.base = nullptr;
		_data.alloc_size = 0;
	}

	// 非成员比较操作
	template <typename Ty_, typename Alloc_>
	[[nodiscard]] friend bool operator==(const seq_list<Ty_, Alloc_> &left, const seq_list<Ty_, Alloc_> &right)
	{
		return std::equal(left.cbegin(), left.cend(), right.cbegin(), right.cend());
	}

	template <typename Ty_, typename Alloc_>
	[[nodiscard]] friend bool operator!=(const seq_list<Ty_, Alloc_> &left, const seq_list<Ty_, Alloc_> &right)
	{
		return !(left == right);
	}

	template <typename Ty_, typename Alloc_>
	[[nodiscard]] friend bool operator<(const seq_list<Ty_, Alloc_> &left, const seq_list<Ty_, Alloc_> &right)
	{
		return std::lexicographical_compare(left.begin(), left.end(), right.begin(), right.end());
	}

	template <typename Ty_, typename Alloc_>
	[[nodiscard]] friend bool operator<=(const seq_list<Ty_, Alloc_> &left, const seq_list<Ty_, Alloc_> &right)
	{
		return !(left > right);
	}

	template <typename Ty_, typename Alloc_>
	[[nodiscard]] friend bool operator>(const seq_list<Ty_, Alloc_> &left, const seq_list<Ty_, Alloc_> &right)
	{
		return right < left;
	}

	template <typename Ty_, typename Alloc_>
	[[nodiscard]] friend bool operator>=(const seq_list<Ty_, Alloc_> &left, const seq_list<Ty_, Alloc_> &right)
	{
		return !(left < right);
	}

	// 替换容器的内容
	seq_list &assign(size_type count, const value_type &value)
	{
		clear();
		insert(begin(), count, value);

		return *this;
	}
	template <typename InputIt, typename = std::enable_if_t<std::is_same_v<
									typename std::iterator_traits<InputIt>::value_type,
									value_type>>>
	seq_list &assign(InputIt first, InputIt last)
	{
		clear();
		insert(begin(), first, last);

		return *this;
	}

	seq_list &assign(std::initializer_list<value_type> ilist)
	{
		clear();
		insert(begin(), ilist);

		return *this;
	}

	// 返回关联的分配器
	[[nodiscard]] allocator_type get_allocator() const { return _allocator; }

public: // 迭代器
	[[nodiscard]] iterator begin() noexcept { return iterator(_data.base, &_data); }
	[[nodiscard]] const_iterator begin() const noexcept { return cbegin(); }
	[[nodiscard]] const_iterator cbegin() const noexcept { return const_iterator(_data.base, &_data); }

	[[nodiscard]] iterator end() noexcept { return iterator(_data.base + _data.size, &_data); }
	[[nodiscard]] const_iterator end() const noexcept { return cend(); }
	[[nodiscard]] const_iterator cend() const noexcept { return const_iterator(_data.base + _data.size, &_data); }

	[[nodiscard]] reverse_iterator rbegin() noexcept { return std::make_reverse_iterator(end()); }
	[[nodiscard]] const_reverse_iterator rbegin() const noexcept { return crbegin(); }
	[[nodiscard]] const_reverse_iterator crbegin() const noexcept { return std::make_reverse_iterator(cend()); }

	[[nodiscard]] reverse_iterator rend() noexcept { return std::make_reverse_iterator(begin()); }
	[[nodiscard]] const_reverse_iterator rend() const noexcept { return crend(); }
	[[nodiscard]] const_reverse_iterator crend() const noexcept { return std::make_reverse_iterator(cbegin()); }

public: // 元素访问
	[[nodiscard]] value_type &at(size_type pos)
	{
		if (pos >= 0 && pos < _data.size)
			return _data.base[pos];
		else
			throw std::out_of_range("下标越界");
	}
	[[nodiscard]] const value_type &at(size_type pos) const
	{
		if (pos >= 0 && pos < _data.size)
			return _data.base[pos];
		else
			throw std::out_of_range("下标越界");
	}

	[[nodiscard]] value_type &operator[](size_type pos)
	{
		assert(pos >= 0 && pos < _data.size);

		return _data.base[pos];
	}
	[[nodiscard]] const value_type &operator[](size_type pos) const
	{
		assert(pos >= 0 && pos < _data.size);

		return _data.base[pos];
	}

	[[nodiscard]] value_type &front() { return *begin(); }
	[[nodiscard]] const value_type &front() const { return *cbegin(); }

	[[nodiscard]] value_type &back() { return *(end() - 1); }
	[[nodiscard]] const value_type &back() const { return *(cend() - 1); }

private: // 辅助函数
	// 分配初始空间
	void _init_alloc(size_type count = _INIT_ALLOC_SIZE)
	{
		assert(count >= 1);

		_data.base = std::allocator_traits<allocator_type>::allocate(_allocator, count);
		_data.alloc_size = count;
	}

	// 扩展（收缩）空间至刚好为 count
	void _re_alloc(size_type count)
	{
		assert(count >= _data.size);

		if (count == 0)
		{
			std::allocator_traits<allocator_type>::deallocate(_allocator, _data.base, _data.alloc_size);
			_data.alloc_size = 0;
			_data.base = nullptr;
			return;
		}

		value_type *tmp = std::allocator_traits<allocator_type>::allocate(_allocator, count);
		std::uninitialized_move_n(_data.base, _data.size, tmp);
		std::allocator_traits<allocator_type>::deallocate(_allocator, _data.base, _data.alloc_size);
		_data.base = tmp;
		_data.alloc_size = count;
	}

	// 将分配空间扩展到至少可容纳 count 个元素，erase 为真则擦除所有元素
	template <bool erase = false>
	void _inc_alloc(size_type count)
	{
		if (_data.alloc_size == 0) // 未分配任何空间
			_init_alloc((count / _INIT_ALLOC_SIZE + 1) * _INIT_ALLOC_SIZE);
		else
		{
			if constexpr (erase) // 擦除所有元素
				clear();

			if (count > _data.alloc_size) // 需分配更大空间
				_re_alloc((count / _data.alloc_size + 1) * _data.alloc_size);
			// count <= _alloc_size 则不做任何事
		}
	}

public: // 容量
	// 检查线性表是否为空
	[[nodiscard]] bool empty() const noexcept { return !static_cast<bool>(_data.size); }

	// 返回线性表的元素数
	[[nodiscard]] size_type size() const noexcept { return _data.size; }

	// 返回此线性表可保有元素的最大值
	[[nodiscard]] size_type max_size() const noexcept { return std::numeric_limits<size_type>::max(); }

	// 返回当前分配存储的容量
	[[nodiscard]] size_type capacity() const noexcept { return _data.alloc_size; }

	// 增大线性表的容量至 new_cap
	seq_list &reserve(size_type new_cap)
	{
		if (new_cap > max_size())
			throw std::length_error("预分配的容量过大");
		else if (new_cap > _data.alloc_size)
			_re_alloc(new_cap);
		// new_cap <= _alloc_size : 不做任何事

		return *this;
	}

	// 移除未使用的容量
	seq_list &shrink_to_fit()
	{
		_re_alloc(_data.size);
		return *this;
	}

public: // 修改器
	// 移除所有元素
	seq_list &clear() noexcept
	{
		if (_data.size != 0)
		{
			std::destroy_n(_data.base, _data.size);
			_data.size = 0;
		}

		return *this;
	}

private: // 辅助函数
	// 为 count 个插入的元素预留空间
	// 返回值：该预留空间的指针
	value_type *_enlarge_and_move(const_iterator pos, size_type count)
	{
		if (capacity() == 0)
		{
			_inc_alloc(count);
			return _data.base;
		}

		difference_type npos = std::distance(cbegin(), pos);
		_inc_alloc(_data.size + count);
		value_type *pt = _data.base + npos;

		std::uninitialized_move(
			std::make_reverse_iterator(_data.base + _data.size),
			std::make_reverse_iterator(pt),
			std::make_reverse_iterator(_data.base + _data.size + count));

		return pt;
	}

public:
	// 插入元素到容器中指定位置
	seq_list &insert(const_iterator pos, const value_type &value)
	{
		assert(_valid_iterator(pos));

		emplace(pos, value);

		return *this;
	}
	seq_list &insert(const_iterator pos, value_type &&value)
	{
		assert(_valid_iterator(pos));

		emplace(pos, std::move(value));

		return *this;
	}
	seq_list &insert(const_iterator pos, size_type count, const value_type &value)
	{
		assert(_valid_iterator(pos));
		assert(count >= 1);

		value_type *pt = _enlarge_and_move(pos, count);
		for (size_t i = 0; i < count; ++i, ++pt)
			std::allocator_traits<allocator_type>::construct(_allocator, pt, value);
		_data.size += count;

		return *this;
	}
	template <typename InputIt,
			  typename = std::enable_if_t<std::is_same_v<
				  typename std::iterator_traits<InputIt>::value_type,
				  value_type>>>
	seq_list &insert(const_iterator pos, InputIt first, InputIt last)
	{
		assert(_valid_iterator(pos));

		size_type count = std::distance(first, last);

		value_type *pt = _enlarge_and_move(pos, count);
		for (; first < last; ++first, ++pt)
			std::allocator_traits<allocator_type>::construct(_allocator, pt, *first);
		_data.size += count;

		return *this;
	}
	seq_list &insert(const_iterator pos, std::initializer_list<value_type> ilist)
	{
		assert(_valid_iterator(pos));

		insert(pos, ilist.begin(), ilist.end());

		return *this;
	}

	// 原位构造元素
	template <typename... Args>
	seq_list &emplace(const_iterator pos, Args &&... args)
	{
		assert(_valid_iterator(pos));

		std::allocator_traits<allocator_type>::construct(
			_allocator, _enlarge_and_move(pos, 1), std::forward<Args>(args)...);

		++_data.size;

		return *this;
	}

	//移除指定元素
	seq_list &erase(const_iterator pos)
	{
		assert(_valid_iterator(pos));
		assert(_valid_iterator(pos + 1)); // pos 不为尾后迭代器

		erase(pos, pos + 1);

		return *this;
	}
	seq_list &erase(const_iterator first, const_iterator last)
	{
		assert(_valid_iterator(first));
		assert(_valid_iterator(last));
		assert(first <= last);

		if (first != last)
		{
			std::destroy(first, last);
			std::uninitialized_move(last._cur, end()._cur, first._cur);
			_data.size -= std::distance(first, last);
		} // first == last : 不做任何事

		return *this;
	}

	// 将指定元素插入到容器尾
	seq_list &push_back(const value_type &value)
	{
		insert(end(), value);

		return *this;
	}
	seq_list &push_back(value_type &&value)
	{
		insert(end(), std::move(value));

		return *this;
	}

	// 在容器尾原位构造元素
	template <typename... Args>
	seq_list &emplace_back(Args &&... args)
	{
		emplace(end(), std::forward<Args>(args)...);

		return *this;
	}

	// 移除容器最末元素
	seq_list &pop_back()
	{
		assert(_data.size >= 1);

		erase(end() - 1);

		return *this;
	}

	// 重设容器大小以容纳 count 个元素
	seq_list &resize(size_type count)
	{
		if (count > _data.size)
		{
			_inc_alloc(count);
			std::uninitialized_default_construct(_data.base + _data.size, _data.base + count);
			_data.size = count;
		}
		else
			erase(begin() + count, end());

		return *this;
	}
	seq_list &resize(size_type count, const value_type &value)
	{
		if (count > _data.size)
			insert(end(), count - _data.size, value);
		else
			erase(begin() + count, end());

		return *this;
	}

	// 与 other 交换内容
	seq_list &swap(seq_list &other) noexcept(std::allocator_traits<allocator_type>::propagate_on_container_swap::value || std::allocator_traits<allocator_type>::is_always_equal::value)
	{
		std::swap(_data, other._data);

		if (std::allocator_traits<allocator_type>::propagate_on_container_swap::value)
			_swap_adl(_allocator, other._allocator); // 交换分配器

		return *this;
	}

private:				// 私有数据
	_data_type _data{}; // 基址与元素数量

	allocator_type _allocator{}; // 分配器

	static constexpr size_type _INIT_ALLOC_SIZE = 10; // 初始化时预分配空间容量

private: // 辅助函数
	// 判断迭代器是否合法
	bool _valid_iterator(const_iterator it)
	{
		return std::addressof(*it) >= _data.base && std::addressof(*it) <= _data.base + _data.size;
	}

}; // class SeqList<>

// swap() 的 SeqList 特化
template <typename Ty, typename Alloc>
inline void swap(seq_list<Ty, Alloc> &left, seq_list<Ty, Alloc> &right) noexcept(noexcept(left.swap(right)))
{
	left.swap(right);
}

} // namespace ds
