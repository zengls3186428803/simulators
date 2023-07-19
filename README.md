- [说明](#说明)
- [alternative-bit-transport-layer](#alternative-bit-transport-layer)
    - [说明](#说明-1)
- [like-GBN-transport-layer](#like-gbn-transport-layer)
    - [说明](#说明-2)
- [distance-vector-route-ip-layer](#distance-vector-route-ip-layer)
    - [说明](#说明-3)
- [MSI-protocal-mul-cache](#msi-protocal-mul-cache)
    - [说明](#说明-4)

## 说明
alternative-bit-transport-layer、like-GBN-transport-layer、distance-vector-route-ip-layer，
上述三个模拟器是根据《Computer Networking: a Top-Down Approach, 7th Edition》里的编成实验改写的,原实验网址：https://media.pearsoncmg.com/aw/ecs_kurose_compnetwork_7/cw/#misclabs <br>
```
重写了事件管理部分的代码，修改了“模拟的网络环境”的一些行为,例如让数据报乱序到达等;添加了实验要求的代码（有的地方没有按实验的推荐的方式实现。
```

## alternative-bit-transport-layer
比特交换协议（等停协议）（wait-stop）<br>
A发“一个”消息，等待B的ack，等不到（超时）就重发；A收到ack后发下一条。<br>
#### 说明
1. 发送方A，接收方B，接收方B不主动给A发消息，但会对A回复“ack”<br>
2. alternative-bit 模拟的网络环境 保证分组（segment）按序到达B<br>
3. 使用方式：运行auto.sh，会得到可执行文件simulator.bin
```bash
[zengls@archlinux simulators]$cd alternative-bit-transport-layer 
[zengls@archlinux alternative-bit-transport-layer]$ls
all_in_one.c  auto.sh  event_manager.c  event_manager.h  main.c  makefile  meg_and_seg.h
[zengls@archlinux alternative-bit-transport-layer]$./auto.sh 
gcc -c event_manager.c
gcc -c main.c
gcc -o simulator.bin main.o event_manager.o
rm *.o
[zengls@archlinux alternative-bit-transport-layer]$ls
all_in_one.c  auto.sh  event_manager.c  event_manager.h  main.c  makefile  meg_and_seg.h  simulator.bin
```

## like-GBN-transport-layer
回退N步（Go Back N Step）协议<br>
A发多条消息，等待ACK，等不到就回退重发；A收到ack后更新A的计时器。<br>
#### 说明
1. 发送方A，接收方B，接收方B不主动给A发消息，但会对A回复“ack”<br>
2. like-GBN 模拟的网络环境 不保证分组（segment）按序到达B<br>
3. 使用方式：运行auto.sh，会得到可执行文件simulator.bin
```bash
[zengls@archlinux simulators]$cd like-GBN-transport-layer 
[zengls@archlinux like-GBN-transport-layer]$ls
auto.sh          event_manager.h  makefile       output.txt  test.c  tool.h             transport_layer.h
event_manager.c  main.c           msg_and_seg.h  tags        tool.c  transport_layer.c
[zengls@archlinux like-GBN-transport-layer]$./auto.sh      
rm: cannot remove '*.bin': No such file or directory
rm *.bin
gcc -c event_manager.c
gcc -c main.c
gcc -c tool.c
cc    -c -o transport_layer.o transport_layer.c
gcc -o simulator.bin main.o event_manager.o tool.o transport_layer.o
rm *.o
[zengls@archlinux like-GBN-transport-layer]$ls
auto.sh          event_manager.h  makefile       output.txt     tags    tool.c  transport_layer.c
event_manager.c  main.c           msg_and_seg.h  simulator.bin  test.c  tool.h  transport_layer.h
```

## distance-vector-route-ip-layer
距离向量路由算法-ip层模拟器<br>
#### 说明
使用方式：运行auto.sh，会得到可执行文件simulator.bin
```bash
[zengls@archlinux simulators]$cd distance-vector-route-ip-layer 
[zengls@archlinux distance-vector-route-ip-layer]$ls
auto.sh  datagram.h  event_manager.c  event_manager.h  main.c  makefile  node.c  tags  test.c  tool.c  tool.h
[zengls@archlinux distance-vector-route-ip-layer]$./auto.sh 
rm: cannot remove '*.bin': No such file or directory
gcc -g -c event_manager.c
gcc -g -c main.c
gcc -g -c tool.c
gcc -g -c node.c
gcc -g -o simulator.bin main.o event_manager.o tool.o node.o
rm *.o
[zengls@archlinux distance-vector-route-ip-layer]$ls
auto.sh  datagram.h  event_manager.c  event_manager.h  main.c  makefile  node.c  simulator.bin  tags  test.c  tool.c  tool.h
```

## MSI-protocal-mul-cache
（多处理机）多cache MSI 监听协议 模拟器（Modify, Share, Invalidate）<br>
#### 说明
1. python 版本>=3.7<br>
2. 使用方式：运行main文件<br>
3. 第三方包依赖：
```
prettytable
pandas
```
4. MSI-protocal-mul-cache目录里的[outline.md](MSI-protocol-mul-cache/outline.md)为MSI模拟器的设计思路和具体使用方法

