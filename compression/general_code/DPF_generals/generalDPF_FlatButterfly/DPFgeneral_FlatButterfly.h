/* 
Butterfly parameters 
start:
*/  
const int LevelNum=3;
const int N=8;
const int radix_k=2;
const int BTorNum=N;
const int BSmax=N;
const int BSwitchPortNum=LevelNum;
const int BDestPathNum=12;
const int BTotalPathNum=BDestPathNum*BTorNum;

/* 
end.
*/ 

int DestPathNum=BDestPathNum;
int TotalPathNum=BTotalPathNum;
const int srcNum=BTorNum;
const int destNum=BTorNum;
const int TorNum=BTorNum;
const int Smax=BSmax;
const int RoundNum=LevelNum;
const int MaxInt=99999999;

