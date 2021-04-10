Tuya IoTOS Embedded Wi-Fi and BLE SDK for bk7231n

[中文版](README_zh.md) | [English](README.md)

## Overview

Developed independently by Tuya Smart, Tuya IoTOS is the world's only IoT operating system covering all levels of IoT sensing, interruption, network, platform, and application. Benefiting from Tuya Smart's accumulation in the IoT industry, TuyaIoTOS provides solutions for a full range of products from product design, R&D, to post-operation.

Tuya IoTOS embedded SDK is an important part of Tuya IoTOS. By virtue of dedicated design, it provides customers with unified APIs, rich SDKs, and DIY functions, enhancing the integrality of the IoT industry. It can be applied to industrial IoT, vehicle networking, security monitoring, outing, and smart home development.

Features of Tuya IoTOS embedded SDK

* Flexible options: Tuya IoTOS embedded SDK provides rich SDKs covering the core of the IoTOS, general IoT functions, and IoT technology stacks in different fields. Developers can select the solutions as needed, and competent developers can make in-depth customization online as needed.
* Cross-platform: The design of Tuya IoTOS embedded SDK supports cross-hardware platforms and operating systems. It can be migrated to any hardware chip and system, which greatly improves the development efficiency.
* Security and privacy: Tuya IoTOS embedded SDK features secure data storage, secure network communication, identity verification, secure startup, and security upgrades, and strict compliance with the security and privacy policies around the world.
* Rich ecology: Tuya Smart is cooperating closely with major chip manufacturers, and providing over 100 models for developers. Furthermore, all products developed in Tuya IoTOS can be linked. Powered by Tuya has achieved a complete ecosystem of the IoT industry.



## Quick start

### Dependency
Some package dependencies need to be installed before compiling.
#### Debian/Ubuntu
``` bash
$ sudo apt-get install wget git python libc6-i386 
```


#### Windows

Supports compilating in Cygwin, MinGW, WSL2 and more environments

### Get SDK
Make sure to get SDK through git recursion:
``` bash
$ git clone --recursive https://github.com/tuya/tuya-iotos-embeded-sdk-wifi-ble-bk7231n.git
```
or
``` bash
$ git clone https://github.com/tuya/tuya-iotos-embeded-sdk-wifi-ble-bk7231n.git
$ git submodule update --init
```

### Compile

The compile project is in the apps directory, you can quickly compile, burn, and run the process through `tuya_demo_template`. Use the following command in the root directory to compile: 
``` bash
$ sh build_app.sh apps/tuya_demo_template tuya_demo_template 1.0.0
```
apps/tuya_demo_template: compile project path

tuya_demo_template: compile project name

1.0.0: The version of the current compilation firmware

### Compilation cleanup

You can clear the compiled intermediate files with the following commands:

``` bash
$ sh build_app.sh apps/tuya_demo_template tuya_demo_template 1.0.0 clean
```

### Burning authorization

For more information about firmware burning authorization, see [Burn and Authorize WB Series Modules](https://developer.tuya.com/en/docs/iot/device-development/burn-and-authorization/burn-and-authorize-wifi-ble-modules/burn-and-authorize-wb-series-modules?id=Ka78f4pttsytd)

### Evaluation kits

For more information about SDK, see [Sandwich Evaluation Kits](https://developer.tuya.com/en/docs/iot/device-development/tuya-development-board-kit/tuya-sandwich-evaluation-kits/-tuya-sandwich-evaluation-kits?id=K97o0ixytemvr)



## Technical support

You can get support from Tuya through the following accesses: 
- [IoT Developer Platform](https://developer.tuya.com/en/)
- [Help Center](https://support.tuya.com/en/help)
- [Service & Support](https://service.console.tuya.com/)
