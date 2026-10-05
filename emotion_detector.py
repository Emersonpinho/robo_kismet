import os
import cv2
import serial
import time
import pygame
from fer.fer import FER

# --- configuração da porta serial ---
PORTA = "COM6"
arduino = serial.Serial(PORTA, 9600)
time.sleep(2)  # espera o Arduino reiniciar após abrir a conexão

# --- tradutor de emoção FER -> comando Arduino ---
MAPA_EMOCOES = {
    "happy": "H",
    "sad": "S",
    "angry": "A",
    "neutral": "N",
    "surprise": "U",
    "fear": "N",      # sem carinha própria (se quiser, troque por "U")
    "disgust": "N",   # sem carinha própria
}

# --- áudios das falas (gerados antes pelo gerar_audios.py) ---
PASTA_AUDIOS = os.path.join(os.path.dirname(os.path.abspath(__file__)), "audios")

pygame.mixer.init()

def falar(comando):
    # não atropela uma fala que já está tocando
    if pygame.mixer.music.get_busy():
        return

    caminho = os.path.join(PASTA_AUDIOS, f"{comando}.mp3")
    if not os.path.exists(caminho):
        print(f"Áudio não encontrado: {caminho}")
        return

    pygame.mixer.music.load(caminho)
    pygame.mixer.music.play()   # toca em segundo plano, não trava a webcam

def enviar_comando(comando, emocao):
    arduino.write(comando.encode())
    print(f"Emoção: {emocao} -> Comando enviado: {comando}")
    falar(comando)

# --- detecção via webcam ---
# se a câmera do Iriun não abrir, descubra o índice certo e troque aqui
# (ex.: cv2.VideoCapture(1, cv2.CAP_DSHOW))
captura = cv2.VideoCapture(3)
detector = FER(mtcnn=False)

if not captura.isOpened():
    print("Não consegui abrir a webcam!")

INTERVALO_MINIMO = 3.0      # segundos de "trava" depois de uma troca
TEMPO_CONFIRMACAO = 0.7     # a emoção nova precisa durar isso pra valer

comando_atual = None        # última letra enviada
ultima_troca = 0
candidato = None            # letra que está "pedindo pra entrar"
candidato_desde = 0

while True:
    ret, frame = captura.read()
    if not ret:
        break

    resultado = detector.detect_emotions(frame)

    if resultado:
        emocoes = resultado[0]["emotions"]
        emocao_dominante = max(emocoes, key=emocoes.get)
        comando = MAPA_EMOCOES.get(emocao_dominante, "N")

        agora = time.time()

        if comando == comando_atual:
            # nada mudou, cancela qualquer candidato
            candidato = None
        elif comando != candidato:
            # letra nova apareceu: começa a contar o tempo dela
            candidato = comando
            candidato_desde = agora
        else:
            # mesma candidata de antes: ela já durou o suficiente?
            confirmada = (agora - candidato_desde) >= TEMPO_CONFIRMACAO
            liberado = (agora - ultima_troca) >= INTERVALO_MINIMO
            if confirmada and liberado:
                enviar_comando(comando, emocao_dominante)
                comando_atual = comando
                ultima_troca = agora
                candidato = None

        (x, y, w, h) = resultado[0]["box"]
        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
        cv2.putText(frame, emocao_dominante, (x, y - 10),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 255, 0), 2)
    else:
        # sem rosto na imagem: zera o candidato
        candidato = None

    cv2.imshow("Deteccao de emocao", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

captura.release()
cv2.destroyAllWindows()
pygame.mixer.quit()
arduino.close()