#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <string>
#include <vector>
#include <utility>
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


struct Notes
{
  //Array2d<char> mmap;
  vector<string> mmap;
  string movestr;
  vector<pair<int64_t,char>> moves;
  size_t nx;
  size_t ny;
};




Notes parse_ip(string fn)
{
  Notes notes;
  auto ip=read_ip(fn);
  string movestr=ip.back();
  notes.movestr=movestr;
  ip.pop_back();
  ip.pop_back();

  //pad strings
  
  size_t len = 0;
  for(string & s : ip) len = max(len,s.size());
  for(auto & s : ip) s.append(len - s.length(), ' ');
  //for(auto & s : ip) cout << "LENGTH = " << s.size() << endl;
  notes.mmap=ip;
  stringstream ss(movestr);
  size_t lasti=0;
  size_t n=0;
  for(size_t i=0;i<movestr.size();i++)
  {
    ++n;
    char c = movestr[i];
    if(c=='L' || c=='R')
    {
      pair<int64_t,char> move{0,'-'};
      string snum = movestr.substr(lasti,n);
      cout << snum << endl;
      int64_t num = atoi(snum.c_str());
      move={num,c};
      notes.moves.push_back(move);
      lasti=i+1;
      n=0;
    }
  }
  notes.moves.push_back({atoi(movestr.substr(lasti).c_str()),'-'});
  for(auto & s : notes.mmap) cout << s << endl;
  for(auto p : notes.moves) cout << p.first << " " << p.second << endl;
  notes.nx=len;
  notes.ny=notes.mmap.size();

  //split into squares
  return notes;

}

pair<int64_t,int64_t> get_start(Notes&notes)
{
  return {notes.mmap[0].find('.',0),0};
}


pair<int64_t,int64_t> change_dir(char c, pair<int64_t,int64_t> dxdy)
{
  switch (c)
  {
  case 'R':
    dxdy={-dxdy.second,dxdy.first};
    break;
  case 'L':
    dxdy={dxdy.second,-dxdy.first};
  default:
    break;
  }
  return dxdy;
}

pair<int64_t,int64_t> move_fwd_one(Notes & notes, pair<int64_t,int64_t> xy, pair<int64_t,int64_t> dxdy,bool & success)
{
  int64_t x=xy.first;
  int64_t y=xy.second;
  int64_t dx=dxdy.first;
  int64_t dy=dxdy.second;
  success=true;
  
      //cout << "dxdy "<<dx << "," << dy << endl;
  if(dx==0)
  {
      //cout << "dx==0" << endl;
    y = y + dy;
    if(y>=notes.ny || y<0 || notes.mmap[y][x]==' ')
    {
      y = dy == 1? 0 : notes.ny-1;
      while(notes.mmap[y][x]==' ')y+=dy;
    }
  }
  else
  {
    x = x + dx;
    if(x>=notes.nx || x<0 || notes.mmap[y][x]==' ')
    {
      x = dx == 1? 0 : notes.nx-1;
      //cout << x << "," << y << endl;
      while(notes.mmap[y][x]==' ')x+=dx;
    }
  }
  if(notes.mmap[y][x]=='#')
  {
    success = false;
    return xy;
  }
  else return {x,y};
}


void single_move(Notes & notes,pair<int64_t,char> & move,pair<int64_t,int64_t> & xy,pair<int64_t,int64_t> & dxdy)
{
  bool success=true;
  for(int64_t i=0;i<move.first;i++)
  {
    cout << xy.first << "," << xy.second << endl;
    xy = move_fwd_one(notes,xy,dxdy,success);
    if(!success)break;
  }
  dxdy = change_dir(move.second,dxdy);
  
  //cout << xy.first << "," << xy.second << endl;
}

const map<pair<int64_t,int64_t>,int64_t> dir_score = {{{1,0},0},{{0,1},1},{{-1,0},2},{{0,-1},3}};

int64_t get_pw(pair<int64_t,int64_t> xy, pair<int64_t,int64_t> dxdy)
{
  //1000*row + 4*column + dir
  return 1000*(xy.second+1) + 4*(xy.first+1) + dir_score.at(dxdy);
}

void play(string fn)
{
  Notes notes = parse_ip(fn);

  auto xy = get_start(notes);
  //cout << xy.first << "," << xy.second << endl;
  pair<int64_t,int64_t> dxdy{1,0};

  for(auto & m : notes.moves)
    single_move(notes,m,xy,dxdy);
  
  int64_t answer = get_pw(xy,dxdy);


  cout << fn << " Part 1 answer = " << answer << endl;

}

int main()
{
  play("input.txt");
  play("test.txt");

  return 0;
}
