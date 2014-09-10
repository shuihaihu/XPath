/* 
BCube parameters 
start:
*/  
const int LevelNum=3;
const int N=4;
const int BserverNum=N*N*N;
const int BsingleLSNum=N*N;
const int BSwitchNum=BsingleLSNum*LevelNum;
const int BSmax=BserverNum+BSwitchNum;
const int BSwitchPortNum=N;
const int BServerPortNum=LevelNum;
const int BDestPathNum=LevelNum*(BserverNum-1);
const int BTotalPathNum=BDestPathNum*BserverNum;

/* 
end.
*/ 

const int DestPathNum=BDestPathNum;
const int TotalPathNum=BTotalPathNum;
const int srcNum=BserverNum;
const int destNum=BserverNum;
const int TorNum=BserverNum;
const int Smax=BSmax;
const int serverNum=BserverNum;
const int RoundNum=LevelNum;
const int MaxInt=99999999;

