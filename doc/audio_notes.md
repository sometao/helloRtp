
AAC���ݿ鳤���ǿɱ�ģ���ҪADTSͷ�������ݽ��зָ
ADTSͷΪ�߸��ֽڣ�



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


jrtplib ���� ADTS
AAC ����ADTS  �� ����
AAC תPCM


AAC ��ʽ���� �����ģ��Ƽ���LATM���ĺ������
http://blog.chinaunix.net/uid-26084833-id-3416600.html



����ý���װ��ʽ��⡿--- AAC ADTS��ʽ����  ������,ADTS ���ĺ������
https://blog.csdn.net/tx3344/article/details/7414543





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
StreamMuxConfig
useSameStreamMux


RTP Payload Format for MPEG-2 and MPEG-4 AAC Streams
https://tools.ietf.org/html/draft-ietf-avt-rtp-mpeg2aac-02



RTP Payload Format for MPEG-2 AAC Streams
https://www.ietf.org/proceedings/46/I-D/draft-ietf-avt-rtp-mpeg2aac-00.txt


en.wikipedia.org/wiki/Advanced_Audio_Coding



RTP�غ�AAC��Ƶ
https://blog.csdn.net/KayChanGEEK/article/details/102468133

RTP��ƵAAC���
https://www.cnblogs.com/Function-All/p/12802107.html	

PCM��Ƶ����
https://www.jianshu.com/p/fd43c1c82945

PCM�ļ���ʽ���
https://blog.csdn.net/ce123_zhouwei/article/details/9359389



AAC��������
https://www.cnblogs.com/ranson7zop/p/7200123.html


AAC֡��ʽ���������
https://blog.csdn.net/jgdu1981/article/details/6870577




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