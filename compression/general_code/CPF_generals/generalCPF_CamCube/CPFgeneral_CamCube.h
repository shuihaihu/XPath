/* 
BCube parameters 
start:
*/ 
const int DimNum=3;
const int radix_k=4;
const int SrcDestPNum=6;
const int CSmax=radix_k*radix_k*radix_k;
const int CDestPathNum=SrcDestPNum*(CSmax-1);
const int CTotalPathNum=CSmax*CDestPathNum;
const int direction_l=0;
const int direction_r=1;

/* 
end.
*/ 

const int TotalPathNum=CTotalPathNum;
const int srcNum=CSmax;
const int destNum=CSmax;
const int TorNum=CSmax;
const int Smax=CSmax;
const int DestPathNum=CDestPathNum;
const int MaxInt=99999999;

