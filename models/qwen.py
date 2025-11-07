import requests


API_URL = "https://api.gitcode.com/api/v5/chat/completions"
MODEL_NAME = "hf_mirrors/Qwen/Qwen3-235B-A22B-Instruct-2507"

def send_request(system_prompt, history, prompt, api_key):
    contents = []
    contents.append({"role": "system", "content": system_prompt})
    for msg in history:
        contents.append({
            "role": msg["role"],
            "content": msg["content"]
        })
    contents.append({"role": "user", "content": prompt})
    headers = {
        "Authorization": f"Bearer {api_key}",
    }
    payload = {
        "messages": contents,
        "model": MODEL_NAME,
        "stream": False
    }

    res = requests.post(API_URL, headers=headers, json=payload)
    res.raise_for_status()
    result = res.json()
    if "choices" in result and len(result["choices"]) > 0:
        return result["choices"][0]["delta"]["content"]
    return None