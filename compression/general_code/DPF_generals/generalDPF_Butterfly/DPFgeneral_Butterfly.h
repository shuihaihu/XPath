/* 
Butterfly parameters 
start:
*/  
const int LevelNum=10;
const int radix_k=2;
const int N=512;
const int BsingleLSNum=N;
const int BTorNum=BsingleLSNum*2;
const int BSmax=LevelNum*BsingleLSNum;
const int BSwitchPortNum=4;
const int BDestPathNum=BTorNum-1;
const int BTotalPathNum=BDestPathNum*BTorNum;

/* 
end.
*/ 

const int DestPathNum=BDestPathNum;
const int TotalPathNum=BTotalPathNum;
const int srcNum=BTorNum;
const int destNum=BTorNum;
const int TorNum=BTorNum;
const int Smax=BSmax;
const int RoundNum=1;
const int MaxInt=99999999;

