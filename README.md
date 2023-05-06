## 说明
alternative-bit-transport-layer、like-GBN-transport-layer、distance-vector-route-ip-layer，
上述三个模拟器是根据《Computer Networking: a Top-Down Approach, 7th Edition》里的编成实验改写的。
原实验网址：https://media.pearsoncmg.com/aw/ecs_kurose_compnetwork_7/cw/#misclabs <br>
重写了事件管理部分的代码（通过函数指针使之面向对象一点），修改了“模拟的网络环境”的一些行为（例如让数据报乱序到达等），添加了实验要求的代码（有的地方没有按实验的推荐的方式实现）

## alternative-bit-transport-layer
比特交换协议（等停协议）（wait-stop）<br>
A发“一个”消息，等待B的ack，等不到（超时）就重发；收到ack后发下一条。<br>
#### 说明
发送方A，接收方B，接收方B不主动给A发消息，但会对A回复“ack”<br>
alternative-bit 模拟的网络环境 保证分组（segment）按序到达B<br>
使用方式：
1. 使用make指令编译，会得到可执行文件simulator.bin
或2. 编译运行all_in_one.c文件 <br>
我的gcc版本：gcc (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0

## like-GBN-transport-layer
回退N步（Go Back N Step）协议<br>
A发多条消息，等待ACK，等不到就回退重发；收到ack后更新A的计时器。<br>
#### 说明
发送方A，接收方B，接收方B不主动给A发消息，但会对A回复“ack”<br>
like-GBN 模拟的网络环境 不保证分组（segment）按序到达B<br>
使用方式：运行auto.sh脚本,会得到可执行文件simulator.bin<br>
我的gcc版本：gcc (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0

## distance-vector-route-ip-layer
距离向量路由算法-ip层模拟器<br>
#### 说明
使用方式：运行auto.sh脚本,会得到可执行文件simulator.bin<br>
我的gcc版本：gcc (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0

## MSI-protocal-mul-cache
（多处理机）多cache MSI协议 模拟器（Modify, share, invalidate）<br>
#### 说明
使用方式：直接运行main文件<br>
我的python 版本： Python 3.8.10<br>
第三方包依赖：
```
from prettytable import PrettyTable
import pandas
```