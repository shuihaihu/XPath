#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include<time.h>
#include <cmath>
#include <string.h>
using namespace std;
#include "Jellyfish_base.h"

class Node{
public:
	int ConnSwitch;
	int ConnPort;
};
int AddRanConn(Node (*GList)[PortToSwitchN]);
int getpermutation(int permutation[Smax]);
int Build_BFS_Tree(int rootnode, Node (*GList)[PortToSwitchN], int flag[Smax],
				   int (*Switch_PathSet)[TotalPathSetNum], int PathSetNum);
int createGraph(Node (*GList)[PortToSwitchN]);
int createPathSet(Node (*GList)[PortToSwitchN], int (*Switch_PathSet)[TotalPathSetNum]);
int SwitchOptimalEncoding(int (*FinalID)[TotalPathSetNum], int (*ST)[TotalPathSetNum]);
int SGN(int a);
int currentMS_init(int *currentMS, int (*LocateTable)[TotalPathSetNum], int (*ST)[TotalPathSetNum]);
int LocateTable_init(int (*LocateTable)[TotalPathSetNum], int (*FinalID)[TotalPathSetNum], int (*ST)[TotalPathSetNum]);
int computeMS(int SourceT,int FixedSwitch,int RefSwitch,int (*FinalID)[TotalPathSetNum],
			  int (*LocateTable)[TotalPathSetNum], int *currentMS, int (*ST)[TotalPathSetNum]);
int ExchangeColumns(int SourceT,int FixedSwitch,int RefSwitch,int (*FinalID)[TotalPathSetNum],
					int (*LocateTable)[TotalPathSetNum], int *currentMS, int (*ST)[TotalPathSetNum]);
int CCencoding(int (*ST)[TotalPathSetNum], ofstream &output);


int main(){
	Node (*GList)[PortToSwitchN]=new Node[Smax][PortToSwitchN];
	int (*Switch_PathSet)[TotalPathSetNum]=new int[Smax][TotalPathSetNum];
	ofstream output("Jellyfish_Output.txt",ios::app);
	output<<setiosflags(ios::fixed)<<setprecision(6);

	memset(GList,0,sizeof(Node)*Smax*PortToSwitchN);
	memset(Switch_PathSet,0,sizeof(int)*Smax*TotalPathSetNum);
	srand((unsigned)time(0));

    createGraph(GList);

	/*for(int s=0; s<Smax; s++){
		cout<<"switch "<<s<<": "<<endl;
		for(int port=0; port<PortToSwitchN; port++)
			cout<<GList[s][port].ConnSwitch<<" "<<GList[s][port].ConnPort<<endl;
	}*/ 
	createPathSet(GList, Switch_PathSet);

	/*for(int i=0; i<TotalPathSetNum; i++){
		cout<<"PathSet "<<i<<": ";
		for(int j=0; j<Smax; j++)
			cout<<Switch_PathSet[j][i]<<" ";
		cout<<endl;
	}*/     
	output<<"the TotalPathSetNum is: "<<TotalPathSetNum<<endl;
	CCencoding(Switch_PathSet,output);
  delete[] Switch_PathSet;
  delete[] GList;
  output.close();

  return 0;
}

int getpermutation(int permutation[Smax]){
	int flag[Smax];
	int Range_Min=0;
	int Range_Max=Smax;
	double Range=Range_Max-Range_Min;

	memset(flag,0,sizeof(int)*Smax);
	for(int i=0; i<Smax; i++){
		int temp=Range_Min+(int)(((double)rand()/(double)RAND_MAX)*Range );
		while(flag[temp]){
			temp++;
			if(temp>=Smax)
				temp-=Smax;
		}
		permutation[i]=temp;
		flag[temp]=1;
	}
	/*cout<<"permutation: "<<endl;
	for(int i=0; i<Smax; i++)
		cout<<permutation[i]<<" ";
	cout<<endl;*/ 
	return 0;

}

int AddRanConn(Node (*GList)[PortToSwitchN]){
	int Range_Min=PortToSwitchN/2;
	int Range_Max=PortToSwitchN;
	double Range=Range_Max-Range_Min;
	int maskcode=1;

	for(int node=0; node<Smax; node++){
		for(int portA=0; portA<PortToSwitchN/2; portA++){

			int portB=Range_Min+(int)(((double)rand()/(double)RAND_MAX)*Range );

			int endSA=GList[node][portA].ConnSwitch;
            int endSAPort=GList[node][portA].ConnPort;
			int endSB=GList[node][portB].ConnSwitch;
			int endSBPort=GList[node][portB].ConnPort;

			GList[node][portA].ConnSwitch=endSB;
			GList[node][portA].ConnPort=endSBPort;
			GList[node][portB].ConnSwitch=endSA;
			GList[node][portB].ConnPort=endSAPort;
			GList[endSA][endSAPort].ConnSwitch=node;
			GList[endSA][endSAPort].ConnPort=portB;
			GList[endSB][endSBPort].ConnSwitch=node;
			GList[endSB][endSBPort].ConnPort=portA;
		}

	}
	return 0;

}
int createGraph(Node (*GList)[PortToSwitchN]){
	int AvailPort[Smax];
	int permutation[Smax];
	int maskcode=1;

	memset(AvailPort,0,sizeof(int)*Smax);
	srand((unsigned)time(0));

	for(int turn=0; turn<PortToSwitchN/2; turn++){
		getpermutation(permutation);
		for(int i=0; i<Smax-1; i++){
			int startnode=permutation[i];
			int endnode=permutation[i+1];
			GList[startnode][ AvailPort[startnode] ].ConnSwitch=endnode;
			GList[startnode][ AvailPort[startnode] ].ConnPort=AvailPort[endnode];
			GList[endnode][ AvailPort[endnode] ].ConnSwitch=startnode;
			GList[endnode][ AvailPort[endnode] ].ConnPort=AvailPort[startnode];
			AvailPort[startnode]++;
			AvailPort[endnode]++;
		}

		int startnode=permutation[Smax-1];
		int endnode=permutation[0];
		GList[startnode][ AvailPort[startnode] ].ConnSwitch=endnode;
		GList[startnode][ AvailPort[startnode] ].ConnPort=AvailPort[endnode];
		GList[endnode][ AvailPort[endnode] ].ConnSwitch=startnode;
		GList[endnode][ AvailPort[endnode] ].ConnPort=AvailPort[startnode];
		AvailPort[startnode]++;
		AvailPort[endnode]++;

	}
	//AddRanConn(GList);
	return 0;
}
int Build_BFS_Tree(int rootnode, Node (*GList)[PortToSwitchN], int flag[Smax],
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
				Switch_PathSet[nextnode][PathSetNum]=nextnodeport;
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

			Switch_PathSet[rootnode][PathSetNum]=rootnodeport;
			if( Build_BFS_Tree(rootnode, GList, flag, Switch_PathSet, PathSetNum)<Smax-1){
				cout<<"the graph is not connected!"<<endl;
				return -1;
			} 
			PathSetNum++;
		}
	return 0;
}

int SwitchOptimalEncoding(int (*FinalID)[TotalPathSetNum], int (*ST)[TotalPathSetNum]){
	int CurrentID;
	int *flag=new int[TotalPathSetNum];
	for(int s=0; s<Smax; s++){
		memset(flag,0, sizeof(int)*TotalPathSetNum);
		CurrentID=0;
		for(int i=0; i<TotalPathSetNum; i++)
			if(flag[i]==0) {
				flag[i]=1;
				FinalID[s][i]=CurrentID++;
				for(int j=i+1; j<TotalPathSetNum; j++)        
					if(ST[s][j]==ST[s][i]){
						flag[j]=1;
						FinalID[s][j]=CurrentID++;
					}
			}
	}  
	delete[] flag;
	return 0;

}
int SGN(int a){
	return a!=0 ? 1:0;
}
int currentMS_init(int *currentMS, int (*LocateTable)[TotalPathSetNum], int (*ST)[TotalPathSetNum]){
	for(int i=0; i<Smax; i++){
		int j, pre;
		pre=0;
		currentMS[i]=1;
		for(j=0; j<TotalPathSetNum;j++)
			if(ST[i][ LocateTable[i][j] ]!=0){
				pre=ST[i][ LocateTable[i][j] ];
				break;
			}
			if(pre!=0)
				for( j++; j<TotalPathSetNum; j++)
					if(ST[i][ LocateTable[i][j] ] != 0){
						currentMS[i]+=SGN(pre^ST[i][ LocateTable[i][j] ]);
						pre=ST[i][ LocateTable[i][j] ];
					}
	}
	return 0;

}
int LocateTable_init(int (*LocateTable)[TotalPathSetNum], int (*FinalID)[TotalPathSetNum], int (*ST)[TotalPathSetNum]){

	for(int s=0; s<Smax; s++)
		for(int tree=0; tree<TotalPathSetNum; tree++)
			LocateTable[s][FinalID[s][tree]]=tree;
return 0;
}
int computeMS(int SourceT,int FixedSwitch,int RefSwitch,int (*FinalID)[TotalPathSetNum],
			  int (*LocateTable)[TotalPathSetNum], int *currentMS, int (*ST)[TotalPathSetNum]){
				  int DesID=FinalID[FixedSwitch][SourceT];
				  int SourceID=FinalID[RefSwitch][SourceT];
				  int DesT=LocateTable[RefSwitch][DesID];
				  int PreSourceT=-1,SucSourceT=-1;
				  int PreDesT=-1,SucDesT=-1;

				  if(SourceID==DesID)
					  return currentMS[RefSwitch];

				  int tempSourceID=SourceID, tempDesID=DesID;
				  while(tempSourceID>0 && tempSourceID!=DesID){
					  PreSourceT=LocateTable[RefSwitch][--tempSourceID];
					  if(ST[RefSwitch][PreSourceT]!=0) break;
						}
				  tempSourceID=SourceID;
				  while(tempSourceID<TotalPathSetNum-1 && tempSourceID!=DesID){
					  SucSourceT=LocateTable[RefSwitch][++tempSourceID];
					  if(ST[RefSwitch][SucSourceT]!=0) break;
						}

				  while(tempDesID>0 && tempDesID!=SourceID){
					  PreDesT=LocateTable[RefSwitch][--tempDesID];
					  if(ST[RefSwitch][PreDesT]!=0) break;
						}
				  tempDesID=DesID;
				  while(tempDesID<TotalPathSetNum-1 && tempDesID!=SourceID){
					  SucDesT=LocateTable[RefSwitch][++tempDesID];
					  if(ST[RefSwitch][SucDesT]!=0) break;
						}

						/*if(SourceID!=0) PreSourceT=LocateTable[RefSwitch][SourceID-1];
						if(SourceID!=(TotalPathSetNum-1)) SucSourceT=LocateTable[RefSwitch][SourceID+1];

						if(DesID!=0) PreDesT=LocateTable[RefSwitch][DesID-1];
						if(DesID!=(TotalPathSetNum-1)) SucDesT=LocateTable[RefSwitch][DesID+1]; */ 

				 int OriScore=0, ExchangeScore=0 ;


				 if(SucSourceT==DesT || SucDesT==SourceT){
					 int MaxID, MinID;
					 int MaxT, MinT;
					 int PreMinT=-1,SucMaxT=-1;

					 if(ST[RefSwitch][SourceT]*ST[RefSwitch][DesT]==0)
						 return currentMS[RefSwitch];

					 if(SucSourceT==DesT){
						 MaxID=DesID;
						 MaxT=DesT;
						 MinID=SourceID;
						 MinT=SourceT;
						 PreMinT=PreSourceT;
						 SucMaxT=SucDesT;
					 }else{
						 MaxID=SourceID;
						 MaxT=SourceT;
						 MinID=DesID;
						 MinT=DesT;
						 PreMinT=PreDesT;
						 SucMaxT=SucSourceT;
					 }


							if(PreMinT!=-1 && ST[RefSwitch][PreMinT]!=0 && ST[RefSwitch][MinT]!=0)
								OriScore+=SGN(ST[RefSwitch][PreMinT]^ST[RefSwitch][MinT]);
							if(SucMaxT!=-1 && ST[RefSwitch][SucMaxT]!=0 && ST[RefSwitch][MaxT]!=0)
								OriScore+=SGN(ST[RefSwitch][SucMaxT]^ST[RefSwitch][MaxT]);

							if(PreMinT!=-1 && ST[RefSwitch][PreMinT]!=0 && ST[RefSwitch][MaxT]!=0)
								ExchangeScore+=SGN(ST[RefSwitch][PreMinT]^ST[RefSwitch][MaxT]);
							if(SucMaxT!=-1 && ST[RefSwitch][SucMaxT]!=0 && ST[RefSwitch][MinT]!=0)
								ExchangeScore+=SGN(ST[RefSwitch][SucMaxT]^ST[RefSwitch][MinT]);

							return (currentMS[RefSwitch]+ExchangeScore-OriScore);


				  } 

				  if(PreSourceT!=-1 && ST[RefSwitch][PreSourceT]!=0 && ST[RefSwitch][SourceT]!=0)
					  OriScore+=SGN(ST[RefSwitch][PreSourceT]^ST[RefSwitch][SourceT]);
				  if(SucSourceT!=-1 && ST[RefSwitch][SucSourceT]!=0 && ST[RefSwitch][SourceT]!=0)
					  OriScore+=SGN(ST[RefSwitch][SucSourceT]^ST[RefSwitch][SourceT]);
				  if(PreSourceT!=-1 && SucSourceT!=-1 && ST[RefSwitch][PreSourceT]!=0 && 
					  ST[RefSwitch][SucSourceT]!=0 && ST[RefSwitch][SourceT]==0)
					  OriScore+=SGN(ST[RefSwitch][SucSourceT]^ST[RefSwitch][PreSourceT]);

				  if(PreDesT!=-1 && ST[RefSwitch][PreDesT]!=0 && ST[RefSwitch][DesT]!=0)
					  OriScore+=SGN(ST[RefSwitch][PreDesT]^ST[RefSwitch][DesT]);
				  if(SucDesT!=-1 && ST[RefSwitch][SucDesT]!=0 && ST[RefSwitch][DesT]!=0)
					  OriScore+=SGN(ST[RefSwitch][SucDesT]^ST[RefSwitch][DesT]);
				  if(PreDesT!=-1 && SucDesT!=-1 && ST[RefSwitch][PreDesT]!=0 && 
					  ST[RefSwitch][SucDesT]!=0 && ST[RefSwitch][DesT]==0)
					  OriScore+=SGN(ST[RefSwitch][SucDesT]^ST[RefSwitch][PreDesT]);



				  if(PreSourceT!=-1 && ST[RefSwitch][PreSourceT]!=0 && ST[RefSwitch][DesT]!=0)
					  ExchangeScore+=SGN(ST[RefSwitch][PreSourceT]^ST[RefSwitch][DesT]);
				  if(SucSourceT!=-1 && ST[RefSwitch][SucSourceT]!=0 && ST[RefSwitch][DesT]!=0)
					  ExchangeScore+=SGN(ST[RefSwitch][SucSourceT]^ST[RefSwitch][DesT]);
				  if(PreSourceT!=-1 && SucSourceT!=-1 && ST[RefSwitch][PreSourceT]!=0 && 
					  ST[RefSwitch][SucSourceT]!=0 && ST[RefSwitch][DesT]==0)
					  ExchangeScore+=SGN(ST[RefSwitch][SucSourceT]^ST[RefSwitch][PreSourceT]);


				  if(PreDesT!=-1 && ST[RefSwitch][PreDesT]!=0 && ST[RefSwitch][SourceT]!=0)
					  ExchangeScore+=SGN(ST[RefSwitch][PreDesT]^ST[RefSwitch][SourceT]);
				  if(SucDesT!=-1 && ST[RefSwitch][SucDesT]!=0 && ST[RefSwitch][SourceT]!=0)
					  ExchangeScore+=SGN(ST[RefSwitch][SucDesT]^ST[RefSwitch][SourceT]);
				  if(PreDesT!=-1 && SucDesT!=-1 && ST[RefSwitch][PreDesT]!=0 && 
					  ST[RefSwitch][SucDesT]!=0 && ST[RefSwitch][SourceT]==0)
					  ExchangeScore+=SGN(ST[RefSwitch][SucDesT]^ST[RefSwitch][PreDesT]);



				  return (currentMS[RefSwitch]+ExchangeScore-OriScore);



}

int ExchangeColumns(int SourceT,int FixedSwitch,int RefSwitch,int (*FinalID)[TotalPathSetNum],
					int (*LocateTable)[TotalPathSetNum], int *currentMS, int (*ST)[TotalPathSetNum]){

						int DesID=FinalID[FixedSwitch][SourceT];
						int SourceID=FinalID[RefSwitch][SourceT];
						int DesT=LocateTable[RefSwitch][DesID];
						int PreSourceT=-1,SucSourceT=-1;
						int PreDesT=-1,SucDesT=-1;

						if(SourceID==DesID)
							return currentMS[RefSwitch];

						int tempSourceID=SourceID, tempDesID=DesID;
						while(tempSourceID>0 && tempSourceID!=DesID){
                              PreSourceT=LocateTable[RefSwitch][--tempSourceID];
							  if(ST[RefSwitch][PreSourceT]!=0) break;
						}
						tempSourceID=SourceID;
						while(tempSourceID<TotalPathSetNum-1 && tempSourceID!=DesID){
							SucSourceT=LocateTable[RefSwitch][++tempSourceID];
							if(ST[RefSwitch][SucSourceT]!=0) break;
						}

						while(tempDesID>0 && tempDesID!=SourceID){
							PreDesT=LocateTable[RefSwitch][--tempDesID];
							if(ST[RefSwitch][PreDesT]!=0) break;
						}
						tempDesID=DesID;
						while(tempDesID<TotalPathSetNum-1 && tempDesID!=SourceID){
							SucDesT=LocateTable[RefSwitch][++tempDesID];
							if(ST[RefSwitch][SucDesT]!=0) break;
						}

						/*if(SourceID!=0) PreSourceT=LocateTable[RefSwitch][SourceID-1];
						if(SourceID!=(TotalPathSetNum-1)) SucSourceT=LocateTable[RefSwitch][SourceID+1];

						if(DesID!=0) PreDesT=LocateTable[RefSwitch][DesID-1];
						if(DesID!=(TotalPathSetNum-1)) SucDesT=LocateTable[RefSwitch][DesID+1]; */ 

						int OriScore=0, ExchangeScore=0 ;


						if(SucSourceT==DesT || SucDesT==SourceT){
							int MaxID, MinID;
							int MaxT, MinT;
							int PreMinT=-1,SucMaxT=-1;
							if(ST[RefSwitch][SourceT]*ST[RefSwitch][DesT]==0){
								LocateTable[RefSwitch][DesID]=SourceT;
								LocateTable[RefSwitch][SourceID]=DesT;

								FinalID[RefSwitch][SourceT]=DesID;
								FinalID[RefSwitch][DesT]=SourceID;
								return 0;

							}
							if(SucSourceT==DesT){
								MaxID=DesID;
								MaxT=DesT;
								MinID=SourceID;
								MinT=SourceT;
								PreMinT=PreSourceT;
								SucMaxT=SucDesT;
							}else{
								MaxID=SourceID;
								MaxT=SourceT;
								MinID=DesID;
								MinT=DesT;
								PreMinT=PreDesT;
								SucMaxT=SucSourceT;
							}
					


							/*if(MinID!=0) PreMinT=LocateTable[RefSwitch][MinID-1];
							if(MaxID!=(TotalPathSetNum-1)) SucMaxT=LocateTable[RefSwitch][MaxID+1]; */ 

							if(PreMinT!=-1 && ST[RefSwitch][PreMinT]!=0 && ST[RefSwitch][MinT]!=0)
								OriScore+=SGN(ST[RefSwitch][PreMinT]^ST[RefSwitch][MinT]);
							if(SucMaxT!=-1 && ST[RefSwitch][SucMaxT]!=0 && ST[RefSwitch][MaxT]!=0)
								OriScore+=SGN(ST[RefSwitch][SucMaxT]^ST[RefSwitch][MaxT]);

							if(PreMinT!=-1 && ST[RefSwitch][PreMinT]!=0 && ST[RefSwitch][MaxT]!=0)
								ExchangeScore+=SGN(ST[RefSwitch][PreMinT]^ST[RefSwitch][MaxT]);
							if(SucMaxT!=-1 && ST[RefSwitch][SucMaxT]!=0 && ST[RefSwitch][MinT]!=0)
								ExchangeScore+=SGN(ST[RefSwitch][SucMaxT]^ST[RefSwitch][MinT]);

							currentMS[RefSwitch]+=ExchangeScore-OriScore;

							LocateTable[RefSwitch][DesID]=SourceT;
							LocateTable[RefSwitch][SourceID]=DesT;

							FinalID[RefSwitch][SourceT]=DesID;
							FinalID[RefSwitch][DesT]=SourceID;


							return 0;


				  } 

						if(PreSourceT!=-1 && ST[RefSwitch][PreSourceT]!=0 && ST[RefSwitch][SourceT]!=0)
							OriScore+=SGN(ST[RefSwitch][PreSourceT]^ST[RefSwitch][SourceT]);
						if(SucSourceT!=-1 && ST[RefSwitch][SucSourceT]!=0 && ST[RefSwitch][SourceT]!=0)
							OriScore+=SGN(ST[RefSwitch][SucSourceT]^ST[RefSwitch][SourceT]);
						if(PreSourceT!=-1 && SucSourceT!=-1 && ST[RefSwitch][PreSourceT]!=0 && 
							ST[RefSwitch][SucSourceT]!=0 && ST[RefSwitch][SourceT]==0)
							OriScore+=SGN(ST[RefSwitch][SucSourceT]^ST[RefSwitch][PreSourceT]);

						if(PreDesT!=-1 && ST[RefSwitch][PreDesT]!=0 && ST[RefSwitch][DesT]!=0)
							OriScore+=SGN(ST[RefSwitch][PreDesT]^ST[RefSwitch][DesT]);
						if(SucDesT!=-1 && ST[RefSwitch][SucDesT]!=0 && ST[RefSwitch][DesT]!=0)
							OriScore+=SGN(ST[RefSwitch][SucDesT]^ST[RefSwitch][DesT]);
						if(PreDesT!=-1 && SucDesT!=-1 && ST[RefSwitch][PreDesT]!=0 && 
							ST[RefSwitch][SucDesT]!=0 && ST[RefSwitch][DesT]==0)
							OriScore+=SGN(ST[RefSwitch][SucDesT]^ST[RefSwitch][PreDesT]);

						if(PreSourceT!=-1 && ST[RefSwitch][PreSourceT]!=0 && ST[RefSwitch][DesT]!=0)
							ExchangeScore+=SGN(ST[RefSwitch][PreSourceT]^ST[RefSwitch][DesT]);
						if(SucSourceT!=-1 && ST[RefSwitch][SucSourceT]!=0 && ST[RefSwitch][DesT]!=0)
							ExchangeScore+=SGN(ST[RefSwitch][SucSourceT]^ST[RefSwitch][DesT]);
						if(PreSourceT!=-1 && SucSourceT!=-1 && ST[RefSwitch][PreSourceT]!=0 && 
							ST[RefSwitch][SucSourceT]!=0 && ST[RefSwitch][DesT]==0)
							ExchangeScore+=SGN(ST[RefSwitch][SucSourceT]^ST[RefSwitch][PreSourceT]);

						if(PreDesT!=-1 && ST[RefSwitch][PreDesT]!=0 && ST[RefSwitch][SourceT]!=0)
							ExchangeScore+=SGN(ST[RefSwitch][PreDesT]^ST[RefSwitch][SourceT]);
						if(SucDesT!=-1 && ST[RefSwitch][SucDesT]!=0 && ST[RefSwitch][SourceT]!=0)
							ExchangeScore+=SGN(ST[RefSwitch][SucDesT]^ST[RefSwitch][SourceT]);
						if(PreDesT!=-1 && SucDesT!=-1 && ST[RefSwitch][PreDesT]!=0 && 
							ST[RefSwitch][SucDesT]!=0 && ST[RefSwitch][SourceT]==0)
							ExchangeScore+=SGN(ST[RefSwitch][SucDesT]^ST[RefSwitch][PreDesT]);

						currentMS[RefSwitch]+=ExchangeScore-OriScore;

						LocateTable[RefSwitch][DesID]=SourceT;
						LocateTable[RefSwitch][SourceID]=DesT;

						FinalID[RefSwitch][SourceT]=DesID;
						FinalID[RefSwitch][DesT]=SourceID;


						return 0;

}


int CCencoding(int (*ST)[TotalPathSetNum], ofstream &output){
	//int FinalID[Smax][TotalPathSetNum]=new int[Smax][TotalPathSetNum];
	int (*FinalID)[TotalPathSetNum]=new int[Smax][TotalPathSetNum] ;
	int currentMS[Smax];
	int MinSwitch;
	int MinMS;
	int (*LocateTable)[TotalPathSetNum]=new int[Smax][TotalPathSetNum];
	//int LocateTable[Smax][TotalPathSetNum];

	for(int i=0; i<Smax; i++)
		for(int j=0; j<TotalPathSetNum; j++)
			FinalID[i][j]=j;
	LocateTable_init(LocateTable,FinalID,ST);
	currentMS_init(currentMS, LocateTable, ST);

	int answer=0;
	for(int i=0; i<Smax; i++)
		if(currentMS[i]>answer)
			answer=currentMS[i];
	//cout<<"before conflict correcting: "<<answer<<endl;

    SwitchOptimalEncoding(FinalID,ST);
	LocateTable_init(LocateTable,FinalID,ST);
    currentMS_init(currentMS, LocateTable, ST);

	

	for(int tree=0; tree<TotalPathSetNum; tree++){
        MinMS=MaxInt;
		int currentMaxMS;
		for(int i=0; i<Smax; i++){
            currentMaxMS=0;

			for(int j=0; j<Smax; j++){
				int temp;
				temp=computeMS(tree,i,j,FinalID,LocateTable,currentMS,ST);
				if(temp>currentMaxMS)
					currentMaxMS=temp;
			}
			if(currentMaxMS<MinMS){
				MinMS=currentMaxMS;
				MinSwitch=i;
			}
		}
		for(int i=0; i<Smax; i++)
			if(i != MinSwitch)
		      ExchangeColumns(tree,MinSwitch,i,FinalID,LocateTable,currentMS,ST);
	}

	answer=0;
	for(int i=0; i<Smax; i++)
		if(currentMS[i]>answer){
			answer=currentMS[i];
		}
	output<<"the routing table size is: "<<answer<<endl;
	delete[] FinalID;
	delete[] LocateTable;
	return 0;
}