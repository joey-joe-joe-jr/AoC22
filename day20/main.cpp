#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <string>
#include <vector>
#include <deque>
#include <array>
#include <set>
#include <map>
#include <bitset>
#include <algorithm>
#include <numeric>
#include <cmath>

using namespace std;

vector<string> read_ip(string s)
{
  vector<string> lines;
  string line;
  ifstream ip(s);
  while(getline(ip,line))
    lines.push_back(line);
  return lines;
}

struct Node
{
  int64_t val;
  size_t pos;
  Node * next;
  Node * prev;
};

class Nodes
{
public:
  vector<Node> nodes;
  Node * front;
  Node * back;

  Nodes(const vector<int64_t> & v) : nodes(v.size())
  {
    for(size_t i=0;i<v.size();i++)
    {
      nodes[i].val=v[i];
      nodes[i].pos=i;
    }

    for(size_t i=1;i<v.size()-1;i++)
    {
      nodes[i].prev=&nodes[i-1]; 
      nodes[i].next=&nodes[i+1];
    }
    nodes[0].prev=&nodes.back();
    nodes[0].next=&nodes[1];
    nodes[nodes.size()-1].prev=&nodes[nodes.size()-2];
    nodes[nodes.size()-1].next=&nodes[0];
    front = &nodes.front();
    back = &nodes.back();
  }

  size_t size(){return nodes.size();}
  Node * begin(){return front;}
  Node * end(){return front;}

  //insert value before pos
  Node * insert(Node * pos, Node & node)
  {
    node.next=pos;
    node.prev=pos->prev;
    pos->prev=&node;
    node.prev->next=&node;
    return &node;
  }

  void print()
  {
    cout << endl;
    auto ptr = front;
    for(int64_t i=0;i<size();i++)
    {
      int64_t v = ptr->val;
      size_t pos = ptr->pos;
      cout << v << endl;
      ptr = ptr->next;
    }
    cout << endl;
  }

  int64_t answer()
  {
    int64_t a=0;
    Node * ptr=nullptr;
    for(Node & n : nodes)
      if(n.val==0)
         ptr=&n;
    
    if(ptr!=nullptr)
    {
      for(int64_t i=1;i<=3000;i++)
      {
        ptr = ptr->next;
        if(i%1000 == 0)
        {
          a+=ptr->val;
        }
      }
    }
    return a;
  }

  int64_t answer2(int64_t k)
  {
    int64_t a=0;
    Node * ptr=nullptr;
    for(Node & n : nodes)
      if(n.val==0)
         ptr=&n;
    
    if(ptr!=nullptr)
    {
      for(int64_t i=1;i<=3000;i++)
      {
        ptr = ptr->next;
        if(i%1000 == 0)
        {
          int64_t v=ptr->val/k;
          a+=v;
        }
      }
    }
    return a*k;
  }

  void move_according_to_val_at_index(int64_t i)
  {
    int64_t val1 = nodes[i].val;
    Node & node = nodes[i];
    int64_t val=val1%(size()-1);
    if(val!=0)
    {
      if(front == &nodes[i]) front = nodes[i].next;
      if(back ==  &nodes[i]) back = nodes[i].prev;
      
      nodes[i].prev->next=nodes[i].next;
      nodes[i].next->prev=nodes[i].prev;

      Node * ptr = &nodes[i];
      
      if(val>0)
      {
        for(int64_t j=0;j<=val;j++)
          ptr=ptr->next;
        insert(ptr,nodes[i]);
      }
      else
      {
        for(int64_t j=-val;j>0;j--)
          ptr=ptr->prev;
        insert(ptr,nodes[i]);
      }
    }
  }


};

auto parse_ip(string fn)
{
  vector<int64_t> vec;
  auto ip = read_ip(fn);
  for(string s : ip)
    vec.push_back(atoi(s.c_str()));

  //for(int64_t v:vec) cout << v << endl;
  Nodes rtn(vec);

  return rtn;
}

void part1(string fn)
{
  Nodes nodes=parse_ip(fn);
  for(int64_t i=0;i<nodes.size();i++)
    nodes.move_according_to_val_at_index(i);
  cout <<endl;
  //nodes.print();
  cout << fn << " Part 1 answer = " << nodes.answer() << endl;
}

void part2(string fn, int64_t ncycles, int64_t k)
{
  Nodes nodes=parse_ip(fn);
  for(int64_t i=0;i<nodes.size();i++)
  {
    nodes.nodes[i].val*=k;
  }
  for(int64_t n=0;n<ncycles;n++)
    for(int64_t i=0;i<nodes.size();i++)
      nodes.move_according_to_val_at_index(i);
  cout <<endl;
  //nodes.print();

  cout << fn << " " << ncycles << " cycles with factor " << k << "  answer = " << nodes.answer2(k) << endl;
}

int main()
{
  part1("input.txt");
  part1("test.txt");
  part2("input.txt",1,1);
  part2("test.txt",1,1);
  part2("input.txt",10,811589153);
  part2("test.txt",10,811589153);

  return 0;
}
