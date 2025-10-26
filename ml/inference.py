from typing import Annotated
import io

from PIL import Image
from fastapi import FastAPI, File, UploadFile
from torchvision import transforms
import torch
import torch.nn as nn
import torch.nn.functional as F

app = FastAPI()

class MLP(nn.Module):
    def __init__(self):
        super().__init__()
        input_size = 28*28
        hidden_size = 256
        output_size = 10 
        self.fc1 = nn.Linear(input_size, hidden_size)
        self.fc2 = nn.Linear(hidden_size, hidden_size)
        self.fc3 = nn.Linear(hidden_size, output_size)
    def forward(self, x):
        x = x.view(x.size(0), -1)
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        return self.fc3(x)

model = MLP()
model.load_state_dict(torch.load('model'))
model.eval()

classes = ('T-shirt/top', 'Trouser', 'Pullover', 'Dress', 'Coat',
           'Sandal', 'Shirt', 'Sneaker', 'Bag', 'Ankle Boot')


# Preprocessing for MNIST
preprocess = transforms.Compose([
        transforms.Resize((28, 28)),   # resize to 28x28
        transforms.ToTensor(),         # converts to shape (1, 28, 28) and values [0,1]
        transforms.Normalize((0.5,), (0.5,))  # optional normalization
    ])

@app.post("/guess/")
async def create_file(file: UploadFile):
    image_bytes = await file.read()
    image = Image.open(io.BytesIO(image_bytes)).convert("L")
    img_tensor = preprocess(image)
    img_tensor = img_tensor.unsqueeze(0) # add batch dim -> (1, 1, H, W)
    with torch.no_grad():
        output = model(img_tensor)
        predicted_class = output.argmax(dim=1).item()
        return {"predicted_class": classes[predicted_class]}
