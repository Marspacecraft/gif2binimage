# gif2binimage
 裁剪gif文件生成单色图片数组bin文件


## 描述
    根据gif生成二进制的单色图片矩阵文件，可以用于单色oled，lcd，墨水屏等的显示。

## bin文件结构
    文件头+连续数据帧
    文件头：bia_t结构体。保存数据一些必要的信息
    数据帧大小：width*height/8


## 依赖
* SDL2库
* [gifdec](https://github.com/lecram/gifdec)

## 编译
* 安装SDL2
* 下载gifdec到gifdec目录
* 执行make

    已经在mac上测试，其他平台没有测试。

## 用法
    执行 `make help` 或 直接运行

usage:

  `./gif2binimage gifurl arrangemode MSB rotate flip width height `

	gifurl: gif file url

	arrangemode:
		1:
			Byte1:|bit7...bit0|	Byte2:|bit7...bit0|	Byten:|bit7...bit0|
			Byten+1:|bit7...bit0|	Byten+2:|bit7...bit0|	Byten+n:|bit7...bit0|

		2:
			Byte1	Byten+1
			 ___ 	 ___ 
			bit7 	bit7 
			  .  	.  
			  .  	.  
			  .  	.  
			bit0 	bit0 
			 ___ 	 ___ 
			Byte2	Byten+2
			 ___ 	 ___ 
			bit7 	bit7 
			  .  	.  
			  .  	.  
			  .  	.  
			bit0 	bit0 
			 ___ 	 ___ 
			Byten	Byten+n
			 ___ 	 ___ 
			bit7 	bit7 
			  .  	.  
			  .  	.  
			  .  	.  
			bit0 	bit0 
			 ___ 	 ___ 
		3:
			Byte1:|bit0...bit7|	Byten+1:|bit0...bit7|
			Byte2:|bit0...bit7|	Byten+2:|bit0...bit7|
			Byten:|bit0...bit7|	Byten+n:|bit0...bit7|
		4:
			Byte1	Byte2	Byten
			 ___ 	 ___ 	 ___ 
			bit7 	bit7 	bit7 
			  .  	  .  	  .  
			  .  	  .  	  .  
			  .  	  .  	  .  
			bit0 	bit0 	bit0 
			 ___ 	 ___ 	 ___ 
			Byten	Byten+1	Byten+n
			 ___ 	 ___ 	 ___ 
			bit7 	bit7 	bit7 
			  .  	  .  	  .  
			  .  	  .  	  .  
			  .  	  .  	  .  
			bit0 	bit0 	bit0 
			 ___ 	 ___ 	 ___ 

	MSB: MSB first if 1

	rotate: rotate 0, 90, 180 or 270 degrees clockwise

	flip: flip left and right if 1

key function:

	F1:zoom out
	F2:zoom in
	F3:gray down
	F4:gray up
	F5:creat blank frame
	F6:creat full frame
	F12:creat the real size image on the top left corner of the screen
	UP,DOWN,LEFT,RIGHT:move the camera
	d:delete current frame
	SPACE:save current frame
	q:quit


--------------------------------------

## 测试
* `make exp_1`
* `make show`

## 运行

### 运行
    `./gif2binimage gifurl arrangemode MSB rotate flip width height` 
| 参数 | 含义 |
| :- | :- | 
|gifurl| gif文件路径|
|arrangemode| 屏幕排列方式（1:水平扫描 2:垂直扫描 3:字节垂直扫描，bit水平扫描 4:字节水平扫描，bit垂直扫描）|
|MSB| 高位优先（1:高位优先 0：低位优先）|
|rotate| gif顺时针旋转角度（0，90，180，270）|
|flip| 左右对调（1:左右对调 0:保持原样）|
|width| 输出显示像素宽度|
|height| 输出显示像素高度|

### 运行界面    
    
![](https://github.com/Marspacecraft/gif2binimage/blob/main/image/1.png)    

| 按键 | 功能 |
| :- | :- | 
|F1 |等比例缩小取景框|
|F2 |等比例扩大取景框|
|方向键 |移动取景框|
|F3 |减小取镜框内灰度阈值|
|F4 |增大取景框内灰度阈值|
|F5 |插入空白帧|
|F6 |插入满屏帧|
|F12 |开启或关闭左上角实际大小取景结果|
|空格键 |保存当前帧|
|d |删除当前帧|
｜q｜结束｜

### 运行结果
    运行结束会生成两个文件 binimage.bin 和 binimage.c。bin文件为图像数据；c文件为对应参数，需要配合 binimagearray.h 使用。
![](https://github.com/Marspacecraft/gif2binimage/blob/main/image/3.png)    
   
### 显示bin文件 
    `./binimageshow binimage.bin`
![](https://github.com/Marspacecraft/gif2binimage/blob/main/image/2.png)   
