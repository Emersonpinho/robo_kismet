import cv2

captura = cv2.VideoCapture(0)

if not captura.isOpened():
    print("Não consegui abrir a webcam!")
else:
    print("Webcam aberta com sucesso. Pressione 'q' para sair.")

while True:
    ret, frame = captura.read()
    if not ret:
        break

    cv2.imshow("Teste de camera", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

captura.release()
cv2.destroyAllWindows()