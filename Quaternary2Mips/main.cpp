#include "asmGenerator.h"
#include "defines.h"



int main()
{
	asmGenerator as;
	cout << "�м���������output�ļ����У�����ΪintermediateCode.txt" << endl;
	cout << "���س���ִ��Ŀ���������" << endl;
	getchar();
	cout << "��������Ŀ�����" << endl;
	as.parse("intermediateCode.txt", "final_mips.asm");
	cout << "Ŀ�����������ϣ�����output�ļ����в鿴final_mips.asm�ļ���" << endl;
	cout << "�밴q�˳�" << endl;
	while (_getch() != 'q') {
		cout << "�밴q�˳�" << endl;
	}
	cout << "�����������ӭ�´�ʹ��^_^" << endl;

	system("pause");
	return 0;
}

