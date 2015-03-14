#include "../include/Cmdline.h"

// 初始化命令行参数
Cmdline::Cmdline(int argc, char * argv[])
{
	int i = 1;
	while (i < argc) {
		// 当前是否是选项
		if ('-' == argv[i][0]) {
			int next = i + 1;
			// 后面跟了一个参数
			if (next < argc && '-' != argv[next][0]) {
				_map.insert(std::make_pair(argv[i], std::string(argv[next])));
				i += 2;
				continue;
			}
			// 单纯选项，后面没跟参数
			else {
				_map.insert(std::make_pair(argv[i], ""));
				i++;
			}
		}
		// 非 “-” 选项略过
	}
}


// 初始化命令行参数
CmdlineList::CmdlineList (int argc, char * argv[])
{
	int i = 1;
	while (i < argc) {

		// 当前是否是选项
		if ('-' == argv[i][0]) {

			int next = i + 1;

			ArgValList avl;
			while (next < argc && '-' != argv[next][0]) {
				avl.push_back (argv[next]);
				next ++;
			}

			_map.insert(std::make_pair(argv[i], avl));
			i = next;
		}
	}
}

