// Stack.hpp : ջ
// 

#pragma once


#include "_common.hpp"
#include "SeqList.hpp"


namespace ds {

// ջ
// ����������
template <typename Ty, typename Container = ds::SeqList<Ty>>
class Stack {
public: // ���Ͷ���
	using container_type = Container;

	using value_type = typename container_type::value_type;
	static_assert(std::is_same_v<Ty, value_type>, "δ������Ϊ��Container::value_type �� Ty ��ͬ");
	using size_type = typename container_type::size_type;
	using reference = typename container_type::reference;
	using const_reference = typename container_type::const_reference;

public: // �������
	Stack() {}

	explicit Stack(const container_type &cont) :_container(cont) {}

	explicit Stack(container_type &&cont) : _container(std::move(cont)) {}

	Stack(const Stack &other) = default;

	Stack(Stack &&other) = default;

	template <typename Alloc, std::enable_if_t<std::uses_allocator_v<container_type, Alloc>>>
	explicit Stack(const Alloc &alloc) : _container(alloc) {}
	
	template <typename Alloc, std::enable_if_t<std::uses_allocator_v<container_type, Alloc>>>
	Stack(const container_type &cont, const Alloc &alloc) : _container(cont, alloc) {}

	template <typename Alloc, std::enable_if_t<std::uses_allocator_v<container_type, Alloc>>>
	Stack(container_type &&cont, const Alloc &alloc) : _container(std::move(cont), alloc) {}

	template <typename Alloc, std::enable_if_t<std::uses_allocator_v<container_type, Alloc>>>
	Stack(const Stack &other, const Alloc &alloc) : _container(other._container, alloc) {}

	template <typename Alloc, std::enable_if_t<std::uses_allocator_v<container_type, Alloc>>>
	Stack(Stack &&other, const Alloc &alloc) : _container(std::move(other._container, alloc)) {}

	~Stack() = default;

	Stack &operator=(const Stack &) = default;
	Stack &operator=(Stack &&) = default;

public: // �ǳ�Ա�Ƚϲ���
	template <typename Ty_, typename Container_> [[nodiscard]] friend
	bool operator==(Stack<Ty_, Container_> &left, Stack<Ty_, Container_> &right) {
		return left._container == right._container;
	}

	template <typename Ty_, typename Container_> [[nodiscard]] friend
	bool operator!=(Stack<Ty_, Container_> &left, Stack<Ty_, Container_> &right) {
		return left._container != right._container;
	}

	template <typename Ty_, typename Container_> [[nodiscard]] friend
	bool operator<(Stack<Ty_, Container_> &left, Stack<Ty_, Container_> &right) {
		return left._container < right._container;
	}

	template <typename Ty_, typename Container_> [[nodiscard]] friend
	bool operator<=(Stack<Ty_, Container_> &left, Stack<Ty_, Container_> &right) {
		return left._container <= right._container;
	}

	template <typename Ty_, typename Container_> [[nodiscard]] friend
	bool operator>(Stack<Ty_, Container_> &left, Stack<Ty_, Container_> &right) {
		return left._container > right._container;
	}

	template <typename Ty_, typename Container_> [[nodiscard]] friend
	bool operator>=(Stack<Ty_, Container_> &left, Stack<Ty_, Container_> &right) {
		return left._container >= right._container;
	}

public: // Ԫ�ط���
	[[nodiscard]] value_type &top() {
		assert(size() >= 1);

		return _container.back();
	}
	[[nodiscard]] const value_type &top() const {
		assert(size() >= 1);

		return _container.back();
	}

public: // ����
	[[nodiscard]] bool empty() const { return _container.empty(); }

	[[nodiscard]] size_type size() const { return _container.size(); }

public: //�޸���
// Ԫ����ջ
	Stack &push(const value_type &value) {
		_container.push_back(value);

		return *this;
	}
	Stack &push(value_type &&value) {
		_container.push_back(std::move(value));

		return *this;
	}

	// ��ջ��ԭλ����Ԫ��
	template <typename ...Args>
	Stack &emplace(Args &&...args) {
		_container.emplace_back(std::forward<Args>(args)...);

		return *this;
	}

	// Ԫ�س�ջ
	Stack &pop() {
		assert(size() >= 1);

		_container.pop_back();

		return *this;
	}

	// �� other �����ײ�����
	Stack &swap(Stack &other) noexcept(std::is_nothrow_swappable_v<container_type>) {
		_swap_adl(_container, other._container);

		return *this;
	}

private:
	container_type _container{};	// �ײ�����
}; // class Stack<>


// swap() �� Stack �ػ�
template <typename Ty, typename Container> inline
void swap(Stack<Ty, Container> &left, Stack<Ty, Container> &right)
	noexcept(noexcept(left.swap(right)))
{
	left.swap(right);
}

} // namespace ds


namespace std {

// uses_allocator() �� Stack �ػ�
template <typename Ty, typename Container, typename Alloc>
struct uses_allocator<ds::Stack<Ty, Container>, Alloc>
	: uses_allocator<Container, Alloc>::type {};

} // namespace std
