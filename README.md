### HelloRtp v0.0.1
---


### Goals
- Try RTP
- RTP/RTCP sending and receiving data Demo.


### TODOS 
- [X] UDP data transmission
- [X] ffmpeg将视频文件转为h264 raw格式
- [X] ffplay播放h264 raw格式视频
- [X] ffmpeg使用rtp推h264 raw格式视频
- [X] RTP receive data and save in file
- [X] RTP receive h264 data and save it
- [X] 尝试将RTP中payload的H264数据直接使用ffmpeg解码，看看是否能播放，实验结果是无法播放，需要包格式转换
- [X] ~~尝试直接用ffmpeg的avformat_open_input url来读取rtp h264流~~ 
- [X] 确认H264的NAL单元格式，以及原理，尝试将RTP中Payload的H264数据，恢复为原始H264数据
- [X] RTP receive h264 data and play it


### 基础数据结构/概念
- RTP：
  - RTP头
  - RTP载荷payload
- NALU: NAL单元，即H264为了方便网络传输所包装起来的数据单元, 包括NALU头和EBSP(扩展字节序列载荷)
  - NALU头: 对该单元进行描述
  - EBSP: 存储有效数据
- NALU包:为了方便NALU们在RTP协议上进行传输, 对NALU进行处理后得到NALU包(NALU Packet).
  - NALU包的头: 对该`NALU包`进行描述
  - NALU包的内容:NALU包的具体内容
  - NALU包也可以看作一种NALU，两者关系并不是完全的对立或包含
  - 这块的细节信息, 请参考RFC6184


### Parser H264 from RTP
- 接收UDP包
- 去掉RTP包头，获得RTP payload，这个payload就是一个`NALU包`，这里称为`NALU包`是为了与H264编码中的`NALU`相区别，`NALU包`有三类
  - 单NALU包：这个包里，有且只有一个完整的NALU
  - 聚合包：这个包里，包含着多个NALU
  - 分片包：这个包里，包含这一个NALU的一段内容
- 通过NALU包的头（也就是NALU包第一个字节），判断当前NALU包的类型，然后将NALU包通过添加起始码的方式，恢复为NALU
- 恢复后的NALU就能丢到解码器里去解码了


### Referance
- RFC6184: RTP Payload Format for H.264 Video: https://tools.ietf.org/html/rfc6184
- H264 的 RTP 格式封装: https://blog.csdn.net/yongkai0214/article/details/88872076
- RFC3550中文版：https://www.jianshu.com/p/d85a6a7b3189  
- RTP 学习笔记：https://www.cnblogs.com/awiki/p/4340917.html
- RTP 协议分析：https://blog.csdn.net/bripengandre/article/details/2238818	 
- RTP/RTCP/RTSP 区别：https://blog.csdn.net/xyz_lmn/article/details/6055175
- RTP相关RFC汇总：https://blog.csdn.net/dong_beijing/article/details/82056484   
- RTSP/RTP/RTCP详解整理：https://blog.csdn.net/ericfantastic/article/details/48998139
- RTP/RTCP&RTSP的关系：https://blog.csdn.net/CSDNhuaong/article/details/78228554
- RTP lib：https://github.com/j0r1/JRTPLIB
- RTP server demo: https://github.com/linshufei/UbuntuReceivePicture
- RTP H264细节：https://blog.csdn.net/jwybobo2007/article/details/7054140
- RTP H264细节：https://blog.csdn.net/davebobo/article/details/52994596
- RTP H264：https://blog.csdn.net/westlor/article/details/50538058
- 接收RTP H264代码：https://github.com/gityf/img-video/blob/master/video/ffmpeg-h264-sdl-view/main.c

