from . import gemini, qwen

MODEL_REGISTRY = {
    "gemini": gemini,
    "qwen": qwen,
}

def get_model_adapter(model_name):
    if model_name in MODEL_REGISTRY:
        return MODEL_REGISTRY[model_name]
    raise ValueError(f"Unsupported model: {model_name}")