###<b>How to compile and run the code?</b>

g++ fast_Bcube.cpp -o fast_Bcube

./fast_Bcube 

(The result can be found in an output file.)

###<b>configuraton</b>

To run the code at different scales, you need to configure variable "LevelNum", "N", "serverNum" and "singleLSNum" in Bcube.h. 

Here serverNum=N^LevelNum;  singleLSNum=N^(LevelNum-1);

E.g., for Bcube(8, 2), set "N=8", "LevelNum=3", "serverNum=N\*N\*N" and "singleLSNum=N\*N".


