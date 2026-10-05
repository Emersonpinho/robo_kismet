## 🎥 Demonstração

[![Robô Kismet em ação](https://img.youtube.com/vi/wecirKYgohA/maxresdefault.jpg)](https://youtu.be/wecirKYgohA)

▶️ Clique na imagem para assistir ao robô reagindo às emoções no YouTube.

 🤖 Robô Kismet: detector de emoções

Robô que **olha para o rosto de uma pessoa pela webcam, descobre a emoção dela e reage**: muda a carinha numa tela colorida, faz um coração bater num display OLED, mexe os bracinhos com servos e ainda fala uma frase em português.

Projeto desenvolvido para a **Semana de Tecnologia 2026** do **IFPB**.

> Autores: Emerson pinho

---

## 📑 Sumário

1. [Como o projeto funciona](#-como-o-projeto-funciona)
2. [O que o robô faz em cada emoção](#-o-que-o-robô-faz-em-cada-emoção)
3. [Hardware necessário](#-hardware-necessário)
4. [Ligações (conexões no Arduino)](#-ligações-conexões-no-arduino)
5. [Estrutura do repositório](#-estrutura-do-repositório)
6. [Instalação: Arduino](#-instalação-arduino)
7. [Instalação: Python](#-instalação-python)
8. [Como rodar](#-como-rodar)
9. [Detalhes do código Arduino](#-detalhes-do-código-arduino)
10. [Detalhes do código Python](#-detalhes-do-código-python)
11. [Voz do robô](#-voz-do-robô)
12. [Problemas comuns](#-problemas-comuns)
13. [Histórico de desenvolvimento](#-histórico-de-desenvolvimento)
14. [Limitações e próximos passos](#-limitações-e-próximos-passos)

---

## 🧠 Como o projeto funciona

O projeto tem duas partes que conversam por cabo USB, como um **cérebro** (notebook) e um **corpo** (Arduino):

```
┌──────────────────────────── NOTEBOOK (Python) ───────────────────────────┐
│                                                                           │
│  Webcam ──► FER (detecta a emoção) ──► filtro anti-tremedeira ──┬──► letra│
│                                                                 │         │
│                                                                 └──► toca │
│                                                                   áudio   │
└────────────────────────────────────┬──────────────────────────────────────┘
                                     │ Serial USB (9600 baud): 1 letra
                                     ▼
┌───────────────────────────── ARDUINO (C++) ──────────────────────────────┐
│  recebe a letra ──► rosto (TFT) + coração (OLED) + braços (servos)       │
└───────────────────────────────────────────────────────────────────────────┘
```

1. A **webcam** captura imagens.
2. A biblioteca **FER** diz qual emoção domina no rosto (feliz, triste, brava, etc.).
3. Um **filtro** só aceita a emoção nova se ela durar um tempo mínimo e se já passou um intervalo desde a última troca. Isso evita que o robô fique "piscando de emoção" a cada frame.
4. O Python manda **uma letra** pela porta serial e **toca um áudio** com a fala da emoção.
5. O Arduino lê a letra e muda **rosto, coração e braços**.

### Protocolo serial

O Python manda uma única letra. O Arduino aceita maiúscula ou minúscula (usa `tolower`).

| Letra | Emoção |
|---|---|
| `N` | Normal (neutro) |
| `H` | Feliz (happy) |
| `S` | Triste (sad) |
| `A` | Brava (angry) |
| `U` | Surpresa (surprise) |

---

## 🎭 O que o robô faz em cada emoção

| Emoção | Rosto (tela TFT) | Coração (OLED) | Braços (servos) |
|---|---|---|---|
| **Normal (N)** | Olhos arredondados com a parte de cima recortada | Contorno, batendo a cada 700 ms | Parados embaixo |
| **Feliz (H)** | Olhos fechados em "^ ^" e boca aberta sorrindo | Cheio, batendo a cada 400 ms | Sobe e acena com os dois juntos, 4 balanços |
| **Triste (S)** | Olhos inclinados para baixo e boca virada para baixo | Cheio com uma **rachadura**, batida lenta (1000 ms) | Descem devagar e dão uns "soluços" no final |
| **Brava (A)** | Olhos inclinados para dentro e boca reta | Cheio, batida rápida (250 ms) e **tremendo** de leve | Alternam rápido, como se socassem o ar |
| **Surpresa (U)** | Olhos e boca em círculos grandes | Cheio, batendo a cada 300 ms | Abrem os dois de uma vez, seguram 1,4 s e descem |

Extras do rosto:

- **Piscadinha:** o robô pisca sozinho em intervalos aleatórios de 3 a 6 segundos. Na emoção feliz ele não pisca, porque os olhos já estão fechados.
- **Coração batendo:** o coração alterna entre dois tamanhos (raio 12 e 14), e o ritmo muda conforme a emoção.

---

## 🔧 Hardware necessário

| Item | Observação |
|---|---|
| Arduino **Mega 2560** | A placa foi inferida pelos pinos usados (53, 49, 48 e I2C nos pinos 20/21). Se usar outra placa, ajuste os pinos |
| Display TFT colorido **ST7789** (240x320) | Interface SPI |
| Display OLED **SSD1306** 128x64 | Interface I2C, endereço `0x3C` |
| 2 servos (ex.: SG90) | Um para cada braço |
| Fonte externa 5 V para os servos | Recomendado, para não sobrecarregar o Arduino |
| Webcam | Pode ser celular com o app **Iriun Webcam** |
| Notebook/PC com Windows | O código usa `COM6` (porta do Windows) |
| Cabo USB | Liga o Arduino ao PC |
| Caixa de som ou saída de áudio do PC | A fala toca pelo computador |

---

## 🔌 Ligações (conexões no Arduino)

> ⚠️ Os nomes dos pinos nos módulos variam de fabricante para fabricante. Confira a serigrafia do seu módulo e verifique se ele aceita 5 V ou só 3,3 V antes de ligar.

### Tela TFT ST7789 (SPI)

| Pino do módulo | Arduino Mega | Observação |
|---|---|---|
| VCC | 3,3 V ou 5 V | Conforme o módulo |
| GND | GND | |
| SCL / SCK | **52** | SPI por hardware |
| SDA / MOSI | **51** | SPI por hardware |
| RES / RST | **48** | Definido no código (`TFT_RST`) |
| DC | **49** | Definido no código (`TFT_DC`) |
| CS | **53** | Definido no código (`TFT_CS`) |
| BLK / LED | 3,3 V ou 5 V | Luz de fundo, conforme o módulo |

### Display OLED SSD1306 (I2C)

| Pino do módulo | Arduino Mega |
|---|---|
| VCC | 3,3 V ou 5 V (conforme o módulo) |
| GND | GND |
| SDA | **20** |
| SCL | **21** |

O endereço I2C usado no código é `0x3C`.

### Servos

| Servo | Pino de sinal | Função |
|---|---|---|
| Braço esquerdo | **9** | `PINO_BRACO_E` |
| Braço direito | **10** | `PINO_BRACO_D` |

- Os fios de alimentação (vermelho e marrom/preto) dos servos devem ir para uma **fonte externa de 5 V**.
- O **GND da fonte externa precisa ser ligado ao GND do Arduino** (terra em comum), senão os servos não respondem direito.
- O braço direito fica montado **espelhado**, por isso o código usa `180 - ângulo` para ele.

---

## 📁 Estrutura do repositório

Sugestão de organização (ajuste ao que você tem):

```
robo_kismet/
├── arduino/
│   └── robo_emocoes/
│       └── robo_emocoes.ino      # sketch do Arduino
├── audios/                       # gerada pelo gerar_audios.py
│   ├── A.mp3  H.mp3  N.mp3  S.mp3  U.mp3
├── emotion_detector.py           # programa principal (webcam + serial + áudio)
├── gerar_audios.py               # gera os mp3 das falas (rodar uma vez, com internet)
├── indentificar_fala.py          # lista as vozes instaladas no Windows (diagnóstico)
├── teste_camera.py               # descobre qual índice/backend de câmera funciona
├── teste_voz.py                  # teste de voz
├── requirements.txt              # dependências do Python
└── README.md
```

> 💡 No Arduino IDE, o arquivo `.ino` precisa ficar dentro de uma pasta com o **mesmo nome** dele.

Sugestão de `.gitignore`: `venv/` e `__pycache__/` (e arquivos de teste, como `teste.mp3`).

---

## 🛠 Instalação: Arduino

1. Instale a [Arduino IDE](https://www.arduino.cc/en/software).
2. Em **Sketch → Incluir Biblioteca → Gerenciar Bibliotecas**, instale:

| Biblioteca | Para quê |
|---|---|
| **Adafruit GFX Library** | Desenhar formas (círculos, retângulos, triângulos) |
| **Adafruit ST7789 Library** | Tela colorida do rosto |
| **Adafruit SSD1306** | Display OLED do coração |
| **Adafruit BusIO** | Dependência das bibliotecas da Adafruit (costuma ser instalada junto) |

`SPI`, `Wire` e `Servo` já vêm com a IDE.

3. Selecione a placa em **Ferramentas → Placa** (Arduino Mega 2560) e a porta correta.
4. Abra o `robo_emocoes.ino` e clique em **Carregar**.

Ao ligar, o robô começa na emoção **normal**. Se o OLED não for encontrado no endereço `0x3C`, o programa trava de propósito (`for(;;)`). Nesse caso, confira a fiação e o endereço.

---

## 🐍 Instalação: Python

1. Instale o **Python 3** (versão compatível com o TensorFlow que a biblioteca FER utiliza).
2. Crie e ative um ambiente virtual:

```bash
python -m venv venv
venv\Scripts\activate
```

3. Instale as dependências:

```bash
pip install -r requirements.txt
```

Ou manualmente:

```bash
pip install opencv-python pyserial fer pygame edge-tts
```

| Biblioteca | Para quê |
|---|---|
| `opencv-python` | Ler a webcam e mostrar a janela com o rosto marcado |
| `pyserial` | Conversar com o Arduino pela porta serial |
| `fer` | Reconhecer a emoção no rosto (costuma trazer dependências pesadas, como TensorFlow, então a instalação pode demorar) |
| `pygame` | Tocar os arquivos de áudio |
| `edge-tts` | Gerar as falas em português do Brasil (só no `gerar_audios.py`) |

---

## ▶️ Como rodar

1. **Grave o sketch no Arduino** e feche o Monitor Serial da IDE. Só um programa por vez consegue usar a porta serial.
2. **Descubra a porta COM** do Arduino (Gerenciador de Dispositivos do Windows ou menu Ferramentas → Porta da IDE) e ajuste `PORTA = "COM6"` no `emotion_detector.py`.
3. **Gere os áudios** (uma vez, com internet):

```bash
python gerar_audios.py
```

4. **Ajuste o índice da câmera** no `emotion_detector.py` (`cv2.VideoCapture(3)`). Veja [Problemas comuns](#-problemas-comuns) se não souber qual.
5. **Rode o programa principal:**

```bash
python emotion_detector.py
```

6. Fique de frente para a câmera. Para sair, aperte **`q`** na janela do vídeo.

**Dica para testar sem webcam:** com o Monitor Serial da IDE (9600 baud), envie `n`, `h`, `s`, `a` ou `u` e veja o robô reagir.

---

## 💻 Detalhes do código Arduino

### Organização do sketch

| Seção | O que faz |
|---|---|
| Tela colorida (rosto) | Funções que desenham olhos e bocas, e uma `carinha...()` para cada emoção |
| OLED (coração) | Desenha o coração (cheio, contorno ou rachado) e controla a batida |
| Piscadinha | Fecha e reabre os olhos em intervalos aleatórios |
| Bracinhos (servos) | Coreografia de cada emoção |
| Robô inteiro | `mudaEmocao()` junta tudo, `setup()` e `loop()` |

### Loop principal sem travar

O `loop()` não usa `delay` para coordenar as coisas (exceto os 120 ms da piscadinha). Ele só olha o relógio (`millis()`) e decide, a cada volta:

1. Chegou uma letra nova pela serial? Então muda a emoção.
2. Passou o tempo da batida do coração? Então redesenha o coração.
3. Chegou a hora de piscar? Então pisca.
4. Está na hora do próximo passo da coreografia? Então move os braços.

### Coreografia dos braços

Cada emoção tem uma lista de passos. Cada passo é `{ângulo esquerdo, ângulo direito, tempo em ms}`:

```cpp
// FELIZ: levanta os dois e balança juntos (acenando)
const Passo SEQ_FELIZ[] = {
  {90, 90, 300},
  {110, 110, 250}, {70, 70, 250},
  ...
  {0, 0, 0}
};
```

Um "diretor" (`atualizaBracos()`) aplica o próximo passo quando o tempo do passo atual termina. Para mudar um movimento, basta editar os números do vetor correspondente.

> ⚠️ Se algum braço bater na estrutura, diminua os ângulos, principalmente o `150` da surpresa.

---

## 🐍 Detalhes do código Python

### Mapa de emoções (FER → Arduino)

O FER devolve 7 emoções, mas o robô tem 5 reações:

| FER | Letra enviada |
|---|---|
| `happy` | `H` |
| `sad` | `S` |
| `angry` | `A` |
| `neutral` | `N` |
| `surprise` | `U` |
| `fear` | `N` (sem carinha própria; pode ser trocado por `U`) |
| `disgust` | `N` (sem carinha própria) |

### Filtro anti-tremedeira

Sem filtro, o robô trocaria de emoção várias vezes por segundo. Dois parâmetros resolvem isso:

| Parâmetro | Valor | Significado |
|---|---|---|
| `TEMPO_CONFIRMACAO` | 0,7 s | A emoção nova precisa se manter por esse tempo para valer |
| `INTERVALO_MINIMO` | 3,0 s | Depois de uma troca, o robô "trava" por esse tempo |

Funcionamento: se a letra detectada é igual à atual, nada acontece. Se é diferente, ela vira **candidata** e começa a contar o tempo. Se ela se mantiver por 0,7 s e o intervalo mínimo já tiver passado, a troca é feita. Se o rosto sumir da imagem, a candidata é descartada.

> O código usa apenas o **primeiro rosto** detectado (`resultado[0]`).

---

## 🔊 Voz do robô

Cada emoção tem uma frase, em português do Brasil:

| Letra | Frase |
|---|---|
| `H` | "Que bom te ver feliz!" |
| `S` | "Ah, você parece triste. Estou aqui com você." |
| `A` | "Opa, alguém está bravo!" |
| `N` | "Olá! Eu sou o robô Kismet." |
| `U` | "Nossa, que susto!" |

### Por que áudios pré-gerados?

A primeira versão usava `pyttsx3`, que fala pelas vozes instaladas no Windows. No computador do projeto só existia uma voz em **inglês** (Microsoft Zira), então o português saía com sotaque inglês e quase ninguém entendia.

A solução foi usar o **`edge-tts`**, que oferece vozes neurais em português do Brasil, mas precisa de internet. Para não depender de Wi-Fi na apresentação, o fluxo ficou assim:

1. `gerar_audios.py` roda **uma vez, com internet**, e salva um `.mp3` por emoção na pasta `audios/`.
2. `emotion_detector.py` apenas **toca o arquivo** da emoção com o `pygame`, **sem internet**.

Para trocar a voz ou as frases, edite `VOZ` e `FRASES` em `gerar_audios.py` e rode de novo. Vozes disponíveis: `pt-BR-AntonioNeural` (masculina, padrão) e `pt-BR-FranciscaNeural` (feminina).

O robô não interrompe uma fala que já está tocando (`pygame.mixer.music.get_busy()`).

---

## 🩺 Problemas comuns

**A câmera não abre / janela preta (principalmente com Iriun Webcam)**

- Confira se o app Iriun está aberto no celular **e** no PC, e se a imagem aparece no Iriun do PC.
- Feche outros programas que usam a câmera (OBS, Zoom, Meet, app Câmera do Windows).
- O índice da câmera muda conforme os dispositivos conectados. Use o `teste_camera.py` para testar os índices e os backends do Windows:

```python
import cv2

backends = {"DSHOW": cv2.CAP_DSHOW, "MSMF": cv2.CAP_MSMF}

for nome, backend in backends.items():
    for i in range(0, 8):
        cap = cv2.VideoCapture(i, backend)
        ok = False
        if cap.isOpened():
            ret, frame = cap.read()
            ok = ret and frame is not None
        print(f"{nome} indice {i}: {'FUNCIONA' if ok else '-'}")
        cap.release()
```

  Depois use o resultado, por exemplo: `cv2.VideoCapture(1, cv2.CAP_DSHOW)`.

- Verifique a permissão de câmera em Configurações → Privacidade → Câmera.

**`could not open port 'COM6'`**

- A porta está errada ou ocupada. Feche o Monitor Serial da IDE e confira a porta no Gerenciador de Dispositivos.

**O robô não reage a uma emoção**

- O FER pode demorar a detectar certas emoções, como surpresa. Exagere a expressão olhando de frente. O texto verde na janela mostra o que o FER enxergou.
- Lembre que `fear` e `disgust` viram `N`.

**A voz sai com sotaque estranho**

- Você está usando `pyttsx3` com voz em inglês. Use o fluxo com `edge-tts` descrito acima.

**`Áudio não encontrado`**

- Rode o `gerar_audios.py` e confira se a pasta `audios/` está na mesma pasta do `emotion_detector.py`.

**Os servos tremem ou o Arduino reinicia**

- Alimente os servos com fonte externa e ligue o GND em comum.

---

## 📜 Histórico de desenvolvimento

O projeto foi construído em etapas, com ajuda do **Claude (Anthropic)** para escrever e revisar o código:

1. **Rosto na tela TFT:** olhos e bocas desenhados com formas geométricas para cada emoção (normal, feliz, triste, brava e surpresa).
2. **Coração no OLED:** coração que bate, com ritmo e aparência diferentes por emoção (contorno, cheio e rachado).
3. **Piscadinha:** olhos que fecham e abrem sozinhos em intervalos aleatórios.
4. **Bracinhos com servos:** primeiro com um aceno só para a emoção feliz.
5. **Movimento diferente para cada emoção:** o aceno virou um sistema de **coreografias** (vetores de passos), sem `delay`, para que rosto, coração e piscadinha continuem funcionando junto.
6. **Detector em Python:** webcam + FER + envio de letra pela serial, com filtro de confirmação e intervalo mínimo.
7. **Correção da surpresa:** o Python ainda mandava `N` para `surprise`, mesmo com a carinha de surpresa pronta no Arduino. O mapa foi corrigido para `U`.
8. **Problema da câmera Iriun:** diagnóstico com teste de índices e backends (`DSHOW` e `MSMF`) e ajustes de câmera.
9. **Voz do robô:** primeiro com `pyttsx3`, depois trocada por `edge-tts` com áudios pré-gerados quando descobrimos que o PC só tinha voz em inglês.
10. **Este README.**

---

## 🚧 Limitações e próximos passos

- `fear` e `disgust` ainda caem em neutro, pois o robô não tem carinha nem movimento para elas.
- Só o primeiro rosto da imagem é considerado.
- O código usa `COM6` e índice de câmera fixos. Pode virar configuração (arquivo ou argumento de linha de comando).
- Os ângulos dos servos precisam ser testados e ajustados na montagem real.
- A piscadinha usa `delay(120)`, que pausa o `loop()` por um instante.
- Ideia futura: caixa de som dentro do robô, usando um módulo DFPlayer Mini com cartão SD.

---

## 🙏 Créditos

- Bibliotecas: [Adafruit GFX / ST7789 / SSD1306](https://github.com/adafruit), [FER](https://github.com/justinshenk/fer), [OpenCV](https://opencv.org/), [pygame](https://www.pygame.org/), [edge-tts](https://github.com/rany2/edge-tts).
- Projeto acadêmico do **IFPB**, Semana de Tecnologia 2026.
