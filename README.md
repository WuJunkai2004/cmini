# CMINI
## Overview
这是一个模仿 Gemini CLI（Command Line Interface 命令行界面）的工具，项目里包含一个简易的聊天客户端与服务端框架，用以通过 HTTP 与远程模型服务交互。项目的演示旨在展示如何在本地启动服务并通过命令行与模型进行交互。

## 后端模块
项目后端主要文件：

```
- server/
  - __main__.py
  - decorators.py
  - verapi.py
  - vercel.py
- models/
  - __init__.py
  - gemini.py
  - qwen.py
  - ...
- chat.py
- login.py
```

## secret.toml（本地密钥文件）
仓库中使用一个 `secret.toml` 文件来管理可用模型和对应的 API 密钥。示例：

```
models = ["gemini", "qwen"]

[gemini]
api_key = "<your-gemini-api-key>"

[qwen]
api_key = "<your-qwen-api-key>"
```

重要提示：
- 不要将包含真实密钥的 `secret.toml` 提交到公共仓库。建议将其加入 `.gitignore`。

## Usage（运行说明）

1) 使用以下命令启动服务
``shell
python -m server
```

2) 验证服务

启动后，你可以访问 http://localhost:15444/login 查看服务是否正常运行。  
如果服务启动成功，应当能看到一个短的字符串作为响应。

3) 其它说明

- `secret.toml` 作为本地密钥的保存方式，应确保该文件不被提交至公共仓库。建议在项目根目录加入 `.gitignore` 并写入 `secret.toml`。