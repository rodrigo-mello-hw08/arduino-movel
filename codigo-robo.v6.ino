/*Lista de TODOs
Ajustar logica de curva
Atualizar fluxograma para encontrar condição de parada da curva
Ver como conectar bateria
Adequar logica a proximidade dos sensores
*/


//Estados da maquina de estados
#define SEGUIR_LINHA 0
#define CURVA 1
#define PARADA 2

//Pinos
#define VEL_MOTOR_ESQUERDO 11
#define VEL_MOTOR_DIREITO 5 
#define LED_DEBUG 13
#define IR_CONTADOR_LINHA 3
#define IR_OBSTACULO 2
#define PWM_MOTOR_DIREITO 5
#define PWM_MOTOR_ESQUERDO 6
#define MOTOR_DIREITO_IN_0 10
#define MOTOR_DIREITO_IN_1 9
#define MOTOR_ESQUERDO_IN_0 8
#define MOTOR_ESQUERDO_IN_1 7

//Constantes
#define LINHA_DA_CURVA 3
#define FRENTE 0
#define TRAS 1
#define LIMITE_MAX_PROXIMIDADE_LINHA 400
#define LIMITE_MAX_PROXIMIDADE_QTD_LINHA 750
#define LIMITE_MAX_PROXIMIDADE_BLOCO 430
#define VEL_MOTOR 150

/*
A5 = sensor linha esquerdo
A4 = sensor linha direito 
A3 = determinar interrupcao
A2 = determinar interrupcao
A1 = sensor qtd linha
A0 = sensor proximidade bloco
*/

//Variaveis globais
int estado = SEGUIR_LINHA;
int iCount = 0;
int isObstaculo = 0;
int em_curva = 0;
int infra_linha_esquerdo_ativo = 0;
int infra_linha_direito_ativo = 0;
int infra_bloco_ativo = 0;
int infra_qtd_linha_ativo = 0;
int led_linha_direito_ativo = 0;
int led_linha_esquerdo_ativo = 0;
int latch_zig_zag = 0;


void setup()
{
  Serial.begin(9600);
  //Configuração dos pinos
  pinMode(LED_DEBUG, OUTPUT);
  pinMode(IR_CONTADOR_LINHA, INPUT);
  pinMode(IR_OBSTACULO, INPUT);
  pinMode(VEL_MOTOR_ESQUERDO, OUTPUT);
  pinMode(VEL_MOTOR_DIREITO, OUTPUT);
  pinMode(PWM_MOTOR_DIREITO, OUTPUT);
  pinMode(PWM_MOTOR_ESQUERDO, OUTPUT);
  pinMode(MOTOR_DIREITO_IN_0, OUTPUT);
  pinMode(MOTOR_DIREITO_IN_1, OUTPUT);
  pinMode(MOTOR_ESQUERDO_IN_0, OUTPUT);
  pinMode(MOTOR_ESQUERDO_IN_1, OUTPUT);

  //attachInterrupt(digitalPinToInterrupt(IR_OBSTACULO), obstaculo, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(IR_CONTADOR_LINHA), contador_linha, RISING);
}

void loop()
{
  ler_infras_linhas();
  analogWrite(VEL_MOTOR_DIREITO, VEL_MOTOR);
  analogWrite(VEL_MOTOR_ESQUERDO, VEL_MOTOR);
  debug(estado, "var estado");
  switch(estado)
  {
    case SEGUIR_LINHA:
      seguidor_de_linha();
      break;
    //case CURVA:
    //  realizar_a_curva_esquerda();
    //  break;
    //case PARADA:
    //  desligar_m_esquerdo();
    //  desligar_m_direito();
    //  break;
  }
}

void ler_infras_linhas() {
  //debug("fazendo leitura dos infravermelhos");
  infra_linha_direito_ativo = digitalRead(A4);
  debug(infra_linha_direito_ativo, "valor leitura infra direito");
  infra_linha_esquerdo_ativo = digitalRead(A5);
  debug(infra_linha_esquerdo_ativo, "valor leitura infra esquerdo");
  infra_qtd_linha_ativo = digitalRead(A1);
  infra_bloco_ativo = digitalRead(A0);
}

int verificaQtdLinha(int entrada) {
	return verificaProximidade(entrada, LIMITE_MAX_PROXIMIDADE_QTD_LINHA);
}

int verificaBloco(int entrada) {
	return verificaProximidade(entrada, LIMITE_MAX_PROXIMIDADE_BLOCO);
}

int verificaProximidade(int entrada, int limite) {
  return entrada >= limite ? 1 : 0;
}

void seguidor_de_linha()
{
  if(infra_linha_direito_ativo == HIGH && infra_linha_esquerdo_ativo == HIGH) {
    ligar_m_esquerdo(FRENTE);
    ligar_m_direito(FRENTE);
    //delay(100);
    //debug("ligando ambos");
  } else if (infra_linha_direito_ativo == HIGH ) {
    ligar_m_esquerdo(FRENTE);
    desligar_m_direito();
    //debug("ligando esquerdo");
    //delay(100);
  } else if (infra_linha_esquerdo_ativo == HIGH) {
    ligar_m_direito(FRENTE);
    desligar_m_esquerdo();
    //delay(100);
    debug("LIGANDO direito");
  } else {
    if (latch_zig_zag = LOW) {
      ligar_m_direito(FRENTE);
      desligar_m_esquerdo();
      //debug("sou o else");
      latch_zig_zag = HIGH;
    } else {
      ligar_m_esquerdo(FRENTE);
      desligar_m_direito();
      //debug("sou o else");
      latch_zig_zag = LOW;
    }
    //delay(100);
  }
  delay(20);
}

//Todo: 
void realizar_a_curva_esquerda()
{
  
  // continua a curva enquanto o infra esquedo nao encontra a linha
  debug("realizando curva a esquerda");
  ligar_m_esquerdo(FRENTE);
  ligar_m_direito(TRAS);

  if (infra_linha_esquerdo_ativo == HIGH) {
    estado = SEGUIR_LINHA;
    em_curva = LOW;
    iCount++;
  }
}

void contador_linha()
{
  /*
  iCount++;
  
  if(iCount == LINHA_DA_CURVA){
    estado = CURVA;
    em_curva = HIGH;
    iCount = 0;
  }
  */
}


// ligar motores
void ligar_m_esquerdo(int sentido) {
  analogWrite(PWM_MOTOR_ESQUERDO, 1);
  //debug("ligando motor esquerdo");
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
  //debug("ligando motor direito");
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
  //debug("desligando motor esquerdo");
  analogWrite(PWM_MOTOR_ESQUERDO, 0);
  digitalWrite(MOTOR_ESQUERDO_IN_0, LOW);
  digitalWrite(MOTOR_ESQUERDO_IN_1, LOW);
}

void desligar_m_direito() {
  //debug("desligando motor direito");
  analogWrite(PWM_MOTOR_DIREITO, 0);
  digitalWrite(MOTOR_DIREITO_IN_0, LOW);
  digitalWrite(MOTOR_DIREITO_IN_1, LOW);
}

//interrupções
void obstaculo()
{
  //isObstaculo =! isObstaculo;
  
  //if(isObstaculo)
  //  estado = PARADA;
  //else
  //  estado = SEGUIR_LINHA;
}

void debug(int valor, String mensagem) {
  //delay(900);
  Serial.print("[DEBUG] ");
  Serial.print(mensagem);
  Serial.print(": ");
  Serial.println(valor);
}

void debug(String mensagem) {
  //delay(900);
  Serial.print("[DEBUG] ");
  Serial.println(mensagem);
}
