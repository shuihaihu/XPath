#include <iostream>
#include <string.h>
#include<time.h>
//#include <fstream>
#include "DPFgeneral_VL2.h"
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
		  PathInfo[path].head->pre=temp;
		  temp->next=PathInfo[path].head;
		  PathInfo[path].head=temp;
	}
	return 0;

}

int getPathInfo(Path *PathInfo){

	for(int DesTorSwitch=0; DesTorSwitch<TorNum; DesTorSwitch++)
		for(int IntSwitch=0; IntSwitch<IntSNum; IntSwitch++){
			int TNum=DesTorSwitch*VDestPSNum+IntSwitch*2;
			for(int num=0; num<2; num++){
				int destgroup=DesTorSwitch/groupsize;
				int CurrentTnum=TNum+num;
				int StartPath=CurrentTnum*(TorNum-1);
				int DesAggrSwitch=TorNum+destgroup*2+num;
				int DesAggrSPort=DesTorSwitch-destgroup*groupsize;
				/*if(CurrentTnum==35){
					cout<<"destgroup = "<<destgroup<<endl;
					cout<<"TorNum = "<<TorNum<<endl;
					cout<<"DesTorSwitch = "<<DesTorSwitch<<endl;
					cout<<"DesAggrSwitch = "<<DesAggrSwitch<<endl;
				}*/ 

				for(int path=StartPath; path<StartPath+TorNum-1; path++)
					addpathlink(PathInfo, path, DesTorSwitch, -1);

				for(int path=StartPath; path<StartPath+TorNum-1; path++)
					addpathlink(PathInfo, path, DesAggrSwitch, DesAggrSPort);

				for(int path=StartPath+DA/2-1; path<StartPath+TorNum-1; path++)
					addpathlink(PathInfo, path, IntSwitch+TorNum+AggrSNum, DesAggrSwitch-TorNum);

				for(int i=TorNum+num; i<TorNum+AggrSNum; i=i+2)
					if(i!=DesAggrSwitch){
						int starttemp=0;
						int temp=i-TorNum;
						if(i>DesAggrSwitch)
							starttemp=StartPath+(temp/2-1)*groupsize+groupsize-1;
						else
							starttemp=StartPath+temp/2*groupsize+groupsize-1;

						for(int path=starttemp; path<starttemp+DA/2; path++){
							//if(CurrentTnum==0)
							//cout<<"path = "<<path<<endl;
							addpathlink(PathInfo, path, i, IntSwitch+DA/2);
						}
						//ST[i][CurrentTnum]=IntSwitch+DA/2+1;
					}
				//ST[IntSwitch+TorNum+AggrSNum][CurrentTnum]=DesAggrSwitch-TorNum+1;
				//if(CurrentTnum==0)
					//cout<<" StartPath = "<<StartPath<<" endpath = "<<StartPath+TorNum-2<<endl;

				for(int AggrSwitch=0; AggrSwitch<AggrSNum; AggrSwitch+=2){
					for(int AggrPort=0; AggrPort<DA/2; AggrPort++){
						int CurrentTor=AggrSwitch/2*groupsize+AggrPort;
						if(CurrentTor!=DesTorSwitch){
							if( num==0 ){
								int path=0;
								int temp=CurrentTor/groupsize;
								int destemp=DesTorSwitch/groupsize;
								int starttemp=temp*groupsize;
								if(temp == destemp){
									if(CurrentTor>DesTorSwitch)
										path=StartPath+CurrentTor-starttemp-1;
									else
										path=StartPath+CurrentTor-starttemp;
								}else{
									if(temp>destemp)
									   path=StartPath+(temp-1)*groupsize+groupsize-1+CurrentTor-starttemp;
									else
									   path=StartPath+temp*groupsize+groupsize-1+CurrentTor-starttemp;
								}
								//if(CurrentTnum==1)
									//cout<<" path = "<<path<<endl;
								addpathlink(PathInfo, path, CurrentTor, 0);
								//ST[CurrentTor][CurrentTnum]=1;
							}
							else{
								int path=0;
								int temp=CurrentTor/groupsize;
								int destemp=DesTorSwitch/groupsize;
								int starttemp=temp*groupsize;
								if(temp == destemp){
									if(CurrentTor>DesTorSwitch)
										path=StartPath+CurrentTor-starttemp-1;
									else
										path=StartPath+CurrentTor-starttemp;
								}else{
									if(temp>destemp)
										path=StartPath+(temp-1)*groupsize+groupsize-1+CurrentTor-starttemp;
									else
										path=StartPath+temp*groupsize+groupsize-1+CurrentTor-starttemp;
								}
								//if(CurrentTnum==0)
								//cout<<" path = "<<path<<endl;
								addpathlink(PathInfo, path, CurrentTor, 1);
				
								//ST[CurrentTor][CurrentTnum]=2;
							}
						}//else{
							
							//ST[TorNum+AggrSwitch+num][CurrentTnum]=AggrPort+1;
							//DesAggrSwitch=TorNum+AggrSwitch+num;
						//}
					}     
				}
				//if(CurrentTnum==0)
					//cout<<" StartPath+DA/2-1 = "<<StartPath+DA/2-1<<endl;
					//cout<<CurrentTnum<<endl;
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

int isConvergent(int path, PSetNode* psettemp, Path* PathInfo){
	PathNode* temp=PathInfo[path].tail;
	temp=temp->pre;
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
						   if(templinknode!=NULL)
							   path=templinknode->path;
						   else return 0;
						   while(templinknode!=NULL && isConvergent(path, psettemp, PathInfo)==0){
							   pre=templinknode;
							   templinknode=templinknode->next;
							   if(templinknode!=NULL)
								   path=templinknode->path;
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

					while(templinknode!=NULL && isDisjoint(path, psettemp, PathInfo)==0){
						pre=templinknode;
						templinknode=templinknode->next;
						if(templinknode!=NULL)
							path=templinknode->path;
					}
					if(templinknode!=NULL){
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
					}

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
		if(round>=VDestPSNum-1){
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
	//cout<<"PathsetNum="<<Pathsetlist->pathsetNum<<endl;
	//cout<<"count"<<count<<endl;
	return 0;
}

int createSPset(int **Switch_PathSet,  
						   PSetLinklist* Pathsetlist){
	   PSetNode* psettemp=Pathsetlist->head;
      for(int pset=0; pset<Pathsetlist->pathsetNum; pset++){
		  for(int s=0; s<Smax; s++)
			  Switch_PathSet[s][pset]=psettemp->flag[s];
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

	KeySwitch=TorNum;
	temp=computeMinMax(KeySwitch, FinalID, LocateTable, currentMS, Switch_PathSet,totalpsnum);
	if(temp<FinalAnswer)
		FinalAnswer=temp;

	KeySwitch=TorNum+AggrSNum;
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

int main(){
	cout<<"DA= "<<DA<<" DI= "<<DI<<endl; 
	cout<<"TotalPathNum = "<<TotalPathNum<<endl;
	Path* PathInfo=new Path[TotalPathNum];
	memset(PathInfo, 0, sizeof(Path)*TotalPathNum);
	getPathInfo(PathInfo);
	/*for(int i=VDestPathNum*(TorNum-1); i<TotalPathNum; i++){
		PathNode *temp=PathInfo[i].head;
		while(temp!=NULL){
			cout<<temp->node<<"  "<<temp->port<<"-->";
			temp=temp->next;
		}
		cout<<endl;
	}  */ 
 
	Linklist (*SrcDestGroup)[destNum]=new Linklist[srcNum][destNum];

	memset(SrcDestGroup, 0, sizeof(Linklist)*srcNum*destNum);
	getSrcDestGroup(PathInfo, SrcDestGroup);
	/*for(int src=0; src<2; src++)
        for(int dest=0; dest<2; dest++)
			if(src!=dest){
				LinkNode* temp=SrcDestGroup[src][dest].head;
				while(temp!=NULL){
					cout<<temp->path<<"--->";
					temp=temp->next;
				}
				cout<<endl;
			}*/ 


	PSetLinklist* Pathsetlist=new PSetLinklist;
	Pathsetlist->head=NULL;
	Pathsetlist->tail=NULL;
	Pathsetlist->pathsetNum=0;
	clock_t start, finish;   
	double     duration; 
	start=clock();   
    createPathSets(PathInfo, SrcDestGroup, Pathsetlist);
	finish=clock();   
	duration=(double)(finish-start)/CLOCKS_PER_SEC; 
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
	/*for(int i=0; i<Pathsetlist->pathsetNum; i++){
		cout<<"PathSet "<<i<<": ";
		for(int j=0; j<Smax; j++)
			cout<<Switch_PathSet[j][i]<<" ";
		cout<<endl;
	} */  
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