// SeqList.hpp : ˳���
//

#pragma once

#include <algorithm>
#include <limits>

#include "_common.hpp"


namespace ds {

// ���Ͷ���
template <typename Ty, typename Alloc>
struct _SeqList_typename {
	using allocator_type = Alloc;

	using value_type = Ty;
	static_assert(std::is_same_v<value_type,
		typename std::allocator_traits<allocator_type>::value_type>,
		"δ������Ϊ��allocator_type::value_type �� Ty ��ͬ");
	using pointer = typename std::allocator_traits<allocator_type>::pointer;
	using reference = value_type & ;
	using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;
	using const_reference = const value_type &;

	using size_type = size_t;
	using difference_type = ptrdiff_t;

	using _data_type = struct {
		value_type *base;		// �洢�ռ��ַ
		size_type size;			// ��ǰԪ������
		size_type alloc_size;	// Ԥ����ռ�����
	};
};

// ˳���� const ������
template <typename Ty, typename Alloc>
struct _SeqList_const_iterator {
// ���Ͷ���
	using iterator_category = std::random_access_iterator_tag;

	using value_type = typename _SeqList_typename<Ty, Alloc>::value_type;
	using pointer = typename _SeqList_typename<Ty, Alloc>::pointer;
	using reference = typename _SeqList_typename<Ty, Alloc>::reference;
	using difference_type = typename _SeqList_typename<Ty, Alloc>::difference_type;

	using _data_type = typename _SeqList_typename<Ty, Alloc>::_data_type;

// �������
	_SeqList_const_iterator(value_type * cur, const _data_type *data) :_cur(cur), _data(data) {
		assert(_data != nullptr);
		assert(_verify_valid(_cur));
	}


// �ǳ�Ա�Ƚϲ���
	template <typename Ty_, typename Alloc_>[[nodiscard]] friend
	bool operator<(
		const _SeqList_const_iterator<Ty_, Alloc_> &left,
		const _SeqList_const_iterator<Ty_, Alloc_> &right)
	{
		assert(left._verify_compare(right));

		return left._cur < right._cur;
	}
	template <typename Ty_, typename Alloc_>[[nodiscard]] friend
	bool operator<=(
		const _SeqList_const_iterator<Ty_, Alloc_> &left,
		const _SeqList_const_iterator<Ty_, Alloc_> &right)
	{
		assert(left._verify_compare(right));

		return !(right < left);
	}

	template <typename Ty_, typename Alloc_>[[nodiscard]] friend
	bool operator>(
		const _SeqList_const_iterator<Ty_, Alloc_> &left,
		const _SeqList_const_iterator<Ty_, Alloc_> &right)
	{
		assert(left._verify_compare(right));

		return right < left;
	}
	template <typename Ty_, typename Alloc_>[[nodiscard]] friend
	bool operator>=(
		const _SeqList_const_iterator<Ty_, Alloc_> &left,
		const _SeqList_const_iterator<Ty_, Alloc_> &right)
	{
		assert(left._verify_compare(right));

		return !(left < right);
	}

	template <typename Ty_, typename Alloc_>[[nodiscard]] friend
	bool operator==(
		const _SeqList_const_iterator<Ty_, Alloc_> &left,
		const _SeqList_const_iterator<Ty_, Alloc_> &right)
	{
		assert(left._verify_compare(right));

		return left._cur == right._cur;
	}
	template <typename Ty_, typename Alloc_>[[nodiscard]] friend
	bool operator!=(
		const _SeqList_const_iterator<Ty_, Alloc_> &left,
		const _SeqList_const_iterator<Ty_, Alloc_> &right)
	{
		assert(left._verify_compare(right));

		return !(left == right);
	}

// �ƶ�����
	_SeqList_const_iterator &operator++() {
		assert(_verify_valid(_cur + 1));	// ��β�������

		++_cur;
		return *this;
	}
	[[nodiscard]] _SeqList_const_iterator operator++(int) {
		assert(_verify_valid(_cur + 1));	// ��β�������

		return _SeqList_const_iterator(_cur++, _data);
	}

	_SeqList_const_iterator &operator--() {
		assert(_verify_valid(_cur - 1));	// ����Ԫ�صĵ�����

		--_cur;
		return *this;
	}
	[[nodiscard]] _SeqList_const_iterator operator--(int) {
		assert(_verify_valid(_cur - 1));	// ����Ԫ�صĵ�����

		return _SeqList_const_iterator(_cur--, _data);
	}

	_SeqList_const_iterator &operator+=(difference_type off) {
		assert(_verify_valid(_cur + off));

		_cur += off;
		return *this;
	}

	_SeqList_const_iterator &operator-=(difference_type off) {
		assert(_verify_valid(_cur - off));

		return operator+=(-off);
	}

	[[nodiscard]] _SeqList_const_iterator operator-(difference_type off) const {
		assert(_verify_valid(_cur - off));

		return *this + (-off);
	}

// �ǳ�Ա�ƶ�����
	template <typename Ty_, typename Alloc_> [[nodiscard]] friend
	_SeqList_const_iterator<Ty_, Alloc_> operator+(
		_SeqList_const_iterator<Ty_, Alloc_> it,
		typename _SeqList_const_iterator<Ty_, Alloc_>::difference_type step)
	{
		assert(it._verify_valid(it._cur + step));

		return _SeqList_const_iterator(it._cur + step, it._data);
	}

	template <typename Ty_, typename Alloc_> [[nodiscard]] friend
	_SeqList_const_iterator<Ty_, Alloc_> operator+(
		typename _SeqList_const_iterator<Ty_, Alloc_>::difference_type step,
		_SeqList_const_iterator<Ty_, Alloc_> it)
	{
		assert(it._verify_valid(it._cur + step));

		return it + step;
	}

	template <typename Ty_, typename Alloc_> [[nodiscard]] friend
	typename _SeqList_typename<Ty_, Alloc_>::difference_type operator-(
		const _SeqList_const_iterator<Ty_, Alloc_> &left,
		const _SeqList_const_iterator<Ty_, Alloc_> &right)
	{
		assert(left._verify_compare(right));

		return left._cur - right._cur;
	}

// ����Ԫ�ز���
	[[nodiscard]] const value_type &operator*() const {
		return *_cur;
	}

	[[nodiscard]] const value_type *operator->() const {
		return _cur;
	}

// ָ��Ԫ�ص�ָ��
	value_type *_cur;
	const _data_type *_data;

// ��������
	// �ж�ָ���Ƿ�Ϸ�
	[[nodiscard]] bool _verify_valid(value_type *off) const {
		return off >= _data->base && off <= _data->base + _data->size;
	}
	// �ж����������Ƿ�ָ��ͬһ����
	[[nodiscard]] bool _verify_compare(const _SeqList_const_iterator &other) const {
		return _data == other._data;
	}

};	// class _SeqList_const_iterator<>



// ˳��������
template <typename Ty, typename Alloc>
struct _SeqList_iterator : public _SeqList_const_iterator<Ty, Alloc> {
// ���Ͷ���
	using iterator_category = std::random_access_iterator_tag;

	using value_type = typename _SeqList_typename<Ty, Alloc>::value_type;
	using pointer = typename _SeqList_typename<Ty, Alloc>::pointer;
	using reference = typename _SeqList_typename<Ty, Alloc>::reference;
	using difference_type = typename _SeqList_typename<Ty, Alloc>::difference_type;

	using _data_type = typename _SeqList_typename<Ty, Alloc>::_data_type;

// �������
	_SeqList_iterator(value_type *cur, const _data_type *data) : _SeqList_const_iterator<Ty, Alloc>(cur,data) {}

// �ƶ�����
	_SeqList_iterator &operator++() {
		assert((_SeqList_const_iterator<Ty, Alloc>::_verify_valid(_SeqList_const_iterator<Ty, Alloc>::_cur + 1)));	// ��β�������

		++_SeqList_const_iterator<Ty, Alloc>::_cur;
		return *this;
	}
	[[nodiscard]] _SeqList_iterator operator++(int) {
		assert((_SeqList_const_iterator<Ty, Alloc>::_verify_valid(_SeqList_const_iterator<Ty, Alloc>::_cur + 1)));	// ��β�������

		return _SeqList_iterator(_SeqList_const_iterator<Ty, Alloc>::_cur++, _SeqList_const_iterator<Ty, Alloc>::_data);
	}

	_SeqList_iterator &operator--() {
		assert((_SeqList_const_iterator<Ty, Alloc>::_verify_valid(_SeqList_const_iterator<Ty, Alloc>::_cur - 1)));	// ����Ԫ�ص�����

		--_SeqList_const_iterator<Ty, Alloc>::_cur;
		return *this;
	}
	[[nodiscard]] _SeqList_iterator operator--(int) {
		assert((_SeqList_const_iterator<Ty, Alloc>::_verify_valid(_SeqList_const_iterator<Ty, Alloc>::_cur - 1)));	// ����Ԫ�ص�����

		return _SeqList_iterator(_SeqList_const_iterator<Ty, Alloc>::_cur--, _SeqList_const_iterator<Ty, Alloc>::_data);
	}

	[[nodiscard]] _SeqList_iterator operator-(difference_type off) {
		assert((_SeqList_const_iterator<Ty, Alloc>::_verify_valid(_SeqList_const_iterator<Ty, Alloc>::_cur - off)));

		return *this + (-off);
	}

	_SeqList_iterator &operator+=(difference_type off) {
		assert((_SeqList_const_iterator<Ty, Alloc>::_verify_valid(_SeqList_const_iterator<Ty, Alloc>::_cur + off)));

		_SeqList_const_iterator<Ty, Alloc>::_cur += off;
		return *this;
	}

	_SeqList_iterator &operator-=(difference_type off) {
		assert((_SeqList_const_iterator<Ty, Alloc>::_verify_valid(_SeqList_const_iterator<Ty, Alloc>::_cur - off)));

		return operator+=(-off);
	}

// �ǳ�Ա�ƶ�����
	template <typename Ty_, typename Alloc_>[[nodiscard]] friend
	_SeqList_iterator<Ty_, Alloc_> operator+(
		_SeqList_iterator<Ty_, Alloc_> it,
		typename _SeqList_iterator<Ty_, Alloc_>::difference_type off)
	{
		assert((it._verify_valid(it._cur + off)));

		return _SeqList_iterator<Ty_, Alloc_>(it._cur + off, it._data);
	}

	template <typename Ty_, typename Alloc_>[[nodiscard]] friend
	_SeqList_iterator<Ty_, Alloc_> operator+(
		typename _SeqList_iterator<Ty_, Alloc_>::difference_type off,
		_SeqList_iterator<Ty_, Alloc_> it)
	{
		assert((it._verify_valid(it._cur + off)));

		return it + off;
	}

// ����Ԫ�ز���
	[[nodiscard]] Ty &operator*() const {
		return *_SeqList_const_iterator<Ty, Alloc>::_cur;
	}
	[[nodiscard]] Ty *operator->() const {
		return _SeqList_const_iterator<Ty, Alloc>::_cur;
	}

};	// class _SeqList_iterator<>




// ˳���
template <typename Ty, typename Alloc = std::allocator<Ty>>
class SeqList {
public: // ���Ͷ���
	using allocator_type = typename _SeqList_typename<Ty, Alloc>::allocator_type;

	using value_type = typename _SeqList_typename<Ty, Alloc>::value_type;
	using pointer = typename _SeqList_typename<Ty, Alloc>::pointer;
	using reference = typename _SeqList_typename<Ty, Alloc>::reference;
	using const_pointer = typename _SeqList_typename<Ty, Alloc>::const_pointer;
	using const_reference = typename _SeqList_typename<Ty, Alloc>::const_reference;

	using size_type = typename _SeqList_typename<Ty, Alloc>::size_type;
	using difference_type = typename _SeqList_typename<Ty, Alloc>::difference_type;

	using iterator = _SeqList_iterator<Ty, Alloc>;
	using const_iterator = _SeqList_const_iterator<Ty, Alloc>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	using _data_type = typename _SeqList_typename<Ty, Alloc>::_data_type;


public: // �������
	SeqList() noexcept(noexcept(allocator_type())) {
		_init_alloc();
	}

	explicit SeqList(const allocator_type &alloc) noexcept : _allocator(alloc) {
		_init_alloc();
	}

	SeqList(size_type count, const value_type &value, const allocator_type &alloc = allocator_type()) : _allocator(alloc) 
	{
		insert(begin(), count, value);
	}

	explicit SeqList(size_type count, const allocator_type &alloc = allocator_type()) : _allocator(alloc)
	{
		if (count == 0)
			_init_alloc();
		else
			resize(count);
	}

	template <typename InputIt, typename = std::enable_if_t<std::is_same_v<
		typename std::iterator_traits<InputIt>::value_type,
		value_type
	>>>
	SeqList(InputIt first, InputIt last, const allocator_type &alloc = allocator_type()) : _allocator(alloc) 
	{
		insert(begin(), first, last);
	}

	SeqList(const SeqList &other) {
		operator=(other);
	}
	SeqList(const SeqList &other, const allocator_type &alloc) : _allocator(alloc) {
		operator=(other);
	}

	SeqList(SeqList &&other) noexcept {
		operator=(std::move(other));
	}
	SeqList(SeqList &&other, const allocator_type &alloc) : _allocator(alloc) {
		operator=(std::move(other));
	}

	SeqList(std::initializer_list<Ty> ilist, const allocator_type &alloc = allocator_type()) : _allocator(alloc) 
	{
		insert(begin(), ilist);
	}

	SeqList &operator=(const SeqList &other) {
		// ������
		std::allocator_traits<allocator_type>::select_on_container_copy_construction(other.get_allocator());

		insert(begin(), other.begin(), other.end());

		return *this;
	}
	SeqList &operator=(SeqList &&other) {
		_data = std::move(other._data);

		other._data.base = nullptr;
		other._data.size = other._data.alloc_size = 0;

		return *this;
	}

	~SeqList() {
		clear();
		std::allocator_traits<allocator_type>::deallocate(_allocator, _data.base, _data.alloc_size);
		_data.base = nullptr;
		_data.alloc_size = 0;
	}

// �ǳ�Ա�Ƚϲ���
	template <typename Ty_, typename Alloc_> [[nodiscard]] friend
	bool operator==(const SeqList<Ty_, Alloc_> &left, const SeqList<Ty_, Alloc_> &right) {
		return std::equal(left.cbegin(), left.cend(), right.cbegin(), right.cend());
	}

	template <typename Ty_, typename Alloc_> [[nodiscard]] friend
	bool operator!=(const SeqList<Ty_, Alloc_> &left, const SeqList<Ty_, Alloc_> &right) {
		return !(left == right);
	}

	template <typename Ty_, typename Alloc_> [[nodiscard]] friend
	bool operator<(const SeqList<Ty_, Alloc_> &left, const SeqList<Ty_, Alloc_> &right) {
		return std::lexicographical_compare(left.begin(), left.end(), right.begin(), right.end());
	}

	template <typename Ty_, typename Alloc_> [[nodiscard]] friend
	bool operator<=(const SeqList<Ty_, Alloc_> &left, const SeqList<Ty_, Alloc_> &right) {
		return !(left > right);
	}

	template <typename Ty_, typename Alloc_> [[nodiscard]] friend
	bool operator>(const SeqList<Ty_, Alloc_> &left, const SeqList<Ty_, Alloc_> &right) {
		return right < left;
	}

	template <typename Ty_, typename Alloc_> [[nodiscard]] friend
	bool operator>=(const SeqList<Ty_, Alloc_> &left, const SeqList<Ty_, Alloc_> &right) {
		return !(left < right);
	}

// �滻����������
	SeqList &assign(size_type count, const value_type &value) {
		clear();
		insert(begin(), count, value);

		return *this;
	}
	template <typename InputIt, typename = std::enable_if_t<std::is_same_v<
		typename std::iterator_traits<InputIt>::value_type,
		value_type
	>>>
	SeqList &assign(InputIt first, InputIt last) {
		clear();
		insert(begin(), first, last);

		return *this;
	}

	SeqList &assign(std::initializer_list<value_type> ilist) {
		clear();
		insert(begin(), ilist);

		return *this;
	}

	// ���ع����ķ�����
	[[nodiscard]] allocator_type get_allocator() const { return _allocator; }

public: // ������
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

public: // Ԫ�ط���
	[[nodiscard]] value_type &at(size_type pos) {
		if (pos >= 0 && pos < _data.size)
			return _data.base[pos];
		else
			throw std::out_of_range("�±�Խ��");
	}
	[[nodiscard]] const value_type &at(size_type pos) const {
		if (pos >= 0 && pos < _data.size)
			return _data.base[pos];
		else
			throw std::out_of_range("�±�Խ��");
	}

	[[nodiscard]] value_type &operator[](size_type pos) {
		assert(pos >= 0 && pos < _data.size);

		return _data.base[pos];
	}
	[[nodiscard]] const value_type &operator[](size_type pos) const {
		assert(pos >= 0 && pos < _data.size);

		return _data.base[pos];
	}

	[[nodiscard]] value_type &front() { return *begin(); }
	[[nodiscard]] const value_type &front() const { return *cbegin(); }

	[[nodiscard]] value_type &back() { return *(end() - 1); }
	[[nodiscard]] const value_type &back() const { return *(cend() - 1); }

private: // ��������
	// �����ʼ�ռ�
	void _init_alloc(size_type count = _INIT_ALLOC_SIZE) {
		assert(count >= 1);

		_data.base = std::allocator_traits<allocator_type>::allocate(_allocator, count);
		_data.alloc_size = count;
	}

	// ��չ���������ռ����պ�Ϊ count
	void _re_alloc(size_type count) {
		assert(count >= _data.size);

		if (count == 0) {
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

	// ������ռ���չ�����ٿ����� count ��Ԫ�أ�erase Ϊ�����������Ԫ��
	template <bool erase = false>
	void _inc_alloc(size_type count) {
		if (_data.alloc_size == 0)	// δ�����κοռ�
			_init_alloc((count / _INIT_ALLOC_SIZE + 1) * _INIT_ALLOC_SIZE);
		else {
			if constexpr (erase)	// ��������Ԫ��
				clear();

			if (count > _data.alloc_size)	// ��������ռ�
				_re_alloc((count / _data.alloc_size + 1) * _data.alloc_size);
			// count <= _alloc_size �����κ���
		}
	}
public: // ����

	// ������Ա��Ƿ�Ϊ��
	[[nodiscard]] bool empty() const noexcept { return !static_cast<bool>(_data.size); }

	// �������Ա��Ԫ����
	[[nodiscard]] size_type size() const noexcept { return _data.size; }

	// ���ش����Ա�ɱ���Ԫ�ص����ֵ
	[[nodiscard]] size_type max_size() const noexcept { return std::numeric_limits<size_type>::max(); }

	// ���ص�ǰ����洢������
	[[nodiscard]] size_type capacity() const noexcept { return _data.alloc_size; }

	// �������Ա�������� new_cap
	SeqList &reserve(size_type new_cap) {
		if (new_cap > max_size())
			throw std::length_error("Ԥ�������������");
		else if (new_cap > _data.alloc_size)
			_re_alloc(new_cap);
		// new_cap <= _alloc_size : �����κ���

		return *this;
	}

	// �Ƴ�δʹ�õ�����
	SeqList &shrink_to_fit() {
		_re_alloc(_data.size);
		return *this;
	}

public: // �޸���

	// �Ƴ�����Ԫ��
	SeqList &clear() noexcept {
		if (_data.size != 0) {
			std::destroy_n(_data.base, _data.size);
			_data.size = 0;
		}

		return *this;
	}

private: // ��������
	// Ϊ count �������Ԫ��Ԥ���ռ�
	// ����ֵ����Ԥ���ռ��ָ��
	value_type *_enlarge_and_move(const_iterator pos, size_type count) {
		if (capacity() == 0) {
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
// ����Ԫ�ص�������ָ��λ��
	SeqList &insert(const_iterator pos, const value_type &value) {
		assert(_valid_iterator(pos));
		
		emplace(pos, value);

		return *this;
	}
	SeqList &insert(const_iterator pos, value_type &&value) {
		assert(_valid_iterator(pos));

		emplace(pos, std::move(value));

		return *this;
	}
	SeqList &insert(const_iterator pos, size_type count, const value_type &value) {
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
			value_type
		>>>
	SeqList &insert(const_iterator pos, InputIt first, InputIt last) {
		assert(_valid_iterator(pos));

		size_type count = std::distance(first, last);

		value_type *pt = _enlarge_and_move(pos, count);
		for (; first < last; ++first, ++pt)
			std::allocator_traits<allocator_type>::construct(_allocator, pt, *first);
		_data.size += count;

		return *this;
	}
	SeqList &insert(const_iterator pos, std::initializer_list<value_type> ilist) {
		assert(_valid_iterator(pos));

		insert(pos, ilist.begin(), ilist.end());
		
		return *this;
	}

	// ԭλ����Ԫ��
	template <typename ...Args>
	SeqList &emplace(const_iterator pos, Args &&...args) {
		assert(_valid_iterator(pos));

		std::allocator_traits<allocator_type>::construct(
			_allocator, _enlarge_and_move(pos, 1), std::forward<Args>(args)...);

		++_data.size;

		return *this;
	}

//�Ƴ�ָ��Ԫ��
	SeqList &erase(const_iterator pos) {
		assert(_valid_iterator(pos));
		assert(_valid_iterator(pos + 1));	// pos ��Ϊβ�������

		erase(pos, pos + 1);

		return *this;
	}
	SeqList &erase(const_iterator first, const_iterator last) {
		assert(_valid_iterator(first));
		assert(_valid_iterator(last));
		assert(first <= last);

		if (first != last) {
			std::destroy(first, last);
			std::uninitialized_move(last._cur, end()._cur, first._cur);
			_data.size -= std::distance(first, last);
		} // first == last : �����κ���

		return *this;
	}

// ��ָ��Ԫ�ز��뵽����β
	SeqList &push_back(const value_type &value) {
		insert(end(), value);

		return *this;
	}
	SeqList &push_back(value_type &&value) {
		insert(end(), std::move(value));

		return *this;
	}

	// ������βԭλ����Ԫ��
	template <typename ...Args>
	SeqList &emplace_back(Args &&...args) {
		emplace(end(), std::forward<Args>(args)...);

		return *this;
	}

	// �Ƴ�������ĩԪ��
	SeqList &pop_back() {
		assert(_data.size >= 1);

		erase(end() - 1);

		return *this;
	}

// ����������С������ count ��Ԫ��
	SeqList &resize(size_type count) {
		if (count > _data.size) {
			_inc_alloc(count);
			std::uninitialized_default_construct(_data.base + _data.size, _data.base + count);
			_data.size = count;
		} else
			erase(begin() + count, end());

		return *this;
	}
	SeqList &resize(size_type count, const value_type &value) {
		if (count > _data.size)
			insert(end(), count - _data.size, value);
		else
			erase(begin() + count, end());

		return *this;
	}

	// �� other ��������
	SeqList &swap(SeqList &other) noexcept(std::allocator_traits<allocator_type>::propagate_on_container_swap::value
		|| std::allocator_traits<allocator_type>::is_always_equal::value) 
	{
		std::swap(_data, other._data);

		if (std::allocator_traits<allocator_type>::propagate_on_container_swap::value)
			_swap_adl(_allocator, other._allocator);	// ����������

		return *this;
	}
	
	
private: // ˽������
	_data_type _data{};	// ��ַ��Ԫ������

	allocator_type _allocator{};	// ������

	static constexpr size_type _INIT_ALLOC_SIZE = 10;	// ��ʼ��ʱԤ����ռ�����

private:	// ��������

	// �жϵ������Ƿ�Ϸ�
	bool _valid_iterator(const_iterator it) {
		return std::addressof(*it) >= _data.base && std::addressof(*it) <= _data.base + _data.size;
	}

}; // class SeqList<>


// swap() �� SeqList �ػ�
template <typename Ty, typename Alloc> inline
void swap(SeqList<Ty, Alloc> &left, SeqList<Ty, Alloc> &right)
	noexcept(noexcept(left.swap(right)))
{
	left.swap(right);
}

} // namespace ds

