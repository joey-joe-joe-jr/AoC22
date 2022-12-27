#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <string>
#include <vector>
#include <array>
#include <set>
#include <map>
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

void print_bmap(set<pair<int,int>> & bm, int rmin, int rmax, int cmin, int cmax)
{
  cout << "rows " << rmin << "->" << rmax << endl;
  cout << "cols " << cmin << "->" << cmax << endl;

  for(int ir=rmin;ir<=rmax;ir++)
  {
      //cout << ir ;
    for(int ic=cmin;ic<=cmax;ic++)
    {
      //cout << ir << "," << ic << " ";
      auto it = bm.find({ir,ic});
      if(it!=bm.end()) {cout << "#";}
      else cout << ".";
    }
    cout << "\n";
  }
  cout << "\n\n" ;
}

bool empty(const vector<pair<int,int>> & check_these, set<pair<int,int>> & positions)
{
  for(auto & p : check_these)
  {
    auto it = positions.find(p);
    if(it!=positions.end()) return false;
  }
  return true;
}

struct Rule
{
  pair<int,int> operator()(pair<int,int> pos, set<pair<int,int>> & positions, int dir)
  {
    //dir %=4;
    int r=pos.first;
    int c=pos.second;
    switch(dir)
    {
      case 0:
        if(empty({{r-1,c-1},{r-1,c},{r-1,c+1}},positions))
        pos={r-1,c};
        break;
      case 1:
        if(empty({{r+1,c-1},{r+1,c},{r+1,c+1}},positions))
        pos={r+1,c};
        break;
      case 2:
        if(empty({{r-1,c-1},{r,c-1},{r+1,c-1}},positions))
        pos={r,c-1};
        break;
      case 3:
        if(empty({{r-1,c+1},{r,c+1},{r+1,c+1}},positions))
        pos={r,c+1};
        break;
      default:
        cout << "ERROR\n";
        exit(1);
    }
    return pos;
  }

};


struct Elfs
{
  Rule rule;
  int imoves=0;
  int idir=0;
  set<pair<int,int>> positions;
  int nr;
  int nc;
  int row_min=0;
  int row_max;
  int col_min=0;
  int col_max;

  Elfs(vector<string> & ip):nr(ip.size()),nc(ip[0].size())
  {
    row_max=nr-1;
    col_max=nc-1;
    for(int ir=0;ir<nr;ir++)
      for(int ic=0;ic<nc;ic++)
        if(ip[ir][ic] == '#')
        {
          positions.insert({ir,ic});
        }
  }

  void update_num_rows_cols()
  {
    row_min = positions.begin()->first;
    row_max = positions.begin()->first;
    col_min = positions.begin()->second;
    col_max = positions.begin()->second;
    for(auto & p : positions)
    {
      int ir = p.first;
      int ic = p.second;
      row_min=min(row_min,ir);
      col_min=min(col_min,ic);
      row_max=max(row_max,ir);
      col_max=max(col_max,ic);
    }
    nr = row_max-row_min+1;
    nc = col_max-col_min+1;
  }

  set<pair<int,int>> get_new_positions(int imoves)
  {
    map<pair<int,int>,pair<int,int>> newold;
    set<pair<int,int>> dups;
    set<pair<int,int>> newpositions;

    for(auto & p : positions)
    {
      int r=p.first;
      int c=p.second;
      pair<int,int> newp;
      for(int id = imoves; id<imoves+4; id++)
      {
        int i = id%4;
        if(empty({{r-1,c-1},{r-1,c},{r-1,c+1},{r,c-1},{r,c+1},{r+1,c-1},{r+1,c},{r+1,c+1}},positions))
          newp=p;
        else
          newp = rule(p,positions,i);
        if(newp!=p) break;
      }
      
      auto it = newold.find(newp);
      if(it==newold.end())
      {
        newold.insert({newp,p});
        newpositions.insert(newp);
      }
      else
      {
        newpositions.erase(newp);
        newpositions.insert(it->second);
        newpositions.insert(p);
      }
    }
    if(positions.size()!=newpositions.size())
    {
      cout << "ERROR - we;ve gained or losr elfs something gone wrong\n";
      exit(1);
    }
    return newpositions;   
  }

  bool move(int pr)
  {
    auto new_positions = get_new_positions(imoves);
    update_num_rows_cols();
    idir++;
    imoves++;

    if(new_positions == positions)
    {
      cout << "After " << imoves << " moves, no elf moves\n" << endl;
      return true;
    }

    positions = new_positions;

    if(imoves%pr==0)
    {
      update_num_rows_cols();
      print();
      int sz = nr*nc;
      int tota = (row_max-row_min+1)*(col_max-col_min+1);
      cout << "nrow x ncol = " << nr << "x" << nc << " num(elf) = " <<positions.size()<<endl;
      cout << "After " << imoves <<  " moves, empty spaces = " << tota - positions.size() << endl;
    }
    return false;
  }

  void print(){update_num_rows_cols();print_bmap(positions,row_min,row_max,col_min,col_max);}
};

void part1(string fn)
{
  Elfs elfs(read_ip(fn));
  elfs.print();

  for(int i=0;i<10;i++)
  {
    elfs.move(10);
  }
}

void part2(string fn)
{
  Elfs elfs(read_ip(fn));
  //elfs.print();

  bool success=false;
  int i=0;
  while(!success)
  {
    success=elfs.move(100);
    i++;
  }

  cout << fn << " Part 2 answer = " << i << endl;

}

int main()
{
  part1("input.txt");
  part1("test.txt");
  part2("input.txt");
  part2("test.txt");

  return 0;
}
