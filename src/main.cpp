#ifdef VARTYPEDICT
#include"VarTypeDict.cpp"
#include<iostream>
struct Long_t;
struct Width_t;
struct Hight_t;

using TestUtil = VarTypeDict<Long_t, Width_t, Hight_t>;
int main()
{
	auto tmp = TestUtil::Create().Set<Long_t>(4).Set<Width_t>(3.5f).Set<Hight_t>(3);
	auto a = tmp.template Get<Long_t>();
	auto b = tmp.template Get<Width_t>();
	auto c = tmp.template Get<Hight_t>();
	std::cout << a + b * c << std::endl;
}
#endif // VARTYPEDICT
#include<iostream>
#include<string>
#include<vector>
typedef struct ListNode
{
	int val;
	ListNode* next;
}* Node;
// 1 2 3 2 1
// 1 2 2 1
bool fun(Node head)
{
	std::vector<int> List;
	while (head)
	{
		List.push_back(head->val);
		head = head->next;
	}
	for (int i = 0; i < List.size() / 2; i++)
	{
		int pos = List.size() - i - 1;
		if (List[pos] != List[i])
			return false;
	}
	return true;
}