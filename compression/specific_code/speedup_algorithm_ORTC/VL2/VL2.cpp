#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <cmath>
#include<time.h>
#include <string.h>
using namespace std;
#include "VL2.h"


int createSpanningTree(int (*ST)[TotalTNum]);
int SwitchOptimalEncoding(int *FinalID, int (*ST)[TotalTNum]);
int currentMS_init(int *currentMS, int *LocateTable, int (*ST)[TotalTNum]);
int LocateTable_init(int *LocateTable, int *FinalID, int (*ST)[TotalTNum]);
int CCencoding_ORTC(int (*ST)[TotalTNum], ofstream &output);


int main(){
	int (*ST)[TotalTNum]=new int[Smax][TotalTNum];
	clock_t start, finish;   
	double  duration; 
	ofstream output("Output_VL2.txt",ios::app);
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
	CCencoding_ORTC(ST,output);
	finish=clock();   
	duration=(double)(finish-start)/CLOCKS_PER_SEC; 
	output<<"The running time of compute maxORT is: "<<duration<<endl; 

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

class ORTC_node{
public:
	int portmap[Pmax+1];
	int routemap[Pmax+1];
	int prefix_enable;
};

int local_OptIP(int s, int * flag, int *IP, int (*ST)[TotalTNum], int maxIP){
	int blocksz[Pmax+1]={0};
	int porder[Pmax+1]={0};

	memset(flag, 0, sizeof(int)*TotalTNum);

	for(int i=0; i<TotalTNum; i++) //compute block size;
		if(flag[i]==0) {
			flag[i]=1;
			blocksz[ST[s][i]]=1;
			for(int j=i+1; j<TotalTNum; j++)        
				if(ST[s][j]==ST[s][i]){
					flag[j]=1;
					blocksz[ST[s][i]]++;
				}
		}

		//  for(int i=0; i<=Pmax; i++)
		//    cout<<" blocksz["<<i<<"]= "<<blocksz[i];
		//  cout<<endl;

		int sorted[Pmax+1];
		memset(sorted, 0, sizeof(int)*(Pmax+1) );
		for(int round=0; round<=Pmax; round++){
			int maxbsz=0;
			int maxp;
			for(int p=0; p<=Pmax; p++) 
				if(sorted[p]==0 && blocksz[p]>maxbsz){
					maxbsz=blocksz[p];
					maxp=p;
				}
				if(maxbsz>0){
					porder[round]=maxp;
					sorted[maxp]=1;
				} else
					porder[round]=-1;
		}
		//for(int i=0; i<=Pmax; i++)
		//  cout<<" porder["<<i<<"]= "<<porder[i];
		//cout<<endl;

		int b_startIP=0;
		int curIP=b_startIP;
		for(int round=0; round<=Pmax; round++)
			if( porder[round]>=0 ){
				int curport=porder[round];
				//	cout<<" curport "<<curport<<endl;
				for(int ps=0; ps<TotalTNum; ps++)
					if(ST[s][ps]==curport)
						IP[ps]=curIP++;
				if(curIP>maxIP)
					cout<<"error1"<<endl;
				for(int i=0; i<IPv4len; i++)
					if(IPsegsz[i]>=blocksz[curport]){
						b_startIP+=IPsegsz[i];
						curIP=b_startIP;
						break;
					}
			}
			return 0;
}

int compute_ORT(int s, int *IP, int ORTC_nnum, int ORTC_leafnnum,
				ORTC_node *ORTC_tree, int (*ST)[TotalTNum]){
					int leaf_startn=ORTC_nnum-ORTC_leafnnum;
					for(int node=0; node<ORTC_nnum; node++){
						memset(ORTC_tree[node].portmap, 0, sizeof(int)*(Pmax+1) );
						memset(ORTC_tree[node].routemap, 0, sizeof(int)*(Pmax+1) );
					}
					for(int node=leaf_startn; node<ORTC_nnum; node++)
						ORTC_tree[node].portmap[0]=1;
					for(int ps=0; ps<TotalTNum; ps++){
						int ip=IP[ps];
						if( ip<0 || (leaf_startn+ip)>=ORTC_nnum)
							cout<<"error2"<<endl;
						ORTC_tree[leaf_startn+ip].portmap[0]=0;
						ORTC_tree[leaf_startn+ip].portmap[ST[s][ps]]=1;
					}

					for(int node=leaf_startn-1; node>=0; node--){
						int lch=2*node+1;
						int rch=2*node+2;
						if( (rch)>ORTC_nnum)
							cout<<"error3"<<endl;
						int intersec=0;
						for(int bit=0; bit<=Pmax; bit++)
							if( (ORTC_tree[lch].portmap[bit] & ORTC_tree[rch].portmap[bit])==1 ){
								intersec=1;
								break;
							}
							if(intersec==0){
								for(int bit=0; bit<=Pmax; bit++)
									ORTC_tree[node].portmap[bit]= ORTC_tree[lch].portmap[bit] | ORTC_tree[rch].portmap[bit];
							}else{
								for(int bit=0; bit<=Pmax; bit++)
									ORTC_tree[node].portmap[bit]= ORTC_tree[lch].portmap[bit] & ORTC_tree[rch].portmap[bit];
							}
					}

					ORTC_tree[0].prefix_enable=0;
					for(int bit=0; bit<=Pmax; bit++)
						if(ORTC_tree[0].portmap[bit]==1){
							ORTC_tree[0].routemap[bit]=1;
							if(bit!=0)
								ORTC_tree[0].prefix_enable=1;
							break;
						}
						for(int node=1; node<ORTC_nnum; node++){
							int parent=(node-1)/2;
							int hasdupp=0;
							for(int bit=0; bit<=Pmax; bit++)
								if( (ORTC_tree[node].portmap[bit] & ORTC_tree[parent].routemap[bit])==1 ){
									hasdupp=1;
									break;
								}
								if(hasdupp==1){
									ORTC_tree[node].prefix_enable=0;
									for(int bit=0; bit<=Pmax; bit++)
										ORTC_tree[node].routemap[bit]=ORTC_tree[parent].routemap[bit];
								}else{
									for(int bit=0; bit<=Pmax; bit++)
										if(ORTC_tree[node].portmap[bit]==1){
											ORTC_tree[node].routemap[bit]=1;
											if(bit!=0)
												ORTC_tree[node].prefix_enable=1;
											else ORTC_tree[node].prefix_enable=0;
											break;
										}

										for(int bit=0; bit<=Pmax; bit++)
											ORTC_tree[node].routemap[bit]=ORTC_tree[node].routemap[bit] | ORTC_tree[parent].routemap[bit];

								}
						}

						int prefixnum=0;
						for(int node=0; node<ORTC_nnum; node++)
							if(ORTC_tree[node].prefix_enable==1)
								prefixnum++;

						return prefixnum;
}

int get_maxORT(int KeySwitch, int * flag, int *IP, int ORTC_nnum, int ORTC_leafnnum, 
			   ORTC_node *ORTC_tree, int (*ST)[TotalTNum]){
				   int answer=0;

				  // local_OptIP(KeySwitch, flag, IP, ST, ORTC_leafnnum);
          // SwitchOptimalEncoding(KeySwitch, IP, ST);
           for(int i=0; i<TotalTNum; i++)
             IP[i]=i;
           for(int i=0; i<Smax; i++){
					   int curORTsz=compute_ORT(i, IP, ORTC_nnum, ORTC_leafnnum, ORTC_tree, ST);
					   if(curORTsz>answer){
						   answer=curORTsz;
					   }
				   }
				   return answer;
}

int CCencoding_ORTC(int (*ST)[TotalTNum], ofstream &output){
	int *IP=new int[TotalTNum];
	int ORTC_nnum;
	int ORTC_leafnnum;
	ORTC_node *ORTC_tree;
	int KeySwitch;
	int FinalAnswer=MaxInt;
	int temp=1;
	int *flag=new int[TotalTNum];

	temp=1;
	for(int i=0; i<IPv4len; i++){
		IPsegsz[i]=temp;
		temp=temp<<1;
	}
	for(int i=0; i<IPv4len; i++)
		if(IPsegsz[i]>=2*TotalTNum){
			ORTC_leafnnum=IPsegsz[i];
			ORTC_nnum=2*ORTC_leafnnum-1;
			break;
		}
		ORTC_tree=new ORTC_node[ORTC_nnum];

	KeySwitch=0;
	temp=get_maxORT(KeySwitch, flag, IP, ORTC_nnum, ORTC_leafnnum, ORTC_tree, ST);
	if(temp<FinalAnswer)
		FinalAnswer=temp;

	KeySwitch=TorSNum;
	temp=get_maxORT(KeySwitch, flag, IP, ORTC_nnum, ORTC_leafnnum, ORTC_tree, ST);
	if(temp<FinalAnswer)
		FinalAnswer=temp;

	KeySwitch=TorSNum+AggrSNum;
	temp=get_maxORT(KeySwitch, flag, IP, ORTC_nnum, ORTC_leafnnum, ORTC_tree, ST);
	if(temp<FinalAnswer)
		FinalAnswer=temp;

	output<<"the routing table size is: "<<FinalAnswer<<endl;

	delete[] IP;
	delete[] flag;
	delete[] ORTC_tree;
	return 0;

}
