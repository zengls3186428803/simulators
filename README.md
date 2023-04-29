# tcp_simulator
## 说明
这是两个传输层协议模拟器，改编（修改、补充）自《Computer Networking: a Top-Down Approach, 7th Edition》中的一个传输层编程作业。
原实验网址: https://media.pearsoncmg.com/aw/aw_kurose_network_3/labs/lab5/lab5.html

## alternative-bit-protocol 为比特交替协议模拟器
all_in_one.c 是一个所有源代码文件（event_manager.c + main.c 以及相关的"*.h"）的汇总,可以直接进行编译。
all_in_one.bin 是在ubuntu环境下，用gcc编译all_in_one.c文件 得到的 可执行文件。

simulator.bin 和 all_in_one.bin一样都是可执行文件（唯一的不同是，simulator是按makefile编译链接的）