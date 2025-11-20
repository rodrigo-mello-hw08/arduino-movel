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
#define MOTOR_DIREITO_IN_0 10
#define MOTOR_DIREITO_IN_1 9
#define MOTOR_ESQUERDO_IN_0 8
#define MOTOR_ESQUERDO_IN_1 7

//Constantes
#define LINHA_DA_CURVA 3
#define FRENTE 0
#define TRAS 1
#define VEL_MOTOR 150

/*
A5 = sensor linha esquerdo
A4 = sensor linha direito 
3 = contador linha interrupcao
2 = determinar interrupcao
A1 = sensor qtd linha - retirar
A0 = sensor proximidade bloco - provisorio
*/

//Variaveis globais
int estado = SEGUIR_LINHA;
int iCount = 0;
int isObstaculo = 0;
int em_curva = 0;
int infra_linha_esquerdo_ativo = 0;
int infra_linha_direito_ativo = 0;
int infra_qtd_linha_ativo = 0;
int led_linha_direito_ativo = 0;
int led_linha_esquerdo_ativo = 0;
int latch_zig_zag = 0;
int leitura_obstaculo = 0;

unsigned long ultima_interrupcao = 0;  
const unsigned long debounce_interrupcao = 100;


void setup()
{
  Serial.begin(9600);
  pinMode(LED_DEBUG, OUTPUT);
  pinMode(IR_CONTADOR_LINHA, INPUT);
  pinMode(IR_OBSTACULO, INPUT);
  pinMode(VEL_MOTOR_ESQUERDO, OUTPUT);
  pinMode(VEL_MOTOR_DIREITO, OUTPUT);
  pinMode(MOTOR_DIREITO_IN_0, OUTPUT);
  pinMode(MOTOR_DIREITO_IN_1, OUTPUT);
  pinMode(MOTOR_ESQUERDO_IN_0, OUTPUT);
  pinMode(MOTOR_ESQUERDO_IN_1, OUTPUT);

  //attachInterrupt(digitalPinToInterrupt(IR_OBSTACULO), lidaObstaculo, FALLING);
  attachInterrupt(digitalPinToInterrupt(IR_CONTADOR_LINHA), contador_linha, RISING);
}

void loop()
{
  ler_infras_linhas();
  interpretar_obstaculo();
  analogWrite(VEL_MOTOR_DIREITO, VEL_MOTOR);
  analogWrite(VEL_MOTOR_ESQUERDO, VEL_MOTOR);
  debug(estado, "var estado");
  switch(estado) {
    case SEGUIR_LINHA: seguidor_de_linha(); break;
    case CURVA:        realizar_curva_esquerda(); break;
    case PARADA:       desligar_ambos_motores(); break;
  }
}

void ler_infras_linhas() {
  //debug("fazendo leitura dos infravermelhos");
  infra_linha_direito_ativo = digitalRead(A4);
  //debug(infra_linha_direito_ativo, "valor leitura infra direito");
  infra_linha_esquerdo_ativo = digitalRead(A5);
  //debug(infra_linha_esquerdo_ativo, "valor leitura infra esquerdo");
  //infra_qtd_linha_ativo = digitalRead(A1); - retirar

  leitura_obstaculo = digitalRead(A0);
  //debug(leitura_obstaculo, "valor leitura obstaculo");
}

void interpretar_obstaculo() {
  if (leitura_obstaculo == LOW) {
    estado = PARADA;
  } else {
    estado = SEGUIR_LINHA;
  }
}


void seguidor_de_linha()
{
    if(infra_linha_direito_ativo == HIGH && infra_linha_esquerdo_ativo == HIGH) {
        ligar_m_esquerdo(FRENTE);
        ligar_m_direito(FRENTE);
    } else if (infra_linha_direito_ativo == HIGH ) {
        ligar_m_esquerdo(FRENTE);
        desligar_m_direito();
    } else if (infra_linha_esquerdo_ativo == HIGH) {
        ligar_m_direito(FRENTE);
        desligar_m_esquerdo();
    } else {
        if (latch_zig_zag == LOW) { //todo: corrigi erro, verificar comportamento
            ligar_m_direito(FRENTE);
            desligar_m_esquerdo();
            latch_zig_zag = HIGH;
        } else {
            ligar_m_esquerdo(FRENTE);
            desligar_m_direito();
            latch_zig_zag = LOW;
        }
    }
    delay(20);
}

/* Curva */
void contador_linha()
{
    unsigned long tempo_atual = millis();
    if (tempo_atual - ultima_interrupcao >= debounce_interrupcao) {
        iCount++;
        ultima_interrupcao = tempo_atual; 
    }
    if(iCount == LINHA_DA_CURVA){
        estado = CURVA;
        em_curva = HIGH;
    }
}

void realizar_curva_esquerda()
{
    ligar_m_direito(FRENTE);
    ligar_m_esquerdo(TRAS);

    loop_curva();

    estado = SEGUIR_LINHA;
    iCount = 0;
    ligar_m_esquerdo(FRENTE);
    desligar_m_direito();
}

void loop_curva() {
    delay(200); //Todo: talvez aumentar debounce para permitir curva
    int sensor_guia = 0;
    while(em_curva == HIGH) {
        sensor_guia = digitalRead(A4); //leitura sensor guia
        if(sensor_guia == HIGH) {
            em_curva = LOW;
        }
        delay(100); 
    }
}

/* Ligar motores */
void ligar_m_esquerdo(int sentido) {
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
    //debug("ligando motor direito");
    if (sentido == TRAS) {
        digitalWrite(MOTOR_DIREITO_IN_0, HIGH);
        digitalWrite(MOTOR_DIREITO_IN_1, LOW);
    } else {
        digitalWrite(MOTOR_DIREITO_IN_0, LOW);
        digitalWrite(MOTOR_DIREITO_IN_1, HIGH);
    }
}

/* Desligar motores */
void desligar_ambos_motores() {
    desligar_m_esquerdo();
    desligar_m_direito();
}

void desligar_m_esquerdo() {
    //debug("desligando motor esquerdo");
    digitalWrite(MOTOR_ESQUERDO_IN_0, LOW);
    digitalWrite(MOTOR_ESQUERDO_IN_1, LOW);
}

void desligar_m_direito() {
    //debug("desligando motor direito");
    digitalWrite(MOTOR_DIREITO_IN_0, LOW);
    digitalWrite(MOTOR_DIREITO_IN_1, LOW);
}


/* Debugs */
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

/* Nao utilizadas TODO: apagar depois
void lidaObstaculo()
{
  debug(isObstaculo, "obsta");
  if (isObstaculo == LOW) {
    isObstaculo = HIGH;
    estado = PARADA;
  } else {
    isObstaculo = LOW;
    estado = SEGUIR_LINHA;
  } 

}
*/