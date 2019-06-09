#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "defines.h"
using namespace std;

//TODO 
//暂时没有考虑segmentFault


//2个编译开关，顾名思义
//#define WRITEBACK_BEFORE_RETURN
#define DELETE_USELESS_LABLE



#define gpNum 17;
#define DATA_SEG_ADDR 0x10010000 //全局变量起始地址(数据段的起始地址)
#define STACK_SEG_ADDR 0x10040000  //堆栈段的起始地址
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
	string RVALUE[17];   //每个寄存器中，当前的变量是什么
	map<string, set<string>>AVALUE;        //变量地址描述数组,key为变量名，
	set<string> one_op_set{ "j" }, two_op_set{ "mul","mult","div","divu","mod","modu","and","or","nor","sll","sllv","sra","srav","srl","srlv","sltu","slt","add","sub","addu","subu" }, two_imm_op_set{ "bne", "addi","addiu","subi","subiu","slti","sltiu","addi","ori","xori","lui","lw","sw","beq" };
	set<string> label_inuse_set;  //使用中的标签编号
	map<string, int> local_var_offset_table;  //局部变量相对于ebp的偏移（存的是正值，用的时候ebp-offset）
	//每进入一个过程块，这个要重置
	map<string, int> global_var_addr_table;
	int param_offset = PARAM_OFFSET_INIT;  //记录这个值用于弹出栈帧的实参区
	int localvar_offset = LOCALVAR_OFFSET_INIT;
	int globalvar_addr = DATA_SEG_ADDR;
	bool in_procedure=false;
	bool showDetail = false;
	string procedureName="";
	ifstream midCodeFile;
	fstream asmCodeFile;
	//priority_queue<RegUse> reglru;  //大顶堆
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
	void generateASM(string midCode);//输入一条中间代码，产生一条目标代码
	int getReg(string str_elem1, string str_elem2, string str_elem3, string str_elem4);   //返回一个整数，regsiters数组的下标，代表分配的寄存器
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