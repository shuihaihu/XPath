#include <iostream>
#include <string.h>
#include<time.h>
#include <cmath>
//#include <fstream>
#include "CPFgeneral_BCube.h"
using namespace std;

int SwitchOptimalEncoding(int s, int *FinalID, int **Switch_PathSet, int totalpsnum);
int currentMS_init(int *currentMS, int *LocateTable, int **Switch_PathSet, int totalpsnum);
int LocateTable_init(int *LocateTable, int *FinalID, int totalpsnum);
int CCencoding(int **Switch_PathSet, int totalpsnum);

class PathNode{
public:
	int node;
	int port;
	PathNode *next;
	PathNode *pre;
};
class Path{
public:
   PathNode *head;
   PathNode *tail;
};

class LinkNode{
public:
	int path;
	LinkNode *next;
	LinkNode *pre;
};
class Linklist{
public:
	LinkNode *head;
	LinkNode *tail;
};

class PSetNode{
public:
	PSetNode* next;
	int flag[Smax];
};
class PSetLinklist{
public:
	int pathsetNum;
	PSetNode* head;
	PSetNode* tail;
};
int addpathlink(Path* PathInfo, int path, int node, int port){
	PathNode* temp=new PathNode;
	temp->node=node;
	temp->port=port;
	temp->next=NULL;
	temp->pre=NULL;
	if(PathInfo[path].tail==NULL){
		   PathInfo[path].head=temp;
		   PathInfo[path].tail=temp;
	}else{
		  PathInfo[path].tail->next=temp;
		  temp->pre=PathInfo[path].tail;
		  PathInfo[path].tail=temp;
	}
	return 0;

}
int copyServer(int *&A, int *B){
	A=new int[LevelNum];
	for(int i=0; i<LevelNum; i++)
		A[i]=B[i];
	return 0;
}

int getSwitchID(int *connServer, int level){
	int temp1=0;
	int temp2=BserverNum;
	int powertemp=1;
	int temp3=0;

	for(int i=0; i<level; i++)
		powertemp*=N;

	for(int i=level-1; i>=0; i--)
		temp1=temp1*N+connServer[i];
	for(int i=LevelNum-1; i>level; i--){
		temp3=temp3*N+connServer[i];
	}
	temp3*=powertemp;

	temp2+=BsingleLSNum*level;

	return temp1+temp2+temp3;
}


inline int getSwitchPortID(int *connServer, int level){

	return connServer[level];

}

int getServerID(int *connServer){
	int temp1=0;
	for(int i=LevelNum-1; i>=0; i--)
		temp1=temp1*N+connServer[i];

	return temp1;
}


inline int getServerPortID(int level){

	return level;
}


int AddEdge(int *StartServer, int level, Path* PathInfo, 
			int &PathNum){
	int SPort;
	int ServerE;

	ServerE=getServerID(StartServer);
	SPort=getServerPortID(level);
	//Switch_PathSet[ServerE][CurrentPSNum]=SPort+1;
	//addLinkNode(ServerE, SPort, PathInfo, PathNum);
	addpathlink(PathInfo, PathNum, ServerE, SPort);

	return 0;
}
int BcubeRouting(Path* PathInfo, int *A, 
				 int *B, int *pi, int &PathNum, int mode){
 int k=LevelNum-1;
 int I_Node[LevelNum];
 for(int i=0; i<LevelNum; i++)
	 I_Node[i]=A[i];
 for(int i=k; i>=0; i--)
	 if(A[pi[i]]!=B[pi[i]]){
/*		 if(mode==0){
			 mode=1;
			 I_Node[pi[i]]=B[pi[i]];
			 int SwitchPort=getSwitchPortID(I_Node, pi[i]);
			 int s=getSwitchID(I_Node, pi[i]);
			 //Switch_PathSet[s][PathSetNum]=SwitchPort+1;
			 //addLinkNode(s, SwitchPort, PathInfo, PathNum);
			 addpathlink(PathInfo, PathNum, s, SwitchPort);
		 }
		 else {*/ 
			 AddEdge(I_Node, pi[i], PathInfo, PathNum);
			 I_Node[pi[i]]=B[pi[i]];
			 int SwitchPort=getSwitchPortID(I_Node, pi[i]);
			 int s=getSwitchID(I_Node, pi[i]);
			 //Switch_PathSet[s][PathSetNum]=SwitchPort+1;
			 //addLinkNode(s, SwitchPort, PathInfo, PathNum);
			 addpathlink(PathInfo, PathNum, s, SwitchPort);
	//	 }

	 }

 int Server=getServerID(I_Node);
 addpathlink(PathInfo, PathNum, Server, -2);
 return 0;
}
int DCRouting(Path* PathInfo, int *A, 
			  int *B, int i, int &PathNum){
	  int k=LevelNum-1;
	  int pi[LevelNum];
	  int m=k;
	  for(int j=i; j>=i-k; j--){
		  pi[m]=(j+LevelNum)%LevelNum;
		  m--;
	  }
	  BcubeRouting(PathInfo, A, B, pi, PathNum, 0);
	  return 0;
}
int AltDCRouting(Path* PathInfo, int *A, int *B, 
				 int i, int *C, int &PathNum){
	 int k=LevelNum-1;
	 int pi[LevelNum];
	 int m=k;
	 for(int j=i-1; j>=i-1-k; j--){
		 pi[m]=(j+LevelNum)%LevelNum;
		 m--;
	 }
	 BcubeRouting(PathInfo, C, B, pi, PathNum, 1);
	 return 0;
}
int BuildSrcDestPath(Path* PathInfo, int SourceS, 
				 int DesS, int &PathNum){
		 int A[LevelNum], B[LevelNum], C[LevelNum];
		 for(int i=0; i<LevelNum; i++){
			 A[i]=SourceS % N;
			 C[i]=A[i];
			 SourceS/=N;
		 }
		 for(int i=0; i<LevelNum; i++){
			 B[i]=DesS % N;
			 DesS/=N;
		 }
		 for(int i=LevelNum-1; i>=0; i--){
			 if(A[i]!=B[i])
				 DCRouting(PathInfo, A, B, i, PathNum);
			 else{
				 int Server=getServerID(A);
				 int ServerPort=getServerPortID(i);
				 addpathlink(PathInfo, PathNum, Server, ServerPort);
				 C[i]=(C[i]+1)%N;
				 int SwitchPort=getSwitchPortID(C, i);
				 int s=getSwitchID(C, i);
				 addpathlink(PathInfo, PathNum, s, SwitchPort);
				 AltDCRouting(PathInfo, A, B, i, C, PathNum);
				 C[i]=A[i];
			 }
			 PathNum++;
		 }
		return 0;
}


int getPathInfo(Path* PathInfo){
	int PathNum=0;
	for(int GroupLen=serverNum/2; GroupLen>0; GroupLen>>=1){
		for(int GroupPairShift=0; GroupPairShift<GroupLen; GroupPairShift++){
			int GroupStartS=0;
			while(GroupStartS<serverNum){
				for(int GSourceS=GroupStartS; GSourceS<GroupStartS+GroupLen; GSourceS++){
					BuildSrcDestPath(PathInfo, GSourceS, 
						GroupStartS+GroupLen+(GSourceS-GroupStartS+GroupPairShift)%GroupLen,  
						PathNum);			

				}
				GroupStartS+=(GroupLen<<1);
			}

			GroupStartS=0;
			while(GroupStartS<serverNum){
				for(int GDesS=GroupStartS; GDesS<GroupStartS+GroupLen; GDesS++){
					BuildSrcDestPath(PathInfo,  
						GroupStartS+GroupLen+(GDesS-GroupStartS+GroupPairShift)%GroupLen, 
						GDesS, PathNum);
				}
				GroupStartS+=(GroupLen<<1);
			}
		}
	}
	return 0;
}

int getSrcDestGroup(Path* PathInfo, Linklist (*SrcDestGroup)[destNum]){

	for(int path=0; path<TotalPathNum; path++){
		int src=PathInfo[path].head->node;
		int dest=PathInfo[path].tail->node;
		LinkNode* temp=new LinkNode;
		temp->path=path;
		temp->next=NULL;
		temp->pre=NULL;
		if(SrcDestGroup[src][dest].head==NULL){
			SrcDestGroup[src][dest].head=temp;
			SrcDestGroup[src][dest].tail=temp;
		}else{
			SrcDestGroup[src][dest].tail->next=temp;
			temp->pre=SrcDestGroup[src][dest].tail;
			SrcDestGroup[src][dest].tail=temp;
		}
	}
	return 0;
}

int reverseSrcDestG(Linklist (*SrcDestGroup)[destNum]){

	LinkNode *temp1, *temp2;
	for(int src=0; src<srcNum; src++)
		if((src & 1)==0)
			for(int dest=0; dest<destNum; dest++)
			    if(src!=dest){
				temp1=SrcDestGroup[src][dest].head;
				temp2=SrcDestGroup[src][dest].tail;
				while(temp2!=temp1){
					temp2->next=temp2->pre;
					temp2=temp2->next;
				}
                SrcDestGroup[src][dest].head=SrcDestGroup[src][dest].tail;
                SrcDestGroup[src][dest].tail=temp1;
				 SrcDestGroup[src][dest].tail->next=NULL;

			}
	return 0;
}

int isconvergent(int path, PSetNode* psettemp, Path* PathInfo){
	PathNode* temp=PathInfo[path].tail;
	//temp=temp->pre;
	int state=0;
	int condegree=0;
	while(temp!=NULL){
		if(psettemp->flag[temp->node]){
			condegree++;
			if(state==1)
				state=2;
		}else
			if (state==0)
				state=1;
		temp=temp->pre;
	}

	if(state==2)
		return 0;
	else return (condegree+1);

}
int addPintoPSet(int path, PSetNode* psettemp, Path* PathInfo){
	PathNode* temp=PathInfo[path].head;
	while(temp!=NULL){
     psettemp->flag[temp->node]=temp->port+1;
     temp=temp->next;
	}
return 0;

}
int createPathSets(Path* PathInfo, Linklist (*SrcDestGroup)[destNum], PSetLinklist* Pathsetlist){

	for(int dest=0; dest<destNum; dest++){
		int pathnum=0;
		while(pathnum<DestPathNum){
		Pathsetlist->pathsetNum++;
		PSetNode* psettemp=new PSetNode;
		psettemp->next=NULL;
		memset(psettemp->flag,0,sizeof(int)*Smax);
		for(int src=0; src<srcNum; src++)
			if(src!=dest && SrcDestGroup[src][dest].head!=NULL){
				LinkNode* templinknode=SrcDestGroup[src][dest].head;
				LinkNode* pre=SrcDestGroup[src][dest].head;
				LinkNode* bestpre=NULL;
				LinkNode* bestlinknode=NULL;
				int condegree=0;
				int bestcondegree=0;
				int path=-1;
				int bestpath=-1;
				while(templinknode!=NULL){
					path=templinknode->path;
					condegree=isconvergent(path, psettemp, PathInfo);
					if(condegree>bestcondegree){
						bestpath=path;
						bestpre=pre;
						bestlinknode=templinknode;
						bestcondegree=condegree;
					}
					pre=templinknode;
					templinknode=templinknode->next;

				}
				if(bestpath!=-1){
					addPintoPSet(bestpath, psettemp, PathInfo);
					pathnum++;
					//cout<<"pathnum= "<<pathnum<<" DestPathnum="<<DestPathNum<<endl;
					if(bestlinknode!=SrcDestGroup[src][dest].head){
					   bestpre->next=bestlinknode->next;
					} 
					else {
						SrcDestGroup[src][dest].head=bestlinknode->next;
					}
					delete bestlinknode;
				}

			}
		if(Pathsetlist->head==NULL){
			Pathsetlist->head=psettemp;
			Pathsetlist->tail=psettemp;
		}else{
			Pathsetlist->tail->next=psettemp;
			Pathsetlist->tail=psettemp;
		}
		/*if(dest==25){
			cout<<"pathnum = "<<pathnum<<endl;
			for(int k=0; k<Smax; k++)
				cout<<psettemp->flag[k]<<" ";
			cout<<endl;
		}*/ 
		
		}
	}
	return 0;
}

int createSPset(int **Switch_PathSet,  
				PSetLinklist* Pathsetlist){
	PSetNode* psettemp=Pathsetlist->head;
	for(int pset=0; pset<Pathsetlist->pathsetNum; pset++){
		for(int s=0; s<Smax; s++)
			if(psettemp->flag[s]>0){
				Switch_PathSet[s][pset]=psettemp->flag[s];
			}else
				Switch_PathSet[s][pset]=0;

			psettemp=psettemp->next;
	}
	return 0;

}

int SwitchOptimalEncoding(int s, int *FinalID, int **Switch_PathSet, int totalpsnum){
	int CurrentID;
	int *flag=new int[totalpsnum];
	memset(flag,0, sizeof(int)*totalpsnum);
	CurrentID=0;
	for(int i=0; i<totalpsnum; i++)
		if(flag[i]==0) {
			flag[i]=1;
			FinalID[i]=CurrentID++;
			for(int j=i+1; j<totalpsnum; j++)        
				if(Switch_PathSet[s][j]==Switch_PathSet[s][i]){
					flag[j]=1;
					FinalID[j]=CurrentID++;
				}
		} 
	delete[] flag;
	return 0;

}


int currentMS_init(int *currentMS, int *LocateTable, int **Switch_PathSet, int totalpsnum){
	for(int i=0; i<Smax; i++){
		int j, pre;
		pre=0;
		currentMS[i]=1;
		for(j=0; j<totalpsnum;j++)
			if(Switch_PathSet[i][ LocateTable[j] ]!=0){
				pre=Switch_PathSet[i][ LocateTable[j] ];
				break;
			}
		if(pre!=0)
			for( j++; j<totalpsnum; j++)
				if(Switch_PathSet[i][ LocateTable[j] ] != 0 && pre!=Switch_PathSet[i][ LocateTable[j] ]){
					currentMS[i]++;
					pre=Switch_PathSet[i][ LocateTable[j] ];
				}
	}
	return 0;

}
int LocateTable_init(int *LocateTable, int *FinalID, int totalpsnum){

	for(int tree=0; tree<totalpsnum; tree++)
		LocateTable[FinalID[tree]]=tree;
return 0;
}


int computeMinMax(int KeySwitch, int *FinalID,
					  int *LocateTable, int *currentMS, int **Switch_PathSet, int totalpsnum){
			int answer=0;

			SwitchOptimalEncoding(KeySwitch, FinalID, Switch_PathSet, totalpsnum);
			LocateTable_init(LocateTable, FinalID, totalpsnum);
		    currentMS_init(currentMS, LocateTable, Switch_PathSet, totalpsnum);
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

int CCencoding(int **Switch_PathSet, int totalpsnum){
	int *FinalID=new int[totalpsnum];
	int *LocateTable=new int[totalpsnum];
	int currentMS[Smax];
	int KeySwitch;
	int FinalAnswer=MaxInt;
	int temp;

	for(int j=0; j<totalpsnum; j++)
		FinalID[j]=j;
	LocateTable_init(LocateTable,FinalID, totalpsnum);
	currentMS_init(currentMS, LocateTable, Switch_PathSet,totalpsnum);

	int answer=0;
	for(int i=0; i<Smax; i++)
		if(currentMS[i]>answer)
			answer=currentMS[i];
	//cout<<"before conflict correcting: "<<answer<<endl;

	KeySwitch=0;
	temp=computeMinMax(KeySwitch, FinalID, LocateTable, currentMS, Switch_PathSet,totalpsnum);
	if(temp<FinalAnswer)
		FinalAnswer=temp;

	cout<<"the TotalPathsetNum is: "<<totalpsnum<<endl;
	cout<<"the routing table size is: "<<FinalAnswer<<endl;
	//currentMS_init(FinalID,Switch_PathSet);
	//LocateTable_init(LocateTable,FinalID,Switch_PathSet);
	//currentMS_init(currentMS, LocateTable, Switch_PathSet);

	delete[] FinalID;
	delete[] LocateTable;
	return 0;

}
int getnode(int nodeID, int* node){
	for(int i=0; i<LevelNum; i++){
		node[i]=nodeID % N;
		nodeID=nodeID/N;
	}
	return 0;
}
int printPath(int path, Path *PathInfo){
	PathNode *temp=PathInfo[path].head;
	int node[LevelNum];
	while(temp!=NULL){
        getnode(temp->node, node);
		if(temp->next!=NULL)
		    cout<<"[ ("<<node[1]<<","<<node[0]<<") , "<<temp->port<<" ] ->";
		else
			cout<<"[ ("<<node[1]<<","<<node[0]<<") , "<<temp->port<<" ]";
		temp=temp->next;
	}
	return 0;
} 
int main(){
	cout<<"LevelNum = "<<LevelNum<<"N = "<<N<<endl;
	cout<<"TotalPathNum = "<<TotalPathNum<<endl;
	Path* PathInfo=new Path[TotalPathNum];
	memset(PathInfo, 0, sizeof(Path)*TotalPathNum);
	getPathInfo(PathInfo);
	
	Linklist (*SrcDestGroup)[destNum]=new Linklist[srcNum][destNum];

	memset(SrcDestGroup, 0, sizeof(Linklist)*srcNum*destNum);
	getSrcDestGroup(PathInfo, SrcDestGroup);
	for(int src=0; src<srcNum; src++)
        for(int dest=0; dest<destNum; dest++)
			if(src==0 && dest==15){
				LinkNode* temp=SrcDestGroup[src][dest].head;
				while(temp!=NULL){
					printPath(temp->path, PathInfo);
					temp=temp->next;
					cout<<endl;
				}
				cout<<endl;
			} 
	reverseSrcDestG(SrcDestGroup);
	/*for(int src=0; src<2; src++)
		for(int dest=0; dest<2; dest++)
			if(src!=dest){
				LinkNode* temp=SrcDestGroup[src][dest].head;
				while(temp!=NULL){
					cout<<temp->path<<"--->";
					temp=temp->next;
				}
				cout<<endl;
			} */ 

	PSetLinklist* Pathsetlist=new PSetLinklist;
	Pathsetlist->head=NULL;
	Pathsetlist->tail=NULL;
	Pathsetlist->pathsetNum=0;
	//clock_t start, finish;   
	//double     duration; 
	//start=clock();   
    createPathSets(PathInfo, SrcDestGroup, Pathsetlist);
	//finish=clock();   
	//duration=(double)(finish-start)/CLOCKS_PER_SEC; 
	//cout<<"The running time of creating CPF path set is: "<<duration<<endl; 
	for(int i=0; i<TotalPathNum; i++){
		PathNode *temp1, *temp2;
		temp1=PathInfo[i].head;
		while(temp1!=NULL){
			temp2=temp1;
			temp1=temp1->next;
			delete temp2;
		}
	}
	delete[] PathInfo;

	delete[] SrcDestGroup;
	//int (*Switch_PathSet)[TotalPathSetNum]=new int[Smax][TotalPathSetNum];
	int **Switch_PathSet=new int*[Smax];
	for(int i=0; i<Smax; i++)
		Switch_PathSet[i]=new int[Pathsetlist->pathsetNum];
    createSPset(Switch_PathSet, Pathsetlist);
	/*for(int i=0; i<Pathsetlist->pathsetNum; i++){
		cout<<"PathSet "<<i<<": ";
		for(int j=0; j<Smax; j++)
			cout<<Switch_PathSet[j][i]<<" ";
		cout<<endl;
	}  */ 
	PSetNode *temp1, *temp2;
	//cout<<Pathsetlist->pathsetNum<<endl;
	temp1=Pathsetlist->head;
	while(temp1!=NULL){
		temp2=temp1;
		temp1=temp1->next;
		delete temp2;
	}
	CCencoding(Switch_PathSet, Pathsetlist->pathsetNum);
	

	return 0;

}
