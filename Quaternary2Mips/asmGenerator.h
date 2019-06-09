#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "defines.h"
using namespace std;

//TODO 
//��ʱû�п���segmentFault


//2�����뿪�أ�����˼��
//#define WRITEBACK_BEFORE_RETURN
#define DELETE_USELESS_LABLE



#define gpNum 17;
#define DATA_SEG_ADDR 0x10010000 //ȫ�ֱ�����ʼ��ַ(���ݶε���ʼ��ַ)
#define STACK_SEG_ADDR 0x10040000  //��ջ�ε���ʼ��ַ
#define PARAM_OFFSET_INIT 8
#define LOCALVAR_OFFSET_INIT -4
#define LRU_MAX_UNUSETIME INT_MAX
#define STR_SIZE 28
struct RegUse
{
	string RegName;
	int regIndex;
	int unuseTime;
	bool operator < (const RegUse& b)const {
		return unuseTime < b.unuseTime;
	}
};

enum OP_TYPE{ CALL,RETURN,ONE,TWO,TWO_IMM,ASSIGN,OP_ERROR,PAR,DEFPAR,FUNDEF};
//const string one_op[] = { "j"  };
//const string two_op[] = { "mul","mult","div","divu","mod","modu","and","or","nor","sll","sllv","sra","srav","srl","srlv","sltu","slt","add","sub","addu","subu"};
//const string two_imm_op[] = {"bne", "addi","addiu","subi","subiu","slti","sltiu","addi","ori","xori","lui","lw","sw","beq"};
class asmGenerator {
private:
	string registers[17] = { "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7" };
	string RVALUE[17];   //ÿ���Ĵ����У���ǰ�ı�����ʲô
	map<string, set<string>>AVALUE;        //������ַ��������,keyΪ��������
	set<string> one_op_set{ "j" }, two_op_set{ "mul","mult","div","divu","mod","modu","and","or","nor","sll","sllv","sra","srav","srl","srlv","sltu","slt","add","sub","addu","subu" }, two_imm_op_set{ "bne", "addi","addiu","subi","subiu","slti","sltiu","addi","ori","xori","lui","lw","sw","beq" };
	set<string> label_inuse_set;  //ʹ���еı�ǩ���
	map<string, int> local_var_offset_table;  //�ֲ����������ebp��ƫ�ƣ��������ֵ���õ�ʱ��ebp-offset��
	//ÿ����һ�����̿飬���Ҫ����
	map<string, int> global_var_addr_table;
	int param_offset = PARAM_OFFSET_INIT;  //��¼���ֵ���ڵ���ջ֡��ʵ����
	int localvar_offset = LOCALVAR_OFFSET_INIT;
	int globalvar_addr = DATA_SEG_ADDR;
	bool in_procedure=false;
	bool showDetail = false;
	string procedureName="";
	ifstream midCodeFile;
	fstream asmCodeFile;
	//priority_queue<RegUse> reglru;  //�󶥶�
	vector<RegUse>reglru;
#ifdef DELETE_USELESS_LABLE
	set<string> usefulLableSet;
#endif 

	string getLruRegName(int exceptIndex=-1);
	int getLruRegIndex(int exceptIndex=-1);
	void useRegInLru(int regIndex);
	void useRegInLru(string regName);
	void LruRound();
	void emit(string s);
	void generateASM(string midCode);//����һ���м���룬����һ��Ŀ�����
	int getReg(string str_elem1, string str_elem2, string str_elem3, string str_elem4);   //����һ��������regsiters������±꣬�������ļĴ���
	int getReg(string varName, int exceptIndex=-1);
	OP_TYPE getOpType(string op, string a1, string a2, string a3);
	void freeReg(string var);
	void deleteUselessLable(string asmCodeFileName);
	void callSystem(string cmd);
public:
	asmGenerator();
	
	//int getReg(string exceptRegName);
	
	void parse(string midCodeFileName, string asmCodeFileName);
	
	void setShowDetail(bool doShowDetail);

};