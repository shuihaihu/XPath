#include <iostream>
#include <string.h>
#include<time.h>
//#include <fstream>
#include "CPFgeneral_HyperX.h"
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

int copySwitch(int *&A, int *B){
	A=new int[LMax];
	for(int i=0; i<LMax; i++)
		A[i]=B[i];
	return 0;
}

int getSwitchID(int *Switch){
	int temp1=0;
	for(int i=LMax-1; i>=0; i--)
		temp1=temp1*S[i]+Switch[i];

	return temp1;
}


int getSwitchPortID(int level, int *StartSwitch, int *EndSwitch){
	int temp=0;
	for(int i=0; i<level; i++)
		temp+=(S[i]-1);
	if(StartSwitch[level]>EndSwitch[level])
		temp+=EndSwitch[level];
	else
		temp+=EndSwitch[level]-1;

	return temp;

}

bool equalSwitch(int *A, int *B){

	for(int i=0; i<LMax; i++)
		if(A[i]!=B[i])
			return false;
	return true;
}

int BuildSingleSPT(int *src, int **Tree, int level, int &head, int &tail,Path* PathInfo, int Tnum){
	int tail2, head2, **Tree2=new int*[Smax];
	int downpathstep=TorNum;
	int uppathstep=TorNum/S[level];
	int downpaggrdegree=downpathstep-uppathstep;
	int uppaggrdegree=uppathstep;
	int srcswtich=getSwitchID(src);
	int starttemp=Tnum*(TorNum-1);
	for(int path=starttemp; path<starttemp+TorNum-1; path++)
		addpathlink(PathInfo, path, srcswtich, -1);

    //if(Tnum==0) cout<<"Tnum = "<<Tnum<<endl;
	for(int i=0; i<LMax; i++){
		int dim= (level+i) % LMax;
		tail2=0; head2=0;
		int StartPath=Tnum*(TorNum-1);
		for(int k=head; k<tail; k++){
			int*A=NULL, *B=NULL, *C=NULL;
			copySwitch(C, Tree[k]);
			A=C;
			B=C;
			int temp=StartPath;
			//if(Tnum==0) cout<<"  StartPath__ = "<<StartPath;
			if( equalSwitch(A,src) ) 
				StartPath+=(uppathstep-1);
			else
				StartPath+=uppathstep;
			//if(Tnum==0) cout<<" downpaggrdegree = "<<downpaggrdegree;
			//if(Tnum==0) cout<<" uppaggrdegree = "<<uppaggrdegree;
			//if(Tnum==0) cout<<"  StartPath = "<<StartPath;

			int SecondStartPath=StartPath;
			StartPath=temp;
			for(int j=0; j<S[dim]-1; j++){
				copySwitch(C,B);
				C[dim]=(C[dim]+1) % S[dim]; 
				Tree2[tail2++]=C;
				//if(Tnum==0) cout<<" SecondStartPath = "<<SecondStartPath;
				int SwitchID=getSwitchID(C);
				int SPortID=getSwitchPortID(dim, C, A);
				for(int path=SecondStartPath; path<SecondStartPath+uppaggrdegree; path++)
					addpathlink(PathInfo, path, SwitchID, SPortID);
				SecondStartPath+=uppathstep;
				B=C;
			}
			//if(Tnum==0) cout<<endl;
			if( equalSwitch(A,src) ) 
				StartPath+=downpathstep-1;
			else StartPath+=downpathstep;
			delete A;

		}
		downpathstep=downpathstep/S[dim];
		uppathstep=uppathstep/S[dim];
		downpaggrdegree=downpathstep-uppathstep;
		uppaggrdegree=uppathstep;
		for(int j=head2; j<tail2; j++)
			Tree[tail++]=Tree2[j];
	}
	delete[] Tree2;
	return 0;
}

int BuildMultipleSPTs(int *src, Path* PathInfo, int STnum){
	int *root;
	int tail, head, **Tree=new int*[Smax];



	for(int i=0; i<LMax; i++){
		root=new int[LMax];
		for(int j=0; j<LMax; j++)
			root[j]=src[j];
		tail=0; head=0;
		Tree[tail++]=root;
		BuildSingleSPT(src, Tree, i, head, tail, PathInfo, STnum+i);
		for(int j=head; j<tail; j++)
			delete[] Tree[j];
	}
	delete[] Tree;
	return 0;
}
int getPathInfo(Path* PathInfo){

	for(int DesSwitch=0; DesSwitch<Smax; DesSwitch++){
		int src[LMax];
		int temp=DesSwitch;
		for(int i=0; i<LMax; i++){
			src[i]=temp % S[i];
			temp/=S[i];
		}
		BuildMultipleSPTs(src,PathInfo,DesSwitch*HDestPSNum);
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
		if(src>srcNum)
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

int isconflict(int path, PSetNode* psettemp, Path* PathInfo){
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
		return -1;
	else return 0;

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

	for(int dest=0; dest<destNum; dest++)
		for(int i=0; i<HDestPSNum; i++){
		Pathsetlist->pathsetNum++;
		PSetNode* psettemp=new PSetNode;
		psettemp->next=NULL;
		memset(psettemp->flag,0,sizeof(int)*Smax);
		for(int src=0; src<srcNum; src++)
			if(src!=dest){
				LinkNode* templinknode=SrcDestGroup[src][dest].head;
				LinkNode* pre=SrcDestGroup[src][dest].head;
				int path=-1;
				if(templinknode!=NULL)
					path=templinknode->path;
				while(templinknode!=NULL && isconflict(path, psettemp, PathInfo)){
					pre=templinknode;
					templinknode=templinknode->next;
					if(templinknode!=NULL)
						path=templinknode->path;
				}
				if(templinknode!=NULL){
					//cout<<"path = "<<path<<endl;
					addPintoPSet(path, psettemp, PathInfo);
					if(templinknode!=SrcDestGroup[src][dest].head){
					   pre->next=templinknode->next;
					}
					else {
						SrcDestGroup[src][dest].head=templinknode->next;
					}
					delete templinknode;
				}

			}
		/*for(int k=0; k<Smax; k++)
			cout<<psettemp->flag[k]<<" ";
		cout<<endl;*/ 
		if(Pathsetlist->head==NULL){
			Pathsetlist->head=psettemp;
			Pathsetlist->tail=psettemp;
		}else{
			Pathsetlist->tail->next=psettemp;
			Pathsetlist->tail=psettemp;
		}
		
	}
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
	cout<<"LMax= "<<LMax<<"     "; 
	for(int i=0; i<LMax; i++)
		cout<<"S["<<i<<"]="<<S[i]<<"; ";
	cout<<endl;
	Path* PathInfo=new Path[TotalPathNum];
	memset(PathInfo, 0, sizeof(Path)*TotalPathNum);
	getPathInfo(PathInfo);
	/*for(int i=0; i<HDestPathNum; i++){
		PathNode *temp=PathInfo[i].head;
		while(temp!=NULL){
			cout<<temp->node<<"  "<<temp->port<<"-->";
			temp=temp->next;
		}
		cout<<endl;
	}*/ 
 
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
	clock_t start, finish;   
	double     duration; 
	start=clock();   
    createPathSets(PathInfo, SrcDestGroup, Pathsetlist);
	finish=clock();   
	duration=(double)(finish-start)/CLOCKS_PER_SEC; 
	cout<<"The running time of creating CPF path set is: "<<duration<<endl; 

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