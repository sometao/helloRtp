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
- [ ] 尝试将RTP中payload的H264数据直接使用ffmpeg解码，看看是否能播放
- [ ] 确认H264的NAL单元格式，以及原理，尝试将RTP中Payload的H264数据，恢复为原始H264数据
- [ ] RTP receive h264 data and play it




### Referance
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

