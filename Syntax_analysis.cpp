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
char IDsymbol;									//�ķ��Ŀ�ʼ����
unordered_map<char, vector<string>> gram;		//�洢�ķ�
string** analysis_table;						//������
unordered_map<char, set<char>> FOLLOW;			//follow��
unordered_map<string, set<char>> FIRST;			//first��
set<char> NTSymbol;								//���ս���ż�
set<char> TSymbol;								//�ս���ż�
int MapVt[N];									//��ÿ���ս����ӳ��Ϊ����
int MapVn[N];									//��ÿ�����ս����ӳ��Ϊ����

int h[N], e[N], ne[N], idx ;					//����ģ���ڽӱ�洢ͼ
//���һ����
void add(char a, char b)
{
	e[idx] = b, ne[idx] = h[a], h[a] = idx++;
}

//��������ķ�����ĳ�����ŷָ�ɶ���ַ���
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
//����ĳ�������Ƿ��Ƿ��ս����
bool isNTSymbol(char c)
{
	auto it = NTSymbol.find(c);
	if (it == NTSymbol.end())
		return false;
	else return true;
}
//����FIRST���ĵݹ麯��
void Compute_FIRST(string S, string cur)
{
	if (cur == string(1,Epsilon))							//Ϊepsilon����epsilon����FIRST�������ء�
	{
		FIRST[S].insert(Epsilon);
		return ;
	}
	if (!isNTSymbol(cur[0]) && cur[0] != Epsilon)			//�����ս����,���뼯���У�ֱ�ӷ���
	{
		FIRST[S].insert(cur[0]);
		return;
	}
	//���ս���ţ�����epsilon��ͷ����Ҫ�ݹ����
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

//��FIRST��
void FirstSet(string S)
{
	if (FIRST.count(S))
		return;
	Compute_FIRST(S, S);
}

//��FOLLOW��
void FollowSet()
{
	//����1
	FOLLOW[IDsymbol].insert('#');						//��ʼ���ŵ�follow������#

	//����2
	for (auto Vn : NTSymbol)							//�������ս����
	{
		for (auto item : gram[Vn])						//����ÿ������
		{
			string rule = item;							//һ������
			if (rule == string(1, Epsilon))				//�ù�����Ҳ�ΪEpsilon
				continue;
			for (int i = 0; i < rule.length() - 1; i++)
				if (isNTSymbol(rule[i]))				//�Ƿ��ս����, �����ұߵķ��Ŵ���FIRST������EPsilon�⣩����FOLLOW����
				{
					string remain = rule.substr(i + 1, rule.length() - i - 1);	//remainΪ�ұߵķ��Ŵ�
					FirstSet(remain);
					
					for (auto a : FIRST[remain])		//�����÷��Ŵ���FIRST��������Epsilon���ŷ��뵽FOLLOW�У�
					{
						if (a == Epsilon)  continue;
						FOLLOW[rule[i]].insert(a);
					}
				}
		}
	}
	//����3
	//���ҳ������������򣬲���ͼ�洢
	memset(h, -1, sizeof h);
	for (auto Vn : NTSymbol)
	{
		for (auto item : gram[Vn])
		{
			string rule = item;							//һ������
			for (int i = 0; i < rule.length(); i++)
			{
				if(!isNTSymbol(rule[i]) ) continue;		//�ų��ս����
				if (i == rule.length() - 1)				//��������һ�����ս���ţ�����Ϲ���3����һ��Vn���÷��ŵıߣ�	
				{										//��ʾVn��FOLLOW��������
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


	//��ͼ��ϣ� ��ʼ��FOLLOW��
	queue<int> q;										//���У���������
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
			int Vn2 = e[i];								//Vn1 ��Vn2����
			int prevSize = FOLLOW[Vn2].size();			//Vn2�ڰ���Vn1֮ǰ�Ĵ�С
			for (auto a : FOLLOW[Vn1])
			{
				FOLLOW[Vn2].insert(a);
			}
			if (FOLLOW[Vn2].size() > prevSize)			//���FOLLOW(Vn2)������Vn2���뵽�����У����򲻷š�
				q.push(Vn2);
		}
	}
	
}
//����������
void AnalysisTable()
{
	int idx = 0;
	for (auto it : NTSymbol)
		MapVn[it] = idx++;
	idx = 0;
	for (auto it : TSymbol)
		MapVt[it] = idx++;
	MapVt['#'] = idx;
	table = new string *[NTSymbol.size()];				//��̬����ռ�
	for (int i = 0; i < NTSymbol.size(); i++)
	{
		table[i] = new string[TSymbol.size() + 1];
	}
	for (int i = 0; i < NTSymbol.size(); i++)			//��ʼ��
		for (int j = 0; j < TSymbol.size()+1; j++)
			table[i][j] = "";
	for (auto Vn : NTSymbol)
		for (auto item : gram[Vn])
		{
			string rule = item;
			FirstSet(rule);
			for (auto Vt : FIRST[rule])
			{
				if (Vt == Epsilon)					//���FIRST���к���Epsilon�� ��FOLLOW���е�
					for (auto fol : FOLLOW[Vn])
					{
						table[MapVn[Vn]][MapVt[fol]] = rule;
					}
				else
					table[MapVn[Vn]][MapVt[Vt]] = rule;
			}
		}
}

//����M[Vn,Vt]��Ӧ�Ĺ���
string Table(char Vn, char Vt)
{
	return table[MapVn[Vn]][MapVt[Vt]];
}

//�ܿس���
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
		if (!isNTSymbol(Sym) && Sym != '#')  //�ս����
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

//����ֱ����ݹ�
void RemoveLeftRecursion()
{
	cout << "*******************\n";
	for (auto Vn : NTSymbol)
	{
		//�����жϸ÷��ս�����Ƿ������ݹ�
		bool flag = false;
		for (auto rule : gram[Vn])
			if (rule[0] == Vn)
			{
				flag = true;
				break;
			}
		if (flag == false) continue;
		//�����������÷�����ݹ���߼�
		//����һ���µķ��ս����
		char newVn;
		int i;
		for (i = 'A'; i <= 'Z'; i++)
			if (NTSymbol.count(i) == 0)
				break;
		newVn = i;
		NTSymbol.insert(newVn);									//���µķ��ս���ż��뵽������
		cout << "��" << Vn << "�������µĹ���" << endl;

		vector<string>::iterator it = gram[Vn].begin();
		//������A::=��A'�͵��¹�������ķ�
		for (; it != gram[Vn].end(); ++it)
		{
			if ((*it)[0] == Vn)
				continue;
			string newRule = (*it) + string(1, newVn);
			*it = newRule;										//���µĹ����滻�ɹ���
			cout << Vn << "=" << newRule << endl;
		}
		//������A' ::=��A'|epsilon ���¹�������ķ�
		it = gram[Vn].begin();
		for (; it != gram[Vn].end(); )
		{
			if ((*it)[0] != Vn)									//ɸѡ����Vn��ͷ���Ҳ�
			{
				it++;
				continue;
			}
			string LRecursionRule;								//������ݹ�Ĺ���
			string Repeat;										//�ù�������ս����ʣ��Ĳ��֣����ù����������ظ����ɵ��ַ�����	
			LRecursionRule = *it;
			Repeat = LRecursionRule.substr(1, LRecursionRule.length() - 1);
			string newRule = Repeat + string(1, newVn);
			gram[newVn].push_back(newRule);
			cout << newVn << "=" << newRule << endl;
			it = gram[Vn].erase(it);							//�Ѹù���ɾ��

		}
		gram[newVn].push_back(string(1, Epsilon));				//��A'::= epsilon����
		cout << newVn << "=" << Epsilon << endl;

	}
}
void Init()
{
	cout<<"�밴����=�Ҳ� �Ĺ������������ķ�������0����\n";
	string s;
	set<char> Symbols;
	bool flag = true;											//���������Ƿ��ǵ�һ���ķ�
	while (cin >> s, s != "0")
	{
		if (flag)												//����ǵ�һ���ķ�������ķ�����Ϊ��ʼ���ţ�
			IDsymbol = s[0];
		flag = false;
		NTSymbol.insert(s[0]);
		string rule = s.substr(2, s.length() - 2);				//�Ҳ�
		vector<string> allRules = split(rule, '|');				//���Ҳ�����|������
		for(auto it : allRules)
			gram[s[0]].push_back(it);
		for (int i = 0; i < s.length(); i++)					//�����з��ŷ��뵽symbol������
			if(s[i]!= '=' && s[i] != '|')
				Symbols.insert(s[i]);
	}
	//���������ͳ���ս���ţ�
	for (auto sym : Symbols)
	{
		if (sym == Epsilon) continue;							//�ų�epsilon�ͷ��ս����
		if (NTSymbol.count(sym)) continue;	
		TSymbol.insert(sym);
	}
	RemoveLeftRecursion();
}
//��ӡ������
void PrintTable()
{
	cout << "*******************\n";
	cout << "���������£�\n";
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

//��ӡ���й���
void PrintRules()
{
	cout << "*******************\n";
	cout << "�������ķ����£�\n";
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
	Init();					//��ʼ���������ķ�
	PrintRules();
	FollowSet();			//���FOLLOW����FIRST��
	AnalysisTable();		//����������
	PrintTable();			//���������
	cout << "*******************\n";
	cout<<"��������ӣ����жϸþ����Ƿ�Ϸ�(��0������:\n";
	string s;
	while (cin >> s, s != "0")
		MasterConProgram(s);
	return 0;
}

/*
*������
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