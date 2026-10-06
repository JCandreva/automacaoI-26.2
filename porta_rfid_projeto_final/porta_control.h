#pragma once

void registrarEvento(const String &nome, const String &acao);
void atualizarIndicadoresPorta();

bool lerReedSwitchFechado() {
  return digitalRead(reedSwitch) == LOW;
}

void atualizarEstadoReedSwitch() {
  if (!usarReedSwitch) {
    return;
  }

  portaFechadaPeloReed = lerReedSwitchFechado();
  portaEstaFechada = portaFechadaPeloReed;
  atualizarIndicadoresPorta();
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
  portaDestravada = true;
  portaAbriuDesdeDestrave = false;
  aguardandoFechamentoPorReed = usarReedSwitch;
  inicioDestravamento = millis();
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
    if (!portaAbriuDesdeDestrave) {
      digitalWrite(saidareleporta, LOW);
      portaAbriuDesdeDestrave = true;
    }
    return;
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
  bool estavaDestravada = portaDestravada;

  if (!estavaDestravada) {
    abrirPorta();
  }
  else {
    fecharPorta();
  }
  registrarEvento(origem, estavaDestravada ? "FECHOU" : "ABRIU");
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
