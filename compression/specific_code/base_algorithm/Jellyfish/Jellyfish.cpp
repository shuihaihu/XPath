#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include<time.h>
#include <cmath>
#include <string.h>
using namespace std;
#include "Jellyfish.h"

class Node{
public:
	int ConnSwitch;
	int ConnPort;
};
int AddRanConn(Node (*GList)[PortToSwitchN]);
int getpermutation(int* permutation);
int Build_BFS_Tree(int rootnode, Node (*GList)[PortToSwitchN], int* flag,
				   int (*Switch_PathSet)[TotalPathSetNum], int PathSetNum);
int createGraph(Node (*GList)[PortToSwitchN]);
int createPathSet(Node (*GList)[PortToSwitchN], int (*Switch_PathSet)[TotalPathSetNum]);
int SwitchOptimalEncoding(int *FinalID, int (*Switch_PathSet)[TotalPathSetNum]);
int currentMS_init(int *currentMS, int *LocateTable, int (*Switch_PathSet)[TotalPathSetNum]);
int LocateTable_init(int *LocateTable, int *FinalID, int (*Switch_PathSet)[TotalPathSetNum]);
int CCencoding(int (*Switch_PathSet)[TotalPathSetNum], ofstream &output);


int main(){
	Node (*GList)[PortToSwitchN]=new Node[Smax][PortToSwitchN];
	int (*Switch_PathSet)[TotalPathSetNum]=new int[Smax][TotalPathSetNum];
	clock_t start, finish;   
	double     duration; 
	ofstream output("Jellyfish_Output.txt",ios::app);
	output<<setiosflags(ios::fixed)<<setprecision(6);
	memset(GList,0,sizeof(Node)*Smax*PortToSwitchN);
	memset(Switch_PathSet,0,sizeof(int)*Smax*TotalPathSetNum);
	srand((unsigned)time(0));

	start=clock();      
    createGraph(GList);
	finish=clock();
	duration=(double)(finish-start)/CLOCKS_PER_SEC; 
	output<<"The running time of creating graph is: "<<duration<<endl; 

	/*for(int s=0; s<Smax; s++){
		cout<<"switch "<<s<<": "<<endl;
		for(int port=0; port<PortToSwitchN; port++)
			cout<<GList[s][port].ConnSwitch<<" "<<GList[s][port].ConnPort<<endl;
	}  */ 
	start=clock();      
	createPathSet(GList, Switch_PathSet);
	finish=clock();
	duration=(double)(finish-start)/CLOCKS_PER_SEC; 
	output<<"The running time of creating pathset is: "<<duration<<endl; 

	/*for(int i=0; i<TotalPathSetNum; i++){
		cout<<"PathSet "<<i<<": ";
		for(int j=0; j<Smax; j++)
			cout<<Switch_PathSet[j][i]<<" ";
		cout<<endl;
	} */ 
	output<<" N= "<<Smax<<" k= "<<PortToSwitchN*2<<" r= "<<PortToSwitchN<<endl;
	output<<"path # is: "<<TotalPathSetNum*(Smax-1)<<endl;
	output<<"device # is: "<<Smax*PortToSwitchN+Smax<<endl;
	output<<"the TotalPathSetNum is: "<<TotalPathSetNum<<endl;
	start=clock();      
	CCencoding(Switch_PathSet,output);
	finish=clock();
	duration=(double)(finish-start)/CLOCKS_PER_SEC; 
	output<<"The running time of creating pathset is: "<<duration<<endl; 

  delete[] Switch_PathSet;
  delete[] GList;
  output.close();

  return 0;
}

int ConnInnerLinks(int circle_s, int* AvailPort, Node (*GList)[PortToSwitchN]){
	int startswitch=circle_s*CirculNodeNum;
	for(int s=startswitch; s<startswitch+CirculNodeNum-1; s++){
		int startnode=s;
		int endnode=s+1;
		GList[startnode][ AvailPort[startnode] ].ConnSwitch=endnode;
		GList[startnode][ AvailPort[startnode] ].ConnPort=AvailPort[endnode];
		GList[endnode][ AvailPort[endnode] ].ConnSwitch=startnode;
		GList[endnode][ AvailPort[endnode] ].ConnPort=AvailPort[startnode];
		AvailPort[startnode]++;
		AvailPort[endnode]++;
	}
	int startnode=startswitch+CirculNodeNum-1;
	int endnode=startswitch;
	GList[startnode][ AvailPort[startnode] ].ConnSwitch=endnode;
	GList[startnode][ AvailPort[startnode] ].ConnPort=AvailPort[endnode];
	GList[endnode][ AvailPort[endnode] ].ConnSwitch=startnode;
	GList[endnode][ AvailPort[endnode] ].ConnPort=AvailPort[startnode];
	AvailPort[startnode]++;
	AvailPort[endnode]++;
 return 0;
}

int getpermutation(int* permutation){
	int flag[PerLen];
	int Range_Min=0;
	int Range_Max=PerLen;
	double Range=Range_Max-Range_Min;

	memset(flag,0,sizeof(int)*PerLen);
	for(int i=0; i<PerLen; i++){
		int temp=Range_Min+(int)(((double)rand()/(double)(RAND_MAX+1))*Range );
		while(flag[temp]){
			temp++;
			if(temp>=PerLen)
				temp-=PerLen;
		}
		permutation[i]=temp;
		flag[temp]=1;
	}
	/*cout<<"permutation: "<<endl;
	for(int i=0; i<PerLen; i++)
		cout<<permutation[i]<<" ";
	cout<<endl; */ 
	return 0;  

}

int ConnOuterLinks(int circle_s,int circle_e, int* permutation,int* AvailPort, Node (*GList)[PortToSwitchN]){

	int temp1=circle_s*CirculNodeNum;
	int temp2=circle_e*CirculNodeNum;

	for(int i=0; i<PerNum; i++){
		int startorder=i*PerLen;
		getpermutation(permutation);
	    for(int j=0; j<PerLen; j++){
			int startnode=temp1+startorder+j;
			int endnode=temp2+startorder+permutation[j];
			GList[startnode][ AvailPort[startnode] ].ConnSwitch=endnode;
			GList[startnode][ AvailPort[startnode] ].ConnPort=AvailPort[endnode];
			GList[endnode][ AvailPort[endnode] ].ConnSwitch=startnode;
			GList[endnode][ AvailPort[endnode] ].ConnPort=AvailPort[startnode];
			AvailPort[startnode]++;
			AvailPort[endnode]++;
		}
	}
return 0;

}


int createGraph(Node (*GList)[PortToSwitchN]){
	int AvailPort[Smax];
	int maskcode=1;
	int permutation[PerLen];

	memset(AvailPort,0,sizeof(int)*Smax);

	for(int circle_s=0; circle_s<CirculNum; circle_s++){
		ConnInnerLinks(circle_s, AvailPort, GList);
		//for(int circle_e=circle_s+1; circle_e<CirculNum; circle_e++)
			//ConnOuterLinks(circle_s, circle_e, AvailPort, GList);
	}

	//for(int i=0; i<CirculNodeNum; i++)
		//permutation[i]=i;
	//getpermutation(permutation);
	for(int circle_s=0; circle_s<CirculNum-1; circle_s++)
		//ConnInnerLinks(circle_s, AvailPort, GList);
		for(int circle_e=circle_s+1; circle_e<CirculNum; circle_e++)
			ConnOuterLinks(circle_s, circle_e, permutation, AvailPort, GList);
	

	return 0;
}
int Build_BFS_Tree(int rootnode, Node (*GList)[PortToSwitchN], int* flag,
				   int (*Switch_PathSet)[TotalPathSetNum], int PathSetNum){
	int NodeQueue[Smax]={0};
	int tail=0, head=0;

	NodeQueue[tail++]=rootnode;
	while(head<tail){
		int node=NodeQueue[head++];
		for(int port=0; port<PortToSwitchN; port++){
			int nextnode=GList[node][port].ConnSwitch;
			if(flag[nextnode]==0){
				int nextnodeport=GList[node][port].ConnPort;
				flag[nextnode]=1;
				Switch_PathSet[nextnode][PathSetNum]=nextnodeport+1;
				NodeQueue[tail++]=nextnode;
			}

		}
		
	}
 //cout<<"tail = "<<tail<<endl;
 return tail;
}

int createPathSet(Node (*GList)[PortToSwitchN], int (*Switch_PathSet)[TotalPathSetNum]){
	int PathSetNum=0;
	int flag[Smax];

	for(int destnode=0; destnode<Smax; destnode++)
		for(int port=0; port<PortToSwitchN; port++){
			memset(flag,0,sizeof(int)*Smax);
			flag[destnode]=1;

			int rootnode=GList[destnode][port].ConnSwitch;
			int rootnodeport=GList[destnode][port].ConnPort;
			flag[rootnode]=1;

			Switch_PathSet[rootnode][PathSetNum]=rootnodeport+1;
			if( Build_BFS_Tree(rootnode, GList, flag, Switch_PathSet, PathSetNum)<Smax-1){
				cout<<"the graph is not connected!"<<endl;
				return -1;
			} 
			PathSetNum++;
		}
	return 0;
}

int SwitchOptimalEncoding(int s, int *FinalID, int (*Switch_PathSet)[TotalPathSetNum]){
	int CurrentID;
	int *flag=new int[TotalPathSetNum];
	memset(flag,0, sizeof(int)*TotalPathSetNum);
	CurrentID=0;
	for(int i=0; i<TotalPathSetNum; i++)
		if(flag[i]==0) {
			flag[i]=1;
			FinalID[i]=CurrentID++;
			for(int j=i+1; j<TotalPathSetNum; j++)        
				if(Switch_PathSet[s][j]==Switch_PathSet[s][i]){
					flag[j]=1;
					FinalID[j]=CurrentID++;
				}
		} 
	delete[] flag;
	return 0;

}


int currentMS_init(int *currentMS, int *LocateTable, int (*Switch_PathSet)[TotalPathSetNum]){
	for(int i=0; i<Smax; i++){
		int j, pre;
		pre=0;
		currentMS[i]=1;
		for(j=0; j<TotalPathSetNum;j++)
			if(Switch_PathSet[i][ LocateTable[j] ]!=0){
				pre=Switch_PathSet[i][ LocateTable[j] ];
				break;
			}
		if(pre!=0)
			for( j++; j<TotalPathSetNum; j++)
				if(Switch_PathSet[i][ LocateTable[j] ] != 0 && pre!=Switch_PathSet[i][ LocateTable[j] ]){
					currentMS[i]++;
					pre=Switch_PathSet[i][ LocateTable[j] ];
				}
	}
	return 0;

}
int LocateTable_init(int *LocateTable, int *FinalID){

	for(int tree=0; tree<TotalPathSetNum; tree++)
		LocateTable[FinalID[tree]]=tree;
return 0;
}


int computeMinMax(int KeySwitch, int *FinalID,
					  int *LocateTable, int *currentMS, int (*Switch_PathSet)[TotalPathSetNum]){
			int answer=0;

			SwitchOptimalEncoding(KeySwitch, FinalID, Switch_PathSet);
			LocateTable_init(LocateTable, FinalID);
		    currentMS_init(currentMS, LocateTable, Switch_PathSet);
			for(int i=0; i<Smax; i++)
				if(currentMS[i]>answer){
					answer=currentMS[i];
				}
			//cout<<"after conflict correcting_3: "<<answer<<endl;
			/*			int index;
			for(int i=0; i<TotalPathSetNum; i++)
				cout<<Switch_PathSet[index][LocateTable[i]]<<" ";
			cout<<endl;*/ 

			return answer;


}
int CCencoding(int (*Switch_PathSet)[TotalPathSetNum], ofstream &output){
	int *FinalID=new int[TotalPathSetNum];
	int *LocateTable=new int[TotalPathSetNum];
	int currentMS[Smax];
	int KeySwitch;
	

	for(int j=0; j<TotalPathSetNum; j++)
		FinalID[j]=j;
	LocateTable_init(LocateTable,FinalID);
	currentMS_init(currentMS, LocateTable, Switch_PathSet);

	int answer=0;
	for(int i=0; i<Smax; i++)
		if(currentMS[i]>answer)
			answer=currentMS[i];
	//cout<<"before conflict correcting : "<<answer<<endl;

	KeySwitch=Smax/4;
	SwitchOptimalEncoding(KeySwitch,FinalID,Switch_PathSet);
	LocateTable_init(LocateTable,FinalID);
	currentMS_init(currentMS, LocateTable, Switch_PathSet);
	answer=0;
	for(int i=0; i<Smax; i++)
		if(currentMS[i]>answer){
			answer=currentMS[i];
				//cout<<"answer_i="<<i<<endl;
		}

			//cout<<"MinSwitch= "<<MinSwitch<<endl;
	//cout<<"after conflict correcting: "<<answer<<endl;
	output<<"the routing table size is: "<<answer<<endl<<endl;


	delete[] FinalID;
	delete[] LocateTable;
	return 0;

}
