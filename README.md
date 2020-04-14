# DS-Works

使用C++编写的，STL风格的数据结构实现。

* AVL树：avl_tree
* B-树：b_tree
* 红黑树：rb_tree
* 顺序表：seq_list
* 栈：stack

## 示例

``` c++
// 顺序表
auto list = ds::seq_list({5, 1, 2, 3});
std::sort(list.begin(), list.end());

for (int i : list)
{
    std::cout << i << " ";
}
// 输出：1 2 3 5

// 红黑树
auto tree = ds::rb_tree(list.rbegin(), list.rend());

std::cout << "\n"
          << std::distance(tree.begin(), tree.find(5));
// 输出：3
```

## 测试

安装MSVC。在VSCode中运行生成任务，然后执行out/test.exe。
