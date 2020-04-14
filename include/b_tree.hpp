// b_tree : B-树
//

#pragma once

#include <array>
#include <cassert>
#include <functional>

namespace ds
{

// B-树
template <typename Ty, size_t Order>
class b_tree
{
    static_assert(Order >= 3);

public:
    using value_type = Ty;

private:
    struct _node
    {
        size_t key_num;                         // 关键字个数
        _node *parent{};                        // 父亲节点
        std::array<value_type, Order> key;      // 关键字
        std::array<_node *, Order + 1> child{}; // 子树指针
    };

public:
    b_tree() = default;

    template <typename InputIt>
    b_tree(InputIt first, InputIt last)
    {
        for (; first != last; ++first)
            insert(*first);
    }

private:
    void _free_node(_node *node)
    {
        if (!node)
            return;

        for (size_t i = 0; i <= node->key_num; ++i)
            _free_node(node->child[i]);

        delete node;
    }

public:
    ~b_tree()
    {
        _free_node(_root);
    }

private:
    std::pair<bool, size_t> _find_pos(const value_type &key, _node *node) const
    {
        assert(node);
        size_t pos = 0;
        for (; pos < node->key_num; ++pos)
        {
            if (node->key[pos] < key)
                continue;
            else if (node->key[pos] > key)
                return {false, pos};
            else
                return {true, pos};
        }
        return {false, pos};
    }

    struct _find_node_return_type
    {
        bool exist;
        _node *node;
        size_t pos;
    };
    _find_node_return_type _find_node(const value_type &key) const
    {
        if (!_root)
            return {false, nullptr, 0};

        _node *p = _root;
        while (true)
        {
            auto [exist, pos] = _find_pos(key, p);
            if (exist)
                return {true, p, pos};
            else if (p->child[pos])
                p = p->child[pos];
            else
                return {false, p, pos};
        }
    }

public:
    // 插入关键字
    void insert(const value_type &val)
    {
        if (!_root)
        {
            _root = new _node{1, nullptr, {val}};
            return;
        }

        // 查找插入位置
        auto [exist, node, pos] = _find_node(val);
        if (exist)
            return; // 关键字已存在

        _node *ap = nullptr;
        value_type key = val;
        while (true)
        {
            // 在节点中插入关键字
            for (size_t i = node->key_num; i > pos; --i)
            {
                node->key[i] = std::move(node->key[i - 1]);
                node->child[i + 1] = node->child[i];
            }
            node->key[pos] = std::move(key);
            node->child[pos + 1] = ap;
            if (ap)
                ap->parent = node;

            if (++node->key_num < Order)
                return;

            // 分裂
            ap = new _node{Order / 2};
            for (size_t i = 0; i < Order / 2; ++i)
            {
                ap->key[i] = std::move(node->key[(Order - 1) / 2 + 1 + i]);
                ap->child[i] = node->child[(Order - 1) / 2 + 1 + i];
                node->child[(Order - 1) / 2 + 1 + i] = nullptr;
                if (ap->child[i])
                    ap->child[i]->parent = ap;
            }
            ap->child[Order / 2] = node->child[Order];
            node->child[Order] = nullptr;
            if (ap->child[Order / 2])
                ap->child[Order / 2]->parent = ap;
            key = std::move(node->key[(Order - 1) / 2]);

            node->key_num -= Order / 2 + 1;
            node = node->parent;
            if (!node)
            {
                _root = new _node{1, nullptr, {key}, {_root, ap}};
                ap->parent = _root->child[0]->parent = _root;
                return;
            }
            pos = _find_pos(key, node).second;
        }
    } // void insert(const value_type &)

    // 删除关键字
    void erase(const value_type &key)
    {
        auto [exist, node, pos] = _find_node(key);
        if (!exist)
            return; // 不存在该关键字

        _node *next = node->child[pos + 1];
        if (next)
        {
            while (next->child[0])
                next = next->child[0];
            node->key[pos] = std::move(next->key[0]);
            node = next;
            pos = 0;
        }

        while (true)
        {
            // 此时 node 为最下层非终端节点
            _node *parent = node->parent;

            if (!parent || node->key_num > Order / 2)
            {
                // 当前节点有足够多的关键字
                if (node->key_num == 1)
                {
                    // 只在根节点发生
                    _root = _root->child[0];
                    delete node;
                    if (_root)
                        _root->parent = nullptr;
                    return;
                }

                for (size_t i = pos + 1; i < node->key_num; ++i)
                {
                    node->key[i - 1] = std::move(node->key[i]);
                    node->child[i] = node->child[i + 1];
                }
                node->child[node->key_num] = nullptr;
                --node->key_num;

                return;
            }
            else if (node->key_num == Order / 2)
            {
                size_t i = 0;
                for (; i <= parent->key_num; ++i)
                    if (parent->child[i] == node)
                        break;

                if (i == parent->key_num)
                {
                    // 不存在右兄弟
                    _node *brother = parent->child[i - 1]; // 左兄弟
                    if (brother->key_num > Order / 2)
                    {
                        // 左兄弟有足够多的关键字
                        for (size_t i = 0; i < pos; ++i)
                        {
                            node->key[i + 1] = std::move(node->key[i]);
                            node->child[i + 1] = node->child[i];
                        }

                        node->key[0] = std::move(parent->key[i - 1]);
                        parent->key[i - 1] = std::move(brother->key[brother->key_num - 1]);
                        --brother->key_num;

                        return;
                    }
                    else
                    {
                        // 左兄弟关键字数量至多为 Order/2，将当前节点与左兄弟合并
                        brother->key[brother->key_num] = parent->key[i - 1];

                        for (size_t i = 0; i < pos; ++i)
                        {
                            brother->child[brother->key_num + 1] = node->child[i];
                            if (node->child[i])
                                node->child[i]->parent = brother;
                            ++brother->key_num;
                            brother->key[brother->key_num] = node->key[i];
                        }
                        brother->child[brother->key_num + 1] = node->child[pos];
                        if (node->child[pos])
                            node->child[pos]->parent = brother;
                        ++brother->key_num;
                        for (size_t i = pos + 1; i < node->key_num; ++i)
                        {
                            brother->key[brother->key_num] = node->key[i];
                            brother->child[brother->key_num + 1] = node->child[i + 1];
                            if (node->child[i + 1])
                                node->child[i + 1]->parent = brother;
                            ++brother->key_num;
                        }

                        delete node;
                        parent->child[i] = nullptr;

                        // 当前节点上升一层
                        node = parent;
                        pos = i - 1;
                    }
                }
                else
                {
                    // 存在右兄弟
                    _node *brother = parent->child[i + 1]; // 右兄弟
                    if (brother->key_num > Order / 2)
                    {
                        // 右兄弟有足够多的关键字
                        for (size_t i = pos; i < node->key_num - 1; ++i)
                        {
                            node->key[pos] = std::move(node->key[pos + 1]);
                            node->child[pos] = node->child[pos + 1];
                        }
                        node->child[node->key_num - 1] = node->child[node->key_num];

                        node->key[node->key_num - 1] = std::move(parent->key[i]);
                        parent->key[i] = std::move(brother->key[0]);

                        for (size_t i = 0; i < brother->key_num - 1; ++i)
                        {
                            brother->key[i] = brother->key[i + 1];
                            brother->child[i] = brother->child[i + 1];
                        }
                        brother->child[brother->key_num - 1] = brother->child[brother->key_num];
                        brother->child[brother->key_num] = nullptr;
                        --brother->key_num;

                        return;
                    }
                    else
                    {
                        // 右兄弟关键字数量至多为 Order/2
                        // 将当前节点与右兄弟合并
                        for (size_t i = brother->key_num; i >= 1; --i)
                        {
                            brother->key[i - 1 + node->key_num] = brother->key[i - 1];
                            brother->child[i + node->key_num] = brother->child[i];
                        }
                        brother->child[node->key_num] = brother->child[0];

                        for (size_t i = 0; i < pos; ++i)
                        {
                            brother->key[i] = node->key[i];
                            brother->child[i] = node->child[i];
                            if (node->child[i])
                                node->child[i]->parent = brother;
                        }
                        brother->child[pos] = node->child[pos];
                        if (node->child[pos])
                            node->child[pos]->parent = brother;

                        for (size_t i = pos + 1; i < node->key_num; ++i)
                        {
                            brother->key[i - 1] = node->key[i];
                            brother->child[i] = node->child[i];
                            if (node->child[i])
                                node->child[i]->parent = brother;
                        }
                        brother->key[node->key_num - 1] = parent->key[i];

                        brother->key_num += node->key_num;

                        delete node;
                        parent->child[i] = parent->child[i + 1];
                        parent->child[i + 1] = nullptr;

                        // 当前节点上升一层
                        node = parent;
                        pos = i;
                    }
                }
            }
            else
                throw std::logic_error("预料之外的 key_num 值");
        }
    } // void erase(const value_type &)

    std::pair<value_type *, value_type *> find(const value_type &key) const
    {
        auto [exist, node, pos] = _find_node(key);
        if (exist)
            return {node->key.data(), node->key.data() + node->key_num};
        else
            return {nullptr, nullptr};
    }

private:
    void _traverse_impl(_node *node, std::function<void(const value_type &)> fn) const
    {
        if (!node)
            return;

        for (size_t i = 0; i < node->key_num; ++i)
        {
            _traverse_impl(node->child[i], fn);
            fn(node->key[i]);
        }
        _traverse_impl(node->child[node->key_num], fn);
    }

public:
    void traverse(std::function<void(const value_type &)> fn) const
    {
        _traverse_impl(_root, fn);
    }

private:
    _node *_root = nullptr;
}; // class b_tree<>

} // namespace ds
