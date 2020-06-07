




## Audio

#### TODOS 
- [X] ffmpeg 提取AAC音频，尝试提取ADTS、~~LATM两种封装音频~~
- [X] 对AAC音频文件二进制分析，数据块结构
- [X] 确认ADTS二进制结构
- [ ] 尝试ffmpeg推RTP音频（AAC），ffplay接收RTP音频并播放
- [ ] ffmpeg 通过RTP推流，编写服务端接收并保存RTP数据包为文件（或者抓包），分析RTP二进制格式
- [ ] 阅读 RFC3640, 弄懂RTP传输MPEG元素的方式
- [ ] 弄懂LATM用RTP传输细节
- [ ] AudioSpecificConfig 内容含义，以及二进制格式
- [ ] ADTS头中获得 LATM AudioSpecificConfig 信息



