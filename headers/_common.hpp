// DataStructureCore.hpp : Data Structure �⹲��ͷ�ļ�
//

#pragma once

#include <memory>
#include <cassert>
#include <type_traits>
#include <initializer_list>
#include <iterator>

namespace ds {

// ʹ�� ADL ���ҵ� swap ����
// ���� swap ������Ա��
template <typename Ty> inline
void _swap_adl(Ty &left, Ty &right) noexcept(noexcept(swap(left, right))) {
	swap(left, right);
}

} // namespace ds