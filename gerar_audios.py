import asyncio
import os
import edge_tts

# rode este script UMA vez, com internet, para gerar os mp3 das falas.
# na apresentação o detector só toca os arquivos, sem precisar de Wi-Fi.

VOZ = "pt-BR-AntonioNeural"   # para voz feminina: "pt-BR-FranciscaNeural"

FRASES = {
    "H": "Que bom te ver feliz!",
    "S": "Ah, você parece triste. Estou aqui com você.",
    "A": "Opa, alguém está bravo!",
    "N": "Olá! Eu sou o robô Kismet.",
    "U": "Nossa, que susto!",
}

PASTA = os.path.join(os.path.dirname(os.path.abspath(__file__)), "audios")
os.makedirs(PASTA, exist_ok=True)

async def gerar():
    for letra, texto in FRASES.items():
        caminho = os.path.join(PASTA, f"{letra}.mp3")
        await edge_tts.Communicate(texto, VOZ).save(caminho)
        print(f"Gerado: {caminho}")

asyncio.run(gerar())
print("Pronto! Todos os áudios foram gerados.")