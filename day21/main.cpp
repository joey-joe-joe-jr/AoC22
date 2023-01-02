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
  map<string,uint64_t > nums;
  map<string,tuple<string,string,string>> sums;

  uint64_t  yell(string name)
  {
    uint64_t  result=0;
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
        result =  yell(monkey1) + yell(monkey2);
        break;
      case '-':
        result =  yell(monkey1) - yell(monkey2);
        break;
      case '/':
        result = yell(monkey1) / yell(monkey2);
        break;
      case '*':
        result =  yell(monkey1) * yell(monkey2);
        break;
      default:
        cout << "ERROR - sum operator not recognised\n";exit(1);
        break;
      }
      nums[name]=result;
      return result;
    }
  }

  bool is_humn_dependent(string name)
  {
    bool flag = false;
    if(name=="humn") flag = true;
    else if(nums.find(name)==nums.end())
    {
      if(is_humn_dependent(get<0>(sums[name]))) return true;
      if(is_humn_dependent(get<1>(sums[name]))) return true;
    }
    return flag;    
  }

  uint64_t  backward_sum(uint64_t  target, uint64_t  known, string op)
  {
    cout << "target ... " << target << endl;
    switch(op[0])
    {
      case '+':
        return target - known;
        break;
      case '-':
        return target + known;
        break;
      case '/':
        return target * known;
        break;
      case '*':
        return target / known;
        break;
      default:
        cout << "ERROR - sum operator not recognised\n";
        exit(1);
        break;
    }
  }

  uint64_t  left_unknown_backward_sum(uint64_t  target, uint64_t  known, string op)
  {
    //cout << "target ... " << target << endl;
    switch(op[0])
    {
      case '+':
        return target - known;
        break;
      case '-':
        return target + known;
        break;
      case '/':
        return target * known;
        break;
      case '*':
        return target / known;
        break;
      default:
        cout << "ERROR - sum operator not recognised\n";
        exit(1);
        break;
    }
  }

  uint64_t  right_unknown_backward_sum(uint64_t  target, uint64_t  known, string op)
  {
    //cout << "target ... " << target << endl;
    switch(op[0])
    {
      case '+':
        return target - known;
        break;
      case '-':
        return known - target;
        break;
      case '/':
        return known / target;
        break;
      case '*':
        return target / known;
        break;
      default:
        cout << "ERROR - sum operator not recognised\n";
        exit(1);
        break;
    }
  }

  uint64_t  get_humn(uint64_t  target, string left, string right, string op)
  {
    string known_monkey, unknown_monkey;
    bool right_known;
    if(is_humn_dependent(left))
    {
        right_known = true;
        known_monkey = right;
        unknown_monkey = left;
    }
    else
    {
        right_known=false;
        known_monkey = left;
        unknown_monkey = right;
    }

    if(unknown_monkey=="humn")
    {
      if(right_known) return left_unknown_backward_sum(target,yell(known_monkey),op);
      else            return right_unknown_backward_sum(target,yell(known_monkey),op);

    }
    else
    {
      uint64_t  new_target; 
      if(right_known) new_target =  left_unknown_backward_sum(target,yell(known_monkey),op);
      else            new_target = right_unknown_backward_sum(target,yell(known_monkey),op);
     // uint64_t  new_target = backward_sum(target,yell(known_monkey),op);
      string new_name1 = get<0>(sums[unknown_monkey]);
      string new_name2 = get<1>(sums[unknown_monkey]);
      string op = get<2>(sums[unknown_monkey]);
      return get_humn(new_target,new_name1,new_name2,op);
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
    uint64_t  num=-1; //i.e. not set: no negatives in input
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


void part2(string fn)
{
  Riddle riddle = parse_ip(fn);
  auto t = riddle.sums["root"];
  string name1 = get<0>(t);
  string name2 = get<1>(t);
  string known_monkey, unknown_monkey;
  
  if(riddle.is_humn_dependent(name1))
  {
    known_monkey = name2;
    unknown_monkey = name1;
  }
  else
  {
    known_monkey = name1;
    unknown_monkey = name2;
  }
  
  uint64_t  target = riddle.yell(known_monkey);
  string new_name1 = get<0>(riddle.sums[unknown_monkey]);
  string new_name2 = get<1>(riddle.sums[unknown_monkey]);
  string op = get<2>(riddle.sums[unknown_monkey]);

  cout << fn << " Target of " << target << " for monkey " << unknown_monkey << endl;

  uint64_t  answer = riddle.get_humn(target,new_name1,new_name2,op);

  cout << fn << " Answer to part 2 = " << answer << endl;

  
  //test
  riddle.nums["humn"]=answer;

  uint64_t  v = riddle.yell(unknown_monkey);
  if(v==target) cout << "test passed\n";
  else cout << "test failed " << v << " not equal to target of " << target << endl;
}

int main()
{
  part1("input.txt");
  part1("test.txt");

  part2("input.txt");
  part2("test.txt");

  return 0;
}
