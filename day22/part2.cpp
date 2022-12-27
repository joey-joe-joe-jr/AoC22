// Gives the correct answer for the test case but not the input data
// The given net is mapped onto (in my opinion!) easier to visualise "cross" net
// Provided the mapping is correct, this means that the code _should_ work for
// test input and actual input (or any net you cared to provide)

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <string>
#include <vector>
#include <array>
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

  size_t n;
  vector<vector<string>> faces;
  array<vector<string>,6> cross;
};




Notes parse_ip(string fn,size_t face_size)
{
  Notes notes;
  notes.n=face_size;
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

  notes.nx=len;
  notes.ny=notes.mmap.size();
 
  size_t nxfaces = notes.nx/face_size;
  size_t nyfaces = notes.ny/face_size; 


  for(size_t j=0;j<nyfaces;j++)
    for(size_t i=0;i<nxfaces;i++)
    {
      vector<string> face{face_size};
      for(size_t jj=0;jj<face_size;jj++)
        face[jj]=ip[j*face_size+jj].substr(i*face_size,face_size);
      notes.faces.push_back(face);
    }
      
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

  for(auto & face : notes.faces)
  {
    cout << endl;
    for(auto & s : face) cout << s << endl;
    cout << endl;
  }

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

//void warp(size_t &iface, pair<int64_t,int64_t> &xy, pair<int64_t,int64_t> &dxdy)
tuple< size_t, pair<int64_t,int64_t>, pair<int64_t,int64_t> > edge(Notes & notes, size_t iface, pair<int64_t,int64_t> xy, pair<int64_t,int64_t> dxdy)
{
  tuple< size_t, pair<int64_t,int64_t>, pair<int64_t,int64_t> > newstate;
  int64_t x=xy.first;
  int64_t y=xy.second;
  int64_t back = notes.n-1;
  pair<int64_t,int64_t> right{1,0};
  pair<int64_t,int64_t> down{0,1};
  pair<int64_t,int64_t> left{-1,0};
  pair<int64_t,int64_t> up{0,-1};
  pair<int64_t,int64_t> xynew;
  int64_t xnew=x;// = xynew.first;
  int64_t ynew=y;// = xynew.second;

  
  //cout << "EDGE! "<<x<<","<<y<<" "<<dxdy.first<<","<<dxdy.second<<" "<<iface<<"->";

  if(iface==0 && dxdy==up)
  {
    iface=5;
    ynew=0;
    dxdy=up;
  }
  else if(iface==0 && dxdy==right)
  {
    iface=3;
    xnew=back-y;
    ynew=0;
    dxdy=down;
  }
  else if(iface==0 && dxdy==left)
  {
    iface=1;
    xnew=y;
    ynew=0;
    dxdy=down;
  }
  else if(iface==0 && dxdy==down)
  {
    iface=2;
    ynew=0;
  }
  else if(iface==1 && dxdy==up)
  {
    iface=0;
    xnew=0;
    ynew=x;
    dxdy=right;
  }
  else if(iface==1 && dxdy==right)
  {
    iface=2;
    xnew=0;
  }
  //???
  else if(iface==1 && dxdy==left)
  {
    iface=5;
    xnew=0;
    ynew=back-y;
    dxdy=right;
  }
  else if(iface==1 && dxdy==down)
  {
    iface=4;
    xnew=0;
    ynew=back-x;
    dxdy=right;
  }
  else if(iface==2 && dxdy==up)
  {
    iface=0;
    ynew=back;
  }
  else if(iface==2 && dxdy==right)
  {
    iface=3;
    xnew=0;
  }
  else if(iface==2 && dxdy==left)
  {
    iface=1;
    xnew=back;
  }
  else if(iface==2 && dxdy==down)
  {
    iface=4;
    ynew=0;
  }
  else if(iface==3 && dxdy==up)
  {
    iface=0;
    xnew=back;
    ynew=back-x;
    dxdy=left;
  }
  else if(iface==3 && dxdy==right)
  {
    iface=5;
    xnew=back;
    ynew=back-y;
    dxdy=left;
  }
  else if(iface==3 && dxdy==left)
  {
    iface=2;
    xnew=back;
  }
  else if(iface==3 && dxdy==down)
  {
    //cout << "x,y " << x << "," << y << endl;
    iface=4;
    xnew=back;
    ynew=x;
    dxdy=left;
    //cout << "ynew " << ynew << endl;
  }
  else if(iface==4 && dxdy==up)
  {
    iface=2;
    ynew=back;
  }
  else if(iface==4 && dxdy==left)
  {
    iface=1;
    xnew=back-y;
    ynew=back;
    dxdy=up;
  }
  else if(iface==4 && dxdy==right)
  {
    iface=1;
    xnew=y;
    ynew=back;
    dxdy=up;
  }
  else if(iface==4 && dxdy==down)
  {
    iface=5;
    ynew=0;
  }
  else if(iface==5 && dxdy==up)
  {
    iface=4;
    ynew=back;
  }
  else if(iface==5 && dxdy==left)
  {
    iface=1;
    xnew=0;
    ynew=back-y;
    dxdy=right;
  }
  else if(iface==5 && dxdy==right)
  {
    iface=3;
    xnew=back;
    ynew=back-y;
    dxdy=left;
  }
  else if(iface==5 && dxdy==down)
  {
    iface=0;
    xnew=x;
    ynew=0;
    dxdy=down;
  }
  //cout << "ynew" << ynew << endl;
  xynew={xnew,ynew};
  newstate={iface,xynew,dxdy};
  
  //cout << iface<<endl;
  //cout << get<0>(newstate) << " xy " << get<1>(newstate).first << "," << get<1>(newstate).second<<" dxdy " << get<2>(newstate).first << "," << get<2>(newstate).second << endl;
  return newstate;
}


tuple< size_t, pair<int64_t,int64_t>, pair<int64_t,int64_t> > move_fwd_one(Notes & notes, size_t & iface, pair<int64_t,int64_t> xy, pair<int64_t,int64_t> dxdy,bool & success)
{
  tuple< size_t, pair<int64_t,int64_t>, pair<int64_t,int64_t> > state;
  pair<int64_t,int64_t> right{1,0};
  pair<int64_t,int64_t> down{0,1};
  pair<int64_t,int64_t> left{-1,0};
  pair<int64_t,int64_t> up{0,-1};
  int64_t x=xy.first;
  int64_t y=xy.second;
  int64_t dx=dxdy.first;
  int64_t dy=dxdy.second;
  success=true;
  
  int64_t back = notes.n;

  x+=dx;
  y+=dy;

  if(x == -1    && dxdy == left  ||
     x == notes.n && dxdy == right ||
     y == -1    && dxdy == up    ||
     y == notes.n && dxdy == down)
  { 
    //cout << "EDGE! "<<x<<","<<y<<" "<<dxdy.first<<","<<dxdy.second<<endl;
    state = edge(notes,iface,{x,y},dxdy);
    //cout << get<0>(state) << " xy " << get<1>(state).first << "," << get<1>(state).second<<" dxdy " << get<2>(state).first << "," << get<2>(state).second << endl;
  }
  else state = {iface,{x,y},dxdy};

  x=get<1>(state).first;
  y=get<1>(state).second;
  //auto dxdynew = get<2>(state);
  int64_t ifacenew=get<0>(state);
  if(notes.cross[ifacenew][y][x]=='#')
  {
    //cout << "HIT A #!!\n";
    success = false;
    return {iface,xy,dxdy};
  }
  else return state;
}


void single_move(Notes & notes,pair<int64_t,char> & move,size_t & iface, pair<int64_t,int64_t> & xy,pair<int64_t,int64_t> & dxdy)
{
  bool success=true;
  for(int64_t i=0;i<move.first;i++)
  {
    //cout << iface <<  " " << xy.first << "," << xy.second << endl;
    auto state = move_fwd_one(notes,iface,xy,dxdy,success);
    //cout << get<0>(state) << " xy " << get<1>(state).first << "," << get<1>(state).second<<" dxdy " << get<2>(state).first << "," << get<2>(state).second << endl;

    iface = get<0>(state);
    xy =  get<1>(state);
    dxdy =  get<2>(state);
    if(!success)break;
  }
  dxdy = change_dir(move.second,dxdy);
  //cout <<"dir"<< dxdy.first << "," << dxdy.second << endl;
  
  //cout << xy.first << "," << xy.second << endl;
}

const map<pair<int64_t,int64_t>,int64_t> dir_score = {{{1,0},0},{{0,1},1},{{-1,0},2},{{0,-1},3}};

int64_t get_pw(pair<int64_t,int64_t> xy, pair<int64_t,int64_t> dxdy)
{
  //1000*row + 4*column + dir
  return 1000*(xy.second+1) + 4*(xy.first+1) + dir_score.at(dxdy);
}

vector<string> rotate_right(vector<string> & ip)
{
  vector<string> op=ip;
  for(string & s : op)for(char & c : s)c=' ';
  size_t n=ip.size();
  for(int64_t i=0;i<n;i++)
    for(int64_t j=0;j<n;j++)
      op[i][j]=ip[n-j-1][i]; 
  return op; 
}


vector<string> rotate(vector<string> ip, int64_t rot)
{
  vector<string> op=ip;
  for(int64_t ir=0;ir<rot;ir++)
  {
    op=rotate_right(op);
  } 
  return op; 
}

array<vector<string>,6> map_cross(vector<vector<string>> & ip, array<pair<size_t,int>,6> & mapping)
{
  array<vector<string>,6> cross;
  for(size_t i=0;i<6;i++)
  {
    cross[i]=ip[mapping[i].first];
    cross[i] = rotate(cross[i],mapping[i].second);
  }
  return cross;
}

void reverse_map(Notes & notes, size_t iface, pair<int64_t,int64_t> & xy, pair<int64_t,int64_t> & dxdy, array<pair<size_t,int>,6> & mapping)
{
  size_t n_orig =  mapping[iface].first;
  cout << "original face # " << n_orig << endl;
  int64_t rot =  mapping[iface].second;
  int64_t dx=dxdy.first;
  int64_t dy=dxdy.second;
  int64_t x=xy.first;
  int64_t y=xy.second;
  int64_t back=notes.n-1;
  
  cout << "rotation " << x << "," << y << "->";
  switch (rot)
  {
  case 0:
    /* code */
    break;
  case 1:
    //turn left
    dxdy = change_dir('L',dxdy);
    xy={y,back-x};
    break;
  case 2:
    /* u turn */
    dxdy = change_dir('L',dxdy);
    dxdy = change_dir('L',dxdy);
    xy={back-x,back-y};
    break;
  case 3:
    /* turn right */
    dxdy = change_dir('R',dxdy);
    xy={back-y,x};
    break;
  default:
    break;
  }
  
  size_t ix = n_orig%(notes.nx/notes.n);
  size_t iy = n_orig/(notes.nx/notes.n);
  x=xy.first;
  y=xy.second;
  cout <<  x << "," << y << endl;
  cout << "Original layout " << (notes.nx/notes.n) << " x " << (notes.ny/notes.n) << endl;
  cout << "Original face x,y " << ix << "," << iy << endl;

  x = xy.first;
  y = xy.second;


  xy={notes.n*ix+x,notes.n*iy+y};


}

void print_cross(Notes & notes, array<vector<string>,6> & cross)
{
  string padding;   
  padding.append(notes.n, ' ');

  cout << endl;
  for(string & s : cross[0]) cout << padding << s << padding<<endl;
  for(size_t i=0;i<notes.n;i++) cout << cross[1][i] << cross[2][i]<<cross[3][i]<<endl;
  for(string & s : cross[4]) cout << padding << s << padding<<endl;
  for(string & s : cross[5]) cout << padding << s << padding<<endl;
  cout << endl;
}

void part2(string fn, size_t face_size,array<pair<size_t,int>,6> & mapping)
{
  Notes notes = parse_ip(fn,face_size);

  notes.cross = map_cross(notes.faces,mapping);
  cout<<endl;

  print_cross(notes,notes.cross);

  //auto xy = get_start(notes);
  pair<int64_t,int64_t> xy={0,0};
  size_t iface=0;
  //cout << xy.first << "," << xy.second << endl;
  pair<int64_t,int64_t> dxdy{1,0};

  for(auto & m : notes.moves)
  {
    single_move(notes,m,iface,xy,dxdy);
    //cout << fn << " Face = " << iface <<
    //            " Position " << xy.first << "," << xy.second <<
     //           " Direction " << dxdy.first << "," << dxdy.second<<endl;
  }
  
  cout << fn << " Final position face = " << iface <<
                " Position " << xy.first << "," << xy.second <<
                " Direction " << dxdy.first << "," << dxdy.second<<endl;

  //convert to original frame of reference
  reverse_map(notes,iface,xy,dxdy,mapping);


  int64_t answer = get_pw(xy,dxdy);


  cout << fn << " Position " << xy.first+1 << "," << xy.second +1<<
                " Direction " << dxdy.first << "," << dxdy.second<<endl;

  cout << fn << " Part 1 answer = " << answer << endl;

}

//mapping to get cube net to look like a cross
// 0  1  2  3        0
// 4  5  6  7  --> 1 2 3
// 8  9 10 11        4
//                   5

array<pair<size_t,int>,6> test_map={pair<size_t,int>({2u,0}),
                                   pair<size_t,int>({5u,0}),
                                   pair<size_t,int>({6u,0}),
                                   pair<size_t,int>({11u,3}),
                                   pair<size_t,int>({10u,0}),
                                   pair<size_t,int>({4u,2})};

array<pair<size_t,int>,6> input_map={pair<size_t,int>({1u,0}),
                                   pair<size_t,int>({6u,1}),
                                   pair<size_t,int>({4u,0}),
                                   pair<size_t,int>({2u,1}),
                                   pair<size_t,int>({7u,0}),
                                   pair<size_t,int>({9u,3})};

int main()
{
  part2("input.txt",50,input_map);
  //part2("test.txt",4,test_map);
  part2("test2.txt",4,input_map);

  return 0;
}
