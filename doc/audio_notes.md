
AAC���ݿ鳤���ǿɱ�ģ���ҪADTSͷ�������ݽ��зָ
ADTSͷΪ�߸��ֽڣ�



ADTS head

```
  total 7 bytes(56 bits)
  28 bits for adts_fixed_header, 
  28 bits for adts_variable_header.

   0                   1                   2                   
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  28 bit
  |1 1 1 1 1 1 1 1 1 1 1 1|I|LYR|p|prf|samplin|p|chanl|o|h| adts_fixed_header
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |c|c|    AAC frame length     |adts buf fullness    |ndb| adts_variable_header
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  reference: 
  - https://wiki.multimedia.cx/index.php?title=ADTS
  - https://www.jianshu.com/p/b5ca697535bd
```


jrtplib ���� ADTS
AAC ����ADTS  �� ����
AAC תPCM


AAC ��ʽ���� �����ģ��Ƽ���LATM ���ĺ����, ADTSҲ���ĺܺá�
https://blog.csdn.net/markman101/article/details/6616170/



ADTS��������
https://blog.csdn.net/tx3344/article/details/7414543
https://www.jianshu.com/p/b5ca697535bd
https://wiki.multimedia.cx/index.php?title=ADTS  ��������


RTP Payload Format for Transport of MPEG-4 Elementary Streams
https://tools.ietf.org/html/rfc3640


ffmpeg/ffplay  aac latm test.


 LATM-based streams consist of a sequence of audioMuxElements that
   include one or more PayloadMux elements that carry the audio frames.
   A complete audioMuxElement or a part of one SHALL be mapped directly
   onto an RTP payload without any removal of audioMuxElement syntax
   elements (see Figure 4).  The first byte of each audioMuxElement
   SHALL be located at the first payload location in an RTP packet.


RTP ���� audioMuxElement 
audioMuxElement ���� PayloadMux 
PayloadMux

muxConfigPresent = 1  => in-of-band
muxConfigPresent = 0  => out-of-band
StreamMuxConfig   => config in SDP
useSameStreamMux

AudioSpecificConfig => MPS-asc in SDP

Audio Specific Config������������ж��壺
https://wiki.multimedia.cx/index.php?title=MPEG-4_Audio#Audio_Specific_Config

Audio Specific Config˵��
https://blog.csdn.net/jwybobo2007/article/details/9221657


AudioSpecificConfig �� StreamMuxConfig ����

ADTS
```
   ADTS AudioMuxElement
   +------------------------------+
   | ADTS header |    AAC ES      |
   +------------------------------+


   AudioMuxElement (out-of-band)
   +------------------------------+
   | PayloadLengthInfo |  AAC ES  |
   +------------------------------+

```


LATM
```
   LATM AudioMuxElement (in-of-band)
   +----------------------------------------------------+
   | AudioSpecificConfig | PayloadLengthInfo |  AAC ES  |
   +----------------------------------------------------+


   LATM AudioMuxElement (out-of-band)
   +------------------------------+
   | PayloadLengthInfo |  AAC ES  |
   +------------------------------+

```


LATM over RTP
```
   RTP (one audioMuxElement in RTP packet, marker = 1 in RTP header)
   +------------------------------------+
   | RTP header |    AudioMuxElement    |
   +------------------------------------+

   RTP (not last audioMuxElement fragment in RTP packet,  marker = 0 in RTP header)
   +------------------------------------+
   | RTP header | AudioMuxEle fragment  |
   +------------------------------------+

```



���㿪ʼдһ��RTSP���������壩RTP����AAC
https://blog.csdn.net/weixin_42462202/article/details/99200935



AAC���ݿ鳤���ǿɱ�ģ���ҪADTSͷ�������ݽ��зָ
ADTSͷΪ�߸��ֽ�


aac��Ƶ���ݵ�rtp������̣�android��  ADTS ת LATM
https://blog.csdn.net/nihao1113/article/details/81675317

aac_adtstoasc bitstream filter
https://blog.csdn.net/liuyl2016/article/details/53080733


AAC��Ƶ�ļ����ADTSͷ
https://blog.csdn.net/chailongger/article/details/84376914

RTP Payload Format for Transport of MPEG-4 Elementary Streams
https://tools.ietf.org/html/rfc3640



RTP Payload Format for MPEG-2 and MPEG-4 AAC Streams
https://tools.ietf.org/html/draft-ietf-avt-rtp-mpeg2aac-02



RTP Payload Format for MPEG-2 AAC Streams
https://www.ietf.org/proceedings/46/I-D/draft-ietf-avt-rtp-mpeg2aac-00.txt


AAC ES�� ���ADTSͷ��
https://blog.csdn.net/acm2008/article/details/42971473


en.wikipedia.org/wiki/Advanced_Audio_Coding



RTP�غ�AAC��Ƶ
https://blog.csdn.net/KayChanGEEK/article/details/102468133

RTP��ƵAAC���
https://www.cnblogs.com/Function-All/p/12802107.html	

PCM��Ƶ����
https://www.jianshu.com/p/fd43c1c82945

PCM�ļ���ʽ���
https://blog.csdn.net/ce123_zhouwei/article/details/9359389


fdk AAC
http://wiki.hydrogenaud.io/index.php?title=Fraunhofer_FDK_AAC

AAC��������
https://www.cnblogs.com/ranson7zop/p/7200123.html


AAC֡��ʽ���������
https://blog.csdn.net/jgdu1981/article/details/6870577


FFmpeg/libavformat/latmenc.c 
https://github.com/FFmpeg/FFmpeg/blob/master/libavformat/latmenc.c


FFmpeg/libavformat/rtpdec_latm.c : latm_parse_packet
https://github.com/FFmpeg/FFmpeg/blob/master/libavformat/rtpdec_latm.c


FFmpeg/libavformat/rtpenc_latm.c : ff_rtp_send_latm
https://github.com/FFmpeg/FFmpeg/blob/master/libavformat/rtpenc_latm.c


����AAC��Ƶ��ʽ����ϸ��������ɲο�ά���ٿ�
http://en.wikipedia.org/wiki/Advanced_Audio_Coding


AAC ��װ �����ֱ��������ܡ�
https://blog.csdn.net/evsqiezi/article/details/51602803



AAC�� LATM ��ʽ?

AAC��ES����

ISO/IEC 14496-3
http://www.doc88.com/p-0337882090192.html

https://wiki.multimedia.cx/index.php?title=MPEG-4_Audio



 AAC_LC��LATM��װheader��Ϣ���� 
 
 https://my.oschina.net/zhangxu0512/blog/204070