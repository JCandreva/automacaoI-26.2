#pragma once

void registrarEvento(const String &nome, const String &acao);
void atualizarIndicadoresPorta();
void abrirPorta();
void fecharPorta();

bool lerReedSwitchFechado() {
  return !digitalRead(reedSwitch);
}

void atualizarEstadoReedSwitch() {
  if (!usarReedSwitch) {
    return;
  }

  portaFechadaPeloReed = lerReedSwitchFechado();

  if (portaAbriuDesdeDestrave && portaFechadaPeloReed && !portaEstaFechada) {
    fecharPorta();
    registrarEvento("REED", "FECHOU_FISICAMENTE");
  }

  atualizarIndicadoresPorta();
}

void atualizarIndicadoresPorta() {
  if (portaEstaFechada) {
    digitalWrite(amarelo, HIGH);
    digitalWrite(verde, LOW);
  }
  else {
    digitalWrite(amarelo, LOW);
    digitalWrite(verde, HIGH);
  }
}

void abrirPorta() {
  digitalWrite(saidareleporta, HIGH);
  portaDestravada = true;
  portaAbriuDesdeDestrave = false;
  aguardandoFechamentoPorReed = usarReedSwitch;
  inicioDestravamento = millis();
  inicioAberturaSemReed = millis();
  portaEstaFechada = false;
  atualizarIndicadoresPorta();
}

void fecharPorta() {
  digitalWrite(saidareleporta, LOW);
  portaDestravada = false;
  portaAbriuDesdeDestrave = false;
  aguardandoFechamentoPorReed = false;
  portaEstaFechada = true;
  atualizarIndicadoresPorta();
}

void atualizarFechamentoAutomatico() {
  if (!usarReedSwitch) {
    if (portaDestravada && millis() - inicioDestravamento >= tempoMaximoDestravado) {
      fecharPorta();
      registrarEvento("AUTO_TIMEOUT", "FECHOU");
    }
    return;
  }

  if (!aguardandoFechamentoPorReed) {
    return;
  }

  if (!portaFechadaPeloReed) {
    digitalWrite(saidareleporta, LOW);
    portaDestravada = false;
    aguardandoFechamentoPorReed = false;
    portaAbriuDesdeDestrave = true;

    // O estado virtual continua ABERTO enquanto o usuário mantém a porta aberta
    // fisicamente. Somente um comando fechar/fechamento manual pode fechá-lo.
    portaEstaFechada = false;
    atualizarIndicadoresPorta();
    registrarEvento("REED", "ABRIU_FISICAMENTE");
    return;
  }

  if (!portaAbriuDesdeDestrave && millis() - inicioDestravamento >= tempoMaximoDestravado) {
    fecharPorta();
    registrarEvento("AUTO_TIMEOUT", "FECHOU");
  }
}

void mudarEstadoPorta(const String &origem) {
  bool estavaFechada = portaEstaFechada;

  if (estavaFechada) {
    abrirPorta();
  }
  else {
    fecharPorta();
  }
  registrarEvento(origem, estavaFechada ? "ABRIU" : "FECHOU");
  tone(buzzer, 500, 500);
}

void processarBotaoFisico() {
  if (!digitalRead(botao)) {
    mudarEstadoPorta("BOTAO");
    while (!digitalRead(botao)) {
      delay(250);
    }
  }
}
