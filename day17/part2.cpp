#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <string>
#include <vector>
#include <tuple>
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
  int64_t type;
  Rock(int64_t i)
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

  
  int64_t ht()
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
  int64_t ht=1;
  int64_t offset=0;
  int64_t iplen;
  int64_t bight=1;  
  int64_t nrock=0; 
  int64_t nshift=0;
  map<pair<int64_t,int64_t>,tuple<deque<bitset<9>>,int64_t,int64_t>> snapshots;
  bool is_repetition = false;
  int64_t rep_start = 0;
  int64_t rep_length = 0;
  int64_t rep_ht_start = 0;
  Cave()
  {
    layers = {bitset<9>{"100000001"},
              bitset<9>{"100000001"},
              bitset<9>{"100000001"},
              bitset<9>{"111111111"}};
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
    ++nshift;
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

    int64_t height()
  {
    for(int i=0;i<layers.size()-1;i++)
    {
      for(int j=1;j<8;j++)
        if(layers[i][j])
        {
          return layers.size()-i;
        }
    }
    return 0;
  }



  void update_height()
  {
    //check to see if two consecutive layers "block" the cave
    //n.b. in the test input a single layer never blocks the cave
    for(int i=1;i<layers.size()-1;i++)
    {
      bitset<9> lyr = layers[i] | layers[i-1];
      if(lyr.all())
      { 
        //print();
        deque<bitset<9>> newlayers(i+1);
        int64_t doff = (layers.size()-i-1);
        offset += doff;
        copy(layers.begin(),layers.begin()+i+1,newlayers.begin());
        layers=newlayers;

        int64_t ht1 = height();

        //if(ht1 < 5 && !is_repetition)
        if(!is_repetition);
        {
          //cout << "TETRIS\n";
          pair<int64_t,int64_t> combo{nrock%5,nshift%iplen};
          if(snapshots.find(combo)==snapshots.end())
          {
            snapshots[combo]={layers,nrock,height()+offset};
          }
          else
          {
            if(get<0>(snapshots[combo])==layers)
            {
              print();
              cout << "TETRIS! ht " << ht1 << " " << get<1>(snapshots[combo]) << " " << nrock << " " << nshift%iplen << endl;
              is_repetition = true;
              rep_start = get<1>(snapshots[combo]);
              rep_length = nrock-rep_start;
              rep_ht_start = get<2>(snapshots[combo]);
            }
            else
            {
               snapshots[combo]={layers,nrock,height()+offset};
            }
            
          }
        }     
        break;  
      }
    }
    ht = height();
    bight = offset + ht;
  }

  void drop_rock(int64_t ir, string & moves)
  {
    nrock++;
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
    update_height();
    ht = layers.size() - (y + 4 - r.ht());
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

void play(string fn, int64_t n)
{
  Cave cave;
  auto ip = read_ip(fn);
  string ipmoves = ip[0];
  reverse(ipmoves.begin(),ipmoves.end());
  cave.iplen = ipmoves.size();
  
  string moves = ipmoves;

  //cave.print();
  int64_t rep_ht=0;
  //for(int i=0; i<n; i++)
  int64_t i=0;
  bool first_rep=true;
  while(i<n)
  {
    if(i%1000==0) cout << i << endl;
    if(moves.size()<10) moves = ipmoves + moves;
    cave.drop_rock(i,moves);
    if(cave.is_repetition && first_rep)
    {
      int64_t htnow = cave.bight-1;
      int64_t dht = htnow + 1 - cave.rep_ht_start;
      int64_t rocks_remaining = n-i-1;
      int64_t nreps_left = rocks_remaining/cave.rep_length;
      int64_t remainder = rocks_remaining%cave.rep_length;
      rep_ht = nreps_left*dht;


      cout << "I think we've seen a repetition at rock = " << i << endl;
      cout << "htnow = " << htnow << endl;
      cout << "dht = " << dht << endl;
      cout << "rocks_remaining = " << rocks_remaining << endl;
      cout << "repetitions remaining = " << nreps_left << endl;
      cout << "remainder after reps = " << remainder << endl;

      //i=n-remainder; 
      i+=nreps_left*cave.rep_length;
      first_rep=false;
    }
    i++;
    //cave.print();
  }
  cave.print();
  cout << cave.offset << endl;
  cout << "height" << cave.bight-1 + rep_ht << endl;
  cout << endl;

}


int main()
{
  play("test.txt",1000000000000);
  play("input.txt",1000000000000);
  //play("input.txt",2022);
  //play("input.txt",292);

  return 0;
}
