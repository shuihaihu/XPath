#include <iostream>
#include <string.h>
#include<time.h>
#include "generalDPF_FatTree.h"
using namespace std;

int SwitchOptimalEncoding(int *FinalID, int (*Switch_PathSet)[TotalPathSetNum]);
int currentMS_init(int *currentMS, int *LocateTable, int (*Switch_PathSet)[TotalPathSetNum]);
int LocateTable_init(int *LocateTable, int *FinalID, int (*Switch_PathSet)[TotalPathSetNum]);
int CCencoding(int (*Switch_PathSet)[TotalPathSetNum]);

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
int getPathInfo(Path* PathInfo){

	for(int destination=0; destination<EdgeSwitchNum;destination++){

		int StartAggrSwitch=EdgeSwitchNum+((int)(destination*2/podsize))*podsize/2;
		int EndAggrSwitch=StartAggrSwitch+podsize/2;

		for(int path=destination*FDestPathNum; path<destination*FDestPathNum+FDestPathNum; path++)
			addpathlink(PathInfo, path, destination, -1);

		for(int i=StartAggrSwitch; i<EndAggrSwitch; i++){
		   int AggreConnPort;
		   int StartTNum;
		   AggreConnPort=destination-((int)(destination*2/podsize))*podsize/2;
		   StartTNum=FTmax*destination+podsize/2*(i-StartAggrSwitch);
 
		   for(int Tnumber=StartTNum; Tnumber<StartTNum+podsize/2; Tnumber++){
			   int startpath=Tnumber*(EdgeSwitchNum-1);
			   for(int path=startpath; path<startpath+EdgeSwitchNum-1; path++)
			       addpathlink(PathInfo, path, i, AggreConnPort);
		   }
			   //CreateNewSPNode(i, AggreConnPort, Tnumber, TCluster);

		   int StartCoreSwitch, EndCoreSwitch;
		   StartCoreSwitch=EdgeSwitchNum+AggreSwitchNum+(i-StartAggrSwitch)*podsize/2;
           EndCoreSwitch=StartCoreSwitch+podsize/2;

		   for(int j=StartCoreSwitch; j<EndCoreSwitch; j++){

			   int StartPod,  CurrentTNum, CoreConnPort;
			   StartPod=destination*2/podsize;
			   CurrentTNum= StartTNum+(j-StartCoreSwitch);
			   CoreConnPort= StartPod; 

			   for(int path=CurrentTNum*(EdgeSwitchNum-1)+podsize/2-1; path<CurrentTNum*(EdgeSwitchNum-1)+EdgeSwitchNum-1; path++)
				   addpathlink(PathInfo, path, j, CoreConnPort);

			   int DownConnSOrder, DownConnPOrder;

			   DownConnSOrder=(j-EdgeSwitchNum-AggreSwitchNum)*2/podsize;
               DownConnPOrder=podsize/2+(j-EdgeSwitchNum-AggreSwitchNum)%(podsize/2);


			   for(int k=EdgeSwitchNum+DownConnSOrder; k<EdgeSwitchNum+AggreSwitchNum; k=k+podsize/2)
			       if((k-EdgeSwitchNum)*2/podsize != StartPod) {

					   int startpath;
					   int currentPod=(k-EdgeSwitchNum)*2/podsize;
					   if(currentPod > StartPod)
						   startpath=CurrentTNum*(EdgeSwitchNum-1)+podsize/2-1+(currentPod-1)*podsize/2;
					   else
						   startpath=CurrentTNum*(EdgeSwitchNum-1)+podsize/2-1+currentPod*podsize/2;
					   for(int path=startpath; path<startpath+podsize/2; path++)
						   addpathlink(PathInfo, path, k, DownConnPOrder);



				   int StartX;
				   StartX=k-EdgeSwitchNum-DownConnSOrder;
				   int path=startpath;
				   for(int x=StartX; x<StartX+podsize/2; x++,path++)
					   if(x!=destination){
                          addpathlink(PathInfo, path, x, DownConnSOrder+podsize/2);
					   }

			   }

		   }
	       for(int j=StartAggrSwitch-EdgeSwitchNum; j<EndAggrSwitch-EdgeSwitchNum; j++)
		       if(j!=destination){
				   for(int k=StartTNum; k<StartTNum+podsize/2; k++){
					   int path=0;
					   if(j>destination)
					      path=k*(EdgeSwitchNum-1)+j-1-(StartAggrSwitch-EdgeSwitchNum);
					   else
						   path=k*(EdgeSwitchNum-1)+j-(StartAggrSwitch-EdgeSwitchNum);
					   addpathlink(PathInfo, path, j, i-StartAggrSwitch+podsize/2);
				   }
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
		if(round>=Tmax-1){
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
	//cout<<"Tmax="<<Tmax<<endl;
	//cout<<"round="<<round<<endl;
	cout<<"PathsetNum="<<Pathsetlist->pathsetNum<<endl;
	//cout<<"count"<<count<<endl;
	return 0;
}

int   createSPset(int (*Switch_PathSet)[TotalPathSetNum],  
						   PSetLinklist* Pathsetlist){
	   PSetNode* psettemp=Pathsetlist->head;
      for(int pset=0; pset<Pathsetlist->pathsetNum; pset++){
		  for(int s=0; s<Smax; s++)
			  Switch_PathSet[s][pset]=psettemp->flag[s];
		  psettemp=psettemp->next;
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

int CCencoding(int (*Switch_PathSet)[TotalPathSetNum]){
	int *FinalID=new int[TotalPathSetNum];
	int *LocateTable=new int[TotalPathSetNum];
	int currentMS[Smax];
	int KeySwitch;
	int FinalAnswer=MaxInt;
	int temp;

	for(int j=0; j<TotalPathSetNum; j++)
		FinalID[j]=j;
	LocateTable_init(LocateTable,FinalID);
	currentMS_init(currentMS, LocateTable, Switch_PathSet);

	int answer=0;
	for(int i=0; i<Smax; i++)
		if(currentMS[i]>answer)
			answer=currentMS[i];
	//cout<<"before conflict correcting: "<<answer<<endl;

	KeySwitch=0;
	temp=computeMinMax(KeySwitch, FinalID, LocateTable, currentMS, Switch_PathSet);
	if(temp<FinalAnswer)
		FinalAnswer=temp;

	KeySwitch=EdgeSwitchNum;
	temp=computeMinMax(KeySwitch, FinalID, LocateTable, currentMS, Switch_PathSet);
	if(temp<FinalAnswer)
		FinalAnswer=temp;

	KeySwitch=EdgeSwitchNum+AggreSwitchNum;
	temp=computeMinMax(KeySwitch, FinalID, LocateTable, currentMS, Switch_PathSet);
	if(temp<FinalAnswer)
		FinalAnswer=temp;

	cout<<"the TotalPathsetNum is: "<<TotalPathSetNum<<endl;
	cout<<"the routing table size is: "<<FinalAnswer<<endl;
	//currentMS_init(FinalID,Switch_PathSet);
	//LocateTable_init(LocateTable,FinalID,Switch_PathSet);
	//currentMS_init(currentMS, LocateTable, Switch_PathSet);

	delete[] FinalID;
	delete[] LocateTable;
	return 0;

}

int main(){
	cout<<podsize<<endl;
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
	clock_t start, finish;   
	double     duration; 
	start=clock();   
    createPathSets(PathInfo, SrcDestGroup, Pathsetlist);
	finish=clock();   
	duration=(double)(finish-start)/CLOCKS_PER_SEC; 
	cout<<"The running time of creating CPF path set is: "<<duration<<endl; 

	int (*Switch_PathSet)[TotalPathSetNum]=new int[Smax][TotalPathSetNum];
    createSPset(Switch_PathSet, Pathsetlist);
	/*for(int i=0; i<TotalPathSetNum; i++){
		cout<<"PathSet "<<i<<": ";
		for(int j=0; j<Smax; j++)
			cout<<Switch_PathSet[j][i]<<" ";
		cout<<endl;
	}*/ 
	CCencoding(Switch_PathSet);

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