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


class Rock
{
public:
  array<bitset<4>,4> shape;
  int type;
  Rock(int i)
  {
    type = i%5;
    switch(i%5)
    {
      case(0):
        shape = {bitset<4>{"0000"},
                 bitset<4>{"0000"},
                 bitset<4>{"0000"},
                 bitset<4>{"1111"}};
        break;
      case(1):
        shape = {bitset<4>{"0000"},
                 bitset<4>{"0010"},
                 bitset<4>{"0111"},
                 bitset<4>{"0010"}};
        break;
      case(2):
        shape = {bitset<4>{"0000"},
                 bitset<4>{"0100"},
                 bitset<4>{"0100"},
                 bitset<4>{"0111"}};
        break;
      case(3):
        shape = {bitset<4>{"0001"},
                 bitset<4>{"0001"},
                 bitset<4>{"0001"},
                 bitset<4>{"0001"}};
        break;
      case(4):
        shape = {bitset<4>{"0000"},
                 bitset<4>{"0000"},
                 bitset<4>{"0011"},
                 bitset<4>{"0011"}};
        break;
    }
  }

  
  int ht()
  {
    switch (type)
    {
    case (0):
      return 1;
      break;
    case (1):
      return 3;
      break;
    case (2):
      return 3;
      break;
    case (3):
      return 4;
      break;
    case (4):
      return 2;
      break;
    }
  }
   
};


class Cave
{
public:
  deque<bitset<9>> layers;
  bitset<9> layer{"100000001"};
  int ht=1;
  Cave()
  {
    layers = {bitset<9>{"100000001"},
              bitset<9>{"100000001"},
              bitset<9>{"100000001"},
              bitset<9>{"111111111"}};
  }

  int offset(int i)
  {

  }
  
  bool touching(Rock & r, int x, int y)
  {
    for(int j=0; j<4; j++)
      for(int i=0; i<4; i++)
        if(r.shape[j][i] && layers[y+j][x+i])
          return true;         
    return false;
  }

  int shiftx(Rock & r, int x, int y, string & moves)
  {
    if(!(moves.size()>0))
    {
      cout << "ERROR OUT OF MOVES\n";exit(1);
    }
    char c = moves.back();
    moves.pop_back();
    int xdash = x;
    //cout << "try to shift " << c ;

    switch(c)
    {
      case('<'):
        xdash = x - 1;
        break;
      case('>'):
        xdash = x + 1;
        break;        
    }
    
    if(touching(r,xdash,y))
    {
      //cout << "touching!" << endl;
      return x;
    }
    else
    {
      //cout << "ok!" << endl;
      return xdash;
    }
  }

  void stop_here(Rock & r, int x, int y)
  {
    for(int j=0; j<4; j++)
      for(int i=0; i<4; i++)
        if(r.shape[j][i])layers[y+j][x+i]=true;
  }

  bool try_drop(Rock & r, int x, int & y)
  {
    //cout << "try to drop " << endl;
    int ydash = y+1;
    bool can_drop = !touching(r,x,ydash);
    if(can_drop) y = ydash;
    return can_drop;
  }

  void drop_rock(int ir, string & moves)
  {
    //make taller
    while(layers.size()<ht+7)
      layers.push_front(layer);

    Rock r(ir);

    int y = 0;
    int x = 3;

    bool falling = true;
    while(falling)
    { 
      x = shiftx(r,x,y,moves);
      falling = try_drop(r,x,y);
    }

    stop_here(r,x,y);
    //ht = y + 4 - r.ht();
    ht = layers.size() - (y + 4 - r.ht());
    //old ht = sz-8
    //sz - (y + 4 - r.ht)
    cout << "new height " << ht << endl;
  }

  void print()
  {
    for(auto & l : layers)
    {
      for(int i=0;i<9;i++)
        if(l[i]) cout << '@';
        else cout << '.';
      cout << endl;
    }
    cout << endl;
  }

   
};

void play(string fn, int n)
{
  Cave cave;
  auto ip = read_ip(fn);
  string ipmoves = ip[0];
  reverse(ipmoves.begin(),ipmoves.end());
  
  string moves = ipmoves;

  //cave.print();
  for(int i=0; i<n; i++)
  {
    if(moves.size()<10) moves = ipmoves + moves;
    cave.drop_rock(i,moves);
    //cave.print();
  }
  cave.print();
  cout << "height" << cave.ht-1 << endl;
  cout << endl;

}


int main()
{
  play("input.txt",2022);
  play("test.txt",10);

  return 0;
}
