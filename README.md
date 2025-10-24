# CMINI
## Overview
这是一个模仿gemini cli(Command Line Interface 命令行界面)，用C语言编写的工具。它是基于共享内存、信号和子进程通信机制的简易聊天客户端与服务端框架，模拟了一个命令行下的“聊天机器人客户端”，通过HTTP请求与远程服务器交互。

## Moduels
- **channels**

    这个文件实现了一个基于 System V 消息队列（System V message queue） 的“通道（channel）”机制，可以理解为一种进程间通信（IPC）的封装。它通过 msgget / msgsnd / msgrcv / msgctl 这些系统调用，**为用户提供了一个简单的“发送消息 / 接收消息”接口**。

- **request**

    这个文件实现了一个基于 TCP socket 的网络请求模块（即简易的客户端请求工具），功能类似于一个简化版的 HTTP 客户端 或 “请求发送器”，封装了 socket 的创建、连接、发送、接收等常见操作。

- **server**

    这个文件实现了一个多客户端 TCP 服务器，使用 select() 实现 多路复用 I/O，可以同时处理多个客户端的连接与消息。

- **sharemem**

    这个文件实现了一个简易版“共享内存 + 信号量锁”的进程间通信（IPC）机制，也就是一个带互斥锁保护的共享内存分配器。

- **subprocess**

    这个文件实现了一个轻量级的子进程与信号管理模块，可以理解为是对 Linux 进程控制相关系统调用（fork、pipe、exec、waitpid、sigaction、kill 等）的一个“封装库”。

- **terminst**

    这个文件实现了一个 基于终端（Terminal）的轻量级图形界面框架，包含了键盘输入、光标控制、窗口绘制等功能，让程序在命令行中动态显示内容。

- **main**

    

