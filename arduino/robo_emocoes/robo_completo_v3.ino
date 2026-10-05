#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>   // NOVO

// ================= TELA COLORIDA (ROSTO) =================
#define TFT_CS  53
#define TFT_DC  49
#define TFT_RST 48

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

uint16_t CIANO = tft.color565(40, 220, 240);
uint16_t PRETO = ST77XX_BLACK;

void olhoNormal(int x, int y) {
  tft.fillRoundRect(x, y, 70, 70, 30, CIANO);
  tft.fillRect(x - 2, y - 2, 74, 28, PRETO);
}

void olhoFeliz(int cx, int cy) {
  tft.fillCircle(cx, cy, 36, CIANO);
  tft.fillCircle(cx, cy, 23, PRETO);
  tft.fillRect(cx - 40, cy, 80, 40, PRETO);
}

void bocaAberta() {
  int cx = 160, cy = 140;
  tft.fillCircle(cx, cy, 46, CIANO);
  tft.fillRect(cx - 50, cy - 50, 100, 50, PRETO);
  tft.fillCircle(cx, cy + 8, 34, PRETO);
  tft.fillRect(cx - 45, cy, 90, 8, CIANO);
}

void olhoInclinado(int x, int y, bool esquerdo, bool bravo) {
  tft.fillRoundRect(x, y, 70, 70, 30, CIANO);
  if (esquerdo != bravo) {
    tft.fillTriangle(x - 2, y - 2, x + 72, y - 2, x - 2, y + 38, PRETO);
  } else {
    tft.fillTriangle(x - 2, y - 2, x + 72, y - 2, x + 72, y + 38, PRETO);
  }
}

void bocaTriste() {
  tft.fillCircle(160, 185, 26, CIANO);
  tft.fillCircle(160, 199, 26, PRETO);
}

void bocaBrava() {
  tft.fillRoundRect(120, 170, 80, 12, 6, CIANO);
}

void olhoSurpresa(int cx, int cy) {
  tft.fillCircle(cx, cy, 42, CIANO);
}

void bocaSurpresa() {
  tft.fillCircle(160, 178, 22, CIANO);
  tft.fillCircle(160, 178, 12, PRETO);
}

void carinhaNormal() {
  tft.fillScreen(PRETO);
  olhoNormal(60, 70);
  olhoNormal(190, 70);
}

void carinhaFeliz() {
  tft.fillScreen(PRETO);
  olhoFeliz(95, 95);
  olhoFeliz(225, 95);
  bocaAberta();
}

void carinhaTriste() {
  tft.fillScreen(PRETO);
  olhoInclinado(60, 60, true, false);
  olhoInclinado(190, 60, false, false);
  bocaTriste();
}

void carinhaBrava() {
  tft.fillScreen(PRETO);
  olhoInclinado(60, 60, true, true);
  olhoInclinado(190, 60, false, true);
  bocaBrava();
}

void carinhaSurpresa() {
  tft.fillScreen(PRETO);
  olhoSurpresa(95, 80);
  olhoSurpresa(225, 80);
  bocaSurpresa();
}

// ================= OLED (CORACAO) =================
Adafruit_SSD1306 oled(128, 64, &Wire, -1);

bool grande = false;
unsigned long ultimaBatida = 0;
char emocao = 'n';

void coracaoCheio(int cx, int cy, int r, uint16_t cor) {
  oled.fillCircle(cx - r, cy, r, cor);
  oled.fillCircle(cx + r, cy, r, cor);
  oled.fillTriangle(cx - 2 * r + 1, cy + r / 2,
                    cx + 2 * r - 1, cy + r / 2,
                    cx, cy + 2 * r + 4, cor);
}

void coracaoContorno(int cx, int cy, int r) {
  coracaoCheio(cx, cy, r, SSD1306_WHITE);
  coracaoCheio(cx, cy + 1, r - 4, SSD1306_BLACK);
}

void rachadura(int cx, int cy) {
  for (int d = -1; d <= 1; d++) {
    oled.drawLine(cx + d,     cy - 6,  cx - 5 + d, cy + 4,  SSD1306_BLACK);
    oled.drawLine(cx - 5 + d, cy + 4,  cx + 4 + d, cy + 14, SSD1306_BLACK);
    oled.drawLine(cx + 4 + d, cy + 14, cx - 1 + d, cy + 26, SSD1306_BLACK);
  }
}

unsigned long periodoDaEmocao() {
  if (emocao == 'h') return 400;
  if (emocao == 'a') return 250;
  if (emocao == 'u') return 300;
  if (emocao == 's') return 1000;
  return 700;
}

void desenhaCoracao() {
  int r = grande ? 14 : 12;
  int cx = 64;
  int cy = 30;

  if (emocao == 'a') cx += random(-2, 3);

  oled.clearDisplay();

  if (emocao == 'h' || emocao == 'a' || emocao == 'u') {
    coracaoCheio(cx, cy, r, SSD1306_WHITE);
  } else if (emocao == 's') {
    coracaoCheio(cx, cy, r, SSD1306_WHITE);
    rachadura(cx, cy);
  } else {
    coracaoContorno(cx, cy, r);
  }

  oled.display();
}

// ================= PISCADINHA =================
unsigned long proximaPiscada = 0;

// apaga a caixa do olho e desenha so uma linha fina no meio (olho fechado)
void fechaOlho(int x, int y, int w, int h) {
  tft.fillRect(x - 3, y - 3, w + 6, h + 6, PRETO);
  tft.fillRoundRect(x, y + h / 2 - 4, w, 8, 4, CIANO);
}

void marcaProximaPiscada() {
  proximaPiscada = millis() + random(3000, 6000);
}

void piscar() {
  // feliz ja tem os olhos fechados (^ ^), entao nao pisca
  if (emocao == 'h') {
    marcaProximaPiscada();
    return;
  }

  // 1) fecha os olhos
  if (emocao == 'n') {
    fechaOlho(60, 96, 70, 44);
    fechaOlho(190, 96, 70, 44);
  } else if (emocao == 's' || emocao == 'a') {
    fechaOlho(60, 60, 70, 70);
    fechaOlho(190, 60, 70, 70);
  } else if (emocao == 'u') {
    fechaOlho(53, 38, 84, 84);
    fechaOlho(183, 38, 84, 84);
  }

  delay(120);

  // 2) abre de novo (apaga a linha e redesenha o olho)
  if (emocao == 'n') {
    tft.fillRect(57, 93, 76, 50, PRETO);
    tft.fillRect(187, 93, 76, 50, PRETO);
    olhoNormal(60, 70);
    olhoNormal(190, 70);
  } else if (emocao == 's') {
    tft.fillRect(57, 57, 76, 76, PRETO);
    tft.fillRect(187, 57, 76, 76, PRETO);
    olhoInclinado(60, 60, true, false);
    olhoInclinado(190, 60, false, false);
  } else if (emocao == 'a') {
    tft.fillRect(57, 57, 76, 76, PRETO);
    tft.fillRect(187, 57, 76, 76, PRETO);
    olhoInclinado(60, 60, true, true);
    olhoInclinado(190, 60, false, true);
  } else if (emocao == 'u') {
    tft.fillRect(50, 35, 90, 90, PRETO);
    tft.fillRect(180, 35, 90, 90, PRETO);
    olhoSurpresa(95, 80);
    olhoSurpresa(225, 80);
  }

  marcaProximaPiscada();
}

// ================= BRACINHOS (SERVOS) =================  // NOVO
Servo bracoE;
Servo bracoD;

const int PINO_BRACO_E = 9;
const int PINO_BRACO_D = 10;

const int ANG_REPOUSO = 0;    // braco embaixo
const int ANG_ALTO    = 90;   // braco levantado
const int ANG_BALANCO = 20;   // quanto balanca pra cada lado
const int QTD_BALANCOS = 8;

bool acenando = false;
bool ladoA = false;
int balancos = 0;
unsigned long proximoPasso = 0;

// o braco direito esta espelhado, entao usa 180 - angulo
void moveBracos(int ang) {
  bracoE.write(ang);
  bracoD.write(180 - ang);
}

void comecaAcenar() {
  acenando = true;
  balancos = 0;
  ladoA = false;
  moveBracos(ANG_ALTO);
  proximoPasso = millis() + 300;
}

void paraAcenar() {
  acenando = false;
  moveBracos(ANG_REPOUSO);
}

void atualizaAcenar() {
  if (!acenando || millis() < proximoPasso) return;

  if (balancos >= QTD_BALANCOS) {
    paraAcenar();
    return;
  }

  ladoA = !ladoA;
  moveBracos(ANG_ALTO + (ladoA ? ANG_BALANCO : -ANG_BALANCO));
  balancos++;
  proximoPasso = millis() + 250;
}

// ================= ROBO INTEIRO =================
void mudaEmocao(char c) {
  emocao = c;

  if (c == 'h') comecaAcenar();       // NOVO
  else if (acenando) paraAcenar();    // NOVO

  if (c == 'n') carinhaNormal();
  if (c == 'h') carinhaFeliz();
  if (c == 's') carinhaTriste();
  if (c == 'a') carinhaBrava();
  if (c == 'u') carinhaSurpresa();

  desenhaCoracao();
  marcaProximaPiscada();
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));

  tft.init(240, 320);
  tft.setRotation(1);

  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }

  bracoE.attach(PINO_BRACO_E);   // NOVO
  bracoD.attach(PINO_BRACO_D);   // NOVO
  moveBracos(ANG_REPOUSO);       // NOVO

  mudaEmocao('n');
}

void loop() {
  // 1) chegou letra nova?
  if (Serial.available() > 0) {
    char c = tolower(Serial.read());
    if ((c == 'n' || c == 'h' || c == 's' || c == 'a' || c == 'u') && c != emocao) {
      mudaEmocao(c);
    }
  }

  // 2) hora de uma batida do coracao?
  if (millis() - ultimaBatida >= periodoDaEmocao()) {
    ultimaBatida = millis();
    grande = !grande;
    desenhaCoracao();
  }

  // 3) hora de piscar?
  if (millis() >= proximaPiscada) {
    piscar();
  }

  // 4) continua o aceno, se estiver acenando   // NOVO
  atualizaAcenar();
}