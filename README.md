# CMINI
## Overview
这是一个模仿gemini cli(Command Line Interface 命令行界面)，用C语言编写的工具。它是基于共享内存、信号和子进程通信机制的简易聊天客户端与服务端框架，模拟了一个命令行下的“聊天机器人客户端”，通过HTTP请求与远程服务器交互。

## Moduels
这里是后端
```
- server
  - ...
- chat.py
- login.py
```

## Usage
1. 设置环境变量  
创建文件run.sh
```
#!/bin/bash

export GEMINI_API_KEY=你的API密钥
python -m server
```
关于API密钥，请去[Ai Studio](https://aistudio.google.com/)申请。

2. 运行服务端  
```bash
bash run.sh
```

3. 使用客户端  
启动后，可以访问[localhost:15444](localhost:15444), 来确认服务端是否启动。
然后，请仅在主目录下新增或者修改python脚本。
