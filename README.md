# 51单片机MP3播放器
****
### Author:Plain Ji
### E-mail:plain_ji@163.com
****
#### 功能
播放音乐，支持MP3、WMA、WAV、MIDI  
电子书  
图片浏览  
文件浏览器  
系统设置  

#### 效果展示
![001.png](https://github.com/PlainJi/MP3PlayerHardware/blob/master/%E6%88%90%E5%93%81%E5%9B%BE%E7%89%87/001.png)
![003.png](https://github.com/PlainJi/MP3PlayerHardware/blob/master/%E6%88%90%E5%93%81%E5%9B%BE%E7%89%87/003.png)
![006.png](https://github.com/PlainJi/MP3PlayerHardware/blob/master/%E6%88%90%E5%93%81%E5%9B%BE%E7%89%87/006.png)
![002.jpg](https://github.com/PlainJi/MP3PlayerHardware/blob/master/%E6%88%90%E5%93%81%E5%9B%BE%E7%89%87/002.jpg)
![004.jpg](https://github.com/PlainJi/MP3PlayerHardware/blob/master/%E6%88%90%E5%93%81%E5%9B%BE%E7%89%87/004.jpg)  
![mp3.jpg](http://r.photo.store.qq.com/psb?/V11QrK0c2iSTgL/J.TEINVtVBF132CN5wnN81cy*izQJFts2Vok6lwSZmE!/r/dPqtXmUNHQAA)
![paint.jpg](http://r.photo.store.qq.com/psb?/V11QrK0c2iSTgL/NcLln.m2NCyGva3uzPpN5akVM7RukeG0IjUCG5gUOC0!/r/dCKW72XwBAAA)
![system.jpg](http://b170.photo.store.qq.com/psb?/V11QrK0c2iSTgL/s95FgzW*CJGZnT3hgOaydV9DzUzUJNAGpVs4OFGZdPc!/b/dC0vYGUFHwAA&bo=sADcAAAAAAABAEs!&rf=viewer_4)
![broswer.jpg](http://b173.photo.store.qq.com/psb?/V11QrK0c2iSTgL/AmLlt2JKbVjv46C9nFtRxF9BXeCvj53mh1gB2MwY3OQ!/b/dJ1uJWc*BAAA&bo=sADcAAAAAAABAEs!&rf=viewer_4)
![main.jpg](http://b171.photo.store.qq.com/psb?/V11QrK0c2iSTgL/aqKQHAbRsBFzIIUhszXXKQTDqBpZ8JZqZ0Wq0OomoEk!/b/dP827mWqBAAA&bo=sADcAAAAAAABAEs!&rf=viewer_4)
![font.jpg](http://b170.photo.store.qq.com/psb?/V11QrK0c2iSTgL/e*4OMJv2rvSlCE3bHLksdFrxG9BJbXJ1iKL5pJWDObs!/b/dCT*VmU7HgAA&bo=sADcAAAAAAABAEs!&rf=viewer_4)
![config.jpg](http://b171.photo.store.qq.com/psb?/V11QrK0c2iSTgL/zMrP17y*YRM5R39ZfMxyH9wzdDxg29AUWYmuiHryICM!/b/dHTo.2WVBAAA&bo=sADcAAAAAAABAEs!&rf=viewer_4)

#### 硬件相关资料
https://github.com/PlainJi/MP3PlayerHardware

#### 注意事项
焊接TFT前，先在FPC粘上3M胶，同时在tft面板上粘上3M胶  
MP3播放背景图片尺寸（176x132）  
MP3工作门槛电压设置为3.40V（再3.3V左右会出现播放卡顿现象，机器已经不能正常工作）  
已经编译好的HEX文件可以直接下载使用：https://github.com/PlainJi/MP3Player/blob/master/OBJ/MP3_PLAYER.hex

#### 遗留问题
MP3音量调节过于灵敏  
TFT读点时，屏幕产生雪花噪声  
mp3播放读取时间有时不稳定  
电子书APP,阅读进度跳转功能？  

#### 历史记录
2.9  
返校继续完善代码  
12.21  
调节音量OK  
12.19
文件浏览器  
12.12  
添加了截屏函数  
添加了获取剩余容量的函数
12.11
添加了MP3播放界面  
12.10  
通过两个对称数组完成了读点函数！！  
改进了DrawBMP函数（掠过0xffff的点，效果不错）  
编写了复制色块函数  
编写了叠加显示效果  
12.9  
修正了TFT画图，画色块的bug（设置坐标函数有误，已更正）  
测试了MP3播放码率，已突破320Kbps！！  
制作了文件浏览器界面  
12.8  
完成了LCD的文字显示部分  
添加了内存管理模块、SD卡驱动模块、spiflash模块  
完成了FATFS的移植，经实测，读取速度为83kb/s(85504bytes/s,6ms读一个扇区，而32字节能支持多少？)  

#### SPI FLSAHS规划
GBK16_12.BIN      0x0 ~ 0x1474e0        大小：1340640bytes  
ICO.BIN				    0X148000 ~ 0x1661f0   大小：123376bytes  
gImage_pause      3872  
gImage_play       3872  
gImage_exit       2048  
gImage_list       2048  
gImage_pre        2048  
gImage_next       2048  
gImage_music      5000  
gImage_ebook      5000  
gImage_picture    5000  
gImage_browser    5000  
gImage_attach     5000  
gImage_sys        5000  
gImage_BackWhite  77440  

#### TFT 规划
12×12                                    16×16  
横向 176 12×14=168……8(29个余2)         16×11=176……0(22个)  
纵向 220 12×18=216……4()                16×13=208……12  
全篇共252个汉字                         全篇共143个汉字  
共504个英文                             共286个英文字母  

#### 文件浏览器屏幕规划
1  标题（电子书、电子相）               （白字，居中显示，蓝色背景）  
2  路径                               （黑字，灰色背景，最多29个字符)  
3  图标（16×16）文件名  
4  图标（16×16）文件名                 （未选中：黑字，白色背景）  
5  图标（16×16）文件名                 （选中：白字，深蓝色背景）  
6  图标（16×16）文件名                 （12×160，最多12.5个汉字，25个ASCII，余10像素做滑块）  
7  图标（16×16）文件名  
8  图标（16×16）文件名  
9  ---12345678.MP3    0.12M  
   |||||||||||||||||||||||||||||  
10  文件夹:000/000 文件:000/000               (SIZE:3位有效数字)  
11  确定                    返回              (同第一行)  

#### 测试过的函数
//	TEST_RAM();  
//	TEST_TFTCopy();  
//	TEST_LCDAlpha();  
//	TEST_SD();  
//	TEST_DISKIO();  
//	TEST_FATFS_READ();  
//	TEST_FATFS_Sudu();  
//	TEST_FATFS_WRITE();  
//	TEST_VS1003MP3();  



