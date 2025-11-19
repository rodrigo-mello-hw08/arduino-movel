

/*
A5 = sensor linha esquerdo  400
A4 = sensor linha direito  400
A3 = determinar interrupcao 
A2 = determinar interrupcao
A1 = sensor qtd linha 800
A0 = sensor proximidade bloco 430
*/

int contagem=0;
unsigned long lastInterruptTime = 0;   // armazena o último tempo válido da interrupção
const unsigned long debounceTime = 100; // debounce em ms

void setup() {
  Serial.begin(9600);
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(3), contador_linha, RISING);
}


void contador_linha()
{
  unsigned long currentTime = millis();
  // verifica debounce
  if (currentTime - lastInterruptTime >= debounceTime) {
    contagem++;
    Serial.print("Contador: ");
    Serial.println(contagem);

    lastInterruptTime = currentTime; // atualiza tempo da última interrupção válida
  }
}


void loop() {
  int iPot = digitalRead(3);
  // int iMap = map(iPot, 0, 1023, 0, 10);
  
  Serial.print("iPot: ");
  Serial.println(iPot);
  // Serial.print("mapeamento: ");
  // Serial.println(mapEmCimaDaLinha(iPot));
  delay(200);
}
