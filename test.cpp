#include <iostream>
#include <array>
#include <algorithm>

#include "include/avl_tree.hpp"
#include "include/seq_list.hpp"
#include "include/stack.hpp"
#include "include/b_tree.hpp"
#include "include/rb_tree.hpp"

void test_seq_list();
void test_stack();
void test_avl_tree();
void test_b_tree();
void test_rb_tree();

int main()
{
    test_seq_list();
    test_stack();
    test_avl_tree();
    test_b_tree();
    test_rb_tree();

    return 0;
}

void test_seq_list()
{
    std::cout << "-------- seq_list --------" << std::endl;

    ds::seq_list<int> list({1, 2, 3});
    list.push_back(0);
    list.erase(list.begin() + 1, list.begin() + 3);

    // 倒序输出
    for (auto it = list.rbegin(), end = list.rend(); it < end; ++it)
    {
        std::cout << *it << " ";
    }

    std::cout << "\n预期输出：0 1\n\n";
}

void test_stack()
{
    std::cout << "-------- stack --------" << std::endl;

    ds::stack<int, ds::seq_list<int>> stack;
    stack.push(1);
    stack.pop();
    stack.emplace(2);

    std::cout << stack.size() << "\n预期输出：1\n\n";
}

void test_avl_tree()
{
    std::cout << "-------- avl_tree --------" << std::endl;

    std::array<int, 5> arr({1, 3, 2, 1, 0});
    ds::avl_tree<int> tree(arr.begin(), arr.end());

    tree[0];
    tree.erase(3);

    auto it = tree.search(3);
    std::cout << (it == tree.end()) << "\n预期输出：1\n\n";
}

void test_b_tree()
{
    std::cout << "-------- b_tree --------" << std::endl;

    ds::b_tree<int, 3> tree;
    for (int i = 0; i < 10; ++i)
    {
        tree.insert(i * 3 % 11);
    }

    tree.traverse([](const int &i) {
        std::cout << i << " ";
    });

    std::cout << "\n预期输出：0 1 2 3 4 5 6 7 9 10\n\n";
}

void test_rb_tree()
{
    std::cout << "-------- rb_tree --------" << std::endl;

    std::array<int, 5> arr({1, 3, 2, 1, 0});
    ds::rb_tree<int> tree(arr.begin(), arr.end());

    for (int i : tree)
    {
        std::cout << i << " ";
    }
    std::cout << "\n预期输出：0 1 1 2 3\n";
}
