import requests

BASE_URL = "https://generativelanguage.googleapis.com/v1beta"
MODEL_NAME = "models/gemini-flash-latest"
URL = f"{BASE_URL}/{MODEL_NAME}:generateContent"

def send_request(system_prompt, history, prompt, api_key):
    contents = []
    for msg in history:
        contents.append({
            "role": msg["role"],
            "parts": [{"text": msg["content"]}]
        })
    contents.append({"role": "user", "content": prompt})
    headers = {
        "Content-Type": "application/json",
        "X-goog-api-key": api_key
    }
    payload = {
        "contents": contents,
        "systemInstruction": {
            "parts": [{"text": system_prompt}]
        }
    }

    res = requests.post(URL, headers=headers, json=payload)
    res.raise_for_status()
    result = res.json()
    if "candidates" in result and len(result["candidates"]) > 0:
        return result["candidates"][0]["content"]["parts"][0]["text"]
    return None