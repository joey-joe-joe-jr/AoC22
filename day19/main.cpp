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
#include <chrono>

using namespace std;

struct ScopeTimer
{
  string msg;
  chrono::steady_clock::time_point begin;
  ScopeTimer():ScopeTimer(""){}
  ScopeTimer(string m):msg(m),begin(chrono::steady_clock::now()){}
  ~ScopeTimer()
  {
    chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    cout << msg << chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << endl;
  }
};

vector<string> read_ip(string s)
{
  vector<string> lines;
  string line;
  ifstream ip(s);
  while(getline(ip,line))
    lines.push_back(line);
  return lines;
}

vector<string> split(string s)
{
  vector<string> words;
  stringstream ss(s);
  string word;
  while(ss >> word)
  {
    words.push_back(word);
    cout << word << " ";
  }
  return words;
}

struct Blueprint
{
  int orebot_orecost;
  int clabot_orecost;
  int obsbot_orecost;
  int obsbot_clacost;
  int geobot_orecost;
  int geobot_obscost;

  int max_orecost;
  
  Blueprint(int a, int b, int c, int d, int e, int f) :
    orebot_orecost(a),clabot_orecost(b),obsbot_orecost(c),
    obsbot_clacost(d),geobot_orecost(e),geobot_obscost(f),
    max_orecost(max(orebot_orecost,max(clabot_orecost,max(obsbot_orecost,geobot_orecost)))){}
  void print(){cout << "blueprint " << 
                       orebot_orecost << " " << 
                       clabot_orecost << " " << 
                       obsbot_orecost << " " << 
                       obsbot_clacost << " " << 
                       geobot_orecost << " " << 
                       geobot_obscost << " " <<
                       max_orecost << endl;}
};

vector<Blueprint> parse_ip(string fn)
{
  vector<Blueprint> bps;
  auto ip = read_ip(fn);
  for(string line : ip)
  {
    int i,a,b,c,d,e,f;
    sscanf(line.c_str(),
    "Blueprint %d: Each ore robot costs %d ore."
    " Each clay robot costs %d ore."
    " Each obsidian robot costs %d ore and %d clay."
    " Each geode robot costs %d ore and %d obsidian.",&i,&a,&b,&c,&d,&e,&f);
    Blueprint bp{a,b,c,d,e,f};          
    bps.push_back(bp);
  }
  return bps;
}

struct State
{
  int nore=0;
  int ncla=0;
  int nobs=0;
  int ngeo=0;
  int norebot=1;
  int nclabot=0;
  int nobsbot=0;
  int ngeobot=0;
  void farm(Blueprint & bp)
  {
    nore+=norebot;
    ncla+=nclabot;
    nobs+=nobsbot;
    ngeo+=ngeobot;
  }
};

string path;

//assume abundance of ore for "max" possible geodes for a given state
//for pruning
int cheat(Blueprint & bp, int steps_left, State state)
{
  State oldstate=state;
  State newstate;  
  while(steps_left!=0)
  {
    --steps_left;
    newstate=oldstate;
    newstate.farm(bp);
    if(oldstate.nobs>=bp.geobot_obscost) ++newstate.ngeobot;
    else if(oldstate.ncla>=bp.obsbot_clacost) ++newstate.nobsbot;
    else ++newstate.nclabot;
    oldstate=newstate;
  }
  return newstate.ngeo;
}

void onestep(Blueprint & bp, int steps_left, State state, int & max_geo)
{
  bool done_something=false;
  int ngeo = state.ngeo;
  int impossible_geo=cheat(bp,steps_left,state);

  if(ngeo > max_geo)
  {
    max_geo = ngeo;
    if(steps_left == 0)cout << path <<  " " << max_geo << endl;
  }

  if(steps_left == 0)return;

  if(impossible_geo<=max_geo)
  {
    //cout << path << " " << impossible_geo << "<" << max_geo << endl;
    return;
  }

  int num_wanted = 0;
  int num_possible = 0;

  // make a geobot?
  bool geobot_wanted = steps_left > 1;
  bool geobot_possible = (state.nore >= bp.geobot_orecost && state.nobs >= bp.geobot_obscost);
  //if(geobot_wanted)num_wanted++;
  //if(geobot_possible)num_possible++;
  if(geobot_wanted   &&
     geobot_possible)
  {
    State newstate = state;  //increment resources and time
    newstate.farm(bp);
    newstate.nore -= bp.geobot_orecost;
    newstate.nobs -= bp.geobot_obscost;
    ++newstate.ngeobot;
    path += "geo  ";
    onestep(bp,steps_left-1,newstate,max_geo);
    path = path.substr(0,path.size()-5);
    done_something=true;
    return; //not sure if we should ALWAYS make geobot if possible??
  }

  //make an obsbot?
  bool obsbot_wanted = (state.nobsbot < bp.geobot_obscost && steps_left > 2);
  //bool obsbot_wanted = (state.nobsbot < bp.geobot_obscost);
  bool obsbot_possible = (state.nore >= bp.obsbot_orecost && state.ncla >= bp.obsbot_clacost);
  if(obsbot_wanted)num_wanted++;
  if(obsbot_possible)num_possible++;
  if(obsbot_possible && obsbot_wanted)
  {
    State newstate = state;
    newstate.farm(bp);
    newstate.nore -= bp.obsbot_orecost;
    newstate.ncla -= bp.obsbot_clacost;
    ++newstate.nobsbot;
    path += "obs  ";
    onestep(bp,steps_left-1,newstate,max_geo);
    path = path.substr(0,path.size()-5);
    done_something=true;
  }

  //make a claybot?
  bool clabot_wanted = (state.nclabot < bp.obsbot_clacost && steps_left > 3);
  //bool clabot_wanted = (state.nclabot < bp.obsbot_clacost);
  bool clabot_possible = (state.nore >= bp.clabot_orecost);
  if(clabot_wanted)num_wanted++;
  if(clabot_possible)num_possible++;
  if(clabot_possible && clabot_wanted)
  {
    State newstate = state;
    newstate.farm(bp);
    newstate.nore -= bp.clabot_orecost;
    ++newstate.nclabot;
    path += "clay ";
    onestep(bp,steps_left-1,newstate,max_geo);
    path = path.substr(0,path.size()-5);
    done_something=true;
  }

  //make an orebot?
  bool orebot_wanted = state.norebot < bp.max_orecost && steps_left > 1;
  //bool orebot_wanted = state.norebot < bp.max_orecost;
  bool orebot_possible = state.nore >= bp.orebot_orecost;
  if(orebot_wanted)num_wanted++;
  if(orebot_possible)num_possible++;
  if(orebot_wanted && orebot_possible)
  {
    State newstate = state;
    newstate.farm(bp);
    //cout << "make orebot!\n" ;
    newstate.nore -= bp.orebot_orecost;
    ++newstate.norebot;
    path += "ore  ";
    onestep(bp,steps_left-1,newstate,max_geo);
    path = path.substr(0,path.size()-5);
    done_something=true;
  }

  //don't make anything
  //need a stockpiling rule?
  if(num_wanted>num_possible || !done_something)
  //if(true)
  {
    State newstate = state;
    newstate.nore += state.norebot;
    newstate.ncla += state.nclabot;
    newstate.nobs += state.nobsbot;
    newstate.ngeo += state.ngeobot;
    path += "wait ";
    onestep(bp,steps_left-1,newstate,max_geo);
    path = path.substr(0,path.size()-5);
  }

}

int most_geodes(Blueprint & bp, int nsteps)
{
  cout << " getting best number of geodes\n";
  path="";

  int max_geo=0;

  State s;

  onestep(bp,nsteps,s,max_geo);

  cout << "num geodes " << max_geo << endl;

  return max_geo;

}


void part1(string fn, int nsteps)
{
  ScopeTimer timer("Part 1 time elapsed = ");
  auto bps = parse_ip(fn);
  for(auto & bp : bps) bp.print();

  int sum=0;
  for(size_t i=0; i<bps.size(); i++)
    sum += (i+1)*most_geodes(bps[i],nsteps);

  int answer = sum;

  cout << fn << " Part 1 answer = " << answer << endl;

}

void part2(string fn, int nsteps)
{
  ScopeTimer timer("Part 2 time elapsed = ");
  auto bps = parse_ip(fn);
  for(auto & bp : bps) bp.print();

  int product=1;
  size_t n = 3 < bps.size() ? 3 : bps.size();
  for(size_t i=0; i<n; i++)
    product *= most_geodes(bps[i],nsteps);

  int answer = product;

  cout << fn << " Part 2 answer = " << answer << endl;

}

int main()
{
  //part1("input.txt",24);
  //part1("test.txt",24);
  part2("input.txt",32);
  //part2("test.txt",32);

  return 0;
}
