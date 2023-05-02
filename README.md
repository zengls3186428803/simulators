#### 这是《计算机网络－自顶向下方法》传输层的编程作业（实验）
原实验网址 https://media.pearsoncmg.com/aw/aw_kurose_network_3/labs/lab5/lab5.html
下面两个模拟器是对上述实验代码的修改和补充
修改了：事件管理的实现代码（没有改变实现思路）、“按序到达”的代码等
补充了：传输层代码等

#### alternative-bit 
alternative-bit是比特交换协议（等停协议）（wait-stop）
A发“一个”消息，等待B的ack，等不到（超时）就重发；收到ack后发下一条。

#### like-GBN
like-GBN 是回退N步协议（Go Back N Step）
A发多条消息，等待ACK，等不到就回退重发；收到ack后更新A的计时器。

#### 说明
发送方A，接收方B，接收方B不主动给A发消息，但会对A回复“ack”
alternative-bit 模拟的网络环境 保证分组（segment）按序到达B
like-GBN 模拟的网络环境 不保证分组（segment）按序到达B

#### 使用方式
simulator.bin 不保证在不同机器上可以正常运行，所以建议重新编译<br>

对于alternative-bit<br>
方式1. 直接用c编译器编译all_in_one.c<br>
方式2. 使用make指令<br>


对于like-GBN<br>
直接运行auto.sh脚本（会得到一个名为simulator.bin的可执行文件）