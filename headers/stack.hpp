// Stack.hpp : 栈
//

#pragma once

#include "_common.hpp"
#include "seq_list.hpp"

namespace ds
{

// 栈
// 容器适配器
template <typename Ty, typename Container = ds::seq_list<Ty>>
class stack
{
public: // 类型定义
	using container_type = Container;

	using value_type = typename container_type::value_type;
	static_assert(std::is_same_v<Ty, value_type>, "未定义行为：Container::value_type 与 Ty 不同");
	using size_type = typename container_type::size_type;
	using reference = typename container_type::reference;
	using const_reference = typename container_type::const_reference;

public: // 构造操作
	stack() {}

	explicit stack(const container_type &cont) : _container(cont) {}

	explicit stack(container_type &&cont) : _container(std::move(cont)) {}

	stack(const stack &other) = default;

	stack(stack &&other) = default;

	template <typename Alloc, std::enable_if_t<std::uses_allocator_v<container_type, Alloc>>>
	explicit stack(const Alloc &alloc) : _container(alloc) {}

	template <typename Alloc, std::enable_if_t<std::uses_allocator_v<container_type, Alloc>>>
	stack(const container_type &cont, const Alloc &alloc) : _container(cont, alloc) {}

	template <typename Alloc, std::enable_if_t<std::uses_allocator_v<container_type, Alloc>>>
	stack(container_type &&cont, const Alloc &alloc) : _container(std::move(cont), alloc) {}

	template <typename Alloc, std::enable_if_t<std::uses_allocator_v<container_type, Alloc>>>
	stack(const stack &other, const Alloc &alloc) : _container(other._container, alloc) {}

	template <typename Alloc, std::enable_if_t<std::uses_allocator_v<container_type, Alloc>>>
	stack(stack &&other, const Alloc &alloc) : _container(std::move(other._container, alloc)) {}

	~stack() = default;

	stack &operator=(const stack &) = default;
	stack &operator=(stack &&) = default;

public: // 非成员比较操作
	template <typename Ty_, typename Container_>
	[[nodiscard]] friend bool operator==(stack<Ty_, Container_> &left, stack<Ty_, Container_> &right)
	{
		return left._container == right._container;
	}

	template <typename Ty_, typename Container_>
	[[nodiscard]] friend bool operator!=(stack<Ty_, Container_> &left, stack<Ty_, Container_> &right)
	{
		return left._container != right._container;
	}

	template <typename Ty_, typename Container_>
	[[nodiscard]] friend bool operator<(stack<Ty_, Container_> &left, stack<Ty_, Container_> &right)
	{
		return left._container < right._container;
	}

	template <typename Ty_, typename Container_>
	[[nodiscard]] friend bool operator<=(stack<Ty_, Container_> &left, stack<Ty_, Container_> &right)
	{
		return left._container <= right._container;
	}

	template <typename Ty_, typename Container_>
	[[nodiscard]] friend bool operator>(stack<Ty_, Container_> &left, stack<Ty_, Container_> &right)
	{
		return left._container > right._container;
	}

	template <typename Ty_, typename Container_>
	[[nodiscard]] friend bool operator>=(stack<Ty_, Container_> &left, stack<Ty_, Container_> &right)
	{
		return left._container >= right._container;
	}

public: // 元素访问
	[[nodiscard]] value_type &top()
	{
		assert(size() >= 1);

		return _container.back();
	}
	[[nodiscard]] const value_type &top() const
	{
		assert(size() >= 1);

		return _container.back();
	}

public: // 容量
	[[nodiscard]] bool empty() const { return _container.empty(); }

	[[nodiscard]] size_type size() const { return _container.size(); }

public: //修改器
		// 元素入栈
	stack &push(const value_type &value)
	{
		_container.push_back(value);

		return *this;
	}
	stack &push(value_type &&value)
	{
		_container.push_back(std::move(value));

		return *this;
	}

	// 在栈顶原位构造元素
	template <typename... Args>
	stack &emplace(Args &&... args)
	{
		_container.emplace_back(std::forward<Args>(args)...);

		return *this;
	}

	// 元素出栈
	stack &pop()
	{
		assert(size() >= 1);

		_container.pop_back();

		return *this;
	}

	// 与 other 交换底层容器
	stack &swap(stack &other) noexcept(std::is_nothrow_swappable_v<container_type>)
	{
		_swap_adl(_container, other._container);

		return *this;
	}

private:
	container_type _container{}; // 底层容器
};								 // class Stack<>

// swap() 的 stack 特化
template <typename Ty, typename Container>
inline void swap(stack<Ty, Container> &left, stack<Ty, Container> &right) noexcept(noexcept(left.swap(right)))
{
	left.swap(right);
}

} // namespace ds

namespace std
{

// uses_allocator() 的 stack 特化
template <typename Ty, typename Container, typename Alloc>
struct uses_allocator<ds::stack<Ty, Container>, Alloc>
	: uses_allocator<Container, Alloc>::type
{
};

} // namespace std
