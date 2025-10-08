void setup() {
  Serial.begin(9600);
  pinMode(A2, INPUT);
}

void loop() {
  int iPot = analogRead(A2);
  int iMap = map(iPot, 0, 1023, 0, 10);
  
  Serial.print("iPot: ");
  Serial.println(iPot);
  Serial.print("mapeamento: ");
  Serial.println(mapEmCimaDaLinha(iPot));
  delay(200);
}

int mapEmCimaDaLinha(int entrada) {
	return entrada >= 450 ? 0 : 1;
}