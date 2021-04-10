Tuya IoTOS Embeded SDK WiFi & BLE for BK7231N

[English](https://github.com/tuya/tuya-iotos-embeded-sdk-wifi-ble-bk7231n/blob/master/README.md) | [中文版]

## 简介

涂鸦 IoTOS 是涂鸦智能自主开发的，全球唯一的覆盖物联网感知、中断、网络、平台、应用各个层次的物联网操作系统。受益于涂鸦智能在物联网行业的积累，涂鸦IoTOS提供了全品类的从产品设计、研发、接入到后期运营解决方案。

涂鸦IoTOS embedded SDK是涂鸦 IoTOS 的重要组成部分，通过良好的设计，为客户提供了统一的开发接口，丰富的开发套件以及自助化定制功能，屏蔽了物联网行业的碎片化特征，可以应用于工业物联网、车联网、安防监控、户外出行、智能家居等行业的物联网应用开发。

涂鸦IoTOS embedded SDK具有以下特点：

* 弹性裁剪：涂鸦IoTOS embedded SDK提供了丰富的开发套件，涵盖了物联网操作系统内核、通用物联网功能、不同领域的物联网技术栈，开发者可以根据自己的产品的需求进行选择对应的解决方案，有能力的开发者，可以在线按需进行深度的定制。
* 跨平台：涂鸦IoTOS embedded SDK设计层面支持了跨硬件平台、操作系统的特性，可以在任意的硬件芯片和系统上进行移植，极大的提高了开发者的开发效率。
* 安全隐私：涂鸦IoTOS embedded SDK提供了安全数据存储、安全网络通信、身份验证、安全启动、安全升级等特性，并严格遵守全球各地的安全、隐私政策，有效的保障产品的安全、隐私特性。
* 丰富生态：涂鸦智能和各大芯片原厂进行了密切的合作，导入了100+款供开发者选择，同时所有使用涂鸦IoTOS 开发的产品都能够进行互联互通，场景联动，Powered by Tuya已经形成了完整的物联网产业生态。



##  快速上手

### 依赖
在编译前需要安装部分软件包依赖:
#### Debian/Ubuntu
``` bash
$ sudo apt-get install wget git python libc6-i386 
```


#### Windows

支持Cygwin、Mingw、WSL2等环境下编译

### 获取SDK
请确认使用git递归模式clone获取SDK：
``` bash
$ git clone --recursive https://github.com/tuya/tuya-iotos-embeded-sdk-wifi-ble-bk7231n.git
```
或者
``` bash
$ git clone https://github.com/tuya/tuya-iotos-embeded-sdk-wifi-ble-bk7231n.git
$ git submodule update --init
```

### 编译

编译工程位于apps目录下，你可以使用`tuya_demo_template`工程快速完成编译、烧录、运行流程，在根目录下使用以下命令编译：
``` bash
$ sh build_app.sh apps/tuya_demo_template tuya_demo_template 1.0.0
```
apps/tuya_demo_template：编译工程路径

tuya_demo_template：编译工程名称

1.0.0：当前编译固件版本，可根据实际情况自行修改

### 编译清除

如需清除编译中间文件，可使用以下命令：

``` bash
$ sh build_app.sh apps/tuya_demo_template tuya_demo_template 1.0.0 clean
```

### 烧录授权

固件烧录授权相关信息请参考：[Wi-Fi + BLE 系列模组烧录授权](https://developer.tuya.com/cn/docs/iot/device-development/burn-and-authorization/burn-and-authorize-wifi-ble-modules/burn-and-authorize-wb-series-modules?id=Ka78f4pttsytd)

### 开发套件

开发板套件相关详情请参考：[三明治开发套件](https://developer.tuya.com/cn/docs/iot/device-development/tuya-development-board-kit/tuya-sandwich-evaluation-kits/-tuya-sandwich-evaluation-kits?id=K97o0ixytemvr)



## 技术支持

您可以通过以下方法获得涂鸦的支持:
- 开发者中心：https://developer.tuya.com/cn/
- 帮助中心: https://support.tuya.com/en/help
- 技术支持工单中心: [https://service.console.tuya.com](https://service.console.tuya.com/)