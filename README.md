# FFMediaPlayer
## todo
美白加速的优化
入队前美白，提前转换rgba，美白磨皮后再入队，出队后只管渲染的事了
自建线程池，类似loop这种，for循环作为任务代码输入，避免重复创建和销毁线程

project 采用 Android MediaPlayer生命周期 + Gstreamer pipeline的思想框架来构建.
- 采用Android MediaPlayer生命周期的原因
  方便上层应用在Android MediaPlayer和project间进行切换, 至于为什么要切换有几方面的考虑:
1. 本人能力有限,做出来的东西比不上Android MediaPlayer
2. 大部分应用是基于MediaPlayer, 如果有需求MediaPlayer无法实现而本project提供了实现的方法,能够方便地切换
3. 在性能上,能够直接和MediaPlayer进行对比

- 采用Gstreamer pipeline思想框架的原因
用户可以通过自己的需求把不同的Elements 排列组合，形成一个又一个的不同的pipeline,具有很高的灵活性.在FFmpeg中, 一般解封装,编解码,播放模块相对固定,avfilter模块功能比较丰富, 用户能够根据需求实现不同的filter模块,而其他模块不需要改变,能够很方便地组成新的功能的播放器. 比如一个需求是希望能够对视频进行旋转, 后来又来个一个需求要在视频中加水印,这时只要把视频旋转的filter替换成水印的filter就可以了.

本来是希望写完一个比较完整的project再公布出来,但在实现过程中发现自己能力有限碰到了不少问题, 如果拖得太久自己又失去了写的动力, 所以就边写边发布,目标也从一个完整的project到先实现一个能够运行的project,后面在一步步完善.

# 如何引用
Step 1. Add the JitPack repository to your build file
```
allprojects {
		repositories {
			...
			maven { url 'https://jitpack.io' }
		}
	}
```  
Step 2. Add the dependency
```
dependencies {
	        implementation 'com.github.yizhongliu:FFMediaPlayer:v1.0'
	}
```

# 工程博客介绍
工程地址:
[https://github.com/yizhongliu/FFMediaPlayer](https://github.com/yizhongliu/FFMediaPlayer)
专题的介绍:
[https://www.jianshu.com/p/c20cc21e29df](https://www.jianshu.com/p/c20cc21e29df)


