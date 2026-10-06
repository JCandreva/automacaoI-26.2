#pragma once

void mudarEstadoPorta(const String &origem);
String decodificarUrl(const String &texto);
String formatarEpoch(long long epochSegundos);
long long converterDataHoraParaEpoch(int ano, unsigned mes, unsigned dia, unsigned hora, unsigned minuto, unsigned segundo);

String uidParaString(byte *uidBytes, byte uidSize) {
  String uid = "";

  for (byte i = 0; i < uidSize; i++) {
    if (uidBytes[i] < 0x10) {
      uid += "0";
    }
    uid += String(uidBytes[i], HEX);
  }

  uid.toUpperCase();
  return uid;
}

void carregarTagsAutorizadas() {
  quantidadeTagsAutorizadas = 0;

  String tagsSalvas = preferences.getString("tagsv2", "");
  int inicio = 0;

  while (inicio < tagsSalvas.length() && quantidadeTagsAutorizadas < MAX_TAGS) {
    int fim = tagsSalvas.indexOf(';', inicio);
    if (fim == -1) {
      fim = tagsSalvas.length();
    }

    String tag = tagsSalvas.substring(inicio, fim);
    tag.trim();

    if (tag.length() > 0) {
      int separador = tag.indexOf('|');
      if (separador > 0) {
        tagsAutorizadas[quantidadeTagsAutorizadas].uid = tag.substring(0, separador);
        tagsAutorizadas[quantidadeTagsAutorizadas].nome = tag.substring(separador + 1);
      }
      else {
        tagsAutorizadas[quantidadeTagsAutorizadas].uid = tag;
        tagsAutorizadas[quantidadeTagsAutorizadas].nome = "Sem nome";
      }
      quantidadeTagsAutorizadas++;
    }

    inicio = fim + 1;
  }
}

void salvarTagsAutorizadas() {
  String tagsSalvas = "";

  for (int i = 0; i < quantidadeTagsAutorizadas; i++) {
    if (i > 0) {
      tagsSalvas += ';';
    }
    tagsSalvas += tagsAutorizadas[i].uid;
    tagsSalvas += '|';
    tagsSalvas += tagsAutorizadas[i].nome;
  }

  preferences.putString("tagsv2", tagsSalvas);
}

void carregarLogsEventos() {
  quantidadeLogs = 0;

  String logsSalvos = preferences.getString("logsv2", "");
  int inicio = 0;

  while (inicio < logsSalvos.length() && quantidadeLogs < MAX_LOGS) {
    int fim = logsSalvos.indexOf(';', inicio);
    if (fim == -1) {
      fim = logsSalvos.length();
    }

    String item = logsSalvos.substring(inicio, fim);
    item.trim();

    int primeiro = item.indexOf('|');
    int segundo = item.indexOf('|', primeiro + 1);
    if (primeiro > 0 && segundo > primeiro) {
      logsEventos[quantidadeLogs].dataHora = item.substring(0, primeiro);
      logsEventos[quantidadeLogs].nome = item.substring(primeiro + 1, segundo);
      logsEventos[quantidadeLogs].acao = item.substring(segundo + 1);
      quantidadeLogs++;
    }

    inicio = fim + 1;
  }
}

void salvarLogsEventos() {
  String logsSalvos = "";

  for (int i = 0; i < quantidadeLogs; i++) {
    if (i > 0) {
      logsSalvos += ';';
    }
    logsSalvos += logsEventos[i].dataHora;
    logsSalvos += '|';
    logsSalvos += logsEventos[i].nome;
    logsSalvos += '|';
    logsSalvos += logsEventos[i].acao;
  }

  preferences.putString("logsv2", logsSalvos);
}

String limparTextoCampo(const String &texto) {
  String resultado = texto;
  resultado.replace(";", " ");
  resultado.replace("|", " ");
  resultado.trim();
  return resultado;
}

String extrairParametro(const String &request, const String &nomeParametro) {
  String assinatura = nomeParametro + "=";
  int inicio = request.indexOf(assinatura);

  if (inicio < 0) {
    return "";
  }

  inicio += assinatura.length();
  int fim = request.indexOf(' ', inicio);
  if (fim < 0) {
    fim = request.length();
  }

  String valor = request.substring(inicio, fim);
  return limparTextoCampo(decodificarUrl(valor));
}

String decodificarUrl(const String &texto) {
  String saida = "";

  for (unsigned int i = 0; i < texto.length(); i++) {
    char c = texto[i];
    if (c == '+') {
      saida += ' ';
    }
    else if (c == '%' && i + 2 < texto.length()) {
      String hex = texto.substring(i + 1, i + 3);
      saida += (char) strtol(hex.c_str(), nullptr, 16);
      i += 2;
    }
    else {
      saida += c;
    }
  }

  return saida;
}

String obterDataHoraAtual() {
  if (horarioDefinido) {
    long long agora = epochBaseSegundos + ((millis() - millisBaseHorario) / 1000LL);
    return formatarEpoch(agora);
  }

  return String("SEM HORA");
}

void registrarEvento(const String &nome, const String &acao) {
  for (int i = MAX_LOGS - 1; i > 0; i--) {
    logsEventos[i] = logsEventos[i - 1];
  }

  logsEventos[0].dataHora = obterDataHoraAtual();
  logsEventos[0].nome = nome;
  logsEventos[0].acao = acao;

  if (quantidadeLogs < MAX_LOGS) {
    quantidadeLogs++;
  }

  salvarLogsEventos();
}

bool localizarTag(const String &uid, int &indice) {
  for (int i = 0; i < quantidadeTagsAutorizadas; i++) {
    if (tagsAutorizadas[i].uid == uid) {
      indice = i;
      return true;
    }
  }

  indice = -1;
  return false;
}

bool tagAutorizada(const String &uid, String &nomeEncontrado) {
  int indice = -1;
  if (!localizarTag(uid, indice)) {
    nomeEncontrado = "Nao autorizada";
    return false;
  }

  nomeEncontrado = tagsAutorizadas[indice].nome;
  return true;
}

bool adicionarTagAutorizada(const String &uid, const String &nome) {
  int indice = -1;
  if (localizarTag(uid, indice)) {
    return false;
  }

  if (quantidadeTagsAutorizadas >= MAX_TAGS) {
    return false;
  }

  tagsAutorizadas[quantidadeTagsAutorizadas].uid = uid;
  tagsAutorizadas[quantidadeTagsAutorizadas].nome = nome.length() > 0 ? nome : "Sem nome";
  quantidadeTagsAutorizadas++;
  salvarTagsAutorizadas();
  return true;
}

void limparTagsAutorizadas() {
  quantidadeTagsAutorizadas = 0;
  salvarTagsAutorizadas();
}

void iniciarHorario() {
  if (strlen(wifiStaSsid) == 0) {
    return;
  }

  configTime(gmtOffsetSeconds, daylightOffsetSeconds, "pool.ntp.org", "time.nist.gov");
  struct tm timeinfo;
  unsigned long inicio = millis();

  while (!getLocalTime(&timeinfo) && millis() - inicio < 10000) {
    delay(250);
  }

  if (getLocalTime(&timeinfo)) {
    epochBaseSegundos = converterDataHoraParaEpoch(
      timeinfo.tm_year + 1900,
      (unsigned)timeinfo.tm_mon + 1,
      (unsigned)timeinfo.tm_mday,
      (unsigned)timeinfo.tm_hour,
      (unsigned)timeinfo.tm_min,
      (unsigned)timeinfo.tm_sec
    );
    millisBaseHorario = millis();
    horarioDefinido = true;
  }
}

long long diasDesdeCivil(int ano, unsigned mes, unsigned dia) {
  ano -= mes <= 2;
  const long long era = (ano >= 0 ? ano : ano - 399) / 400;
  const unsigned anoDoCiclo = (unsigned)(ano - era * 400);
  const unsigned diaDoAno = (153 * (mes + (mes > 2 ? -3 : 9)) + 2) / 5 + dia - 1;
  const unsigned diaDoCiclo = anoDoCiclo * 365 + anoDoCiclo / 4 - anoDoCiclo / 100 + diaDoAno;
  return era * 146097 + (long long)diaDoCiclo - 719468;
}

void civilFromDays(long long dias, int &ano, unsigned &mes, unsigned &dia) {
  dias += 719468;
  const long long era = (dias >= 0 ? dias : dias - 146096) / 146097;
  const unsigned diasDoEra = (unsigned)(dias - era * 146097);
  const unsigned anoDoCiclo = (diasDoEra - diasDoEra / 1460 + diasDoEra / 36524 - diasDoEra / 146096) / 365;
  ano = (int)anoDoCiclo + (int)(era * 400);
  const unsigned diasDoAno = diasDoEra - (365 * anoDoCiclo + anoDoCiclo / 4 - anoDoCiclo / 100);
  const unsigned mesDoPeriodo = (5 * diasDoAno + 2) / 153;
  dia = diasDoAno - (153 * mesDoPeriodo + 2) / 5 + 1;
  mes = mesDoPeriodo < 10 ? mesDoPeriodo + 3 : mesDoPeriodo - 9;
  ano += (mes <= 2);
}

long long converterDataHoraParaEpoch(int ano, unsigned mes, unsigned dia, unsigned hora, unsigned minuto, unsigned segundo) {
  return diasDesdeCivil(ano, mes, dia) * 86400LL + (long long)hora * 3600LL + (long long)minuto * 60LL + segundo;
}

String formatarEpoch(long long epochSegundos) {
  long long dias = epochSegundos / 86400LL;
  long long restante = epochSegundos % 86400LL;

  if (restante < 0) {
    restante += 86400LL;
    dias -= 1;
  }

  int ano = 0;
  unsigned mes = 0;
  unsigned dia = 0;
  civilFromDays(dias, ano, mes, dia);

  unsigned hora = (unsigned)(restante / 3600LL);
  unsigned minuto = (unsigned)((restante % 3600LL) / 60LL);
  unsigned segundo = (unsigned)(restante % 60LL);

  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%02u/%02u/%04d %02u:%02u:%02u", dia, mes, ano, hora, minuto, segundo);
  return String(buffer);
}

bool definirHorarioAtual(const String &valor) {
  int ano = 0;
  int mes = 0;
  int dia = 0;
  int hora = 0;
  int minuto = 0;
  int segundo = 0;

  if (sscanf(valor.c_str(), "%d-%d-%d %d:%d:%d", &ano, &mes, &dia, &hora, &minuto, &segundo) != 6) {
    return false;
  }

  epochBaseSegundos = converterDataHoraParaEpoch(ano, (unsigned)mes, (unsigned)dia, (unsigned)hora, (unsigned)minuto, (unsigned)segundo);
  millisBaseHorario = millis();
  horarioDefinido = true;
  return true;
}

void nfcLoop(){
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  String uid = uidParaString(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.print("Tag lida: ");
  Serial.println(uid);

  if (modoCadastroTag) {
    if (adicionarTagAutorizada(uid, nomeCadastroPendente)) {
      Serial.println("Tag salva com sucesso.");
      registrarEvento(nomeCadastroPendente, "TAG CADASTRADA");
    }
    else {
      Serial.println("Nao foi possivel salvar a tag.");
    }
    modoCadastroTag = false;
    nomeCadastroPendente = "";
  }
  else {
    String nomeTag = "";
    if (tagAutorizada(uid, nomeTag)) {
      Serial.println("Tag autorizada.");
      mudarEstadoPorta(nomeTag);
    }
    else {
      Serial.println("Tag nao autorizada.");
      registrarEvento("NAO AUTORIZADA", "NEGADO");
    }
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  while (mfrc522.PICC_IsNewCardPresent()) {
    delay(100);
  }
}
