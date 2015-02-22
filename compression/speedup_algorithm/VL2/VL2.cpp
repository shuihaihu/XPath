#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <cmath>
#include<time.h>
#include <string>
using namespace std;
#include "VL2.h"


int createSpanningTree(int (*ST)[TotalTNum]);
int SwitchOptimalEncoding(int *FinalID, int (*ST)[TotalTNum]);
int currentMS_init(int *currentMS, int *LocateTable, int (*ST)[TotalTNum]);
int LocateTable_init(int *LocateTable, int *FinalID, int (*ST)[TotalTNum]);
int CCencoding(int (*ST)[TotalTNum], ofstream &output);


int main(){
	int (*ST)[TotalTNum]=new int[Smax][TotalTNum];
	clock_t start, finish;   
	double     duration; 
	ofstream output("VL2_Output.txt",ios::app);
	output<<setiosflags(ios::fixed)<<setprecision(6);

	output<<"DA= "<<DA<<" DI= "<<DI<<endl; 

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


int createSpanningTree(int (*ST)[TotalTNum]){

	for(int DesTorSwitch=0; DesTorSwitch<TorSNum; DesTorSwitch++)
		for(int IntSwitch=0; IntSwitch<IntSNum; IntSwitch++){
			int TNum=DesTorSwitch*SingleTNum+IntSwitch*2;
			for(int num=0; num<2; num++){
				int CurrentTnum=TNum+num;
				int DesAggrSwitch=0;
				for(int AggrSwitch=0; AggrSwitch<AggrSNum; AggrSwitch+=2){
					for(int AggrPort=0; AggrPort<DA/2; AggrPort++){
						int CurrentTor=AggrSwitch/2*DA/2+AggrPort;
						if(CurrentTor!=DesTorSwitch){
							if( num==0 )
								//ST[CurrentTor][CurrentTnum]=ST[CurrentTor][CurrentTnum]|const_1;
								ST[CurrentTor][CurrentTnum]=1;
							else
								//ST[CurrentTor][CurrentTnum]=ST[CurrentTor][CurrentTnum]|(const_1<<1);
								ST[CurrentTor][CurrentTnum]=2;
						}else{
							//ST[TorSNum+AggrSwitch+num][CurrentTnum]=ST[TorSNum+AggrSwitch+num][CurrentTnum]|(const_1<<AggrPort);
							ST[TorSNum+AggrSwitch+num][CurrentTnum]=AggrPort+1;
							DesAggrSwitch=TorSNum+AggrSwitch+num;
						}
					}     
				}
				//ST[IntSwitch+TorSNum+AggrSNum][CurrentTnum]=ST[IntSwitch+TorSNum+AggrSNum][CurrentTnum]|( const_1<<(DesAggrSwitch-TorSNum) );
				ST[IntSwitch+TorSNum+AggrSNum][CurrentTnum]=DesAggrSwitch-TorSNum+1;
				for(int i=TorSNum+num; i<TorSNum+AggrSNum; i=i+2)
					if(i!=DesAggrSwitch)
						//ST[i][CurrentTnum]=ST[i][CurrentTnum]|( const_1<<(IntSwitch+DA/2) );
						ST[i][CurrentTnum]=IntSwitch+DA/2+1;
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

	KeySwitch=0;
	temp=computeMinMax(KeySwitch, FinalID, LocateTable, currentMS, ST);
	if(temp<FinalAnswer)
		FinalAnswer=temp;

	KeySwitch=TorSNum;
	temp=computeMinMax(KeySwitch, FinalID, LocateTable, currentMS, ST);
	if(temp<FinalAnswer)
		FinalAnswer=temp;

	KeySwitch=TorSNum+AggrSNum;
	temp=computeMinMax(KeySwitch, FinalID, LocateTable, currentMS, ST);
	if(temp<FinalAnswer)
		FinalAnswer=temp;

	output<<"the routing table size is: "<<FinalAnswer<<endl;
	//currentMS_init(FinalID,ST);
	//LocateTable_init(LocateTable,FinalID,ST);
	//currentMS_init(currentMS, LocateTable, ST);

	delete[] FinalID;
	delete[] LocateTable;
	return 0;

}
