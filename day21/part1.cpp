#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <tuple>
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

struct Riddle
{
  map<string,int64_t> nums;
  map<string,tuple<string,string,string>> sums;

  int64_t yell(string name)
  {
    auto it = nums.find(name);
    if(it!=nums.end()) return it->second;
    else
    {
      auto it2 = sums.find(name);
      tuple<string,string,string> & sum = it2->second;
      string & monkey1 = get<0>(sum);
      string & monkey2 = get<1>(sum);
      char op = get<2>(sum)[0];
      switch (op)
      {
      case '+':
        return yell(monkey1) + yell(monkey2);
        break;
      case '-':
        return yell(monkey1) - yell(monkey2);
        break;
      case '/':
        return yell(monkey1) / yell(monkey2);
        break;
      case '*':
        return yell(monkey1) * yell(monkey2);
        break;
      default:
        cout << "ERROR - sum operator not recognised\n";exit(1);
        break;
      }
    }
  }

  void print()
  {
    for(auto & p:nums) cout << p.first << " : " << p.second << endl;
    for(auto & p:sums) cout << p.first << " : " << get<0>(p.second) << get<2>(p.second) << get<1>(p.second) << endl;
  }
};


Riddle parse_ip(string fn)
{
  Riddle riddle;
  auto ip = read_ip(fn);
  for(string s : ip)
  {
    string snum,dep1,dep2,op;
    string name = s.substr(0,4);
    string job = s.substr(6);
    int64_t num=-1; //i.e. not set: no negatives in input
    if(job.size()==11)
    {
      dep1=job.substr(0,4);
      op=job.substr(5,1);
      dep2=job.substr(7);
      riddle.sums[name]={dep1,dep2,op};
    }
    else
    {
      snum=job.substr(0);
      num=atoi(snum.c_str());
      riddle.nums[name]=num;
    }
  }
  return riddle;
}

void part1(string fn)
{
  Riddle riddle = parse_ip(fn);
  //riddle.print();
  cout << fn << " Asnwer to part 1 = " << riddle.yell("root") << endl;
}

int main()
{
  part1("input.txt");
  part1("test.txt");

  return 0;
}
