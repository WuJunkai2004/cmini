import requests

BASE_URL = "https://generativelanguage.googleapis.com/v1beta"
MODEL_NAME = "models/gemini-flash-latest"

def send_request(prompt_list, system_prompt, api_key):
    print("gemini")
    url = f"{BASE_URL}/{MODEL_NAME}:generateContent"
    headers = {
        "Content-Type": "application/json",
        "X-goog-api-key": api_key
    }
    payload = {
        "contents": prompt_list,
        "systemInstruction": {"parts": [{"text": system_prompt}]} if system_prompt else None
    }

    res = requests.post(url, headers=headers, json=payload)
    res.raise_for_status()
    result = res.json()
    print(f"result:{result}")
    if "candidates" in result and len(result["candidates"]) > 0:
        return result["candidates"][0]["content"]["parts"][0]["text"]
    return None