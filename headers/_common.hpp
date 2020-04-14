// DataStructureCore.hpp : Data Structure 库共有头文件
//

#pragma once

#include <memory>
#include <cassert>
#include <type_traits>
#include <initializer_list>
#include <iterator>

namespace ds {

// 使用 ADL 查找的 swap 函数
// 用于 swap 函数成员中
template <typename Ty> inline
void _swap_adl(Ty &left, Ty &right) noexcept(noexcept(swap(left, right))) {
	swap(left, right);
}

} // namespace ds