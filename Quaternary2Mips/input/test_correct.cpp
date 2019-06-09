/*用于测试的正确程序*/
int program(int i,int j){
	i=i+j;
	return i;
}

int main()
{
	int result;
	int a,b,c,d;
	int MIRE;

	a=1;b=2;c=3;d=4;
	MIRE=program(1,a);
	result=3;

	result=360;


	while(result>350)
	{
		++d;
		if(d==7)
		{
			result=result+c;
			break;
		}
		else
		{
			while(c>0)
				--c;
		}
		result-=1;
	}
	return 0;
}