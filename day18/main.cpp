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

set<array<int,3>> parse_ip(string fn)
{
  auto ip = read_ip(fn);
  set<array<int,3>> data;
  for(string & line : ip)
  {
    int x,y,z;
    sscanf(line.c_str(),"%d,%d,%d",&x,&y,&z);
    data.insert({x,y,z});
  }

  for(auto & a : data)
    cout << a[0] << "," << a[1] << "," << a[2] << endl;
  
  return data;
}

bool is_at(const set<array<int,3>> & pts,const array<int,3> & pt)
{
  if(pts.find(pt)!=pts.end()) return true;
  else return false;
}

bool is_adj(const set<array<int,3>> & pts,const array<int,3> & pt)
{
  int i=pt[0];
  int j=pt[1];
  int k=pt[2];
  if(is_at(pts,{i-1,j,k}))return true;
  if(is_at(pts,{i+1,j,k}))return true;
  if(is_at(pts,{i,j-1,k}))return true;
  if(is_at(pts,{i,j+1,k}))return true;
  if(is_at(pts,{i,j,k-1}))return true;
  if(is_at(pts,{i,j,k+1}))return true;
  return false;
}

size_t nsurfaces(const set<array<int,3>> & pts,const array<int,3> & pt)
{
  int nadj=0;
  int i=pt[0];
  int j=pt[1];
  int k=pt[2];
  if(is_at(pts,{i-1,j,k})){nadj++;}
  if(is_at(pts,{i+1,j,k})){nadj++;}
  if(is_at(pts,{i,j-1,k})){nadj++;}
  if(is_at(pts,{i,j+1,k})){nadj++;}
  if(is_at(pts,{i,j,k-1})){nadj++;}
  if(is_at(pts,{i,j,k+1})){nadj++;}
  return 6 - nadj;
}

size_t count_surfaces(const set<array<int,3>> & pts)
{
  size_t sum=0;
  for(const auto & pt : pts)
  {
    sum+=nsurfaces(pts,pt);
  }
  return sum;
}

void part1(string fn)
{
  auto pts = parse_ip(fn);

  int answer = count_surfaces(pts);

  cout << fn << " Part 1 answer = " << answer << endl;
  

}



void print(set<array<int,3>> & pts, int n)
{
  for(int i=0; i<n; i++)
  {
    cout << " slice " << i << endl<<endl;
    for(int j=0; j<n; j++)
    {
      for(int k=0; k<n; k++)
      {
        if(is_at(pts,{i,j,k})) cout << "#";
        else cout << ".";
      }
      cout << endl;
    }
    cout << endl << endl;
  }

}

size_t flood_fill(set<array<int,3>> & pts, set<array<int,3>> & filled, array<int,3> pt, int n)
{
  int i,j,k;
  array<int,3> p;
  int i0=pt[0];
  int j0=pt[1];
  int k0=pt[2];
  filled.insert(pt);
  
  i=i0-1;p={i,j0,k0};
  if(i>=0 && !is_at(pts,p) && !is_at(filled,p))
    flood_fill(pts,filled,p,n);
  i=i0+1;p={i,j0,k0};
  if(i<n && !is_at(pts,p) && !is_at(filled,p))
    flood_fill(pts,filled,p,n);
  j=j0-1;p={i0,j,k0};
  if(j>=0 && !is_at(pts,p) && !is_at(filled,p))
    flood_fill(pts,filled,p,n);
  j=j0+1;p={i0,j,k0};
  if(j<n && !is_at(pts,p) && !is_at(filled,p))
    flood_fill(pts,filled,p,n);
  k=k0-1;p={i0,j0,k};
  if(k>=0 && !is_at(pts,p) && !is_at(filled,p))
    flood_fill(pts,filled,p,n);
  k=k0+1;p={i0,j0,k};
  if(k<n && !is_at(pts,p) && !is_at(filled,p))
    flood_fill(pts,filled,p,n);

  return filled.size();
}

void part2(string fn,int n)
{
  auto pts = parse_ip(fn);
  
  set<array<int,3>> air;

  for(int i=-1;i<n+1;i++)
    for(int j=-1;j<n+1;j++)
      for(int k=-1;k<n+1;k++)
        if(!is_at(pts,{i,j,k}))
          air.insert({i,j,k});

  map<size_t,set<array<int,3>>> airsets;
  size_t nsets;
 
  set<array<int,3>> outside,inside;
  size_t nout = flood_fill(pts,outside,{0,0,0},n);
  for(int i=0;i<n;i++)
    for(int j=0;j<n;j++)
      for(int k=0;k<n;k++)
        if(!is_at(outside,{i,j,k}))
          inside.insert({i,j,k});
  print(inside,n);


  int answer = count_surfaces(inside);
  cout << inside.size() << endl;
  cout << fn << " Part 2 answer = " << answer << endl;
  
  //print(pts,n);
}

int main()
{
  //part1("input.txt");
  //part1("test.txt");
 // part2("input.txt",22);
  part2("input.txt",22);
  //part2("test.txt",7);
  return 0;
}
