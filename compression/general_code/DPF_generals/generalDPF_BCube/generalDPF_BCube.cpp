#include <iostream>
#include <string.h>
#include<time.h>
#include "generalDPF_BCube.h"
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
			SrcDestGroup[src][dest].tail=temp;
		}
	}
	return 0;
}
/*int getSrcGroup(Path* PathInfo, Linklist (*SrcGroup)[srcNum]){

	for(int path=0; path<TotalPathNum; path++){
		int src=PathInfo[path].head->node;
		LinkNode* temp=new LinkNode;
		temp->path=path;
		temp->next=NULL;
		temp->pre=NULL;
		if(SrcGroup[src].head==NULL){
			SrcGroup[src].head=temp;
			SrcGroup[src].tail=temp;
		}else{
			SrcGroup[src].tail->next=temp;
			SrcGroup[src].tail=temp;
		}
	}
	return 0;
}*/ 
int isConvergent(int path, PSetNode* psettemp, Path* PathInfo){
	PathNode* temp=PathInfo[path].tail;
	//temp=temp->pre;
	int state=0;
	while(temp!=NULL){
		if(psettemp->flag[temp->node]){
			if(state==1)
				state=2;
		}else
			if (state==0)
				state=1;
		temp=temp->pre;
	}

	if(state==2)
		return 0;
	else return 1;

}
int isDisjoint(int path, PSetNode* psettemp, Path* PathInfo){
	PathNode* temp=PathInfo[path].tail;
	while(temp!=NULL){
		if(psettemp->flag[temp->node])
			return 0;
		temp=temp->pre;
	}

    return 1;

}
int addPintoPSet(int path, PSetNode* psettemp, Path* PathInfo){
	PathNode* temp=PathInfo[path].head;
	while(temp!=NULL){
     psettemp->flag[temp->node]=temp->port+1;
	 //cout<<(temp->port+1)<<" ";
     temp=temp->next;
	}
	//cout<<endl;
return 0;

}
int addConPath(Path* PathInfo, int src, PSetNode* psettemp, 
			   Linklist (*SrcDestGroup)[destNum], int TorState[TorNum]){
   for(int i=0; i<TorNum; i++)
	   if(TorState[i]==2){
		   int dest=i;
		   LinkNode* templinknode=SrcDestGroup[src][dest].head;
		   LinkNode* pre=SrcDestGroup[src][dest].head;
		   int path=-1;
		 //  if(templinknode!=NULL)
			//   path=templinknode->path;
		 //  else continue;
		   while(templinknode!=NULL){
			   path=templinknode->path;
			   if(isConvergent(path, psettemp, PathInfo)==0){
				  pre=templinknode;
				  templinknode=templinknode->next;
			   }else 
				   break;
		   }
		   if(templinknode!=NULL){
			   addPintoPSet(path, psettemp, PathInfo);
			   TorState[src]=1;
			   if(templinknode!=SrcDestGroup[src][dest].head){
				   pre->next=templinknode->next;
			   }
			   else {
				   SrcDestGroup[src][dest].head=templinknode->next;
			   }
			   delete templinknode;
			   return 0;
		   }
	   }
	   return 0;
}
int addDistPath(Path* PathInfo, int src, int dest, PSetNode* psettemp, 
				Linklist (*SrcDestGroup)[destNum], int TorState[TorNum]){

	LinkNode* templinknode=SrcDestGroup[src][dest].head;
	LinkNode* pre=SrcDestGroup[src][dest].head;
	int path=-1;
	if(templinknode!=NULL)
		path=templinknode->path;
	else return 0;

	while(isDisjoint(path, psettemp, PathInfo)==0){
		pre=templinknode;
		templinknode=templinknode->next;
		if(templinknode!=NULL)
			path=templinknode->path;
		else return 0;
	}
	addPintoPSet(path, psettemp, PathInfo);
	TorState[src]=1;
	TorState[dest]=2;
	if(templinknode!=SrcDestGroup[src][dest].head){
		pre->next=templinknode->next;
	}
	else {
		SrcDestGroup[src][dest].head=templinknode->next;
	}
	delete templinknode;
	

	return 0;

}
int addPathSet(PSetNode* psettemp, PSetLinklist* Pathsetlist){
	if(Pathsetlist->head==NULL){
		Pathsetlist->head=psettemp;
		Pathsetlist->tail=psettemp;
	}else{
		Pathsetlist->tail->next=psettemp;
		Pathsetlist->tail=psettemp;
	}
	return 0;
}
int createPathSets(Path* PathInfo, Linklist (*SrcDestGroup)[destNum], PSetLinklist* Pathsetlist){

    int TorState[TorNum];
	int AllPiscovered=0;
	int round=0;
	while(AllPiscovered==0){
		for(int GroupLen=TorNum/2; GroupLen>0; GroupLen>>=1){
			for(int GroupPairShift=0; GroupPairShift<GroupLen; GroupPairShift++){
				memset(TorState, 0, sizeof(int)*TorNum);
				Pathsetlist->pathsetNum++;
				PSetNode* psettemp=new PSetNode;
				memset(psettemp->flag,0,sizeof(int)*Smax);
				psettemp->next=NULL;
				int GroupStartS=0;
				while(GroupStartS<TorNum){
					for(int GSourceS=GroupStartS; GSourceS<GroupStartS+GroupLen; GSourceS++){
						int GDesS=GroupStartS+GroupLen+(GSourceS-GroupStartS+GroupPairShift)%GroupLen;
						addDistPath(PathInfo, GSourceS, GDesS, psettemp, SrcDestGroup, TorState);	
					}
					GroupStartS+=(GroupLen<<1);
				}
				for(int i=0; i<TorNum; i++)
					if(TorState[i]==0)
						addConPath(PathInfo, i, psettemp, SrcDestGroup, TorState);
				addPathSet(psettemp, Pathsetlist);

				memset(TorState, 0, sizeof(int)*TorNum);
				Pathsetlist->pathsetNum++;
				psettemp=new PSetNode;
				memset(psettemp->flag,0,sizeof(int)*Smax);
				psettemp->next=NULL;
				GroupStartS=0;
				while(GroupStartS<TorNum){
					for(int GDesS=GroupStartS; GDesS<GroupStartS+GroupLen; GDesS++){
						int GSourceS=GroupStartS+GroupLen+(GDesS-GroupStartS+GroupPairShift)%GroupLen;
						addDistPath(PathInfo, GSourceS, GDesS, psettemp, SrcDestGroup, TorState);	
					}
					GroupStartS+=(GroupLen<<1);
				}
				for(int i=0; i<TorNum; i++)
					if(TorState[i]==0)
						addConPath(PathInfo, i, psettemp, SrcDestGroup, TorState);
				addPathSet(psettemp, Pathsetlist);
			}
		}
		if(round>=RoundNum-1){
			AllPiscovered=1;
			//cout<<"LOL1"<<endl;
			for(int src=0; src<srcNum; src++)
				for(int dest=0; dest<destNum; dest++)
					if(src!=dest)
						if(SrcDestGroup[src][dest].head!=NULL){
							AllPiscovered=0;
							//cout<<"LOL2"<<endl;
							break;
							
						}
		}
	round++;
	}
	//cout<<"BSingleSPT="<<BSingleSPT<<endl;
	//cout<<"round="<<round<<endl;
	cout<<"PathsetNum="<<Pathsetlist->pathsetNum<<endl;
	//cout<<"count"<<count<<endl;
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

	for(int i=0; i<LevelNum; i++){
		KeySwitch=BserverNum+BsingleLSNum*i;
		temp=computeMinMax(KeySwitch, FinalID, LocateTable, currentMS, Switch_PathSet,totalpsnum);
		if(temp<FinalAnswer)
			FinalAnswer=temp;
	}

	//cout<<"the TotalPathsetNum is: "<<TotalPathSetNum<<endl;
	cout<<"the routing table size is: "<<FinalAnswer<<endl;
	//currentMS_init(FinalID,Switch_PathSet);
	//LocateTable_init(LocateTable,FinalID,Switch_PathSet);
	//currentMS_init(currentMS, LocateTable, Switch_PathSet);

	delete[] FinalID;
	delete[] LocateTable;
	return 0;

}

int main(){
	cout<<"LevelNum = "<<LevelNum<<"N = "<<N<<endl;
	cout<<"TotalPathNum = "<<TotalPathNum<<endl;
	Path* PathInfo=new Path[TotalPathNum];
	memset(PathInfo, 0, sizeof(Path)*TotalPathNum);
	getPathInfo(PathInfo);


	//int (*PConfTable)[PathNum]=new int[PathNum][PathNum];
	//computeConfTab(PathInfo, PConfTable);

	Linklist (*SrcDestGroup)[destNum]=new Linklist[srcNum][destNum];
	memset(SrcDestGroup, 0, sizeof(Linklist)*srcNum*destNum);
	getSrcDestGroup(PathInfo, SrcDestGroup);

	/*Linklist (*SrcGroup)[destNum]=new Linklist[srcNum];
	memset(SrcGroup, 0, sizeof(Linklist)*srcNum);
	getSrcGroup(PathInfo, SrcGroup);*/ 

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

	int **Switch_PathSet=new int*[Smax];
	for(int i=0; i<Smax; i++)
		Switch_PathSet[i]=new int[Pathsetlist->pathsetNum];

	//int (*Switch_PathSet)[TotalPathSetNum]=new int[Smax][TotalPathSetNum];
    createSPset(Switch_PathSet, Pathsetlist);
	/*for(int i=0; i<TotalPathSetNum; i++){
		cout<<"PathSet "<<i<<": ";
		for(int j=0; j<Smax; j++)
			cout<<Switch_PathSet[j][i]<<" ";
		cout<<endl;
	}*/ 
	CCencoding(Switch_PathSet, Pathsetlist->pathsetNum);

	
    PSetNode *temp1, *temp2;
	//cout<<Pathsetlist->pathsetNum<<endl;
	temp1=Pathsetlist->head;
	while(temp1!=NULL){
		temp2=temp1;
		temp1=temp1->next;
		delete temp2;
	}
	return 0;

}