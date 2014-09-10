#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <cmath>
#include<time.h>
#include <string.h>
using namespace std;
#include "FatTree_CC.h"


int createSpanningTree(int (*ST)[TotalTNum]);
int SwitchOptimalEncoding(int (*FinalID)[TotalTNum], int (*ST)[TotalTNum]);
int SGN(int a);
int currentMS_init(int *currentMS, int (*LocateTable)[TotalTNum], int (*ST)[TotalTNum]);
int LocateTable_init(int (*LocateTable)[TotalTNum], int (*FinalID)[TotalTNum], int (*ST)[TotalTNum]);
int computeMS(int SourceT,int FixedSwitch,int RefSwitch,int (*FinalID)[TotalTNum],
			  int (*LocateTable)[TotalTNum], int *currentMS, int (*ST)[TotalTNum]);
int ExchangeColumns(int SourceT,int FixedSwitch,int RefSwitch,int (*FinalID)[TotalTNum],
					int (*LocateTable)[TotalTNum], int *currentMS, int (*ST)[TotalTNum]);
int CCencoding(int (*ST)[TotalTNum]);


int main(){
	int (*ST)[TotalTNum]=new int[Smax][TotalTNum];
	clock_t start, finish;   
	double     duration; 
	ofstream output("FatTree_CC_Output.txt",ios::app);
	output<<setiosflags(ios::fixed)<<setprecision(6);

	output<<"podsize= "<<podsize<<endl; 

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


int createSpanningTree(int (*ST)[TotalTNum]){

	for(int destination=0; destination<EdgeSwitchNum;destination++){

		int StartAggrSwitch=EdgeSwitchNum+((int)(destination*2/podsize))*podsize/2;
		int EndAggrSwitch=StartAggrSwitch+podsize/2;

		for(int i=StartAggrSwitch; i<EndAggrSwitch; i++){
		   int AggreConnPort;
		   int StartTNum;
		   AggreConnPort=destination-((int)(destination*2/podsize))*podsize/2;
		   StartTNum=Tmax*destination+podsize/2*(i-StartAggrSwitch);
		  /*if(destination==0){
			   cout<<"StartAggrSwitch: "<<StartAggrSwitch<<endl;
			   cout<<"EndAggrSwitch: "<<EndAggrSwitch<<endl;
			   cout<<"StartTNum: "<<StartTNum<<endl;
		   }  */ 
		   for(int Tnumber=StartTNum; Tnumber<StartTNum+podsize/2; Tnumber++)
			   ST[i][Tnumber]=AggreConnPort+1;
			   //CreateNewSPNode(i, AggreConnPort, Tnumber, TCluster);

		   int StartCoreSwitch, EndCoreSwitch;
		   StartCoreSwitch=EdgeSwitchNum+AggreSwitchNum+(i-StartAggrSwitch)*podsize/2;
           EndCoreSwitch=StartCoreSwitch+podsize/2;

		   for(int j=StartCoreSwitch; j<EndCoreSwitch; j++){

			   int StartPod,  CurrentTNum, CoreConnPort;
			   StartPod=destination*2/podsize;
			   CurrentTNum= StartTNum+(j-StartCoreSwitch);
			   CoreConnPort= StartPod; 
               ST[j][CurrentTNum]=CoreConnPort+1;
			   //CreateNewSPNode(j, CoreConnPort, CurrentTNum, TCluster);

			   int DownConnSOrder, DownConnPOrder;

			   DownConnSOrder=(j-EdgeSwitchNum-AggreSwitchNum)*2/podsize;
               DownConnPOrder=podsize/2+(j-EdgeSwitchNum-AggreSwitchNum)%(podsize/2);


			   for(int k=EdgeSwitchNum+DownConnSOrder; k<EdgeSwitchNum+AggreSwitchNum; k=k+podsize/2)
			       if((k-EdgeSwitchNum)*2/podsize != StartPod) {
                       ST[k][CurrentTNum]=DownConnPOrder+1;
				      //CreateNewSPNode(k, DownConnPOrder, CurrentTNum, TCluster);

				   int StartX;
				   StartX=k-EdgeSwitchNum-DownConnSOrder;
				   for(int x=StartX; x<StartX+podsize/2; x++)
					   if(x!=destination)
                          ST[x][CurrentTNum]=DownConnSOrder+podsize/2+1;
				   		  //CreateNewSPNode(x, DownConnSOrder+podsize/2, CurrentTNum, TCluster);
			   }

		   }
	       for(int j=StartAggrSwitch-EdgeSwitchNum; j<EndAggrSwitch-EdgeSwitchNum; j++)
		       if(j!=destination){
				   for(int k=StartTNum; k<StartTNum+podsize/2; k++)
					   //CreateNewSPNode(j, i-StartAggrSwitch+podsize/2, k, TCluster);
					   ST[j][k]=i-StartAggrSwitch+podsize/2+1;
			   }

           

		}
	}

	return 0;
}

int SwitchOptimalEncoding(int (*FinalID)[TotalTNum], int (*ST)[TotalTNum]){
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
int currentMS_init(int *currentMS, int (*LocateTable)[TotalTNum], int (*ST)[TotalTNum]){
	for(int i=0; i<Smax; i++){
		int j, pre;
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
int LocateTable_init(int (*LocateTable)[TotalTNum], int (*FinalID)[TotalTNum], int (*ST)[TotalTNum]){

	for(int s=0; s<Smax; s++)
		for(int tree=0; tree<TotalTNum; tree++)
			LocateTable[s][FinalID[s][tree]]=tree;
return 0;
}
int computeMS(int SourceT,int FixedSwitch,int RefSwitch,int (*FinalID)[TotalTNum],
			  int (*LocateTable)[TotalTNum], int *currentMS, int (*ST)[TotalTNum]){
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
					int (*LocateTable)[TotalTNum], int *currentMS, int (*ST)[TotalTNum]){

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


int CCencoding(int (*ST)[TotalTNum]){
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

	int answer=0;
	for(int i=0; i<Smax; i++)
		if(currentMS[i]>answer)
			answer=currentMS[i];
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
	//cout<<"after conflict correcting: "<<answer<<endl;
	/*for(int i=0; i<Smax; i++){
		cout<<"Switch number "<<i<<": ";
		for(int j=0; j<TotalTNum; j++)
			cout<<FinalID[i][j]<<" ";
		cout<<endl;
	} 
	for(int i=0;i<TotalTNum; i++){
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
	}*/
	delete[] FinalID;
	delete[] LocateTable;
	return 0;
}
