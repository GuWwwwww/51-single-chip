# 51-single-chip
‼️**本仓库内的所有的代码基于普中51单片机开发板所写**‼️([原理图](https://github.com/GuWwwwww/51-single-chip/tree/main/普中-2%26普中-3%26普中-4开发板原理图.pdf))
## 📂Repository Structure
仓库中的结构分为基础模块代码编写和相关的设计应用。

1️⃣基础模块：LED模块([LED](https://github.com/GuWwwwww/51-single-chip/tree/main/LED))、按键模块([keystroke](https://github.com/GuWwwwww/51-single-chip/tree/main/keystroke))、七段数码管([seg_led](https://github.com/GuWwwwww/51-single-chip/tree/main/seg_led))、LCD显示模块([LCD1602](https://github.com/GuWwwwww/51-single-chip/tree/main/LCD1602))、温度传感器模块([DS18b20](https://github.com/GuWwwwww/51-single-chip/tree/main/DS18b20))、定时器模块([time_interrupt](https://github.com/GuWwwwww/51-single-chip/tree/main/time_interrupt))、PWM模块([PWM](https://github.com/GuWwwwww/51-single-chip/tree/main/PWM))、PID控制([PID](https://github.com/GuWwwwww/51-single-chip/tree/main/PID))和uart串口([uart](https://github.com/GuWwwwww/51-single-chip/tree/main/uart))。

2️⃣应用设计：多功能计算器，密码锁和智能鱼缸喂食系统设计。

其中，基础代码模块包括.hex文件和工程文件等，应用设计部分只有.C文件。

⚠️**下面主要介绍应用设计部分**

## 1. 智能鱼缸喂食系统
在该设计中，采用 LCD1602、独立按键、DS1302 时钟、28BYJ-48 步进电机、ESP8266-01S 模块，实现了手动/自动/WI-FI 远程喂鱼。

**电路使用元件及接线图如下：**
![电路使用元件](https://raw.githubusercontent.com/GuWwwwww/51-single-chip/main/智能鱼缸喂食系统设计/电路使用元件.png)

如下图所示，设置四个独立按键，按键 1 可以切换模式（正常运行模式、调整时间模式、设置定时模式）。在正常运行模式下，LCD 显示当前时间（年-月-日、时:分:秒、星期）、手动/自动模式和小/中/大投食量，按键 2 可以修改投食量，按键 3 可以修改投喂模式，按键 4 为手动投食按钮。在调整时间模式下，LCD 显示当前时间，按键 2 可以选择调整的位，并带有闪烁功能，按键 3 为加键，调整时间。在设置定时模式下，LCD 显示当前设置的定时时间，按键 2选择调整时分秒的位，按键 3 为加键。 

然后，步进电机模拟喂食过程，正转半圈打开喂食阀门，反转半圈关闭喂食阀门。喂食量通过定时器设置步进电机的正反转的时间间隔模拟，小量为 2s，中量为 3s，大量为 4s。自动喂食也是通过定时器来实现，当设置的定时时间结束后，开始喂食，然后定时时间自动恢复到初始设置，从而实现定时循环喂食。 

在实现 Wi-Fi 远程控制喂食的场景下，将 ESP8266-01S 配置为接入点（AP）服务器模式。此时，手机连接至 ESP8266 的 Wi-Fi 信号，随后建立 TCP 连接以进行信息传输。ESP8266 通过串口与 51 单片机进行通信交互，以此实现手机对鱼缸喂食的远程控制功能，同时能够接收来自单片机的反馈信息。 

![按键与LCD显示](https://raw.githubusercontent.com/GuWwwwww/51-single-chip/main/智能鱼缸喂食系统设计/按键与LCD显示模块.png)

**未来改进想法：**

加入温度传感器 DS18B02 模块和加热继电器，实时检测水温并控制水温恒定；加入压力传感器，实时检测喂食器内的食物剩余量，如果低于设定的阈值 LED 灯变亮并通过 TCP 传输信息给手机；加入水质检测传感器和水泵模块，如果浑浊度过高，自动抽水换水等。
