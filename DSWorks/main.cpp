// main.cpp : 程序入口
//


#include <iostream>
#include <array>

#include "Stack.hpp"

class Maze {
private:
	struct _point { size_t x; size_t y; };

public:
	Maze() { _path_exist = _find_path(); }

	// 在 _os 中输出迷宫
	void print() const {
		for (auto &i : _data) {
			for (auto j : i)
				_os << (j ? "*" : "  ");
			_os << std::endl;
		}
	}

	// 在 _os 中输出路径
	void print_path() const {
		if (!_path_exist) {
			_os << "" << std::endl;
			return;
		}

		for (size_t i = 0; i < _MAZE_WIDTH + 2; ++i) {
			for (size_t j = 0; j < _MAZE_HEIGHT + 2; ++j) {
				if (_data[i][j])
					_os << "*";
				else if (_path[i][j])
					_os << "*";
				else
					_os << "  ";
			}
			_os << std::endl;
		}
	}
private: // 辅助函数

	// 寻找路径
	// 返回值：存在路径则返回 true , 否则返回 false
	bool _find_path() {
		auto get = [this](const _point &p)->bool& {
			assert(p.x >= 0 && p.x <= _MAZE_WIDTH + 1);
			assert(p.y >= 0 && p.y <= _MAZE_HEIGHT + 1);

			return _path[p.y][p.x];
		};

		assert(!_data[_begin.y][_begin.x] && !_data[_end.y][_end.x]);	// 起点和终点不是墙

		// 将 _path 作为临时存储空间
		for (size_t i = 0; i < _MAZE_WIDTH + 2; ++i)
			for (size_t j = 0; j < _MAZE_HEIGHT + 2; ++j)
				_path[i][j] = _data[i][j];

		_point cur = _begin;
		ds::Stack<_point> stack;	// 栈
		
		while(true) {
			if (cur.x == _end.x && cur.y == _end.y) {	// 到达终点
				// 将 _path 清空
				for (int i = 0; i < _MAZE_WIDTH + 2; ++i)
					for (int j = 0; j < _MAZE_HEIGHT + 2; ++j)
						_path[i][j] = false;

				// 画出路径
				get(cur) = true;
				while (!stack.empty()) {
					get(stack.top()) = true;
					stack.pop();
				}
				return true;	// 存在路径
			}
			
			if (!get(_point{ cur.x,cur.y - 1 })) {	// 向上
				stack.push(cur);
				get(cur) = true;	// 封堵来路
				--cur.y;

				continue;
			}
			if (!get(_point{ cur.x + 1,cur.y })) {	// 向右
				stack.push(cur);
				get(cur) = true;
				++cur.x;

				continue;
			}
			if (!get(_point{ cur.x,cur.y + 1 })) {	// 向下
				stack.push(cur);
				get(cur) = true;
				++cur.y;

				continue;
			}
			if (!get(_point{ cur.x - 1,cur.y })) {	// 向左
				stack.push(cur);
				get(cur) = true;
				--cur.x;

				continue;
			}

			// 无路，退回
			if (!stack.empty()) {
				get(cur) = true;	// 封堵退路
				cur = stack.top();
				stack.pop();
			} else
				return false;	// 不存在路径
			
		}

		assert(false);
		return false;
	}

private:
	static constexpr size_t _MAZE_WIDTH = 10;	// 迷宫长度
	static constexpr size_t _MAZE_HEIGHT = 10;	// 迷宫高度

	bool _data[_MAZE_WIDTH + 2][_MAZE_HEIGHT + 2] = {
		{1,1,1,1,1,1,1,1,1,1,1,1},	// ########################
		{1,0,1,0,0,0,0,0,1,1,1,1},	// ##  ##          ########
		{1,0,1,0,0,1,0,0,0,0,0,1},	// ##  ##    ##          ##
		{1,0,0,0,1,1,1,0,1,1,0,1},	// ##      ######  ####  ##
		{1,0,1,0,1,0,1,0,1,1,1,1},	// ##  ##  ##  ##  ########
		{1,0,1,0,1,0,1,0,0,0,0,1},	// ##  ##  ##  ##        ##
		{1,0,0,0,1,0,1,1,1,1,0,1},	// ##      ##  ########  ##
		{1,0,1,0,1,0,0,1,0,0,0,1},	// ##  ##  ##    ##      ##
		{1,0,1,0,0,0,1,1,0,1,0,1},	// ##  ##      ####  ##  ##
		{1,0,0,1,1,0,0,0,1,1,0,1},	// ##    ####      ####  ##
		{1,0,1,1,0,0,1,0,1,0,0,1},	// ##  ####    ##  ##    ##
		{1,1,1,1,1,1,1,1,1,1,1,1}	// ########################
	};	// 存在#回路#

	_point _begin{ 3,2 }, _end{ 10,8 };	// 起点和终点

	bool _path[_MAZE_WIDTH + 2][_MAZE_HEIGHT + 2];	// 从起点到终点的路径
	bool _path_exist = false;	// 是否存在路径

	std::ostream &_os = std::cout;	// 输出流
};

int main() {
	Maze maze;
	maze.print_path();

	return 0;
}