#include<iostream>
#include<stack>
#include<set>
#include<unordered_map>
#include<string>
#include<queue>
#define analysis_table table
#define Epsilon '0'
using namespace std;
const int N = 128;
char IDsymbol;									//文法的开始符号
unordered_map<char, vector<string>> gram;		//存储文法
string** analysis_table;						//分析表
unordered_map<char, set<char>> FOLLOW;			//follow集
unordered_map<string, set<char>> FIRST;			//first集
set<char> NTSymbol;								//非终结符号集
set<char> TSymbol;								//终结符号集
int MapVt[N];									//将每个终结符号映射为数字
int MapVn[N];									//将每个非终结符号映射为数字

int h[N], e[N], ne[N], idx ;					//数组模拟邻接表存储图
//添加一条边
void add(char a, char b)
{
	e[idx] = b, ne[idx] = h[a], h[a] = idx++;
}

//将输入的文法根据某个符号分割成多个字符串
vector<string> split(string s, char c)
{
	vector<string> res;
	for (int i = 0; i < s.length(); i++)
	{
		int j = i;
		while (s[j] != c && j != s.length()-1) j++;
		string part;
		if (j == s.length() - 1)
			 part = s.substr(i, j - i + 1);
		else
			 part = s.substr(i, j - i);
		res.push_back(part);
		i = j;
	}
	return res;
}
//返回某个符号是否是非终结符号
bool isNTSymbol(char c)
{
	auto it = NTSymbol.find(c);
	if (it == NTSymbol.end())
		return false;
	else return true;
}
//计算FIRST集的递归函数
void Compute_FIRST(string S, string cur)
{
	if (cur == string(1,Epsilon))							//为epsilon，则将epsilon放入FIRST集，返回。
	{
		FIRST[S].insert(Epsilon);
		return ;
	}
	if (!isNTSymbol(cur[0]) && cur[0] != Epsilon)			//遇到终结符号,放入集合中，直接返回
	{
		FIRST[S].insert(cur[0]);
		return;
	}
	//非终结符号，或以epsilon开头，需要递归调用
	if (cur[0] == Epsilon)
	{
		string next = cur.substr(1, cur.length() - 1);
		Compute_FIRST(S, next);
	}
	else
	{
		for (auto item : gram[cur[0]])
		{
			string next = item + cur.substr(1, cur.length() - 1);
			Compute_FIRST(S, next);
		}
	}
	
}

//求FIRST集
void FirstSet(string S)
{
	if (FIRST.count(S))
		return;
	Compute_FIRST(S, S);
}

//求FOLLOW集
void FollowSet()
{
	//规则1
	FOLLOW[IDsymbol].insert('#');						//开始符号的follow集放入#

	//规则2
	for (auto Vn : NTSymbol)							//遍历非终结符号
	{
		for (auto item : gram[Vn])						//遍历每条规则
		{
			string rule = item;							//一条规则
			if (rule == string(1, Epsilon))				//该规则的右步为Epsilon
				continue;
			for (int i = 0; i < rule.length() - 1; i++)
				if (isNTSymbol(rule[i]))				//是非终结符号, 把其右边的符号串的FIRST集（除EPsilon外）放入FOLLOW集中
				{
					string remain = rule.substr(i + 1, rule.length() - i - 1);	//remain为右边的符号串
					FirstSet(remain);
					
					for (auto a : FIRST[remain])		//遍历该符号串的FIRST集，将非Epsilon符号放入到FOLLOW中；
					{
						if (a == Epsilon)  continue;
						FOLLOW[rule[i]].insert(a);
					}
				}
		}
	}
	//规则3
	//先找出所有依赖规则，并用图存储
	memset(h, -1, sizeof h);
	for (auto Vn : NTSymbol)
	{
		for (auto item : gram[Vn])
		{
			string rule = item;							//一条规则
			for (int i = 0; i < rule.length(); i++)
			{
				if(!isNTSymbol(rule[i]) ) continue;		//排除终结符号
				if (i == rule.length() - 1)				//如果是最后一各非终结符号，则符合规则3，连一条Vn到该符号的边，	
				{										//表示Vn的FOLLOW集被包含
					add(Vn, rule[i]);
					continue;
				}
				string remain = rule.substr(i + 1, rule.length() - i - 1);
				FirstSet(remain);
				if (FIRST[remain].count(Epsilon))
					add(Vn, rule[i]);
			}
		}
	}


	//建图完毕， 开始求FOLLOW集
	queue<int> q;										//队列，用于搜索
	for (auto Vn : NTSymbol)
	{
			q.push(Vn);
	}

	while (q.size())
	{
		int  Vn1 = q.front();
		q.pop();
		for (int i = h[Vn1]; i != -1; i = ne[i])
		{
			int Vn2 = e[i];								//Vn1 被Vn2包含
			int prevSize = FOLLOW[Vn2].size();			//Vn2在包含Vn1之前的大小
			for (auto a : FOLLOW[Vn1])
			{
				FOLLOW[Vn2].insert(a);
			}
			if (FOLLOW[Vn2].size() > prevSize)			//如果FOLLOW(Vn2)扩大，则将Vn2放入到队列中，否则不放。
				q.push(Vn2);
		}
	}
	
}
//建立分析表
void AnalysisTable()
{
	int idx = 0;
	for (auto it : NTSymbol)
		MapVn[it] = idx++;
	idx = 0;
	for (auto it : TSymbol)
		MapVt[it] = idx++;
	MapVt['#'] = idx;
	table = new string *[NTSymbol.size()];				//动态分配空间
	for (int i = 0; i < NTSymbol.size(); i++)
	{
		table[i] = new string[TSymbol.size() + 1];
	}
	for (int i = 0; i < NTSymbol.size(); i++)			//初始化
		for (int j = 0; j < TSymbol.size()+1; j++)
			table[i][j] = "";
	for (auto Vn : NTSymbol)
		for (auto item : gram[Vn])
		{
			string rule = item;
			FirstSet(rule);
			for (auto Vt : FIRST[rule])
			{
				if (Vt == Epsilon)					//如果FIRST集中含有Epsilon， 则将FOLLOW集中的
					for (auto fol : FOLLOW[Vn])
					{
						table[MapVn[Vn]][MapVt[fol]] = rule;
					}
				else
					table[MapVn[Vn]][MapVt[Vt]] = rule;
			}
		}
}

//返回M[Vn,Vt]对应的规则
string Table(char Vn, char Vt)
{
	return table[MapVn[Vn]][MapVt[Vt]];
}

//总控程序
void MasterConProgram(string sen)
{
	stack<char> Sanalysis, Sremain;
	Sanalysis.push('#');
	Sanalysis.push(IDsymbol);
	Sremain.push('#');
	for (int i = sen.length() - 1; i >= 0; i--)
	{
		Sremain.push(sen[i]);
	}
	char c = Sremain.top();
	bool Flag = true;
	while (Flag)
	{	
		c = Sremain.top();
		char Sym = Sanalysis.top();
		Sanalysis.pop();
		if (!isNTSymbol(Sym) && Sym != '#')  //终结符号
		{
			if (Sym == c)
			{
				Sremain.pop();
			}
			else
			{
				cout << "ERROR!!!" << endl;
				return;
			}
		}
		else if (Sym == '#')
		{
			if (Sym == c)
				Flag = false;
			else
			{
				cout << "ERROR!!!" << endl;
				return ;
			}
		}
		else
		{
			string s = Table(Sym,c);
			if (s[0] == Epsilon)	continue;
			if (s == "")
			{
				cout << "ERROR!!!" << endl;
				return ;
			}
			for (int i = s.length() - 1; i >= 0; i--)
			{
				Sanalysis.push(s[i]);
			}
		}
		
	}
	cout << "SUCCESS!!!" << endl;
}

//消除直接左递归
void RemoveLeftRecursion()
{
	cout << "*******************\n";
	for (auto Vn : NTSymbol)
	{
		//首先判断该非终结符号是否存在左递归
		bool flag = false;
		for (auto rule : gram[Vn])
			if (rule[0] == Vn)
			{
				flag = true;
				break;
			}
		if (flag == false) continue;
		//下面是消除该符号左递归的逻辑
		//产生一个新的非终结符号
		char newVn;
		int i;
		for (i = 'A'; i <= 'Z'; i++)
			if (NTSymbol.count(i) == 0)
				break;
		newVn = i;
		NTSymbol.insert(newVn);									//将新的非终结符号加入到集合中
		cout << "因" << Vn << "产生的新的规则：" << endl;

		vector<string>::iterator it = gram[Vn].begin();
		//将所有A::=βA'型的新规则加入文法
		for (; it != gram[Vn].end(); ++it)
		{
			if ((*it)[0] == Vn)
				continue;
			string newRule = (*it) + string(1, newVn);
			*it = newRule;										//把新的规则替换旧规则
			cout << Vn << "=" << newRule << endl;
		}
		//将所有A' ::=αA'|epsilon 的新规则加入文法
		it = gram[Vn].begin();
		for (; it != gram[Vn].end(); )
		{
			if ((*it)[0] != Vn)									//筛选出以Vn开头的右部
			{
				it++;
				continue;
			}
			string LRecursionRule;								//含有左递归的规则；
			string Repeat;										//该规则除非终结符号剩余的部分，即该规则所负责重复生成的字符串。	
			LRecursionRule = *it;
			Repeat = LRecursionRule.substr(1, LRecursionRule.length() - 1);
			string newRule = Repeat + string(1, newVn);
			gram[newVn].push_back(newRule);
			cout << newVn << "=" << newRule << endl;
			it = gram[Vn].erase(it);							//把该规则删除

		}
		gram[newVn].push_back(string(1, Epsilon));				//将A'::= epsilon加入
		cout << newVn << "=" << Epsilon << endl;

	}
}
void Init()
{
	cout<<"请按：左部=右部 的规则输入若干文法规则，以0结束\n";
	string s;
	set<char> Symbols;
	bool flag = true;											//标记输入的是否是第一个文法
	while (cin >> s, s != "0")
	{
		if (flag)												//如果是第一个文法，则该文法的左部为开始符号；
			IDsymbol = s[0];
		flag = false;
		NTSymbol.insert(s[0]);
		string rule = s.substr(2, s.length() - 2);				//右部
		vector<string> allRules = split(rule, '|');				//将右部根据|符分离
		for(auto it : allRules)
			gram[s[0]].push_back(it);
		for (int i = 0; i < s.length(); i++)					//将所有符号放入到symbol集合中
			if(s[i]!= '=' && s[i] != '|')
				Symbols.insert(s[i]);
	}
	//输入结束，统计终结符号；
	for (auto sym : Symbols)
	{
		if (sym == Epsilon) continue;							//排除epsilon和非终结符号
		if (NTSymbol.count(sym)) continue;	
		TSymbol.insert(sym);
	}
	RemoveLeftRecursion();
}
//打印分析表
void PrintTable()
{
	cout << "*******************\n";
	cout << "分析表如下：\n";
	cout << "  " << "\t";
	for (auto Vt : TSymbol)
		cout << Vt << "\t";
	cout << "#" << endl;
	int i = 0;
	for (auto Vn : NTSymbol)
	{
		cout << Vn << "\t";
		for (int j = 0; j < TSymbol.size() + 1; j++)
			cout << table[i][j] << "\t";
		cout << endl;
		i++;
	}
}

//打印所有规则
void PrintRules()
{
	cout << "*******************\n";
	cout << "处理后的文法如下：\n";
	for (auto Vn : NTSymbol)
	{
		string Grammer = string(1, Vn) + "::=";
		for (auto rule : gram[Vn])
		{
			Grammer += rule + "|";
		}
		Grammer = Grammer.substr(0, Grammer.length() - 1);
		cout << Grammer << endl;
	}
}


int main()
{
	Init();					//初始化，输入文法
	PrintRules();
	FollowSet();			//求解FOLLOW集和FIRST集
	AnalysisTable();		//构建分析表
	PrintTable();			//输出分析表
	cout << "*******************\n";
	cout<<"请输入句子，以判断该句子是否合法(以0结束）:\n";
	string s;
	while (cin >> s, s != "0")
		MasterConProgram(s);
	return 0;
}

/*
*样例：
E=TA
A=+E|0
T=FB
B=T|0
F=PC
C=*C|0
P=(E)|a|b|^
0

a+b*a


E=E+T|T
T=T*F|F
F=(E)|i
0

A=Ab|Ac|Ad|ef|cg
0
*/