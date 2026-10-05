import asyncio
import edge_tts
import pygame

TEXTO = "Olá! Eu sou o robô Kismet. Que bom te ver feliz!"
VOZ = "pt-BR-AntonioNeural"   # voz masculina; para feminina: pt-BR-FranciscaNeural

async def gerar():
    comunicar = edge_tts.Communicate(TEXTO, VOZ)
    await comunicar.save("teste.mp3")

asyncio.run(gerar())

pygame.mixer.init()
pygame.mixer.music.load("teste.mp3")
pygame.mixer.music.play()
while pygame.mixer.music.get_busy():
    pygame.time.wait(100)