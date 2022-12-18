// done on my phone dont judge
// in hindsight, not the best approach
// to tackle part 2!

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <utility>

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

void nearest(const string & s1,
                map<string,vector<string>> & tunnels,
                map<string,int64_t> & dist,
                int64_t d)
{
	d++;
	for(string & s : tunnels[s1])
		if(dist[s]>d)
		{
			dist[s]=d;
			nearest(s,tunnels,dist,d);
		}
}

struct Sys
{
	int64_t nsteps=30;
	int64_t minloss;

  //problem data
	map<string,int64_t> rates;
	map<string,vector<string>> tunnels;

  //reduced problem data
	vector<string> nodes;
	map<string,vector<string>> links;
	map<pair<string,string>,int64_t> dists;

  //for print/debugging
	string path;
	
	void print()
	{
		for(auto p:rates)
		{
			cout << p.first << p.second << " ";
			for(auto v 	: tunnels[p.first])
			  cout << v << " ";
			cout << endl;
		}
		cout << "nodes\n";
		for(string s:nodes)
		  cout << s << endl;
		for(auto p : dists)
		  cout << p.first.first << ">" << p.first.second << " " << p.second<<endl;
	}
	bool comp(string & s1, string & s2){return rates[s1]>rates[s2];}
	void sort_nodes(map<string,int64_t> & r){sort(nodes.begin(),nodes.end(),
		[&r](string & s1, string & s2){return r[s1]>r[s2];});}
	void sort_nodes(){sort_nodes(rates);}
	void get_dists()
	{
		map<string,int64_t> dist;
		for(auto & p : rates)dist[p.first]=rates.size()*2;
		dist["AA"]=0;
		nearest("AA",tunnels,dist,0);
		for(string s : nodes)
		  dists[{"AA",s}]=dist[s]+1;//plus one for opening valve
		for(string s : nodes)
		{
			for(auto & p : dist) p.second=rates.size()*2;
			dist[s]=0;
			nearest(s,tunnels,dist,0);
			for(string s2 : nodes)
			  if(s!=s2)
		      dists[{s,s2}]=dist[s2]+1;//plus one for opening valve
		}	
	}
	
	void step(string v, int64_t loss, int64_t closedsum,
	          int64_t isteps, map<string,bool> & is_open)
	{
		if(loss>minloss)return;
		path+=v+" ";
		is_open[v]=true;
		int64_t newclosedsum = closedsum - rates[v];
		bool all_open=true;
		for(auto s : nodes)
		{
			if(!is_open[s])
			{
			  int64_t d=dists[{v,s}];
			  int64_t newloss = loss + d*newclosedsum;

        //only move to next valve if we have time - otherwise mark
        //as "open" to force algorithm to end
			  if(isteps+d<=nsteps)
			  {
			    step(s,newloss,newclosedsum,isteps+d,is_open); 
			    all_open=false; 
			  }
		  }
		}
		if(all_open)
		{
      //include remaining time in "loss"
			int64_t newloss = loss+(nsteps-isteps)*newclosedsum;
      //update minimum loss if necessary
			minloss=min(minloss,newloss);
			cout << path << loss+newclosedsum << " " << isteps << endl;
		}
    //update data as we leave the functuon
		is_open[v]=false;
		path.pop_back();path.pop_back();path.pop_back();
		return;
	}
	
	void get_best()
	{
		int64_t ratesum = 0;
		for(auto p:rates) ratesum+=p.second;
		int64_t flowsum=ratesum*nsteps;
		minloss=flowsum*200;
		map<string,bool> is_open;
		
		int64_t loss = 0;
		for(auto s : nodes)
		{
			for(auto s : nodes)is_open[s]=false;
			path="";
			int64_t d=dists[{"AA",s}];
			loss = d*ratesum;
			step(s,loss,ratesum,d,is_open);
		}
		cout << "min loss " << minloss << " out of " << flowsum <<endl;
		cout << "best flow " << flowsum-minloss << endl;
	}
	
};            

Sys parse(string fn)
{
	Sys rtn;
  auto ip=read_ip(fn);
  for(string s : ip	)
  {
    cout << s << endl;
    string dummy, name, srate, name2;
    stringstream ss(s);
    ss >> dummy >> name >> dummy >> dummy >> srate >> dummy >> dummy >> dummy >> dummy;
    srate=srate.substr(5);
    srate.pop_back();
    int64_t rate = atoi(srate.c_str());
    rtn.rates[name]=rate;
    vector<string> t;
    while(ss >> name2)
    {
    	if(name2.size()==3) name2.pop_back();
    	t.push_back(name2);
   	}  
   	rtn.tunnels[name]=t	;  
  }
  
  for(auto p : rtn.rates)
    if(p.second > 0) rtn.nodes.push_back(p.first);
  rtn.sort_nodes(rtn.rates);
  rtn.get_dists();
  return rtn;
}

void calc(string fn)
{
	Sys sys =parse(fn);
	sys.print();
	
	sys.get_best();

}

int main()
{
  calc("input.txt");
  calc("test.txt");
  return 0;
}
