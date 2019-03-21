## 音频解码转码pcm
</br></br>
### 流程
&nbsp;解码流程与视频一致</br>
&nbsp;&nbsp; 1 注册组建</br>
&nbsp;&nbsp; 2 打开输入音频文件</br>
&nbsp;&nbsp; 3 获取音频信息</br>
&nbsp;&nbsp; 4 获取音频解码器 (在此之前需要先获取到音频流信息)</br>
&nbsp;&nbsp; 5 打开音频解码器</br>
&nbsp;&nbsp; 6 一帧一帧读取压缩音频数据AVPacket 一帧一帧的音频包 （之前需要设置重采样参数）</br>
&nbsp;&nbsp; 　　6.1 重采样 读取解码buffer</br>
&nbsp;&nbsp; 　　6.2 写入文件</br>
&nbsp;&nbsp; 7 关闭释放资源</br>

</br></br>
### 注意
&nbsp;&nbsp; 1 麦克风采集的数据一般为44100hz </br>
&nbsp;&nbsp; 2 使用立体声声道布局 </br>
&nbsp;&nbsp; 3 16bit的数据与32bit数据人耳分辨不出，为省内存使用16bit </br>

</br></br>
### 遗留问题
&nbsp;&nbsp; <font color=red > 纯音频数据重采样成功，视频文件重采样音频文件采集完成后崩溃 </font></br>
