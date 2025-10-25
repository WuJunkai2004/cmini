import os
import requests

import server.vercel as vercel

API_KEY = os.getenv("GEMINI_API_KEY", "")
if not API_KEY:
    raise ValueError("GEMINI_API_KEY is not set in environment variables")

BASE_URL = "https://generativelanguage.googleapis.com/v1beta"
MODEL_NAME = "models/gemini-flash-latest"
URL = f"{BASE_URL}/{MODEL_NAME}:generateContent"

# username:
#   - {role: "user"/"assistant"/"system", content: "text"}
history = {}

@vercel.register
def chat(response, headers, data):
    if('raw' not in data.keys() or 'Auth' not in headers.keys()):
        response.send_code(400)
        response.send_json({"error": "Invalid request format"})
        return
    prompt = data['raw']
    user = headers['Auth']
    if user not in history.keys():
        history[user] = [{"role": "system", "content": "你是一个智能助手，正在通过终端与用户对话。请遵循以下要求：\n\n# 回答风格\n- 回答要简洁明了，直接切入重点\n- 使用中文回答，语言自然流畅\n- 避免冗长的解释，优先提供实用信息\n\n# 格式要求\n- 只能使用#开头作为标题（会显示为粗体）\n- 不要使用其他markdown格式（如**粗体**、*斜体*、```代码块```等）\n- 代码直接展示，不要用代码块包围\n- 列表使用简单的-或数字开头\n\n# 内容要求\n- 针对技术问题提供准确的解决方案\n- 对于编程相关问题，给出可直接使用的代码\n- 回答控制在合理长度内，避免刷屏"}]
    # 添加当前用户输入到历史记录
    history[user].append({"role": "user", "content": prompt})

    # 构建完整的对话历史
    contents = []
    system_prompt = None

    for msg in history[user]:
        if msg["role"] == "system":
            system_prompt = msg["content"]
        elif msg["role"] == "user":
            contents.append({
                "role": "user",
                "parts": [{"text": msg["content"]}]
            })
        elif msg["role"] == "assistant":
            contents.append({
                "role": "model",
                "parts": [{"text": msg["content"]}]
            })

    headers = {
        "Content-Type": "application/json",
        "X-goog-api-key": API_KEY
    }
    payload = {
        "contents": contents,
        "systemInstruction": {
            "parts": [{"text": system_prompt}]
        } if system_prompt else None
    }
    response.send_code(200)
    response.send_header("Content-Type", "text/plain; charset=utf-8")
    try:
        res = requests.post(URL, headers=headers, json=payload)
        res.raise_for_status()
        result = res.json()
        if "candidates" in result and len(result["candidates"]) > 0:
            content = result["candidates"][0]["content"]["parts"][0]["text"]
            # 将AI回复添加到历史记录
            history[user].append({"role": "assistant", "content": content})
            # 发送回复
            response.send_text(content)
            return
        else:
            # 错误，未收到有效回复，删去
            if history[user][-1]["role"] == "user":
                del history[user][-1]
            response.send_text("No valid response from AI")
            return
    except Exception as e:
        response.send_text(f"Error: {str(e)}")
        return
