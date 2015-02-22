#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <cmath>
#include<time.h>
#include <string.h>
using namespace std;
#include "Fat_Tree.h"


int createSpanningTree(int (*ST)[TotalTNum]);
int SwitchOptimalEncoding(int *FinalID, int (*ST)[TotalTNum]);
int currentMS_init(int *currentMS, int *LocateTable, int (*ST)[TotalTNum]);
int LocateTable_init(int *LocateTable, int *FinalID, int (*ST)[TotalTNum]);
int CCencoding(int (*ST)[TotalTNum], ofstream &output);


int main(){
	int (*ST)[TotalTNum]=new int[Smax][TotalTNum];
	clock_t start, finish;   
	double     duration; 
	ofstream output("FatTree_Output.txt",ios::app);
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

 /*for(int i=0; i<Smax; i++){
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

int printRange(int StartSNum, int EndSNum, int *currentMS){
	int min=MaxInt;

	int max=0;
	for(int i=StartSNum; i<EndSNum; i++){
		if(currentMS[i]<min)
			min=currentMS[i];
		if(currentMS[i]>max)
			max=currentMS[i];
	}
	cout<<"min = "<<min<<" max = "<<max<<endl;

	return 0;
}
int computeMinMax(int KeySwitch, int *FinalID,
					  int *LocateTable, int *currentMS, int (*ST)[TotalTNum]){
			int answer=0;

			SwitchOptimalEncoding(KeySwitch, FinalID, ST);
			LocateTable_init(LocateTable, FinalID);
		    currentMS_init(currentMS, LocateTable, ST);

			for(int i=0; i<Smax; i++){
				//cout<<"i = "<<currentMS[i]<<endl;
				if(currentMS[i]>answer){
					answer=currentMS[i];
				}
			}

			/*for(int i=0; i<Smax; i++){
				cout<<"Switch number "<<i<<": "<<currentMS[i]<<endl;
				for(int j=0; j<TotalTNum; j++)
					cout<<ST[i][ LocateTable[j] ]<<" ";
				cout<<endl;
			}*/ 
			//cout<<"after conflict correcting_3: "<<answer<<endl;
			/*			int index;
			for(int i=0; i<TotalTNum; i++)
				cout<<ST[index][LocateTable[i]]<<" ";
			cout<<endl;*/ 
			//cout<<endl;

			return answer;


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
	//cout<<"before conflict correcting_3: "<<answer<<endl;
	/*for(int i=0; i<Smax; i++){
		cout<<"Switch number "<<i<<": "<<currentMS[i]<<endl;
		for(int j=0; j<TotalTNum; j++)
			cout<<ST[i][ LocateTable[j] ]<<" ";
		cout<<endl;
	}  
    cout<<endl;*/

  /* optimal encoding */
  int curID=0;
  for(int t=0; t<Tmax; t=t++) // enumerate IDs of trees dested for each edgeswitch
    for (int i=0; i<EdgeSwitchNum; i++) //enumerate edgeswitch
      FinalID[i*Tmax+t]=curID++;
  LocateTable_init(LocateTable, FinalID);
  currentMS_init(currentMS, LocateTable, ST);
   
  answer=0;
  for(int i=0; i<Smax; i++){
    //cout<<"i = "<<currentMS[i]<<endl;
    if(currentMS[i]>answer){
      answer=currentMS[i];
    }
  }
if(answer<FinalAnswer){
		FinalAnswer=answer;
		printRange(0, EdgeSwitchNum, currentMS);
		printRange(EdgeSwitchNum, EdgeSwitchNum+AggreSwitchNum, currentMS);
		printRange(EdgeSwitchNum+AggreSwitchNum, Smax, currentMS);
    //for (int i=0; i<TotalTNum; i++)
    //  cout<<FinalID[i]<<endl;
    for(int sw=10; sw<11; sw++){
      cout<<"switch "<<sw<<endl;
      for(int id=0; id<TotalTNum; id++)
        cout<<" "<<id<<" "<<ST[sw][LocateTable[id]]<<endl;
    }
	}

/* end of optimal encoding */ 
	KeySwitch=0;
	temp=computeMinMax(KeySwitch, FinalID, LocateTable, currentMS, ST);
	if(temp<FinalAnswer){
		FinalAnswer=temp;
		printRange(0, EdgeSwitchNum, currentMS);
		printRange(EdgeSwitchNum, EdgeSwitchNum+AggreSwitchNum, currentMS);
		printRange(EdgeSwitchNum+AggreSwitchNum, Smax, currentMS);
    for (int i=0; i<TotalTNum; i++)
      cout<<" "<<FinalID[i]<<endl;
	}
	cout<<( TotalTNum*(EdgeSwitchNum-1) )<<" "<<TotalTNum<<" "<<temp;

	KeySwitch=EdgeSwitchNum;
	temp=computeMinMax(KeySwitch, FinalID, LocateTable, currentMS, ST);
	if(temp<FinalAnswer){
		FinalAnswer=temp;
		printRange(0, EdgeSwitchNum, currentMS);
		printRange(EdgeSwitchNum, EdgeSwitchNum+AggreSwitchNum, currentMS);
		printRange(EdgeSwitchNum+AggreSwitchNum, Smax, currentMS);
	}

	KeySwitch=EdgeSwitchNum+AggreSwitchNum;
	temp=computeMinMax(KeySwitch, FinalID, LocateTable, currentMS, ST);
	if(temp<FinalAnswer){
		FinalAnswer=temp;
		printRange(0, EdgeSwitchNum, currentMS);
		printRange(EdgeSwitchNum, EdgeSwitchNum+AggreSwitchNum, currentMS);
		printRange(EdgeSwitchNum+AggreSwitchNum, Smax, currentMS);
	}

	//output<<"the TotalTNum is: "<<TotalTNum<<endl;
	//output<<"the routing table size is: "<<FinalAnswer<<endl;

	//currentMS_init(FinalID,ST);
	//LocateTable_init(LocateTable,FinalID,ST);
	//currentMS_init(currentMS, LocateTable, ST);

	delete[] FinalID;
	delete[] LocateTable;
	return 0;

}
