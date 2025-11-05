

/*
A5 = sensor linha esquerdo  400
A4 = sensor linha direito  400
A3 = determinar interrupcao 
A2 = determinar interrupcao
A1 = sensor qtd linha 800
A0 = sensor proximidade bloco 430
*/


void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
}


void loop() {
  int iPot = digitalRead(A0);
  // int iMap = map(iPot, 0, 1023, 0, 10);
  
  Serial.print("iPot: ");
  Serial.println(iPot);
  // Serial.print("mapeamento: ");
  // Serial.println(mapEmCimaDaLinha(iPot));
  delay(200);
}

int mapEmCimaDaLinha(int entrada) {
	return entrada >= 750 ? 0 : 1;
}
