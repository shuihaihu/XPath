const int LevelNum=5;
const int N=8;
const int serverNum=N*N*N*N*N;
const int singleLSNum=N*N*N*N;
const int SingleSPT=LevelNum;
const int SwitchNum=singleLSNum*LevelNum;
const int Smax=serverNum+SwitchNum;
const int TotalTNum=serverNum*LevelNum;
const int Pmax=N;
const int SwitchPortNum=N;
const int ServerPortNum=LevelNum;
const int MaxportNum=SwitchPortNum;
const int MaxInt=99999999;
const int Maskcode=0x000000ff;
const int RoundMax=500;
const double Randa=0.5;
const int MaxExchangetime=100;
const int ClassNum=LevelNum+1;
const int IPv4len=32;
int IPsegsz[IPv4len]={0};
const int MapLen=3;
const int Intbitlen=sizeof(int)*8;