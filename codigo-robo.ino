//Estados da maquina de estados
#define SEGUIR_LINHA 0
#define CURVA 1
#define PARADA 2

//Pinos
#define LED_DEBUG 13
#define IR_CONTADOR_LINHA 3
#define IR_OBSTACULO 2
#define IR_INFRA_DIREITO 11
#define IR_INFRA_ESQUERDO 12
#define PWM_MOTOR_DIREITO 5
#define PWM_MOTOR_ESQUERDO 6
#define MOTOR_DIREITO_IN_0 8
#define MOTOR_DIREITO_IN_1 7
#define MOTOR_ESQUERDO_IN_0 10
#define MOTOR_ESQUERDO_IN_1 9

//Constantes
#define LINHA_DA_CURVA 3
#define FRENTE 0
#define TRAS 1

//Variaveis globais
int estado = SEGUIR_LINHA;
int iCount = 0;
int isObstaculo = 0;
int em_curva = 0;

void setup()
{
  //Configuração dos pinos
  pinMode(LED_DEBUG, OUTPUT);
  pinMode(IR_CONTADOR_LINHA, INPUT);
  pinMode(IR_OBSTACULO, INPUT);
  pinMode(IR_INFRA_DIREITO, INPUT);
  pinMode(IR_INFRA_ESQUERDO, INPUT);
  pinMode(PWM_MOTOR_DIREITO, OUTPUT);
  pinMode(PWM_MOTOR_ESQUERDO, OUTPUT);
  pinMode(MOTOR_DIREITO_IN_0, OUTPUT);
  pinMode(MOTOR_DIREITO_IN_1, OUTPUT);
  pinMode(MOTOR_ESQUERDO_IN_0, OUTPUT);
  pinMode(MOTOR_ESQUERDO_IN_1, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(IR_OBSTACULO), obstaculo, CHANGE);
  attachInterrupt(digitalPinToInterrupt(IR_CONTADOR_LINHA), contador_linha, RISING);
}

void loop()
{
  switch(estado)
  {
    case SEGUIR_LINHA:
      seguidor_de_linha();
      break;
    case CURVA:
      realizar_a_curva_esquerda();
      break;
    case PARADA:
      desligar_m_esquerdo();
      desligar_m_direto();
      break;
  }
}

void seguidor_de_linha()
{
  if(IR_INFRA_DIREITO == HIGH && IR_INFRA_DIREITO == HIGH) {
    ligar_m_esquerdo(FRENTE);
    ligar_m_direito(FRENTE);
  } else if (IR_INFRA_DIREITO == HIGH) {
    ligar_m_esquerdo(FRENTE);
    desligar_m_direto();
  } else if (IR_INFRA_ESQUERDO == HIGH) {
    ligar_m_direito(FRENTE);
    desligar_m_esquerdo();
  } else {
    ligar_m_esquerdo(FRENTE);
    desligar_m_direto();
  }
}

//Todo: atualizar fluxograma para encontrar condição de parada da curva
void realizar_a_curva_esquerda()
{
  
  // continua a curva enquanto o infra esquedo nao encontra a linha
  ligar_m_esquerdo(FRENTE);
  ligar_m_direito(TRAS);

  if (IR_INFRA_ESQUERDO == HIGH) {
    estado = SEGUIR_LINHA;
    em_curva = LOW;
    iCount++;
  }
}

void contador_linha()
{
  iCount++;
  
  if(iCount == LINHA_DA_CURVA){
    estado = CURVA;
    em_curva = HIGH;
    iCount = 0;
  }
}


// ligar motores
void ligar_m_esquerdo(int sentido) {
  analogWrite(PWM_MOTOR_ESQUERDO, 1);
  if (sentido == TRAS) {
    digitalWrite(MOTOR_ESQUERDO_IN_0, HIGH);
    digitalWrite(MOTOR_ESQUERDO_IN_1, LOW);
  } else {
    digitalWrite(MOTOR_ESQUERDO_IN_0, LOW);
    digitalWrite(MOTOR_ESQUERDO_IN_1, HIGH);
  }
}

void ligar_m_direito(int sentido) {
  analogWrite(PWM_MOTOR_DIREITO, 1);
  if (sentido == TRAS) {
    digitalWrite(MOTOR_DIREITO_IN_0, HIGH);
    digitalWrite(MOTOR_DIREITO_IN_1, LOW);
  } else {
    digitalWrite(MOTOR_DIREITO_IN_0, LOW);
    digitalWrite(MOTOR_DIREITO_IN_1, HIGH);
  }
}

//desligar motores
void desligar_m_esquerdo() {
  analogWrite(PWM_MOTOR_ESQUERDO, 0);
  digitalWrite(MOTOR_ESQUERDO_IN_0, LOW);
  digitalWrite(MOTOR_ESQUERDO_IN_1, LOW);
}

void desligar_m_direto() {
  analogWrite(PWM_MOTOR_DIREITO, 0);
  digitalWrite(MOTOR_DIREITO_IN_0, LOW);
  digitalWrite(MOTOR_DIREITO_IN_1, LOW);
}

//interrupções
void obstaculo()
{
  isObstaculo =! isObstaculo;
  
  if(isObstaculo)
    estado = PARADA;
  else
    estado = SEGUIR_LINHA;
}