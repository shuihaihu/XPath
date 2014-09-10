#include <iostream>
#include <string.h>
#include<time.h>
#include <cmath>
//#include <fstream>
#include "DPFgeneral_FlatButterfly.h"
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
		if(PathInfo[path].head==NULL){
		   PathInfo[path].head=temp;
		   PathInfo[path].tail=temp;
	}else{
		  PathInfo[path].tail->next=temp;
		  temp->pre=PathInfo[path].tail;
		  PathInfo[path].tail=temp;
	}
	return 0;

}
int getnode(int nodeID, int* node){

	for(int i=0; i<LevelNum; i++){
		node[i]=nodeID % radix_k;
		nodeID=nodeID/radix_k;
	}
	return 0;
}
int getnodeID(int *tempnode){
	int temp=0;
	for(int i=LevelNum-1; i>=0; i--){
		temp=temp*radix_k+tempnode[i];
	}
	return temp;
}

int getsrcdestpath(int *srcnode, int *destnode, int dim, Path* PathInfo, int pathnum){
	int tempnode[LevelNum];
	int port=0;
	for(int i=0; i<LevelNum; i++)
		tempnode[i]=srcnode[i];

	for(int i=0; i<LevelNum; i++){
		if(tempnode[dim]!=destnode[dim]){
			addpathlink(PathInfo, pathnum, getnodeID(tempnode), dim);
			tempnode[dim]=destnode[dim];
		}
		dim++;
		if(dim==LevelNum) 
			dim=0;
	}
	addpathlink(PathInfo, pathnum, getnodeID(tempnode), -2);

	return 0;
}
int getPathInfo(Path* PathInfo){
	int pathnum=0;
	int srcnode[LevelNum], destnode[LevelNum];
	for(int src=0; src<srcNum; src++)
		for(int dest=0; dest<destNum; dest++)
		     if(src!=dest){
            getnode(src, srcnode);
			getnode(dest, destnode);
			for(int dim=0; dim<LevelNum; dim++)
				if(srcnode[dim]!=destnode[dim]){
					getsrcdestpath(srcnode, destnode, dim, PathInfo, pathnum);
					pathnum++;
				}
		}
	/*for(int src=srcNum/2; src<srcNum; src++)
		for(int dest=0; dest<destNum/2; dest++)
			if(src!=dest){
				getnode(src, srcnode);
				getnode(dest, destnode);
				getsrcdestpath(srcnode, destnode, PathInfo, pathnum);
				pathnum++;
			}*/    
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
				//if(Pathsetlist->pathsetNum==1)
				  // cout<<"pathsetNum = "<<Pathsetlist->pathsetNum<<endl;
				PSetNode* psettemp=new PSetNode;
				memset(psettemp->flag,0,sizeof(int)*Smax);
				psettemp->next=NULL;
				//int pspnum=0;
				int GroupStartS=0;
				while(GroupStartS<TorNum){
					for(int GSourceS=GroupStartS; GSourceS<GroupStartS+GroupLen; GSourceS++){
						int GDesS=GroupStartS+GroupLen+(GSourceS-GroupStartS+GroupPairShift)%GroupLen;
						addDistPath(PathInfo, GSourceS, GDesS, psettemp, SrcDestGroup, TorState);	
					}
					GroupStartS+=(GroupLen<<1);
				}
				/*if(Pathsetlist->pathsetNum==1){
				   cout<<"pspnum= "<<pspnum<<endl;
				   for(int ss=0; ss<Smax; ss++)
					   if(psettemp->flag[ss]!=0)
						   cout<<ss<<" ";
				   cout<<endl;
				}*/ 
				for(int i=0; i<TorNum; i++)
					if(TorState[i]==0)
						addConPath(PathInfo, i, psettemp, SrcDestGroup, TorState);
				addPathSet(psettemp, Pathsetlist);

				memset(TorState, 0, sizeof(int)*TorNum);
				Pathsetlist->pathsetNum++;
				//cout<<"pathsetNum = "<<Pathsetlist->pathsetNum<<endl;
				psettemp=new PSetNode;
				memset(psettemp->flag,0,sizeof(int)*Smax);
				psettemp->next=NULL;
				GroupStartS=0;
				//pspnum=0;
				while(GroupStartS<TorNum){
					for(int GDesS=GroupStartS; GDesS<GroupStartS+GroupLen; GDesS++){
						int GSourceS=GroupStartS+GroupLen+(GDesS-GroupStartS+GroupPairShift)%GroupLen;
						addDistPath(PathInfo, GSourceS, GDesS, psettemp, SrcDestGroup, TorState);	
					}
					GroupStartS+=(GroupLen<<1);
				}
				//cout<<"pspnum= "<<pspnum<<endl;
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
	//cout<<"Tmax="<<Tmax<<endl;
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
	
	/*for(int i=0; i<LevelNum; i++){
		KeySwitch=i*N;
		temp=computeMinMax(KeySwitch, FinalID, LocateTable, currentMS, Switch_PathSet,totalpsnum);
		if(temp<FinalAnswer){
			FinalAnswer=temp;
			for(int j=0; j<Smax; j++){
				cout<<"switch "<<j<<": ";
				for(int k=0; k<totalpsnum; k++)
					cout<<Switch_PathSet[j][LocateTable[k]]<<" ";
				cout<<endl;
			}
		}
	}*/

	cout<<"the TotalPathsetNum is: "<<totalpsnum<<endl;
	cout<<"the routing table size is: "<<FinalAnswer<<endl;
	//currentMS_init(FinalID,Switch_PathSet);
	//LocateTable_init(LocateTable,FinalID,Switch_PathSet);
	//currentMS_init(currentMS, LocateTable, Switch_PathSet);

	delete[] FinalID;
	delete[] LocateTable;
	return 0;

}
int printPath(int path, Path *PathInfo){
	PathNode *temp=PathInfo[path].head;
	int node[LevelNum];
	while(temp!=NULL){
        getnode(temp->node, node);
		if(temp->next!=NULL)
		    cout<<"[ ("<<node[2]<<","<<node[1]<<","<<node[0]<<") , "<<temp->port<<" ] ->";
		else
			cout<<"[ ("<<node[2]<<","<<node[1]<<","<<node[0]<<") , "<<temp->port<<" ]";
		temp=temp->next;
	}
	return 0;
}
int getC(int n, int i){
	if(i==n || i==0)
		return 1;

	int a=n;
	for(int j=n-1; j>i; j--)
	    a*=j;

	int b=n-i;
	for(int j=n-i-1; j>1; j--)
		b*=j;

	return a/b;
	  
}
int getDestPathNum(int LevelNum){
	int sum=0;
	for(int i=LevelNum; i>0; i--){
        sum+=getC(LevelNum, i)*i;
	}
	return sum;
}
int main(){
	cout<<" DimNum = "<<LevelNum<<"   N = "<<N<<endl;

	DestPathNum=getDestPathNum(LevelNum);
	TotalPathNum=DestPathNum*TorNum;

	cout<<" TotalPathNum= "<<TotalPathNum<<endl;
	Path* PathInfo=new Path[TotalPathNum];
	memset(PathInfo, 0, sizeof(Path)*TotalPathNum);
	getPathInfo(PathInfo);
	
	Linklist (*SrcDestGroup)[destNum]=new Linklist[srcNum][destNum];

	memset(SrcDestGroup, 0, sizeof(Linklist)*srcNum*destNum);
	getSrcDestGroup(PathInfo, SrcDestGroup);
	/*for(int src=0; src<srcNum; src++)
        for(int dest=0; dest<destNum; dest++)
			if(src==7 && dest==4){
				LinkNode* temp=SrcDestGroup[src][dest].head;
				while(temp!=NULL){
					printPath(temp->path, PathInfo);
					temp=temp->next;
					cout<<endl;
				}
				cout<<endl;
			}*/ 
	/*reverseSrcDestG(SrcDestGroup);
	for(int src=0; src<2; src++)
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
	} */   
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
