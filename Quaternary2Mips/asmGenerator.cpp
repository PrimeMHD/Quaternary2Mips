#include "asmGenerator.h"

//TODO AVALUE��RVALUE�Ĺ���

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
	//��ʼ��ֵΪnull,���˳�һ�����̺�ֲ�������Ҫ�ͷ�


}

void asmGenerator::generateASM(const string midCode)
{

	//lru�仯
	LruRound();



	//1.��midcode�з������Ԫ
	int separator1 = midCode.find('(');
	string string_seq = midCode.substr(0, separator1 - 0);
	int sequence = stoi(string_seq);  //��Ԫʽ���

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

	//ÿһ����Ԫʽ��ת����ʱ����������Ϊlabel���ڵڶ���ɨ���ʱ���ٰѲ���Ҫɾ��
	emit("lable_" + string_seq + ":");


	int regIndex;
	string regNameA,regNameB, regName_C;
	set<string> emptyset{};//����һ���ֲ�����
	//����ָ����ݲ�ͬ��op������ͬ�����
	switch (getOpType(string_elem_1, string_elem_2, string_elem_3, string_elem_4))
	{
		
	case CALL://���̵��ã���Ҫ����ջ֡����ת(�����Ѿ�ѹջѹ����)
		emit("jal " + string_elem_4); //��ת�ӳ���string_elem_4Ϊ��������Ҳ�ǹ��̵���ʼ��ǩ
		break;
	case RETURN://���̷��أ���Ҫ����ջ֡����ת
		emit("move $v1," + *(AVALUE.at("RETURN_stored").begin()));//������ֵ�ŵ�$v1��


		//�ͷžֲ�����ռ�õļĴ���,�޸�AVALUE��RVALUE
		for (auto it = local_var_offset_table.begin(); it != local_var_offset_table.end(); it++) {
			freeReg(it->first);
		}

		emit("move $sp,$fp");
		emit("addi $sp,$sp," + to_string(param_offset));//�޸�esp��mips����$sp��
		emit("lw $ra,4($fp)");//�����ص�ַ��$ra
		emit("lw $fp,0($fp)");	       //�޸�ebp��mips����$fp��
		
		
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
	case ONE:  //A:=op B������
		if (!string_elem_1.compare("j")) {
			emit("j lable_" + string_elem_4);
			usefulLableSet.insert("lable_"+string_elem_4+":");
		}
		break;
	case TWO://A:=B op C������
		//A�����Ѿ����ڴ��У�Ҳ�����ǵ�һ�γ���
		regIndex = getReg(string_elem_1, string_elem_2, string_elem_3, string_elem_4);
				

		if (AVALUE.at(string_elem_2).empty()) { //B��ֵ���ڼĴ�����
			//���B��ֵ���ڼĴ����У���B���ڴ���ص��Ĵ���
			if (local_var_offset_table.find(string_elem_2)!= local_var_offset_table.end()) { //���B�Ǿֲ�����
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
				cout << "����" << endl;
				//����
			}
			regNameB = registers[regIndex];
			//AVALUE.at(string_elem_2).insert(regNameB);
			//RVALUE[regIndex] = string_elem_2;

		}
		else {  // B��ֵ�ڼĴ�����
			//if (AVALUE.at(string_elem_2).find(registers[regIndex])== AVALUE.at(string_elem_2).end()) {//B�ڼĴ����е��ǲ��ڷ����A�ļĴ�����
			//	emit("move " + registers[regIndex] + "," + *(AVALUE.at(string_elem_2).begin()));
			//}
			regNameB=*(AVALUE.at(string_elem_2).begin());

		}
		//����B��ֵ�Ѿ��ڼĴ���������

		
		if (AVALUE.at(string_elem_3).empty()) { //C��ֵ���ڼĴ�����
			int regIndex_C = getReg(string_elem_3, regIndex);   //��C����һ���Ĵ��������˸ղŷ����A�Ĳ�����
			
			if (local_var_offset_table.find(string_elem_3) != local_var_offset_table.end()) {  //C��һ���ֲ�����
				int offset = local_var_offset_table.at(string_elem_3);
				emit("lw " + registers[regIndex_C] + "," + to_string(offset) + "($fp)");
				useRegInLru(regIndex_C);


			}
			else if (global_var_addr_table.find(string_elem_3) != global_var_addr_table.end()) {  //C��һ��ȫ�ֱ���
				int addr = global_var_addr_table.at(string_elem_3);
				emit("lw " + registers[regIndex_C] + "," + to_string(addr) + "($zero)");
				useRegInLru(regIndex);

			}
			else {
				cout << "����" << endl;
				//����
			}
			regName_C = registers[regIndex_C];
		}
		else { //C��ֵ�ڼĴ�����
			regName_C = *(AVALUE.at(string_elem_3).begin());
		}
		//����C�Ѿ��ڼĴ���regNameC��
		useRegInLru(regIndex);
		useRegInLru(regNameB);
		useRegInLru(regName_C);
		emit(string_elem_1 + " " + registers[regIndex] + "," + regNameB + "," + regName_C);
		break;
	case TWO_IMM:  

		regIndex = getReg(string_elem_1,string_elem_2,string_elem_3,string_elem_4);//�ȸ�A����һ���Ĵ���
		
		if (AVALUE.at(string_elem_2).empty()) { //B��ֵ���ڼĴ�����
			//���B��ֵ���ڼĴ����У���B���ڴ���ص��Ĵ���
			if (local_var_offset_table.find(string_elem_2) != local_var_offset_table.end()) { //���B�Ǿֲ�����
				int offset = local_var_offset_table.at(string_elem_2);
				emit("lw " + registers[regIndex] + "," + to_string(offset) + "($fp)");
			}
			else if (global_var_addr_table.find(string_elem_2) != global_var_addr_table.end()) {
				int addr = global_var_addr_table.at(string_elem_2);
				emit("lw " + registers[regIndex] + "," + to_string(addr) + "($zero)");
			}
			else {
				cout << "����" << endl;
				//����
			}
			regNameB = registers[regIndex];
			useRegInLru(regNameB);

			//�����޸�RVALUE��AVALUE��Ϊ���Bֻ����ʱ��
		}
		else {  // B��ֵ�ڼĴ�����
			//if (AVALUE.at(string_elem_2).find(registers[regIndex])== AVALUE.at(string_elem_2).end()) {//B�ڼĴ����е��ǲ��ڷ����A�ļĴ�����
			//	emit("move " + registers[regIndex] + "," + *(AVALUE.at(string_elem_2).begin()));
			//}
			regNameB = *(AVALUE.at(string_elem_2).begin());

		}
		//����B��ֵ�Ѿ��ڼĴ���������
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
		//���ж�elem4(Ŀ�����)��û�з����ڴ�ռ�
		//if (local_var_offset_table.find(string_elem_4) != local_var_offset_table.end()) {
		//	//elem4��һ������ÿռ�ľֲ�����
		//	
		//	if (AVALUE.at(string_elem_4).empty()) {//�����ڼĴ����У������Ĵ���
		//		regIndex = getReg(midCode);
		//		regNameA = registers[regIndex];
		//	}
		//	else {//���ڼĴ����У����ȡ����Ĵ���������
		//		regNameA = *(AVALUE.at(string_elem_4).begin());
		//	}

		//}
		//else if (global_var_addr_table.find(string_elem_4)!= global_var_addr_table.end()) {
		//	//elem4��һ������ÿռ��ȫ�ֱ���
		//	
		//	if (AVALUE.at(string_elem_4).empty()) {//�����ڼĴ����У������Ĵ���
		//		regIndex = getReg(midCode);
		//		regNameA = registers[regIndex];
		//	}
		//	else {//���Ѿ��ڼĴ����У����ȡ����Ĵ���������
		//		regNameA = *(AVALUE.at(string_elem_4).begin());
		//	}

		//}
		//else {
		//	//��û�з����ڴ�ռ�
		//	if (in_procedure) {//�Ǿֲ�����
		//		//�ھֲ�����ƫ�Ʊ��з���
		//		local_var_offset_table.insert(pair<string, int>(string_elem_4, localvar_offset));
		//		localvar_offset += 4;
		//	}
		//	else {  //��ȫ�ֱ���
		//		//��ȫ�ֱ�����ַ���з����ַ
		//		global_var_addr_table.insert(pair<string, int>(string_elem_4, globalvar_addr));
		//		globalvar_addr += 4;
		//	}

		//	regIndex = getReg(midCode);
		//	regNameA = registers[regIndex];
		//	//һ�������ڼĴ����У�����Ĵ���,��üĴ�����
		//}

		
		regIndex = getReg(string_elem_1, string_elem_2, string_elem_3, string_elem_4);
		regNameA = registers[regIndex];

		if (string_elem_2.at(0)>='0'&&string_elem_2.at(0)<='9') {//�������������ֵ
			emit("addi " + registers[regIndex] + ",$zero," + string_elem_2);
			useRegInLru(regIndex);

		}
		else if (string_elem_2.at(0)=='$') {//����ǼĴ�����ֵ
			emit("move " + regNameA + "," + string_elem_2);
			useRegInLru(regNameA);
			useRegInLru(string_elem_2);
		}
		else {//�Ǳ�����ֵ
			if (!AVALUE.at(string_elem_2).empty()) {//�����ֵ�ڼĴ�����
				regNameB = *(AVALUE.at(string_elem_2).begin());//�����ֵ���ڼĴ�����
				emit("move " + regNameA + "," + regNameB);
				useRegInLru(regNameA);
				useRegInLru(regNameB);

			}
			else {//�����ֵ���ڼĴ�����
				//�����ֵ�������ڴ��ַ
				if (local_var_offset_table.find(string_elem_2)!= local_var_offset_table.end()) {//�����ֵΪ�ֲ�����
					int offset = local_var_offset_table.at(string_elem_2);
					emit("lw " + regNameA + "," + to_string(offset) + "($fp)");
				}
				else if(global_var_addr_table.find(string_elem_2)!=global_var_addr_table.end()) {//�����ֵΪȫ�ֱ���
					int addr = global_var_addr_table.at(string_elem_2);
					emit("lw" + regNameA + ",-" + to_string(addr) + "($zero)");
				}
				else{
					//����
				}
				useRegInLru(regNameA);

			}


		}
		break;
	case OP_ERROR:
		break;
	case PAR:
		//����������ѹջ
		//���жϲ����Ƿ��ڼĴ�����(���ǲ����������Ǿֲ�����Ҳ������ȫ�ֱ���)	
		if (AVALUE.at(string_elem_2).empty()) {
			if (local_var_offset_table.find(string_elem_2)!= local_var_offset_table.end()) {//�ж��Ǿֲ�������
				int offset = local_var_offset_table.at(string_elem_2);
				regIndex = getReg(string_elem_2);
				emit("lw " + registers[regIndex] + "," + to_string(offset) + "($fp)");
				emit("subi $sp,$sp,4");
				emit("sw " + registers[regIndex] + ",($sp)");
			}
			else if (global_var_addr_table.find(string_elem_2)!= global_var_addr_table.end()) {//�ж��Ǿֲ�������
				int addr = global_var_addr_table.at(string_elem_2);
				regIndex = getReg(string_elem_2);
				emit("lw " + registers[regIndex]+ "," + to_string(addr) + "($zero)");
				emit("subi $sp,$sp,4");
				emit("sw " + registers[regIndex] + ",($sp)");
			}
			else {
				//����
			}
			useRegInLru(regIndex);

		}//�������ڼĴ�����
		else {
			emit("subi $sp,$sp,4");
			emit("sw " + *(AVALUE.at(string_elem_2).begin()) + ",($sp)");
			useRegInLru(*(AVALUE.at(string_elem_2).begin()));

		}//�����ڼĴ�����
		break;
	case DEFPAR:
		//Ϊ�βα��������ڴ�ռ䣨����ƫ�Ƶ�ַ��
		local_var_offset_table.insert(pair<string, int>(string_elem_4, param_offset));
		//localvar_offset += 4;
		param_offset += 4;
		
		//AVALUE��map<string,set<string>>��ȫ�ֶ���
		AVALUE.insert(pair<string, set<string>>(string_elem_4, emptyset));
		break;
	case FUNDEF:
		//����һ�����̿飬���þֲ�ƫ�Ʊ���β�ƫ�Ʊ�
		in_procedure = true;
		procedureName = string_elem_1;
		local_var_offset_table.clear();
		param_offset = PARAM_OFFSET_INIT;  
		localvar_offset = LOCALVAR_OFFSET_INIT;
		usefulLableSet.insert(string_elem_1+":");
		emit(string_elem_1 + ":");//�������̱��
		emit("subi $sp,$sp,4");
		emit("sw $ra,0($sp)");//ѹջ���ص�ַ
		emit("subi $sp,$sp,4");
		emit("sw $fp,0($sp)");//ѹջ�ɵ�$fp
		emit("move $fp,$sp");  //��ֵ��$fp
		break;
	default:
		break;
	}


}


//NOTE!getregֻ�����Ĵ������漰��ռʱ��д�ᣩ�����ǲ��ὫvarNameд��üĴ���
/*
Function:Ϊelem4�õ�һ���Ĵ���
Procedure:
1.ȷ��elem4���ڴ����з���ռ䣬��elem4�����ڴ��У�
    Ϊ֮�����ڴ�ռ䣬
2.��elem4�ڼĴ����У�ֱ�ӷ��ؼĴ������
  ����ι������һ����ţ����ط���ļĴ�����š�
ReturnValue:
-1:����ʧ��
0~sizeof(registers)-1:����ɹ�
*/
int asmGenerator::getReg(string str_elem1,string str_elem2,string str_elem3,string str_elem4)
{
	int regIndex=-1;
	string regName;

	//Step1ȷ��elem4���ڴ����з���ռ䣬��elem4�����ڴ��У�Ϊ֮�����ڴ�ռ�
	if (local_var_offset_table.find(str_elem4) == local_var_offset_table.end()&& global_var_addr_table.find(str_elem4) == global_var_addr_table.end()) {
		//elem4��δ����ÿռ�ı���
		//�ж��Ǿֲ���������ȫ�ֱ���
		if (str_elem4.at(str_elem4.rfind('_', str_elem4.rfind('_') - 1) + 1) == '0'){
			//��ȫ�ֱ�����ַ���з����ַ
			global_var_addr_table.insert(pair<string, int>(str_elem4, globalvar_addr));
			globalvar_addr += 4;
		}
		else {
			//�ھֲ�����ƫ�Ʊ��з���
			local_var_offset_table.insert(pair<string, int>(str_elem4, localvar_offset));
			localvar_offset -= 4;
		}
		set<string> emptyset{};//����һ���ֲ�����
		//AVALUE��map<string,set<string>>��ȫ�ֶ���
		AVALUE.insert(pair<string,set<string>>(str_elem4, emptyset));
	}

	//Step2
	if (AVALUE.at(str_elem4).empty()) {//�����ڼĴ����У������Ĵ���
		

		//TODO �Ĵ��������㷨��������
		//1.�����ÿ����
		//2.�ٷ�����ռlru��reg
		for (int i = 0; i < sizeof(RVALUE)/STR_SIZE; i++) {
			if(!RVALUE[i].compare("null")) {
				regIndex = i;//�ҵ��˿��еļĴ���
				//ռ������Ĵ������޸�RVAULE��AVALUE
				AVALUE.at(str_elem4).insert(registers[regIndex]);
				RVALUE[regIndex] = str_elem4;
				break;
			}
		}//�������޿��е�reg
		
		if (regIndex == -1) {//û���ҵ����е�reg��׼����ռһ��reg
			string toSeizeRegName = getLruRegName();//���Ҫ��ռ�ļĴ�����
			int toSeizeRegIndex= getLruRegIndex();//���Ҫ��ռ�ļĴ������
			string toSeizeVarName = RVALUE[toSeizeRegIndex];
			if (AVALUE.at(toSeizeVarName).size()>=2) {//�������ҪΪ�Ĵ����еı���V���ɴ���ָ��
				//����Ҫ���ɴ���ָ�������������һ�����ɣ���
				//1.AVALUE�п���V�������ڱ�ļĴ�����
				//2.���V��A���Ҳ���B��C���ݲ����ã�
				//3.���V������֮��ʹ�ã��ݲ����ã�
			}
			else {//��Ҫ���ɴ���ָ��
				//�ж�V�Ǿֲ���������ȫ�ֱ������ò�ͬ��д�᷽ʽ
				if (local_var_offset_table.find(toSeizeVarName) != local_var_offset_table.end()) {
					//�Ǿֲ�����
					int offset = local_var_offset_table.at(toSeizeVarName);
					emit("sw " + toSeizeRegName + "," + to_string(offset) + "($fp)");
				}
				else if(global_var_addr_table.find(toSeizeVarName)!=global_var_addr_table.end()) {
					//��ȫ�ֱ���
					int addr = global_var_addr_table.at(toSeizeVarName);
					emit("sw " + toSeizeRegName + "," + to_string(addr) + "($zero)");
				}
				else {
					//����
				}
				useRegInLru(toSeizeRegName);

			}
			AVALUE.at(toSeizeVarName).erase(toSeizeRegName);
			regIndex = toSeizeRegIndex;
			RVALUE[regIndex] = str_elem4;
			AVALUE.at(str_elem4).insert(registers[regIndex]);
			//���滻
		}

	}
	else {//���Ѿ��ڼĴ����У����ȡ����Ĵ���������
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






//ʹ�ñ����غ�����ʱ��Ӧ����Ϊ�Ҳ���������Ĵ���������varNameһ���Ѿ����ڴ�����λ����
//NOTE!getregֻ�����Ĵ������漰��ռʱ��д�ᣩ�����ǲ��ὫvarNameд��üĴ���
int asmGenerator::getReg( string varName, int exceptIndex)
{
	int regIndex = -1;

		//1.�����ÿ����
		//2.�ٷ�����ռlru��reg
	
	for (int i = 0; i < sizeof(RVALUE)/ STR_SIZE; i++) {
		if (!RVALUE[i].compare("null")) {
			regIndex = i;//�ҵ��˿��еļĴ���
			//ռ������Ĵ������޸�RVAULE��AVALUE
			AVALUE.at(varName).insert(registers[regIndex]);
			RVALUE[regIndex] = varName;
			break;
		}
	}//�������޿��е�reg

	if (regIndex == -1) {//û���ҵ����е�reg��׼����ռһ��reg
		string toSeizeRegName = getLruRegName(exceptIndex);//���Ҫ��ռ�ļĴ�����
		int toSeizeRegIndex = getLruRegIndex(exceptIndex);//���Ҫ��ռ�ļĴ������
		string toSeizeVarName = RVALUE[toSeizeRegIndex];
		if (AVALUE.at(toSeizeVarName).size() >= 2) {//�������ҪΪ�Ĵ����еı���V���ɴ���ָ��
			//����Ҫ���ɴ���ָ�������������һ�����ɣ���
			//1.AVALUE�п���V�������ڱ�ļĴ�����
			//2.���V��A���Ҳ���B��C���ݲ����ã�
			//3.���V������֮��ʹ�ã��ݲ����ã�
		}
		else {//��Ҫ���ɴ���ָ��
			//�ж�V�Ǿֲ���������ȫ�ֱ������ò�ͬ��д�᷽ʽ
			if (local_var_offset_table.find(toSeizeVarName) != local_var_offset_table.end()) {
				//�Ǿֲ�����
				int offset = local_var_offset_table.at(toSeizeVarName);
				emit("sw " + toSeizeRegName + "," + to_string(offset) + "($fp)");
			}
			else if (global_var_addr_table.find(toSeizeVarName) != global_var_addr_table.end()) {
				//��ȫ�ֱ���
				int addr = global_var_addr_table.at(toSeizeVarName);
				emit("sw " + toSeizeRegName + "," + to_string(addr) + "($zero)");
			}
			else {
				//����
			}
			useRegInLru(toSeizeRegName);

		}
		AVALUE.at(toSeizeVarName).erase(toSeizeRegName);
		regIndex = toSeizeRegIndex;
		RVALUE[regIndex] = varName;
		AVALUE.at(varName).insert(registers[regIndex]);
		//���滻
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
	//	cerr << "Ŀ������ļ���ʧ��!" << endl;
	//	exit(-1);
	//}
	asmCodeFile.seekg(0, std::ios::beg);
	final_asm.open(final_filename, ios::out);
	if (!final_asm.is_open()) {
		cerr << "finalĿ������ļ���ʧ��!" << endl;
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
	//NOTE! �������Ҫ�м�asm�ļ��Ļ������԰���������ע�͵�
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
		cout << "��ʼ�����Ŀ����룡" << endl;
	}

	string line_buf;
	//��Ҫ��Ϊ$sp��ֵesp
	//Ϊ$fp��ֵebp

	
	midCodeFile.open("output/"+midCodeFileName, ios::in);
	if (!midCodeFile.is_open()) {
		cerr << "�м�����ʧ��!" << endl;
		midCodeFile.close();
		exit(-1);
	}
	callSystem("type nul> temp/" + asmCodeFileName + ".temp");
	asmCodeFile.open("temp/"+asmCodeFileName+".temp", ios::out|ios::in);
	if (!asmCodeFile.is_open()) {
		cerr << "tempĿ������ļ���ʧ��!" << endl;
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
�����ͷţ����𲻶�
�����ͷŸñ�������ռ�õļĴ���
Ҳ�ͷ�AVALUE�еĽṹ
����Ҫд���ڴ�
*/
void asmGenerator::freeReg(string var)
{
	if (AVALUE.find(var) != AVALUE.end()) {
#ifdef WRITEBACK_BEFORE_RETURN
		if (!AVALUE.at(var).empty()) {
			if (local_var_offset_table.find(var) != local_var_offset_table.end()) {
				//�Ǿֲ�����
				int offset = local_var_offset_table.at(var);
				emit("sw " + *(AVALUE.at(var).begin()) + "," + to_string(offset) + "($fp)");
			}
			else if (global_var_addr_table.find(var) != global_var_addr_table.end()) {
				//��ȫ�ֱ���
				int addr = global_var_addr_table.at(var);
				emit("sw " + *(AVALUE.at(var).begin()) + "," + to_string(addr) + "($zero)");
			}
			else {
				cout << "����" << endl;
				//����
			}
		}
		
#endif

		for (auto it = AVALUE.at(var).begin(); it != AVALUE.at(var).end(); it++) {
			//����var���ڵ�ÿ���Ĵ���
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



