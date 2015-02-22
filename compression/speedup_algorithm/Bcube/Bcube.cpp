#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <cmath>
#include<time.h>
#include <string.h>
using namespace std;
#include "Bcube.h"

int copyServer(int *&A, int *B);
int getSwitchID(int *connServer, int level);
inline int getSwitchPortID(int *connServer, int level);
int getServerID(int *connServer);
inline int getServerPortID(int level);
bool equalServer(int *A, int *B);
int AddEdges(int *StartServer, int *EndServer, int level, int (*ST)[TotalTNum], int Tnum);
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
	ofstream output("Bcube_Output.txt",ios::app);
	output<<setiosflags(ios::fixed)<<setprecision(6);

	output<<"LevelNum= "<<LevelNum<<" N= "<<N<<endl; 

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


int copyServer(int *&A, int *B){
	A=new int[LevelNum];
	for(int i=0; i<LevelNum; i++)
		A[i]=B[i];
	return 0;
}

int getSwitchID(int *connServer, int level){
    int temp1=0;
    int temp2=serverNum;
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

	temp2+=singleLSNum*level;

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

bool equalServer(int *A, int *B){

	for(int i=0; i<LevelNum; i++)
		if(A[i]!=B[i])
			return false;
	return true;
}
int AddEdges(int *StartServer, int *EndServer, int level, int (*ST)[TotalTNum], int Tnum){
	int SPort;
	int ServerE;

	ServerE=getServerID(EndServer);
	SPort=getServerPortID(level);
	//CreateNewSPNode(ServerE, SPort, Tnum, TCluster);
	ST[ServerE][Tnum]=SPort+1;


    return 0;
}

int BuildSingleSPT(int *src, int **Tree, int level, int &head, int &tail,int (*ST)[TotalTNum], int Tnum){
	int tail2, head2, **Tree2=new int*[serverNum];

	//AddEdges(src, Tree[head], level, TCluster, Tnum);

	for(int i=0; i<LevelNum; i++){
		int dim= (level+i) % LevelNum;
        tail2=0; head2=0;
		for(int k=head; k<tail; k++){
			int*A=NULL, *B=NULL, *C=NULL;
			copyServer(C, Tree[k]);
            A=C;
			B=C;
			int Switch, SPort;
			Switch=getSwitchID(A, dim);
			SPort=getSwitchPortID(A, dim);
			//CreateNewSPNode(Switch, SPort, Tnum, TCluster);
            ST[Switch][Tnum]=SPort+1;
			for(int j=0; j<N-1; j++){
				copyServer(C,B);
				C[dim]=(C[dim]+1) % N; 
				//if( equalServer(C,src) ) 
				//	break;
				Tree2[tail2++]=C;
				AddEdges(A, C, dim, ST, Tnum);
				B=C;
			}
		}
		for(int j=head2; j<tail2; j++)
			Tree[tail++]=Tree2[j];
	}
	delete[] Tree2;

	/*for(int i=0; i<serverNum; i++){
		int server[LevelNum];
		int temp=i;
		for(int j=0; j<LevelNum; j++){
			server[j]=temp % N;
			temp/=N;
		}
		int *A=NULL;
		if(!equalServer(src,server) && server[level]==src[level]){
			copyServer(A,server);
			A[level]=(A[level]-1+N) % N;
			AddEdges(A, server, level, TCluster, Tnum);
		}
		
	} */ 
	return 0;
}

int BuildMultipleSPTs(int *src, int (*ST)[TotalTNum], int STnum){
	int *root;
	int tail, head, **Tree=new int*[serverNum];


	for(int i=0; i<LevelNum; i++){
		root=new int[LevelNum];
		for(int j=0; j<LevelNum; j++)
			root[j]=src[j];
		//root[i]=(src[i]+1) % N;
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

	for(int server=0; server<serverNum; server++){
		int src[LevelNum];
		int temp=server;
		for(int i=0; i<LevelNum; i++){
            src[i]=temp % N;
			temp/=N;
		}
		BuildMultipleSPTs(src, ST, server*SingleSPT);
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

int printRange(int StartSNum, int EndSNum, int *currentMS){
  int min=MaxInt;
  
  int max=0;
  for(int i=StartSNum; i<EndSNum; i++){
     if(currentMS[i]<min)
       min=currentMS[i];
     if(currentMS[i]>max)
       max=currentMS[i];
  }
  cout<<"min= "<<min<<" max= "<<max<<endl;

  return 0;
}

int CCencoding(int (*ST)[TotalTNum], ofstream &output){
	int *FinalID=new int[TotalTNum];
	int *LocateTable=new int[TotalTNum];
	int currentMS[Smax];
	int KeySwitch;
	int FinalAnswer=MaxInt;
	int temp;

	for(int j=0; j<TotalTNum; j++)
		FinalID[j]=j;
	LocateTable_init(LocateTable,FinalID);
	currentMS_init(currentMS, LocateTable, ST);

	int answer=0;
	for(int i=0; i<Smax; i++)
		if(currentMS[i]>answer)
			answer=currentMS[i];
	//cout<<"before conflict correcting: "<<answer<<endl;

/* optimal encoding 
  int curID=0;
  //for(int t=0; t<SingleSPT; t=t++)
  //  for (int i=0; i<serverNum; i++)
  //    FinalID[i*SingleSPT+t]=curID++;
  for (int i=0; i<serverNum; i++) 
    for(int t=0; t<SingleSPT; t++)
      FinalID[i*SingleSPT+t]=curID++; 
  LocateTable_init(LocateTable, FinalID);
  currentMS_init(currentMS, LocateTable, ST);

  answer=0;
  for(int i=0; i<Smax; i++){
    if(currentMS[i]>answer)
      answer=currentMS[i];
  }

  if(answer<FinalAnswer){
    FinalAnswer=answer;
    printRange(0, serverNum, currentMS);
    printRange(serverNum, serverNum+singleLSNum, currentMS); 
    printRange(serverNum+singleLSNum, Smax, currentMS);
    for (int i=0; i<TotalTNum; i++)
      cout<<FinalID[i]<<endl;
    for(int sw=0; sw<1; sw++){
      cout<<"switch "<<sw<<endl;
      for(int id=0; id<TotalTNum; id++)
        cout<<" "<<id<<" "<<ST[sw][id]<<endl;
    }
  }
end of optimal encoding*/ 

	KeySwitch=0;
	temp=computeMinMax(KeySwitch, FinalID, LocateTable, currentMS, ST);
  if(temp<FinalAnswer){
    FinalAnswer=temp;
    printRange(0, serverNum, currentMS);
    printRange(serverNum, serverNum+singleLSNum, currentMS);
    printRange(serverNum+singleLSNum, Smax, currentMS);
    for (int i=0; i<TotalTNum; i++)
      cout<<FinalID[i]<<endl;
    for(int sw=0; sw<16; sw++){
      cout<<"switch "<<sw<<endl;
      for(int id=0; id<TotalTNum; id++)
        cout<<" "<<id<<" "<<ST[sw][LocateTable[id]]<<endl;
    }

  }

	for(int i=0; i<LevelNum; i++){
		KeySwitch=serverNum+singleLSNum*i;
		temp=computeMinMax(KeySwitch, FinalID, LocateTable, currentMS, ST);
		if(temp<FinalAnswer)
			FinalAnswer=temp;
	}

	output<<"the routing table size is: "<<FinalAnswer<<endl;
	//currentMS_init(FinalID,ST);
	//LocateTable_init(LocateTable,FinalID,ST);
	//currentMS_init(currentMS, LocateTable, ST);

	delete[] FinalID;
	delete[] LocateTable;
	return 0;

}
