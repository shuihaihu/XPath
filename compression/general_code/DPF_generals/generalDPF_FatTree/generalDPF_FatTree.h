/* 
FatTree parameters 
start:
*/ 
const int podsize=16;
const int podnum=podsize;
const int EdgeSwitchNum=podnum*podsize/2;
const int AggreSwitchNum=podnum*podsize/2;
const int CoreSwitchNum=(podsize/2)*(podsize/2);
const int FPmax=podsize;
const int FTmax=podsize*podsize/4;
const int FSmax=EdgeSwitchNum+AggreSwitchNum+CoreSwitchNum;
const int FDestPathNum=FTmax*(EdgeSwitchNum-1);
const int FPathNum=FTmax*EdgeSwitchNum*(EdgeSwitchNum-1);
const int FPathSetNum=FTmax*2*(EdgeSwitchNum-1);


/* 
end.
*/ 

const int DestPathNum=FDestPathNum;
const int TotalPathNum=FPathNum;
const int srcNum=EdgeSwitchNum;
const int destNum=EdgeSwitchNum;
const int TorNum=EdgeSwitchNum;
const int Tmax=FTmax;
const int Smax=EdgeSwitchNum+AggreSwitchNum+CoreSwitchNum;
const int TotalPathSetNum=FPathSetNum;
const int MaxInt=99999999;

