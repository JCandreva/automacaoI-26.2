#pragma once

void registrarEvento(const String &nome, const String &acao);

bool lerReedSwitchFechado() {
  return digitalRead(reedSwitch) == LOW;
}

void atualizarEstadoReedSwitch() {
  portaFechadaPeloReed = lerReedSwitchFechado();
}

void atualizarIndicadoresPorta() {
  if (portaEstaFechada) {
    digitalWrite(saidareleporta, LOW);
    digitalWrite(amarelo, HIGH);
    digitalWrite(verde, LOW);
  }
  else {
    digitalWrite(saidareleporta, HIGH);
    digitalWrite(amarelo, LOW);
    digitalWrite(verde, HIGH);
  }
}

void abrirPorta() {
  digitalWrite(saidareleporta, HIGH);
  portaEstaFechada = false;
  portaDestravada = true;
  portaAbriuDesdeDestrave = false;
  inicioDestravamento = millis();
  atualizarIndicadoresPorta();
}

void fecharPorta() {
  digitalWrite(saidareleporta, LOW);
  portaEstaFechada = true;
  portaDestravada = false;
  portaAbriuDesdeDestrave = false;
  atualizarIndicadoresPorta();
}

void atualizarFechamentoAutomatico() {
  if (!portaDestravada) {
    return;
  }

  if (!portaFechadaPeloReed) {
    portaAbriuDesdeDestrave = true;
  }

  if (!portaAbriuDesdeDestrave) {
    if (millis() - inicioDestravamento >= tempoMaximoDestravado) {
      fecharPorta();
      registrarEvento("AUTO_TIMEOUT", "FECHOU");
    }
    return;
  }

  if (portaFechadaPeloReed) {
    fecharPorta();
    registrarEvento("AUTO_REED", "FECHOU");
  }
}

void mudarEstadoPorta(const String &origem) {
  if (portaEstaFechada) {
    abrirPorta();
  }
  else {
    fecharPorta();
  }
  registrarEvento(origem, portaEstaFechada ? "FECHOU" : "ABRIU");
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
