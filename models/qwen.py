import requests
from openai import OpenAI
import dashscope
from dashscope import Generation
MODEL_NAME = "qwen3-max"



def send_request(prompt_list, system_prompt, api_key):
    print("qwen")
    # 转换为 Qwen 接口格式
    messages = []
    if system_prompt:
        messages.append({"role": "system", "content": system_prompt})
    for msg in prompt_list:
        messages.append({"role": msg["role"], "content": msg["parts"][0]["text"]})


    client = OpenAI( 
        api_key = api_key,        
        base_url = "https://dashscope.aliyuncs.com/compatible-mode/v1",    
    )

    response = client.chat.completions.create(
        model = MODEL_NAME,   
        messages = messages,
    )
    result = (response.choices[0].message.content)
    print(f"result:{result}")

    try:
        return result
    except KeyError:
        return None