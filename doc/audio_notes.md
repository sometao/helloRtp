
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





### RTP Payload Format for Transport of MPEG-4 Elementary Streams


2.11.  Global Structure of Payload Format
```
         +---------+-----------+-----------+---------------+
         | RTP     | AU Header | Auxiliary | Access Unit   |
         | Header  | Section   | Section   | Data Section  |
         +---------+-----------+-----------+---------------+

                   <----------RTP Packet Payload----------->

            Figure 1: Data sections within an RTP packet
```



3.2.  RTP Payload Structure

3.2.1.  The AU Header Section
```
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- .. -+-+-+-+-+-+-+-+-+-+
      |AU-headers-length|AU-header|AU-header|      |AU-header|padding|
      |                 |   (1)   |   (2)   |      |   (n)   | bits  |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- .. -+-+-+-+-+-+-+-+-+-+

                   Figure 2: The AU Header Section
```
- ���AU-header������Ϊ�գ���ô`AU-headers-length`�ֶ�Ҳ�������
- ���AU-header����Ϊ�ǿգ���ô`AU-headers-length`Ϊ�����ֽڣ��������е�AU-header����������bit�����������������bit������
- AU-header��bit�ܳ��ȣ���Ҫ���뵽8�ı�������ಹ7��bit



3.2.1.1.  The AU-header
```

      +---------------------------------------+
      |     AU-size                           |
      +---------------------------------------+
      |     AU-Index / AU-Index-delta         |
      +---------------------------------------+
      |     CTS-flag                          |
      +---------------------------------------+
      |     CTS-delta                         |
      +---------------------------------------+
      |     DTS-flag                          |
      +---------------------------------------+
      |     DTS-delta                         |
      +---------------------------------------+
      |     RAP-flag                          |
      +---------------------------------------+
      |     Stream-state                      |
      +---------------------------------------+

   Figure 3: The fields in the AU-header.  If used, the AU-Index field
             only occurs in the first AU-header within an AU Header
             Section; in any other AU-header, the AU-Index-delta field
             occurs instead.
```


3.2.2.  The Auxiliary Section
```
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- .. -+-+-+-+-+-+-+-+-+
      | auxiliary-data-size   | auxiliary-data       |padding bits |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- .. -+-+-+-+-+-+-+-+-+

           Figure 4: The fields in the Auxiliary Section
```


3.2.3.  The Access Unit Data Section
```
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |AU(1)                                                          |
      +                                                               |
      |                                                               |
      |               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |               |AU(2)                                          |
      +-+-+-+-+-+-+-+-+                                               |
      |                                                               |
      |                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                               | AU(n)                         |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |AU(n) continued|
      |-+-+-+-+-+-+-+-+

        Figure 5: Access Unit Data Section; each AU is octet-aligned.
```


3.3.  Usage of this Specification
3.3.1.  General
```
a=rtpmap:<payload type> <encoding name>/<clock rate>[/<encoding parameters>]
```
For audio streams, \<encoding parameters\> specifies the number of audio channels


3.3.2.  The Generic Mode
```
   m=video 49230 RTP/AVP 96
   a=rtpmap:96 mpeg4-generic/1000
   a=fmtp:96 streamtype=3; profile-level-id=1807; mode=generic; objectType=2; config=0842237F24001FB400094002C0; sizeLength=10; CTSDeltaLength=16; randomAccessIndication=1; streamStateIndication=4
```
- `mode=generic` stream������Ϊ generic
- `sizeLength=10` ���壺 AU header �� AU-size �ֶε�bit����Ϊ10��
- `CTSDeltaLength=16`  ���壺 AU header �� CTS-delta �ֶε�bit����Ϊ16��
- `randomAccessIndication=1`  ���壺 AU header �а���1 bit���ȵ�RAP-flag��
- `streamStateIndication=4`  ���壺 AU header �а��� 4 bit���ȵ�Stream-State�ֶΣ�


3.3.4.  Variable Bit-rate CELP
```
   m=audio 49230 RTP/AVP 96
   a=rtpmap:96 mpeg4-generic/16000/1
   a=fmtp:96 streamtype=5; profile-level-id=14; mode=CELP-vbr; config=
   440F20; sizeLength=6; indexLength=2; indexDeltaLength=2;
   constantDuration=160; maxDisplacement=5
```
- `mode=generic` stream������Ϊ CELP-vbr
- `sizeLength=6` ���壺 AU header �� AU-size �ֶε�bit����Ϊ6��
- `indexLength=2`  ���壺 AU header �� AU-Index �ֶε�bit����Ϊ2��AU-index�ֶ�ֻ�������׸�AU�ʵ����ֵ�̶�Ϊ0��
- `indexDeltaLength=2`  ���壺 AU header �� AU-Index-delta �ֶε�bit����Ϊ2��
- ��AU-header Section�У���ǰ����16 bit��`AU-headers-length`�ֶΣ������ž��Ǹ���AU-header
- CELP frames �� Access Unit ʱ���ǹ̶��ģ�ͨ��`constantDuration`����ָ����



3.3.5.  Low Bit-rate AAC
```
   m=audio 49230 RTP/AVP 96
   a=rtpmap:96 mpeg4-generic/22050/1
   a=fmtp:96 streamtype=5; profile-level-id=14; mode=AAC-lbr; config=
   1388; sizeLength=6; indexLength=2; indexDeltaLength=2;
   constantDuration=1024; maxDisplacement=5
```
- `mode=generic` stream������Ϊ AAC-lbr
- `sizeLength=6` ���壺 AU header �� AU-size �ֶε�bit����Ϊ6��
- `indexLength=2`  ���壺 AU header �� AU-Index �ֶε�bit����Ϊ2��AU-index�ֶ�ֻ�������׸�AU�ʵ����ֵ�̶�Ϊ0��
- `indexDeltaLength=2`  ���壺 AU header �� AU-Index-delta �ֶε�bit����Ϊ2��
- ��AU-header Section�У���ǰ����16 bit��`AU-headers-length`�ֶΣ������ž��Ǹ���AU-header
- AAC frames �� Access Unit ʱ���ǹ̶��ģ�ͨ��`constantDuration`����ָ����




