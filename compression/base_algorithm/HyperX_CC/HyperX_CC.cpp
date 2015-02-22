#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <cmath>
#include<time.h>
#include <string>
using namespace std;
#include "HyperX_CC.h"

int copySwitch(int *&A, int *B);
int getSwitchID(int *Switch);
int getSwitchPortID(int level, int *StartSwitch, int *EndSwitch);
int AddEdge(int *StartSwitch, int *EndSwitch, int level, int (*ST)[TotalTNum], int Tnum);
int BuildSingleSPT(int *src, int **Tree, int level, int &head, int &tail,int (*ST)[TotalTNum], int Tnum);
int BuildMultipleSPTs(int *src, int (*ST)[TotalTNum], int STnum);
int createSpanningTree(int (*ST)[TotalTNum]);
int SwitchOptimalEncoding(int (*FinalID)[TotalTNum], int(*ST)[TotalTNum]);
int SGN(int a);
int currentMS_init(int *currentMS, int (*LocateTable)[TotalTNum], int(*ST)[TotalTNum]);
int LocateTable_init(int (*LocateTable)[TotalTNum], int (*FinalID)[TotalTNum], int(*ST)[TotalTNum]);
int computeMS(int SourceT,int FixedSwitch,int RefSwitch,int (*FinalID)[TotalTNum],
			  int (*LocateTable)[TotalTNum], int *currentMS, int(*ST)[TotalTNum]);
int ExchangeColumns(int SourceT,int FixedSwitch,int RefSwitch,int (*FinalID)[TotalTNum],
					int (*LocateTable)[TotalTNum], int *currentMS, int(*ST)[TotalTNum]);
int CCencoding(int(*ST)[TotalTNum]);


int main(){
	int (*ST)[TotalTNum]=new int[Smax][TotalTNum];
	clock_t start, finish;   
	double     duration; 
	ofstream output("HyperX_CC_Output.txt",ios::app);
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
	CCencoding(ST);
	finish=clock();   
	duration=(double)(finish-start)/CLOCKS_PER_SEC; 
	//cout<<"The running time of creating spanning trees is: "<<duration<<endl; 
	output<<"The running time of computing routing table size is: "<<duration<<endl; 

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

int SwitchOptimalEncoding(int (*FinalID)[TotalTNum], int(*ST)[TotalTNum]){
	int CurrentID;
	int *flag=new int[TotalTNum];
	for(int s=0; s<Smax; s++){
		memset(flag,0, sizeof(int)*TotalTNum);
		CurrentID=0;
		for(int i=0; i<TotalTNum; i++)
			if(flag[i]==0) {
				flag[i]=1;
				FinalID[s][i]=CurrentID++;
				for(int j=i+1; j<TotalTNum; j++)        
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
int currentMS_init(int *currentMS, int (*LocateTable)[TotalTNum], int(*ST)[TotalTNum]){
	for(int i=0; i<Smax; i++){
		int j; 
		int	pre;
		pre=0;
		currentMS[i]=1;
		for(j=0; j<TotalTNum;j++)
			if(ST[i][ LocateTable[i][j] ]!=0){
				pre=ST[i][ LocateTable[i][j] ];
				break;
			}
			if(pre!=0)
				for( j++; j<TotalTNum; j++)
					if(ST[i][ LocateTable[i][j] ] != 0){
						currentMS[i]+=SGN(pre^ST[i][ LocateTable[i][j] ]);
						pre=ST[i][ LocateTable[i][j] ];
					}
	}
	return 0;

}
int LocateTable_init(int (*LocateTable)[TotalTNum], int (*FinalID)[TotalTNum], int(*ST)[TotalTNum]){

	for(int s=0; s<Smax; s++)
		for(int tree=0; tree<TotalTNum; tree++)
			LocateTable[s][FinalID[s][tree]]=tree;
return 0;
}
int computeMS(int SourceT,int FixedSwitch,int RefSwitch,int (*FinalID)[TotalTNum],
			  int (*LocateTable)[TotalTNum], int *currentMS, int(*ST)[TotalTNum]){
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
				  while(tempSourceID<TotalTNum-1 && tempSourceID!=DesID){
					  SucSourceT=LocateTable[RefSwitch][++tempSourceID];
					  if(ST[RefSwitch][SucSourceT]!=0) break;
						}

				  while(tempDesID>0 && tempDesID!=SourceID){
					  PreDesT=LocateTable[RefSwitch][--tempDesID];
					  if(ST[RefSwitch][PreDesT]!=0) break;
						}
				  tempDesID=DesID;
				  while(tempDesID<TotalTNum-1 && tempDesID!=SourceID){
					  SucDesT=LocateTable[RefSwitch][++tempDesID];
					  if(ST[RefSwitch][SucDesT]!=0) break;
						}

						/*if(SourceID!=0) PreSourceT=LocateTable[RefSwitch][SourceID-1];
						if(SourceID!=(TotalTNum-1)) SucSourceT=LocateTable[RefSwitch][SourceID+1];

						if(DesID!=0) PreDesT=LocateTable[RefSwitch][DesID-1];
						if(DesID!=(TotalTNum-1)) SucDesT=LocateTable[RefSwitch][DesID+1]; */ 

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


							/*if(MinID!=0) PreMinT=LocateTable[RefSwitch][MinID-1];
							if(MaxID!=(TotalTNum-1)) SucMaxT=LocateTable[RefSwitch][MaxID+1]; */ 

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

int ExchangeColumns(int SourceT,int FixedSwitch,int RefSwitch,int (*FinalID)[TotalTNum],
					int (*LocateTable)[TotalTNum], int *currentMS, int(*ST)[TotalTNum]){

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
						while(tempSourceID<TotalTNum-1 && tempSourceID!=DesID){
							SucSourceT=LocateTable[RefSwitch][++tempSourceID];
							if(ST[RefSwitch][SucSourceT]!=0) break;
						}

						while(tempDesID>0 && tempDesID!=SourceID){
							PreDesT=LocateTable[RefSwitch][--tempDesID];
							if(ST[RefSwitch][PreDesT]!=0) break;
						}
						tempDesID=DesID;
						while(tempDesID<TotalTNum-1 && tempDesID!=SourceID){
							SucDesT=LocateTable[RefSwitch][++tempDesID];
							if(ST[RefSwitch][SucDesT]!=0) break;
						}

						/*if(SourceID!=0) PreSourceT=LocateTable[RefSwitch][SourceID-1];
						if(SourceID!=(TotalTNum-1)) SucSourceT=LocateTable[RefSwitch][SourceID+1];

						if(DesID!=0) PreDesT=LocateTable[RefSwitch][DesID-1];
						if(DesID!=(TotalTNum-1)) SucDesT=LocateTable[RefSwitch][DesID+1]; */ 

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
							if(MaxID!=(TotalTNum-1)) SucMaxT=LocateTable[RefSwitch][MaxID+1]; */ 

							if(PreMinT!=-1 && ST[RefSwitch][PreMinT]!=0 && ST[RefSwitch][MinT]!=0)
								OriScore+=SGN(ST[RefSwitch][PreMinT]^ST[RefSwitch][MinT]);
							if(SucMaxT!=-1 && ST[RefSwitch][SucMaxT]!=0 && ST[RefSwitch][MaxT]!=0)
								OriScore+=SGN(ST[RefSwitch][SucMaxT]^ST[RefSwitch][MaxT]);

							if(PreMinT!=-1 && ST[RefSwitch][PreMinT]!=0 && ST[RefSwitch][MaxT]!=0)
								ExchangeScore+=SGN(ST[RefSwitch][PreMinT]^ST[RefSwitch][MaxT]);
							if(SucMaxT!=-1 && ST[RefSwitch][SucMaxT]!=0 && ST[RefSwitch][MinT]!=0)
								ExchangeScore+=SGN(ST[RefSwitch][SucMaxT]^ST[RefSwitch][MinT]);

							/*if(RefSwitch==9){
								cout<<"Switch number "<<9<<": ";
								for(int j=0; j<TotalTNum; j++)
									cout<<ST[9][LocateTable[9][j]]<<" ";
								//cout<<endl;
								cout<<"SourceID="<<SourceID<<"   DesID="<<DesID<<endl;
								cout<<"currentMS[RefSwitch]="<<currentMS[RefSwitch]<<endl;
								cout<<"OriScore="<<OriScore<<"   ExchangeScore="<<ExchangeScore<<endl<<endl;
							} */ 

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

						/*if(RefSwitch==9){
							cout<<"Switch number "<<9<<": ";
							for(int j=0; j<TotalTNum; j++)
								cout<<ST[9][LocateTable[9][j]]<<" ";
							//cout<<endl;
							cout<<"SourceID="<<SourceID<<"   DesID="<<DesID<<endl;
							cout<<"currentMS[RefSwitch]="<<currentMS[RefSwitch]<<endl;
							cout<<"OriScore="<<OriScore<<"   ExchangeScore="<<ExchangeScore<<endl<<endl;
						}   */ 


						currentMS[RefSwitch]+=ExchangeScore-OriScore;

						LocateTable[RefSwitch][DesID]=SourceT;
						LocateTable[RefSwitch][SourceID]=DesT;

						FinalID[RefSwitch][SourceT]=DesID;
						FinalID[RefSwitch][DesT]=SourceID;


						return 0;

}


int CCencoding(int(*ST)[TotalTNum]){
	//int FinalID[Smax][TotalTNum]=new int[Smax][TotalTNum];
	int (*FinalID)[TotalTNum]=new int[Smax][TotalTNum] ;
	int currentMS[Smax];
	int MinSwitch;
	int MinMS;
	int (*LocateTable)[TotalTNum]=new int[Smax][TotalTNum];
	//int LocateTable[Smax][TotalTNum];

	for(int i=0; i<Smax; i++)
		for(int j=0; j<TotalTNum; j++)
			FinalID[i][j]=j;
	LocateTable_init(LocateTable,FinalID,ST);
	currentMS_init(currentMS, LocateTable, ST);

	/*for(int i=0; i<Smax; i++){
		cout<<"Switch number "<<i<<": ";
		for(int j=0; j<TotalTNum; j++)
			cout<<ST[i][LocateTable[i][j]]<<" ";
		cout<<endl;
	}*/ 

	int answer=0;
	for(int i=0; i<Smax; i++)
		if(currentMS[i]>answer){
			answer=currentMS[i];
			//cout<<"i="<<i<<endl;
			//cout<<"current["<<i<<"]="<<currentMS[i]<<endl;
		}
	//cout<<"before conflict correcting: "<<answer<<endl;

    SwitchOptimalEncoding(FinalID,ST);
	LocateTable_init(LocateTable,FinalID,ST);
    currentMS_init(currentMS, LocateTable, ST);

	

	for(int tree=0; tree<TotalTNum; tree++){
        MinMS=MaxInt;
		int currentMaxMS;
		for(int i=0; i<Smax; i++){
            currentMaxMS=0;

			for(int j=0; j<Smax; j++){
				int temp;
				temp=computeMS(tree,i,j,FinalID,LocateTable,currentMS,ST);
				if(temp>currentMaxMS)
					currentMaxMS=temp;
				/*if(tree==10 && i==0){
					cout<<"tree: "<<tree<<"  i: "<<i<<"  j: "<<j<<endl;
					cout<<"FinalID[i][tree]: "<<FinalID[i][tree]<<" FinalID[j][tree]: "<<FinalID[j][tree]<<endl;
					for(int m=0; m<TotalTNum; m++)
						cout<<ST[j][ LocateTable[j][m] ]<<" ";
					cout<<endl<<"temp: "<<temp<<endl;
					cout<<"currentMS[j]:"<<currentMS[j]<<endl;
				}  */ 
			}
			if(currentMaxMS<MinMS){
				MinMS=currentMaxMS;
				MinSwitch=i;
			}
		}
		/* if(tree==4 )
		    cout<<"MinSwitch: "<<MinSwitch<<endl;
        cout<<"MinMS: "<<MinMS<<endl;*/
		for(int i=0; i<Smax; i++)
			if(i != MinSwitch)
		      ExchangeColumns(tree,MinSwitch,i,FinalID,LocateTable,currentMS,ST);
	}

	/*for(int i=0; i<Smax; i++){
		cout<<"Switch number "<<i<<": ";
		for(int j=0; j<TotalTNum; j++)
			cout<<ST[i][LocateTable[i][j]]<<" ";
		cout<<endl;
	}  */ 
	//currentMS_init(currentMS, LocateTable, ST);
	answer=0;
	for(int i=0; i<Smax; i++)
		if(currentMS[i]>answer){
			answer=currentMS[i];
		    //cout<<"i="<<i<<endl;
			//cout<<"current["<<i<<"]="<<currentMS[i]<<endl;
		}
	cout<<"after conflict correcting: "<<answer<<endl;
	/*for(int i=0; i<Smax; i++){
		cout<<"Switch number "<<i<<": ";
		for(int j=0; j<TotalTNum; j++)
			cout<<FinalID[i][j]<<" ";
		cout<<endl;
	} */ 
	/*for(int i=0;i<TotalTNum; i++){
		int flag=0;
		int initval=FinalID[0][i];
		for(int j=1; j<Smax; j++)
			if(FinalID[j][i]!=initval){
				flag=1;
				cout<<"[j][i]: "<<j<<"  "<<i<<endl;
				break;
			}
			if(flag){
				cout<<"wrong1"<<endl;
				break;
			}

	}
	for(int i=0;i<TotalTNum; i++){
		int flag=0;
		for(int j=0; j<TotalTNum;j++)
			if(i!=j)
				if(FinalID[0][i]==FinalID[0][j]){
					flag=1;
					break;
				}
				if(flag){
					cout<<"wrong2"<<endl;
					break;
				}
	} */ 
	delete[] FinalID;
	delete[] LocateTable;
	return 0;
}
