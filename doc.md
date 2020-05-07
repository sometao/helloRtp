

### RTP

分包模式：
sdp中的 packetization-mode 这个字段，不同的值代表支持不同的分包模式。如下：
- 单一NAL单元模式（ Single NAL unit mode）：packetization-mode = 0 或者无此字段时缺省
- 非交错模式（Non-interleaved mode）: packetization-mode = 1
- 交错模式（Interleaved mode）: packetization-mode = 2


各个模式下，支持的包
- 对于 Single NAL Unit Mode，仅含有 single NAL unit packet，所以一般都是较小的包，太大的包可能分片，也不能适用此场景。协议要求接收方必须支持，只能 say YES。
- 对于 Non-interleaved mode，包含 single NAL unit packets，STAP-As 和 FU-As，不能包含 STAP-Bs，MTAPs 和 FU-Bs。协议要求接收方应该支持，即在一般场景都应该支持。这也是现在应用最广的模式，在通信领域，在互联网通信基本上都会支持。
- 对于 Interleaved mode，包含 STAP-Bs，MTAPs，FU-As，and FU-Bs，不能存在 STAP-As 和 single NAL unit packets。协议要求接收方可以支持，即按照场景决定是否支持，可以选择 NO 。


### H264
- NAL TODO
- 在STAP-A类型的NALU中，聚合包中的所有按照[长度][NALU][长度][NALU]，将整个包进行解析
- FU-A TODO


### RTP payload处理流程（packetization-mode = 1）
- single NAL unit packet：头部添加起始码后，存入新AVPacket，送到解码器
- STAP-A：进行拆包，根据长度分割出每个NAL，在每个NAL前增加起始码之后，重新拼在一起，存入新AVPacket，送到解码器
- FU-A：如果是首段，就在前面增加`起始码`和`NAL HEADER`，然后存入新AVPacket，送到解码器
如果不是首段，就直接存入新AVPacket，送到解码器；
疑惑：需要保证rtp包的顺序，才能这样处理吧。




H264有两种编码方式：

| type |  desc |   VCL or Not  |
|:--------------:|:---------------:|:---------------:|
|0      |Unspecified                                                    |non-VCL  |
|1      |Coded slice of a non-IDR picture                               |VCL      |
|2      |Coded slice data partition A                                   |VCL      |
|3      |Coded slice data partition B                                   |VCL      |
|4      |Coded slice data partition C                                   |VCL      |
|5      |Coded slice of an IDR picture                                  |VCL      |
|6      |Supplemental enhancement information (SEI)                     |non-VCL  |
|7      |Sequence parameter set                                         |non-VCL  |
|8      |Picture parameter set                                          |non-VCL  |
|9      |Access unit delimiter                                          |non-VCL  |
|10     |End of sequence                                                |non-VCL  |
|11     |End of stream                                                  |non-VCL  |
|12     |Filler data                                                    |non-VCL  |
|13     |Sequence parameter set extension                               |non-VCL  |
|14     |Prefix NAL unit                                                |non-VCL  |
|15     |Subset sequence parameter set                                  |non-VCL  |
|16     |Depth parameter set                                            |non-VCL  |
|17..18 |Reserved                                                       |non-VCL  |
|19     |Coded slice of an auxiliary coded picture without partitioning |non-VCL  |
|20     |Coded slice extension                                          |non-VCL  |
|21     |Coded slice extension for depth view components                |non-VCL  |
|22..23 |Reserved                                                       |non-VCL  |
|24..31 |Unspecified                                                    |non-VCL  |
       



### 主要参考文献：

H264理解: https://blog.csdn.net/go_str/article/details/80340564
H264 over RTP: https://blog.csdn.net/yongkai0214/article/details/88872076
RFC6184[RTP Payload Format for H.264 Video]: https://tools.ietf.org/html/rfc6184
H264 on RTP (stackoverflow): https://stackoverflow.com/questions/24884827/possible-locations-for-sequence-picture-parameter-sets-for-h-264-stream/24890903#24890903
h264_handle_packet[rtpdec_h264.c]: https://github.com/FFmpeg/FFmpeg/blob/master/libavformat/rtpdec_h264.c#L312



