#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <cmath>
#include<time.h>
#include <string.h>
using namespace std;
#include "HyperX.h"

int copySwitch(int *&A, int *B);
int getSwitchID(int *Switch);
int getSwitchPortID(int level, int *StartSwitch, int *EndSwitch);
int AddEdge(int *StartSwitch, int *EndSwitch, int level, int (*ST)[TotalTNum], int Tnum);
int BuildSingleSPT(int *src, int **Tree, int level, int &head, int &tail,int (*ST)[TotalTNum], int Tnum);
int BuildMultipleSPTs(int *src, int (*ST)[TotalTNum], int STnum);
int createSpanningTree(int (*ST)[TotalTNum]);
int SwitchOptimalEncoding(int *FinalID, int (*ST)[TotalTNum]);
int currentMS_init(int *currentMS, int *LocateTable, int (*ST)[TotalTNum]);
int LocateTable_init(int *LocateTable, int *FinalID, int (*ST)[TotalTNum]);
int CCencoding(int (*ST)[TotalTNum], ofstream &output);


int main(){
	int (*ST)[TotalTNum]=new int[Smax][TotalTNum];
	clock_t start, finish;   
	double     duration; 
	ofstream output("HyperX_Output.txt",ios::app);
	output<<setiosflags(ios::fixed)<<setprecision(6);

	output<<"LMax= "<<LMax<<"     "; 
	for(int i=0; i<LMax; i++)
		output<<"S["<<i<<"]="<<S[i]<<"; ";
	output<<endl;

	memset(ST,0,sizeof(int)*Smax*TotalTNum);
	srand((unsigned)time(0));
             
	start=clock();   
	createSpanningTree(ST);
	finish=clock();   
	duration=(double)(finish-start)/CLOCKS_PER_SEC; 
	//cout<<"The running time of creating spanning trees is: "<<duration<<endl; 
	output<<"The running time of creating spanning trees is: "<<duration<<endl; 

/* for(int i=0; i<Smax; i++){
	  cout<<"Switch number "<<i<<": ";
      for(int j=0; j<TotalTNum; j++)
          cout<<ST[i][j]<<" ";
	  cout<<endl;
  }*/   
	start=clock();   
	CCencoding(ST,output);
	finish=clock();   
	duration=(double)(finish-start)/CLOCKS_PER_SEC; 
	//cout<<"The running time of creating spanning trees is: "<<duration<<endl; 
	output<<"The running time of creating spanning trees is: "<<duration<<endl; 

  delete[] ST;
  output.close();

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


int AddEdge(int *StartSwitch, int *EndSwitch, int level, int (*ST)[TotalTNum], int Tnum){
	int SPortID;
	int SwitchID;

	SwitchID=getSwitchID(StartSwitch);
	SPortID=getSwitchPortID(level,StartSwitch,EndSwitch);
	//ST[SwitchID][Tnum]=ST[SwitchID][Tnum]|(const_1<<SPortID);
	ST[SwitchID][Tnum]=SPortID+1;
	return 0;
}

int BuildSingleSPT(int *src, int **Tree, int level, int &head, int &tail,int (*ST)[TotalTNum], int Tnum){
	int tail2, head2, **Tree2=new int*[Smax];

	//AddEdges(src, Tree[head], level, TCluster, Tnum);

	for(int i=0; i<LMax; i++){
		int dim= (level+i) % LMax;
		tail2=0; head2=0;
		for(int k=head; k<tail; k++){
			int*A=NULL, *B=NULL, *C=NULL;
			copySwitch(C, Tree[k]);
			A=C;
			B=C;
			for(int j=0; j<S[dim]-1; j++){
				copySwitch(C,B);
				C[dim]=(C[dim]+1) % S[dim]; 
				Tree2[tail2++]=C;
				AddEdge(C, A, dim, ST, Tnum);
				B=C;
			}
			delete A;
		}
		for(int j=head2; j<tail2; j++)
			Tree[tail++]=Tree2[j];
	}
	delete[] Tree2;
	return 0;
}

int BuildMultipleSPTs(int *src, int (*ST)[TotalTNum], int STnum){
	int *root;
	int tail, head, **Tree=new int*[Smax];



	for(int i=0; i<LMax; i++){
		root=new int[LMax];
		for(int j=0; j<LMax; j++)
			root[j]=src[j];
		tail=0; head=0;
		Tree[tail++]=root;
		BuildSingleSPT(src, Tree, i, head, tail, ST, STnum+i);
		for(int j=head; j<tail; j++)
			delete[] Tree[j];
	}
	delete[] Tree;
	return 0;
}
int createSpanningTree(int (*ST)[TotalTNum]){

	for(int DesSwitch=0; DesSwitch<Smax; DesSwitch++){
		int src[LMax];
		int temp=DesSwitch;
		for(int i=0; i<LMax; i++){
			src[i]=temp % S[i];
			temp/=S[i];
		}
		BuildMultipleSPTs(src,ST,DesSwitch*SingleTNum);
	}

	return 0;
}

int SwitchOptimalEncoding(int s, int *FinalID, int (*ST)[TotalTNum]){
	int CurrentID;
	int *flag=new int[TotalTNum];
	memset(flag,0, sizeof(int)*TotalTNum);
	CurrentID=0;
	for(int i=0; i<TotalTNum; i++)
		if(flag[i]==0) {
			flag[i]=1;
			FinalID[i]=CurrentID++;
			for(int j=i+1; j<TotalTNum; j++)        
				if(ST[s][j]==ST[s][i]){
					flag[j]=1;
					FinalID[j]=CurrentID++;
				}
		} 
	delete[] flag;
	return 0;

}


int currentMS_init(int *currentMS, int *LocateTable, int (*ST)[TotalTNum]){
	for(int i=0; i<Smax; i++){
		int j, pre;
		pre=0;
		currentMS[i]=1;
		for(j=0; j<TotalTNum;j++)
			if(ST[i][ LocateTable[j] ]!=0){
				pre=ST[i][ LocateTable[j] ];
				break;
			}
		if(pre!=0)
			for( j++; j<TotalTNum; j++)
				if(ST[i][ LocateTable[j] ] != 0 && pre!=ST[i][ LocateTable[j] ]){
					currentMS[i]++;
					pre=ST[i][ LocateTable[j] ];
				}
	}
	return 0;

}
int LocateTable_init(int *LocateTable, int *FinalID){

	for(int tree=0; tree<TotalTNum; tree++)
		LocateTable[FinalID[tree]]=tree;
return 0;
}


int computeMinMax(int KeySwitch, int *FinalID,
					  int *LocateTable, int *currentMS, int (*ST)[TotalTNum]){
			int answer=0;

			SwitchOptimalEncoding(KeySwitch, FinalID, ST);
			LocateTable_init(LocateTable, FinalID);
		    currentMS_init(currentMS, LocateTable, ST);
			for(int i=0; i<Smax; i++)
				if(currentMS[i]>answer){
					answer=currentMS[i];
				}
			//cout<<"after conflict correcting_3: "<<answer<<endl;
			/*			int index;
			for(int i=0; i<TotalTNum; i++)
				cout<<ST[index][LocateTable[i]]<<" ";
			cout<<endl;*/ 

			return answer;


}
int CCencoding(int (*ST)[TotalTNum], ofstream &output){
	int *FinalID=new int[TotalTNum];
	int *LocateTable=new int[TotalTNum];
	int currentMS[Smax];
	int KeySwitch;
	

	for(int j=0; j<TotalTNum; j++)
		FinalID[j]=j;
	LocateTable_init(LocateTable,FinalID);
	currentMS_init(currentMS, LocateTable, ST);

	int answer=0;
	for(int i=0; i<Smax; i++)
		if(currentMS[i]>answer)
			answer=currentMS[i];
	//cout<<"before conflict correcting : "<<answer<<endl;

	KeySwitch=Smax/4;
	SwitchOptimalEncoding(KeySwitch,FinalID,ST);
	LocateTable_init(LocateTable,FinalID);
	currentMS_init(currentMS, LocateTable, ST);
	answer=0;
	for(int i=0; i<Smax; i++)
		if(currentMS[i]>answer){
			answer=currentMS[i];
				//cout<<"answer_i="<<i<<endl;
		}

			//cout<<"MinSwitch= "<<MinSwitch<<endl;
	//cout<<"after conflict correcting: "<<answer<<endl;
	output<<"the TotalTNum is: "<<TotalTNum<<endl;
	output<<"the routing table size is: "<<answer<<endl;

	//currentMS_init(FinalID,ST);
	//LocateTable_init(LocateTable,FinalID,ST);
	//currentMS_init(currentMS, LocateTable, ST);

	delete[] FinalID;
	delete[] LocateTable;
	return 0;

}
