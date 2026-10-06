#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Preferences.h>
#include <time.h>

static const int saidareleporta = 13;
static const int botao = 12;
static const int buzzer = 26;
static const int amarelo = 27;
static const int verde = 14;
static const int reedSwitch = 33;

const char* wifiStaSsid = "";
const char* wifiStaPassword = "";
const long gmtOffsetSeconds = -3 * 3600;
const int daylightOffsetSeconds = 0;

WiFiServer server(80);
MFRC522 mfrc522(5, 21);
Preferences preferences;

bool portaEstaFechada = true;
bool portaFechadaPeloReed = true;
bool portaDestravada = false;
bool portaAbriuDesdeDestrave = false;
bool modoCadastroTag = false;
String nomeCadastroPendente = "";
bool horarioDefinido = false;
long long epochBaseSegundos = 0;
unsigned long millisBaseHorario = 0;
unsigned long inicioDestravamento = 0;
const unsigned long tempoMaximoDestravado = 30000;

static const int MAX_TAGS = 10;
static const int MAX_LOGS = 10;

struct TagAutorizada {
  String uid;
  String nome;
};

struct LogEvento {
  String dataHora;
  String nome;
  String acao;
};

TagAutorizada tagsAutorizadas[MAX_TAGS];
int quantidadeTagsAutorizadas = 0;
LogEvento logsEventos[MAX_LOGS];
int quantidadeLogs = 0;

#include "armazenamento.h"
#include "porta_control.h"
#include "pagina.h"

void setup() {
  Serial.begin(9600);
  pinMode(saidareleporta, OUTPUT);
  pinMode(botao, INPUT_PULLUP);
  pinMode(amarelo, OUTPUT);
  pinMode(verde, OUTPUT);
  pinMode(reedSwitch, INPUT_PULLUP);
  digitalWrite(amarelo, HIGH);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("portafoda", "toctocqueme");
  if (strlen(wifiStaSsid) > 0) {
    WiFi.begin(wifiStaSsid, wifiStaPassword);
  }
  iniciarHorario();
  server.begin();
  SPI.begin();
  mfrc522.PCD_Init();
  preferences.begin("rfiddoor", false);
  carregarTagsAutorizadas();
  carregarLogsEventos();
  atualizarEstadoReedSwitch();
  portaEstaFechada = portaFechadaPeloReed;
  atualizarIndicadoresPorta();
}

void loop() {
  atualizarEstadoReedSwitch();
  atualizarFechamentoAutomatico();
  processarBotaoFisico();
  wifiLoop();
  nfcLoop();
}
