
AAC数据块长度是可变的，需要ADTS头来对数据进行分割；
ADTS头为七个字节：



ADTS head

```
   0                   1                   2                   
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  28 bit
  |1 1 1 1 1 1 1 1 1 1 1 1|I|LYR|p|prf|samplin|p|chanl|o|h| adts_fixed_header
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |c|c|    AAC frame length     |adts buf fullness    |ndb| adts_variable_header
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  from https://blog.csdn.net/tx3344/article/details/7414543
```


jrtplib 发送 ADTS
AAC 发送ADTS  与 接收
AAC 转PCM


AAC 格式分析 【好文，推荐，LATM讲的很清楚】
http://blog.chinaunix.net/uid-26084833-id-3416600.html



【多媒体封装格式详解】--- AAC ADTS格式分析  【靠谱,ADTS 讲的很清楚】
https://blog.csdn.net/tx3344/article/details/7414543





ffmpeg/ffplay  aac latm test.


 LATM-based streams consist of a sequence of audioMuxElements that
   include one or more PayloadMux elements that carry the audio frames.
   A complete audioMuxElement or a part of one SHALL be mapped directly
   onto an RTP payload without any removal of audioMuxElement syntax
   elements (see Figure 4).  The first byte of each audioMuxElement
   SHALL be located at the first payload location in an RTP packet.


RTP 包含 audioMuxElement 
audioMuxElement 包含 PayloadMux 
PayloadMux

muxConfigPresent = 1  => in-of-band
muxConfigPresent = 0  => out-of-band
StreamMuxConfig
useSameStreamMux


RTP Payload Format for MPEG-2 and MPEG-4 AAC Streams
https://tools.ietf.org/html/draft-ietf-avt-rtp-mpeg2aac-02



RTP Payload Format for MPEG-2 AAC Streams
https://www.ietf.org/proceedings/46/I-D/draft-ietf-avt-rtp-mpeg2aac-00.txt


en.wikipedia.org/wiki/Advanced_Audio_Coding



RTP载荷AAC音频
https://blog.csdn.net/KayChanGEEK/article/details/102468133

RTP音频AAC封包
https://www.cnblogs.com/Function-All/p/12802107.html	

PCM音频数据
https://www.jianshu.com/p/fd43c1c82945

PCM文件格式简介
https://blog.csdn.net/ce123_zhouwei/article/details/9359389



AAC编码流程
https://www.cnblogs.com/ranson7zop/p/7200123.html


AAC帧格式及编码介绍
https://blog.csdn.net/jgdu1981/article/details/6870577




关于AAC音频格式更详细的情况，可参考维基百科
http://en.wikipedia.org/wiki/Advanced_Audio_Coding


AAC 封装 【各种编码器介绍】
https://blog.csdn.net/evsqiezi/article/details/51602803



AAC的 LATM 格式?

AAC的ES流？

ISO/IEC 14496-3
http://www.doc88.com/p-0337882090192.html

https://wiki.multimedia.cx/index.php?title=MPEG-4_Audio



 AAC_LC用LATM封装header信息解析 
 
 https://my.oschina.net/zhangxu0512/blog/204070