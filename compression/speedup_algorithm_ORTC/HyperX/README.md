###<b>How to compile and run the code?</b>

g++ fast_HyperX.cpp -o fast_HyperX

./fast_HyperX 

(The result can be found in an output file.)

###<b>configuraton</b>

To run the code at different scales, you need to configure variables "LMax", "S[LMax]", "Smax" and "Pmax" in HyperX.h. 

Here Smax=S\[0\]\*S\[1\]\*...\*S\[LMax-1\]; Pmax=S\[0\]\+S\[1\]\+...\+S\[LMax-1\]-LMax;

E.g., for HyperX(3, 4, 40), set "LMax=3", "S[LMax]={4, 4, 4}", "Smax=64" and "Pmax=9".


