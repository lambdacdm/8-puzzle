#include<iostream>
#include<vector>
#include<queue>
#include<cmath>
#include<string>
#include<algorithm>
#include<ctime>
#include<cstdint>
using namespace std;
const vector<unsigned long long> gamma={0,1,1,2,6,24,120,720,5040,40320,362880,3628800
,39916800,479001600,6227020800,87178291200,1307674368000,20922789888000};
const vector<int8_t> dual = {0,3,4,1,2};
struct state
{
    string board;
    int8_t predirection;
    short zeroplace;
    short evaluation;
    short depth;
};
unsigned long long cantor(string a)
{
    int n = a.size();
    unsigned long long cantorvalue= 0;
    unsigned lessnum = 0;
    for (int i = 0; i < n;++i)
    {
        lessnum = 0;
        for (int j = i + 1; j < n;++j)
        {
            if(a[j]<a[i])
                ++lessnum;
        }
        cantorvalue += lessnum * gamma[n - i];
    }
    return cantorvalue;
}
string inversecantor(unsigned long long x,int n)
{
    vector<char> candidate(n);
    for (int i = 0; i < n;++i)
        candidate[i] = i + '1';
    string a;
    for (int i = 0; i < n;++i)
    {
        unsigned long long temp = gamma[n - i];
        unsigned r =x / temp;
        a.push_back(candidate[r]);
        candidate.erase(candidate.begin() + r);
        x %= temp;
    }
    return a;
}
short evaluate(string a)
{
    int n = sqrt(a.size());
    short s = 0;
    for (int i = 0; i < int(a.size());++i)
    {
        s += abs(i/n-(a[i]-'1')/n) + abs(i%n-(a[i]-'1')%n);
    }
    return s;
}
void disp(string a)
{
    int n = sqrt(a.size());
    cout<< "---------------------------------" << endl;
    for (int i = 0; i <n;++i)
    {
        for (int j = 0; j <n;++j)
        {
            int location = i * n + j;
            if(a[location]==int(a.size())+'0')
                cout << ' ';
            else
                cout << a[location];
        }
        cout << endl;
    }
    cout << "---------------------------------" << endl;
    cout << endl<<endl;
}
bool go(state &one,int8_t direction,int n)
{
    int8_t p = one.zeroplace;
    if(direction==1 && p-n>=0)//上
    {
        swap(one.board[p], one.board[p - n]);
        one.zeroplace -= n;
        return true;
    }
    if(direction==2 && (p+1)%n!=0)//右
    {
        swap(one.board[p], one.board[p + 1]);
        one.zeroplace +=1;
        return true;
    }
    if(direction==3 && p+n<n*n)//下
    {
        swap(one.board[p], one.board[p + n]);
        one.zeroplace += n;
        return true;
    }
    if(direction==4 && p%n!=0)//左
    {
        swap(one.board[p], one.board[p-1]);
        one.zeroplace -= 1;
        return true;
    }
    return false;
}
state initialize(string initialboard)
{
    state initialstate;
    initialstate.board = initialboard;
    int p=0;
    for (unsigned i = 0; i < initialboard.size();++i)
        if(initialboard[i]=='0')
            p=i;
    initialstate.zeroplace = p;
    initialstate.predirection = 0;
    initialstate.board[p]=initialboard.size()+'0';
    initialstate.evaluation = evaluate(initialboard);
    initialstate.depth = 0;
    return initialstate;
}
template<class pq>
void disproute(const state &initialstate,const pq &tree,int n,const vector<int8_t> &predirectionlist)
{
    if(tree.size()>0)
    {
        auto node = tree.top();
        deque<int8_t> directionlist;
        while(node.board!=initialstate.board)
        {
            auto direction =predirectionlist[cantor(node.board)];
            directionlist.push_front(direction);
            go(node, dual[direction], n);
        }
        auto curstate = initialstate;
        cout << "最短步数：" << directionlist.size() << "步" << endl;
        for (unsigned i = 0; i < directionlist.size();++i)
        {
            cout << "第" << i + 1 << "步" << endl;
            auto direction = directionlist[i];
            go(curstate, direction, n);
            disp(curstate.board);
        }
    }
    else
    {
        cout << "无解" << endl;
    }
}
template <class pq>
bool expand(pq &tree,const state &initialstate,vector<bool> &visited,vector<int8_t> &predirectionlist)
{
    int n = sqrt(initialstate.board.size());
    auto currentnode = tree.top();
    unsigned long long cantorvalue = cantor(currentnode.board);
    if(cantorvalue==0 || tree.size()==0)
    {
        predirectionlist[cantorvalue] = currentnode.predirection;
        return false;
    }  
    tree.pop();
    if(!visited[cantorvalue])
    {
        predirectionlist[cantorvalue] = currentnode.predirection;
        visited[cantorvalue] = true;
        for(int8_t direction:{1,2,3,4})
        {
            auto child = currentnode;
            if(go(child, direction, n) )
            {
                unsigned long long childcantorvalue = cantor(child.board);
                if( !visited[childcantorvalue])
                {
                    child.depth = currentnode.depth + 1;
                    child.evaluation = evaluate(child.board)+child.depth;
                    child.predirection = direction;
                    tree.push(child);
                }
            }      
        }
    }
    return true;
}
void game(string initialboard)
{
    clock_t st1,et1,st2,et2;
    st1=clock();
    int n = sqrt(initialboard.size());
    state initialstate = initialize(initialboard);
    auto cmp = [](state left, state right) { return left.evaluation>right.evaluation; };//优先队列的top是evaluation小的
    priority_queue<state, vector<state>, decltype(cmp)> tree(cmp);
    tree.push(initialstate);
    vector<bool> visited(gamma[initialboard.size()+1]);
    vector<int8_t> predirectionlist(gamma[initialboard.size() + 1]);
    while (expand(tree, initialstate, visited,predirectionlist));
    et1=clock();
    st2 = clock();
    disproute(initialstate, tree, n,predirectionlist);
    et2 = clock();
    cout << "搜索用时：" << double(et1 - st1)/1000 << "秒" << endl;
    cout<<"打印解法步骤用时："<<double(et2 - st2)/1000 << "秒" << endl<<endl;
}
int main()
{
    while(1)
    {
        cout << "请输入3*3数字华容道盘面，空格以0表示" << endl;
        cout << "输入实例：" << endl;
        cout << "7 5 4\n2 3 0\n8 6 1" << endl;
        cout << "在下面输入：" << endl;
        string a("123456780");
        for (int i = 0; i < 9;++i)
            cin >> a[i];
        game(a);
    }
    system("pause");
    return 0;
}