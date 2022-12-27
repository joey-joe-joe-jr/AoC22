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

  void print_bmap(set<pair<int64_t,int64_t>> & bm, size_t nr, size_t nc)
  {
    string s;
    s.append(nc+2, '#');
    cout << s << endl;
    for(size_t ir=0;ir<nr;ir++)
    {
      cout << "#";
      for(size_t ic=0;ic<nc;ic++)
      {
         auto it = bm.find({ir,ic});
         if(it!=bm.end()) {cout << "x";}
        else cout << ".";
      }
      cout << "#\n";
    }
    cout << s << endl;
    cout << "\n\n" ;

  }

template<typename T>
class Array2d
{
  vector<T> data_;
  size_t n1_=0;
  size_t n2_=0;
public:
  Array2d(){}
  Array2d(size_t n1, size_t n2){resize(n1,n2);}
  Array2d(size_t n1, size_t n2, T v){resize(n1,n2);fill(v);}

  auto & operator()(size_t i, size_t j){return data_[n2_*i+j];}
  void  resize(size_t n1, size_t n2)
  {
    if(n1!=n1_ || n2!=n2_)
    {
      n1_=n1;
      n2_=n2;
      data_.resize(n1*n2);
    }
  }
  void fill(T v){for(auto & e : data_)e=v;}
  auto begin(){return data_.begin();}
  auto end(){return data_.end();}
  void print()
  {
    for(size_t i=0;i<n1_;i++)
    {
      for(size_t j=0;j<n2_;j++)
        cout << (*this)(i,j);
      cout << endl;
    }
  }
  size_t ni(){return n1_;}
  size_t nj(){return n2_;}
      
};


//relative positions of "reachable" neighbours
array<pair<int64_t,int64_t>,5> nbours{pair<int64_t,int64_t>{-1,0},
                                      pair<int64_t,int64_t>{1,0},
                                      pair<int64_t,int64_t>{0,-1},
                                      pair<int64_t,int64_t>{0,1},
                                      pair<int64_t,int64_t>{0,0}};


struct Valley
{
  size_t nr;
  size_t nc;
  Array2d<int64_t> curr;
  array<vector<pair<int64_t,int64_t>>,4> bliz;
  set<pair<int64_t,int64_t>> canreach;
  Valley(size_t nrow,size_t ncol):nr(nrow),nc(ncol),curr(nr,nc,0){}

  void move_bliz(pair<int64_t,int64_t> & b, int64_t dir)
  {
    switch(dir)
    {
      case(0)://R
        ++b.second;
        if(b.second==nc)b.second=0;
        break;
      case(1)://D
        ++b.first;
        if(b.first==nr)b.first=0;
        break;
      case(2)://L
        --b.second;
        if(b.second==-1)b.second=nc-1;
        break;
      case(3)://R
        --b.first;
        if(b.first==-1)b.first=nr-1;
        break;
    } 
  }

  bool in_bounds(int64_t ir, int64_t ic)
  {
    return ir >= 0 && ir < nr && ic >=0 && ic < nc;
  }

  bool is_reachable(int64_t ir, int64_t ic)
  {
    if(!in_bounds(ir,ic))
    {
        return false;
    }
    else if(curr(ir,ic)==0)
    {
        return true;
    }
    else
    {
        return false;
    }
  }
  void move()
  {
    for(size_t idir=0;idir<4;idir++)
    {
      for(auto & b : bliz[idir])
      {
        --curr(b.first,b.second);
        move_bliz(b,idir);
        ++curr(b.first,b.second);
      }
    }
  }

  //return true when target reached
  bool update(pair<int64_t,int64_t> origin,pair<int64_t,int64_t> target)
  {
    move();
    set<pair<int64_t,int64_t>>canreach2;
    bool success=false;
    if(curr(origin.first,origin.second)==0)
    {
      canreach2.insert(origin);
    }
    for(auto & p : canreach)
    {
          for(auto & d : nbours)
          {
            int64_t ir = p.first + d.first;
            int64_t ic = p.second + d.second;
            if(is_reachable(ir,ic))canreach2.insert({ir,ic});
          }
    }
    canreach=canreach2;
    return canreach.find(target)!=canreach.end();
  }

  void print()
  {
    string s;
    s.append(nc+2, '#');
    cout << s << endl;
    for(size_t ir=0;ir<nr;ir++)
    {
      cout << '#';
      for(size_t ic=0;ic<nc;ic++)
      {
        if(curr(ir,ic)>0) cout << curr(ir,ic);
        else cout << '.';
      }
      cout << "#\n";
    }
    cout << s << endl;
    cout << "\n\n" ;
  }

  void print_reachable()
  {
    print_bmap(canreach,nr,nc);
  }

};


Valley parse_ip(string fn)
{
  auto ip=read_ip(fn);
  size_t nr=ip.size();
  size_t nc=ip[0].size();
  cout << nr << "x"<<nc<<endl;
  Valley v(nr-2,nc-2);
  pair<int64_t,int64_t> pos;
  for(size_t ir=0; ir<nr; ir++)
  {
    for(size_t ic=0; ic<nc; ic++)
    {
      switch (ip[ir][ic])
      {
      case '>':
        pos={ir-1,ic-1};
        v.bliz[0].push_back(pos);
        ++v.curr(ir-1,ic-1);
        break;
      case 'v':
        pos={ir-1,ic-1};
        v.bliz[1].push_back(pos);
        ++v.curr(ir-1,ic-1);
        break;
      case '<':
        pos={ir-1,ic-1};
        v.bliz[2].push_back(pos);
        ++v.curr(ir-1,ic-1);
        break;
      case '^':
        pos={ir-1,ic-1};
        v.bliz[3].push_back(pos);
        ++v.curr(ir-1,ic-1);
        break;  
      default:
        break;
      }
    }
  }
  v.print();
  return v;
  
}

void part1(string fn)
{
  Valley v=parse_ip(fn);
  set<pair<int64_t,int64_t>> canreach;
  pair<int64_t,int64_t> origin={0,0};
  pair<int64_t,int64_t> target={v.nr-1,v.nc-1};
  int mins=1;
  bool success = false;
  while(!success)
  {
    success=v.update(origin,target);
    //v.print();
    //v.print_reachable();
    mins++;
  }
  cout << fn << " Part 1 SUCCESS after " << mins << " mins\n\n";
}

void part2(string fn)
{
  Valley v=parse_ip(fn);
  int mins=1;
  bool success = false;
  pair<int64_t,int64_t> origin={0,0};
  pair<int64_t,int64_t> target={v.nr-1,v.nc-1};
  while(!success)
  {
    success=v.update(origin,target);
    mins++;
  }

  v.canreach.clear();
  swap(origin,target);
  success=false;
  while(!success)
  {
    success=v.update(origin,target);
    mins++;
  }

  v.canreach.clear();
  swap(origin,target);
  success=false;
  while(!success)
  {
    success=v.update(origin,target);
    mins++;
  }

  cout << fn << " Part 2 SUCCESS after " << mins << " mins\n\n";
}

int main()
{
  part1("input.txt");
  part1("test.txt");
  part2("input.txt");
  part2("test.txt");

  return 0;
}
