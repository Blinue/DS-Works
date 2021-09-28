# DS-Works
<!-- ALL-CONTRIBUTORS-BADGE:START - Do not remove or modify this section -->
[![All Contributors](https://img.shields.io/badge/all_contributors-1-orange.svg?style=flat-square)](#contributors-)
<!-- ALL-CONTRIBUTORS-BADGE:END -->

使用C++编写的，STL风格的数据结构实现。

* AVL树：avl_tree
* B-树：b_tree
* 红黑树：rb_tree
* 顺序表：seq_list
* 栈：stack

所有实现均为泛型且header-only

## 示例

``` c++
// 顺序表
ds::seq_list list({5, 1, 2, 3});
std::sort(list.begin(), list.end());

for (int i : list)
{
    std::cout << i << " ";
}
// 输出：1 2 3 5

// 红黑树
ds::rb_tree tree(list.rbegin(), list.rend());

std::cout << "\n"
          << std::distance(tree.begin(), tree.find(5));
// 输出：3
```

## 测试

安装MSVC。在VSCode中运行生成任务，然后执行out/test.exe。

## Contributors ✨

Thanks goes to these wonderful people ([emoji key](https://allcontributors.org/docs/en/emoji-key)):

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->
<table>
  <tr>
    <td align="center"><a href="https://github.com/Blinue"><img src="https://avatars.githubusercontent.com/u/34770031?v=4?s=100" width="100px;" alt=""/><br /><sub><b>刘旭</b></sub></a><br /><a href="#maintenance-Blinue" title="Maintenance">🚧</a> <a href="https://github.com/Blinue/DS-Works/commits?author=Blinue" title="Code">💻</a></td>
  </tr>
</table>

<!-- markdownlint-restore -->
<!-- prettier-ignore-end -->

<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all-contributors](https://github.com/all-contributors/all-contributors) specification. Contributions of any kind welcome!