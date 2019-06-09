#include "asmGenerator.h"
#include "defines.h"



int main()
{
	asmGenerator as;
	cout << "中间代码请放在output文件夹中，命名为intermediateCode.txt" << endl;
	cout << "按回车键执行目标代码生成" << endl;
	getchar();
	cout << "正在生成目标代码" << endl;
	as.parse("intermediateCode.txt", "final_mips.asm");
	cout << "目标代码生成完毕！请在output文件夹中查看final_mips.asm文件！" << endl;
	cout << "请按q退出" << endl;
	while (_getch() != 'q') {
		cout << "请按q退出" << endl;
	}
	cout << "程序结束！欢迎下次使用^_^" << endl;

	system("pause");
	return 0;
}

