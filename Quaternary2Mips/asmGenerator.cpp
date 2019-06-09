#include "asmGenerator.h"

//TODO AVALUE和RVALUE的管理

asmGenerator::asmGenerator()
{
	
	
	//

	//for (int i = 0; i < sizeof(one_op); i++) {
	//	cout << sizeof(one_op) << endl;
	//	one_op_set.insert(one_op[i]);
	//}
	//for (int i = 0; i < sizeof(two_op); i++) {
	//	two_op_set.insert(two_op[i]);
	//}
	//for (int i = 0; i < sizeof(two_imm_op); i++) {
	//	two_imm_op_set.insert(two_imm_op[i]);
	//}

	for (int i = 0; i < sizeof(registers)/ STR_SIZE; i++) {
		reglru.push_back(RegUse{ registers[i],i,0});
	}

	for (int i = 0; i < sizeof(RVALUE)/ STR_SIZE; i++) {
		RVALUE[i] = "null";
	}
	//初始赋值为null,在退出一个过程后局部变量需要释放


}

void asmGenerator::generateASM(const string midCode)
{

	//lru变化
	LruRound();



	//1.从midcode中分离出四元
	int separator1 = midCode.find('(');
	string string_seq = midCode.substr(0, separator1 - 0);
	int sequence = stoi(string_seq);  //四元式标号

	int separator2 = midCode.find(',', separator1 + 1);
	string string_elem_1 = midCode.substr(separator1 + 1, separator2 - separator1 - 1);

	int separator3 = midCode.find(',', separator2 + 1);
	string string_elem_2 = midCode.substr(separator2 + 1, separator3 - separator2 - 1);

	int separator4 = midCode.find(',', separator3 + 1);
	string string_elem_3 = midCode.substr(separator3 + 1, separator4 - separator3 - 1);

	int separator5 = midCode.find(')', separator4 + 1);
	string string_elem_4 = midCode.substr(separator4 + 1, separator5 - separator4 - 1);

	//cout << sequence << "++++++++++++++++++" << endl;
	//cout << string_elem_1 << "++++++++++++++++++" << endl;
	//cout << string_elem_2 << "++++++++++++++++++" << endl;
	//cout << string_elem_3 << "++++++++++++++++++" << endl;
	//cout << string_elem_4 << "++++++++++++++++++" << endl;

	//每一条四元式在转换的时候，输出标号作为label，在第二遍扫描的时候再把不需要删掉
	emit("lable_" + string_seq + ":");


	int regIndex;
	string regNameA,regNameB, regName_C;
	set<string> emptyset{};//这是一个局部对象
	//解析指令，根据不同的op做出不同的输出
	switch (getOpType(string_elem_1, string_elem_2, string_elem_3, string_elem_4))
	{
		
	case CALL://过程调用，需要创建栈帧并跳转(参数已经压栈压好了)
		emit("jal " + string_elem_4); //跳转子程序，string_elem_4为函数名，也是过程的起始标签
		break;
	case RETURN://过程返回，需要撤销栈帧并跳转
		emit("move $v1," + *(AVALUE.at("RETURN_stored").begin()));//将返回值放到$v1中


		//释放局部变量占用的寄存器,修改AVALUE与RVALUE
		for (auto it = local_var_offset_table.begin(); it != local_var_offset_table.end(); it++) {
			freeReg(it->first);
		}

		emit("move $sp,$fp");
		emit("addi $sp,$sp," + to_string(param_offset));//修改esp（mips中是$sp）
		emit("lw $ra,4($fp)");//将返回地址给$ra
		emit("lw $fp,0($fp)");	       //修改ebp（mips中是$fp）
		
		
		param_offset = PARAM_OFFSET_INIT;
		localvar_offset = LOCALVAR_OFFSET_INIT;
		local_var_offset_table.clear();
		in_procedure = false;
		if (!procedureName.compare("main")) {
			//
		}
		else {
			emit("jr $ra");
		}

		procedureName = "";

		break;
	case ONE:  //A:=op B的类型
		if (!string_elem_1.compare("j")) {
			emit("j lable_" + string_elem_4);
			usefulLableSet.insert("lable_"+string_elem_4+":");
		}
		break;
	case TWO://A:=B op C的类型
		//A可能已经在内存中，也可能是第一次出现
		regIndex = getReg(string_elem_1, string_elem_2, string_elem_3, string_elem_4);
				

		if (AVALUE.at(string_elem_2).empty()) { //B的值不在寄存器中
			//如果B的值不在寄存器中，将B从内存加载到寄存器
			if (local_var_offset_table.find(string_elem_2)!= local_var_offset_table.end()) { //如果B是局部变量
				int offset = local_var_offset_table.at(string_elem_2);
				emit("lw " + registers[regIndex] + "," + to_string(offset) + "($fp)");
				useRegInLru(regIndex);
			}
			else if(global_var_addr_table.find(string_elem_2)!=global_var_addr_table.end()) {
				int addr = global_var_addr_table.at(string_elem_2);
				emit("lw " + registers[regIndex] + "," + to_string(addr) + "($zero)");
				useRegInLru(regIndex);

			}
			else {
				cout << "出错" << endl;
				//出错
			}
			regNameB = registers[regIndex];
			//AVALUE.at(string_elem_2).insert(regNameB);
			//RVALUE[regIndex] = string_elem_2;

		}
		else {  // B的值在寄存器中
			//if (AVALUE.at(string_elem_2).find(registers[regIndex])== AVALUE.at(string_elem_2).end()) {//B在寄存器中但是不在分配给A的寄存器中
			//	emit("move " + registers[regIndex] + "," + *(AVALUE.at(string_elem_2).begin()));
			//}
			regNameB=*(AVALUE.at(string_elem_2).begin());

		}
		//现在B的值已经在寄存器当中了

		
		if (AVALUE.at(string_elem_3).empty()) { //C的值不在寄存器中
			int regIndex_C = getReg(string_elem_3, regIndex);   //给C分配一个寄存器，除了刚才分配给A的不可以
			
			if (local_var_offset_table.find(string_elem_3) != local_var_offset_table.end()) {  //C是一个局部变量
				int offset = local_var_offset_table.at(string_elem_3);
				emit("lw " + registers[regIndex_C] + "," + to_string(offset) + "($fp)");
				useRegInLru(regIndex_C);


			}
			else if (global_var_addr_table.find(string_elem_3) != global_var_addr_table.end()) {  //C是一个全局变量
				int addr = global_var_addr_table.at(string_elem_3);
				emit("lw " + registers[regIndex_C] + "," + to_string(addr) + "($zero)");
				useRegInLru(regIndex);

			}
			else {
				cout << "出错" << endl;
				//出错
			}
			regName_C = registers[regIndex_C];
		}
		else { //C的值在寄存器中
			regName_C = *(AVALUE.at(string_elem_3).begin());
		}
		//现在C已经在寄存器regNameC中
		useRegInLru(regIndex);
		useRegInLru(regNameB);
		useRegInLru(regName_C);
		emit(string_elem_1 + " " + registers[regIndex] + "," + regNameB + "," + regName_C);
		break;
	case TWO_IMM:  

		regIndex = getReg(string_elem_1,string_elem_2,string_elem_3,string_elem_4);//先给A分配一个寄存器
		
		if (AVALUE.at(string_elem_2).empty()) { //B的值不在寄存器中
			//如果B的值不在寄存器中，将B从内存加载到寄存器
			if (local_var_offset_table.find(string_elem_2) != local_var_offset_table.end()) { //如果B是局部变量
				int offset = local_var_offset_table.at(string_elem_2);
				emit("lw " + registers[regIndex] + "," + to_string(offset) + "($fp)");
			}
			else if (global_var_addr_table.find(string_elem_2) != global_var_addr_table.end()) {
				int addr = global_var_addr_table.at(string_elem_2);
				emit("lw " + registers[regIndex] + "," + to_string(addr) + "($zero)");
			}
			else {
				cout << "出错" << endl;
				//出错
			}
			regNameB = registers[regIndex];
			useRegInLru(regNameB);

			//不用修改RVALUE和AVALUE因为这个B只是临时的
		}
		else {  // B的值在寄存器中
			//if (AVALUE.at(string_elem_2).find(registers[regIndex])== AVALUE.at(string_elem_2).end()) {//B在寄存器中但是不在分配给A的寄存器中
			//	emit("move " + registers[regIndex] + "," + *(AVALUE.at(string_elem_2).begin()));
			//}
			regNameB = *(AVALUE.at(string_elem_2).begin());

		}
		//现在B的值已经在寄存器当中了
		useRegInLru(regIndex);

		if (!string_elem_1.compare("bne")) {

			emit("bnez " + regNameB + "," + "lable_"+string_elem_4);
			usefulLableSet.insert("lable_" + string_elem_4 + ":");
		}
		else {
			emit(string_elem_1 + " " + registers[regIndex] + "," + regNameB + "," + string_elem_3);

		}



		break;
	case ASSIGN:
		//先判断elem4(目标变量)有没有分配内存空间
		//if (local_var_offset_table.find(string_elem_4) != local_var_offset_table.end()) {
		//	//elem4是一个分配好空间的局部变量
		//	
		//	if (AVALUE.at(string_elem_4).empty()) {//若不在寄存器中，则分配寄存器
		//		regIndex = getReg(midCode);
		//		regNameA = registers[regIndex];
		//	}
		//	else {//若在寄存器中，则获取这个寄存器的名字
		//		regNameA = *(AVALUE.at(string_elem_4).begin());
		//	}

		//}
		//else if (global_var_addr_table.find(string_elem_4)!= global_var_addr_table.end()) {
		//	//elem4是一个分配好空间的全局变量
		//	
		//	if (AVALUE.at(string_elem_4).empty()) {//若不在寄存器中，则分配寄存器
		//		regIndex = getReg(midCode);
		//		regNameA = registers[regIndex];
		//	}
		//	else {//若已经在寄存器中，则获取这个寄存器的名字
		//		regNameA = *(AVALUE.at(string_elem_4).begin());
		//	}

		//}
		//else {
		//	//还没有分配内存空间
		//	if (in_procedure) {//是局部变量
		//		//在局部变量偏移表中分配
		//		local_var_offset_table.insert(pair<string, int>(string_elem_4, localvar_offset));
		//		localvar_offset += 4;
		//	}
		//	else {  //是全局变量
		//		//在全局变量地址表中分配地址
		//		global_var_addr_table.insert(pair<string, int>(string_elem_4, globalvar_addr));
		//		globalvar_addr += 4;
		//	}

		//	regIndex = getReg(midCode);
		//	regNameA = registers[regIndex];
		//	//一定还不在寄存器中，分配寄存器,获得寄存器名
		//}

		
		regIndex = getReg(string_elem_1, string_elem_2, string_elem_3, string_elem_4);
		regNameA = registers[regIndex];

		if (string_elem_2.at(0)>='0'&&string_elem_2.at(0)<='9') {//如果是立即数赋值
			emit("addi " + registers[regIndex] + ",$zero," + string_elem_2);
			useRegInLru(regIndex);

		}
		else if (string_elem_2.at(0)=='$') {//如果是寄存器赋值
			emit("move " + regNameA + "," + string_elem_2);
			useRegInLru(regNameA);
			useRegInLru(string_elem_2);
		}
		else {//是变量赋值
			if (!AVALUE.at(string_elem_2).empty()) {//如果右值在寄存器中
				regNameB = *(AVALUE.at(string_elem_2).begin());//获得右值所在寄存器名
				emit("move " + regNameA + "," + regNameB);
				useRegInLru(regNameA);
				useRegInLru(regNameB);

			}
			else {//如果右值不在寄存器中
				//获得右值变量的内存地址
				if (local_var_offset_table.find(string_elem_2)!= local_var_offset_table.end()) {//如果右值为局部变量
					int offset = local_var_offset_table.at(string_elem_2);
					emit("lw " + regNameA + "," + to_string(offset) + "($fp)");
				}
				else if(global_var_addr_table.find(string_elem_2)!=global_var_addr_table.end()) {//如果右值为全局变量
					int addr = global_var_addr_table.at(string_elem_2);
					emit("lw" + regNameA + ",-" + to_string(addr) + "($zero)");
				}
				else{
					//出错
				}
				useRegInLru(regNameA);

			}


		}
		break;
	case OP_ERROR:
		break;
	case PAR:
		//将参数变量压栈
		//先判断参数是否在寄存器中(考虑参数即可能是局部变量也可能是全局变量)	
		if (AVALUE.at(string_elem_2).empty()) {
			if (local_var_offset_table.find(string_elem_2)!= local_var_offset_table.end()) {//判断是局部变量吗
				int offset = local_var_offset_table.at(string_elem_2);
				regIndex = getReg(string_elem_2);
				emit("lw " + registers[regIndex] + "," + to_string(offset) + "($fp)");
				emit("subi $sp,$sp,4");
				emit("sw " + registers[regIndex] + ",($sp)");
			}
			else if (global_var_addr_table.find(string_elem_2)!= global_var_addr_table.end()) {//判断是局部变量吗
				int addr = global_var_addr_table.at(string_elem_2);
				regIndex = getReg(string_elem_2);
				emit("lw " + registers[regIndex]+ "," + to_string(addr) + "($zero)");
				emit("subi $sp,$sp,4");
				emit("sw " + registers[regIndex] + ",($sp)");
			}
			else {
				//出错
			}
			useRegInLru(regIndex);

		}//参数不在寄存器中
		else {
			emit("subi $sp,$sp,4");
			emit("sw " + *(AVALUE.at(string_elem_2).begin()) + ",($sp)");
			useRegInLru(*(AVALUE.at(string_elem_2).begin()));

		}//参数在寄存器中
		break;
	case DEFPAR:
		//为形参变量分配内存空间（给定偏移地址）
		local_var_offset_table.insert(pair<string, int>(string_elem_4, param_offset));
		//localvar_offset += 4;
		param_offset += 4;
		
		//AVALUE是map<string,set<string>>，全局对象
		AVALUE.insert(pair<string, set<string>>(string_elem_4, emptyset));
		break;
	case FUNDEF:
		//进入一个过程块，重置局部偏移表和形参偏移表
		in_procedure = true;
		procedureName = string_elem_1;
		local_var_offset_table.clear();
		param_offset = PARAM_OFFSET_INIT;  
		localvar_offset = LOCALVAR_OFFSET_INIT;
		usefulLableSet.insert(string_elem_1+":");
		emit(string_elem_1 + ":");//产生过程标号
		emit("subi $sp,$sp,4");
		emit("sw $ra,0($sp)");//压栈返回地址
		emit("subi $sp,$sp,4");
		emit("sw $fp,0($sp)");//压栈旧的$fp
		emit("move $fp,$sp");  //赋值新$fp
		break;
	default:
		break;
	}


}


//NOTE!getreg只会分配寄存器（涉及抢占时会写会），但是不会将varName写入该寄存器
/*
Function:为elem4得到一个寄存器
Procedure:
1.确保elem4在内存中有分配空间，若elem4不在内存中，
    为之分配内存空间，
2.若elem4在寄存器中，直接返回寄存器标号
  否则喂它分配一个标号，返回分配的寄存器标号。
ReturnValue:
-1:分配失败
0~sizeof(registers)-1:分配成功
*/
int asmGenerator::getReg(string str_elem1,string str_elem2,string str_elem3,string str_elem4)
{
	int regIndex=-1;
	string regName;

	//Step1确保elem4在内存中有分配空间，若elem4不在内存中，为之分配内存空间
	if (local_var_offset_table.find(str_elem4) == local_var_offset_table.end()&& global_var_addr_table.find(str_elem4) == global_var_addr_table.end()) {
		//elem4是未分配好空间的变量
		//判断是局部变量还是全局变量
		if (str_elem4.at(str_elem4.rfind('_', str_elem4.rfind('_') - 1) + 1) == '0'){
			//在全局变量地址表中分配地址
			global_var_addr_table.insert(pair<string, int>(str_elem4, globalvar_addr));
			globalvar_addr += 4;
		}
		else {
			//在局部变量偏移表中分配
			local_var_offset_table.insert(pair<string, int>(str_elem4, localvar_offset));
			localvar_offset -= 4;
		}
		set<string> emptyset{};//这是一个局部对象
		//AVALUE是map<string,set<string>>，全局对象
		AVALUE.insert(pair<string,set<string>>(str_elem4, emptyset));
	}

	//Step2
	if (AVALUE.at(str_elem4).empty()) {//若不在寄存器中，则分配寄存器
		

		//TODO 寄存器分配算法就在这里
		//1.优先用空余的
		//2.再否则抢占lru的reg
		for (int i = 0; i < sizeof(RVALUE)/STR_SIZE; i++) {
			if(!RVALUE[i].compare("null")) {
				regIndex = i;//找到了空闲的寄存器
				//占用这个寄存器，修改RVAULE和AVALUE
				AVALUE.at(str_elem4).insert(registers[regIndex]);
				RVALUE[regIndex] = str_elem4;
				break;
			}
		}//查找有无空闲的reg
		
		if (regIndex == -1) {//没有找到空闲的reg，准备抢占一个reg
			string toSeizeRegName = getLruRegName();//获得要抢占的寄存器名
			int toSeizeRegIndex= getLruRegIndex();//获得要抢占的寄存器序号
			string toSeizeVarName = RVALUE[toSeizeRegIndex];
			if (AVALUE.at(toSeizeVarName).size()>=2) {//如果不需要为寄存器中的变量V生成存数指令
				//不需要生成存数指令的条件（满足一个即可）：
				//1.AVALUE中看出V还保存在别的寄存器中
				//2.如果V是A，且不是B或C（暂不采用）
				//3.如果V不会在之后被使用（暂不采用）
			}
			else {//需要生成存数指令
				//判断V是局部变量还是全局变量采用不同的写会方式
				if (local_var_offset_table.find(toSeizeVarName) != local_var_offset_table.end()) {
					//是局部变量
					int offset = local_var_offset_table.at(toSeizeVarName);
					emit("sw " + toSeizeRegName + "," + to_string(offset) + "($fp)");
				}
				else if(global_var_addr_table.find(toSeizeVarName)!=global_var_addr_table.end()) {
					//是全局变量
					int addr = global_var_addr_table.at(toSeizeVarName);
					emit("sw " + toSeizeRegName + "," + to_string(addr) + "($zero)");
				}
				else {
					//出错
				}
				useRegInLru(toSeizeRegName);

			}
			AVALUE.at(toSeizeVarName).erase(toSeizeRegName);
			regIndex = toSeizeRegIndex;
			RVALUE[regIndex] = str_elem4;
			AVALUE.at(str_elem4).insert(registers[regIndex]);
			//做替换
		}

	}
	else {//若已经在寄存器中，则获取这个寄存器的名字
		regName = *(AVALUE.at(str_elem4).begin());
		for (int i = 0; i < sizeof(registers)/ STR_SIZE; i++) {
			if (!registers[i].compare(regName)) {
				regIndex = i;
				break;
			}
		}
	}
	return regIndex;
}






//使用本重载函数的时候，应该是为右部变量分配寄存器，所以varName一定已经在内存中有位置了
//NOTE!getreg只会分配寄存器（涉及抢占时会写会），但是不会将varName写入该寄存器
int asmGenerator::getReg( string varName, int exceptIndex)
{
	int regIndex = -1;

		//1.优先用空余的
		//2.再否则抢占lru的reg
	
	for (int i = 0; i < sizeof(RVALUE)/ STR_SIZE; i++) {
		if (!RVALUE[i].compare("null")) {
			regIndex = i;//找到了空闲的寄存器
			//占用这个寄存器，修改RVAULE和AVALUE
			AVALUE.at(varName).insert(registers[regIndex]);
			RVALUE[regIndex] = varName;
			break;
		}
	}//查找有无空闲的reg

	if (regIndex == -1) {//没有找到空闲的reg，准备抢占一个reg
		string toSeizeRegName = getLruRegName(exceptIndex);//获得要抢占的寄存器名
		int toSeizeRegIndex = getLruRegIndex(exceptIndex);//获得要抢占的寄存器序号
		string toSeizeVarName = RVALUE[toSeizeRegIndex];
		if (AVALUE.at(toSeizeVarName).size() >= 2) {//如果不需要为寄存器中的变量V生成存数指令
			//不需要生成存数指令的条件（满足一个即可）：
			//1.AVALUE中看出V还保存在别的寄存器中
			//2.如果V是A，且不是B或C（暂不采用）
			//3.如果V不会在之后被使用（暂不采用）
		}
		else {//需要生成存数指令
			//判断V是局部变量还是全局变量采用不同的写会方式
			if (local_var_offset_table.find(toSeizeVarName) != local_var_offset_table.end()) {
				//是局部变量
				int offset = local_var_offset_table.at(toSeizeVarName);
				emit("sw " + toSeizeRegName + "," + to_string(offset) + "($fp)");
			}
			else if (global_var_addr_table.find(toSeizeVarName) != global_var_addr_table.end()) {
				//是全局变量
				int addr = global_var_addr_table.at(toSeizeVarName);
				emit("sw " + toSeizeRegName + "," + to_string(addr) + "($zero)");
			}
			else {
				//出错
			}
			useRegInLru(toSeizeRegName);

		}
		AVALUE.at(toSeizeVarName).erase(toSeizeRegName);
		regIndex = toSeizeRegIndex;
		RVALUE[regIndex] = varName;
		AVALUE.at(varName).insert(registers[regIndex]);
		//做替换
	}
	return regIndex;
}

void asmGenerator::emit(string s) {

	asmCodeFile << s << '\n';
}

void asmGenerator::deleteUselessLable(string asmCodeFileName)
{
	string line_buf;
	string final_filename = "output/"+asmCodeFileName;
	string temp_filename = "temp/"+asmCodeFileName + ".temp";
	ofstream final_asm;
	//asmCodeFile.open(temp_filename, ios::in|ios::_Nocreate);
	//if (!asmCodeFile.is_open()) {
	//	cerr << "目标代码文件打开失败!" << endl;
	//	exit(-1);
	//}
	asmCodeFile.seekg(0, std::ios::beg);
	final_asm.open(final_filename, ios::out);
	if (!final_asm.is_open()) {
		cerr << "final目标代码文件打开失败!" << endl;
		final_asm.close();
		exit(-1);
	}

	while (getline(asmCodeFile, line_buf)) {
		if (line_buf.back() == ':') {
			if (usefulLableSet.find(line_buf) != usefulLableSet.end()) {
				final_asm << line_buf<<'\n';
			}
		}
		else {
			final_asm << line_buf<< '\n';
		}
	}
	asmCodeFile.close();
	//NOTE! 如果不想要中间asm文件的话，可以把下面这行注释掉
	//callSystem("del temp\\" + asmCodeFileName + ".temp");
	final_asm.close();


}

void asmGenerator::callSystem(string cmd)
{
	char* cmdtemp1;
	const int len = cmd.length();
	cmdtemp1 = new char[len + 1];
	strcpy(cmdtemp1, cmd.c_str());
	system(cmdtemp1);
	delete[] cmdtemp1;
}


void asmGenerator::parse(string midCodeFileName,string asmCodeFileName)
{


	if (showDetail) {
		cout << "开始翻译成目标代码！" << endl;
	}

	string line_buf;
	//需要先为$sp赋值esp
	//为$fp赋值ebp

	
	midCodeFile.open("output/"+midCodeFileName, ios::in);
	if (!midCodeFile.is_open()) {
		cerr << "中间代码打开失败!" << endl;
		midCodeFile.close();
		exit(-1);
	}
	callSystem("type nul> temp/" + asmCodeFileName + ".temp");
	asmCodeFile.open("temp/"+asmCodeFileName+".temp", ios::out|ios::in);
	if (!asmCodeFile.is_open()) {
		cerr << "temp目标代码文件打开失败!" << endl;
		asmCodeFile.close();
		exit(-1);
	}

	emit("addi $sp,$sp," + to_string(STACK_SEG_ADDR));
	emit("addi $fp,$fp," + to_string((STACK_SEG_ADDR - 4)));
	while (getline(midCodeFile,line_buf)) {
		generateASM(line_buf);
	}

#ifdef DELETE_USELESS_LABLE
	deleteUselessLable(asmCodeFileName);
#else
	asmCodeFile.close();
#endif // DELETE_USELESS_LABLE
	midCodeFile.close();


}

OP_TYPE asmGenerator::getOpType(string op, string a1, string a2, string a3) {
	if (!op.compare("="))
		return ASSIGN;
	else if (!op.compare("call"))
		return CALL;
	else if (!op.compare("ret"))
		return RETURN;
	else if (!op.compare("par"))
		return PAR;
	else if (!op.compare("defpar"))
		return DEFPAR;
	else if (one_op_set.find(op) != one_op_set.end())
		return  ONE;
	else if (two_op_set.find(op) != two_op_set.end())
		return TWO;
	else if (two_imm_op_set.find(op) != two_imm_op_set.end())
		return TWO_IMM;
	else if (!a1.compare("-") && !a2.compare("-"))
		return FUNDEF;
	else
		return OP_ERROR;
}

/*
有则释放，无责不动
不仅释放该变量可能占用的寄存器
也释放AVALUE中的结构
变量要写回内存
*/
void asmGenerator::freeReg(string var)
{
	if (AVALUE.find(var) != AVALUE.end()) {
#ifdef WRITEBACK_BEFORE_RETURN
		if (!AVALUE.at(var).empty()) {
			if (local_var_offset_table.find(var) != local_var_offset_table.end()) {
				//是局部变量
				int offset = local_var_offset_table.at(var);
				emit("sw " + *(AVALUE.at(var).begin()) + "," + to_string(offset) + "($fp)");
			}
			else if (global_var_addr_table.find(var) != global_var_addr_table.end()) {
				//是全局变量
				int addr = global_var_addr_table.at(var);
				emit("sw " + *(AVALUE.at(var).begin()) + "," + to_string(addr) + "($zero)");
			}
			else {
				cout << "出错" << endl;
				//出错
			}
		}
		
#endif

		for (auto it = AVALUE.at(var).begin(); it != AVALUE.at(var).end(); it++) {
			//对于var所在的每个寄存器
			for (int i = 0; i < sizeof(registers) / STR_SIZE; i++) {
				if (!(*it).compare(registers[i])) {
					RVALUE[i] = "null";
				}
			}
		}
		AVALUE.erase(var);
	}
}



void asmGenerator::setShowDetail(bool doShowDetail)
{
	showDetail = doShowDetail;
}

string asmGenerator::getLruRegName(int exceptIndex) {
	int max_index = 0;
	int max_unuse = 0;
	for (size_t i = 0; i < reglru.size(); i++) {
		if (reglru[i].unuseTime > max_unuse) {
			if(exceptIndex==-1||exceptIndex!= reglru[i].regIndex){
				max_index = i;
				max_unuse = reglru[i].unuseTime;
			}
		
		}
	}
	return reglru[max_index].RegName;

}
int asmGenerator::getLruRegIndex(int exceptIndex) {
	int max_index = 0;
	int max_unuse = 0;
	for (size_t i = 0; i < reglru.size(); i++) {
		if (reglru[i].unuseTime > max_unuse) {
			if (exceptIndex == -1 || exceptIndex != reglru[i].regIndex) {
				max_index = i;
				max_unuse = reglru[i].unuseTime;
			}
		}
	}
	return reglru[max_index].regIndex;

}
void asmGenerator::useRegInLru(int regIndex) {
	for (auto it = reglru.begin(); it != reglru.end(); it++) {
		if (it->regIndex == regIndex) {
			it->unuseTime = 0;
			return;
		}
	}
}
void asmGenerator::useRegInLru(string regName) {
	for (auto it = reglru.begin(); it != reglru.end(); it++) {
		if (!it->RegName.compare(regName)) {
			it->unuseTime = 0;
			return;
		}
	}

}
void asmGenerator::LruRound() {
	for (auto it = reglru.begin(); it != reglru.end(); it++) {
		if(it->unuseTime<LRU_MAX_UNUSETIME)
			it->unuseTime++;
	}
}



