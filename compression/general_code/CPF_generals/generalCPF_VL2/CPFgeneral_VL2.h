/* 
HyperX parameters 
start:
*/ 
const int DA=32;
const int DI=16;
const int VTorNum=DA*DI/4;
const int AggrSNum=DI;
const int IntSNum=DA/2;
const int VDestPSNum=DA;
const int groupsize=DA/2;
const int VDestPathNum=VDestPSNum*(VTorNum-1);
const int VTotalPNum=VDestPathNum*VTorNum;
const int VSmax=VTorNum+AggrSNum+IntSNum;
const int Pmax=DA;
/* 
end.
*/ 

const int DestPathNum=VDestPathNum;
const int TotalPathNum=VTotalPNum;
const int srcNum=VTorNum;
const int destNum=VTorNum;
const int TorNum=VTorNum;
const int Smax=VSmax;
const int MaxInt=99999999;

