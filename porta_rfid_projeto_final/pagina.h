// Web UI module for the ESP32 door controller.
#pragma once

void mudarEstadoPorta(const String &origem);
void limparTagsAutorizadas();
bool definirHorarioAtual(const String &valor);
String obterDataHoraAtual();
String extrairParametro(const String &request, const String &nomeParametro);

void wifiLoop(){
	WiFiClient client = server.available();
	if (!client) {
		return;
	}

	Serial.println("New Client.");
	client.setTimeout(200);

	String request = client.readStringUntil('\n');
	request.trim();
	while (client.available()) {
		client.read();
	}

	if (request.indexOf("GET /mudarestadoporta") >= 0) {
		mudarEstadoPorta("WEB");
	}

	if (request.indexOf("GET /cadastrotag") >= 0) {
		nomeCadastroPendente = extrairParametro(request, "nome");
		if (nomeCadastroPendente.length() > 0) {
			modoCadastroTag = true;
		}
	}

	if (request.indexOf("GET /limpartags") >= 0) {
		limparTagsAutorizadas();
		modoCadastroTag = false;
		nomeCadastroPendente = "";
	}

	if (request.indexOf("GET /sethora") >= 0) {
		String valorHora = extrairParametro(request, "valor");
		if (definirHorarioAtual(valorHora)) {
			Serial.println("Horario sincronizado pelo navegador.");
		}
	}

	atualizarEstadoReedSwitch();

	if (request.indexOf("GET /status") >= 0) {
		client.println("HTTP/1.1 200 OK");
		client.println("Content-type:text/plain");
		client.println("Connection: close");
		client.println();
		client.println(obterDataHoraAtual());
		client.println(portaEstaFechada ? "1" : "0");
		client.println(portaFechadaPeloReed ? "1" : "0");
		client.println(String(quantidadeTagsAutorizadas));
		client.println(modoCadastroTag ? "1" : "0");
		client.println(nomeCadastroPendente);

		String tagsRows = "";
		for (int i = 0; i < quantidadeTagsAutorizadas; i++) {
			if (i > 0) {
				tagsRows += "||";
			}
			tagsRows += tagsAutorizadas[i].uid;
			tagsRows += '|';
			tagsRows += tagsAutorizadas[i].nome;
		}
		client.println(tagsRows);

		String logsRows = "";
		for (int i = 0; i < quantidadeLogs; i++) {
			if (i > 0) {
				logsRows += "||";
			}
			logsRows += logsEventos[i].dataHora;
			logsRows += '|';
			logsRows += logsEventos[i].nome;
			logsRows += '|';
			logsRows += logsEventos[i].acao;
		}
		client.println(logsRows);
		client.println();
		client.stop();
		return;
	}

	client.println("HTTP/1.1 200 OK");
	client.println("Content-type:text/html");
	client.println("Connection: close");
	client.println();
	client.println("<!DOCTYPE html><html>");
	client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
	client.println("<link rel=\"icon\" href=\"data:,\">");
	client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
	client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
	client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
	client.println(".button2 {background-color: #555555;}");
	client.println("table { margin: 0 auto; border-collapse: collapse; }");
	client.println("td, th { border: 1px solid #ccc; padding: 6px 10px; }");
	client.println("form { margin: 12px 0; }</style>");
	client.println("<script>");
	client.println("async function atualizarPagina(){");
	client.println("const resposta = await fetch('/status');");
	client.println("const linhas = (await resposta.text()).split('\\n');");
	client.println("document.getElementById('horario').textContent = linhas[0] || '';");
	client.println("document.getElementById('porta').textContent = (linhas[1] === '1') ? 'fechada' : 'aberta';");
	client.println("document.getElementById('reed').textContent = (linhas[2] === '1') ? 'fechada' : 'aberta';");
	client.println("document.getElementById('totaltags').textContent = linhas[3] || '0';");
	client.println("const modoAtivo = linhas[4] === '1';");
	client.println("document.getElementById('modoCadastro').style.display = modoAtivo ? 'block' : 'none';");
	client.println("document.getElementById('modoCadastroAviso').style.display = modoAtivo ? 'block' : 'none';");
	client.println("document.getElementById('nomeCadastro').textContent = linhas[5] || '';");
	client.println("const tagsBody = document.getElementById('tagsBody'); tagsBody.innerHTML = '';");
	client.println("if (linhas[6]) { linhas[6].split('||').forEach(function(item){ if (!item) return; const partes = item.split('|'); const row = document.createElement('tr'); row.innerHTML = '<td>' + (partes[0] || '') + '</td><td>' + (partes[1] || '') + '</td>'; tagsBody.appendChild(row); }); }");
	client.println("const logsBody = document.getElementById('logsBody'); logsBody.innerHTML = '';");
	client.println("if (linhas[7]) { linhas[7].split('||').forEach(function(item){ if (!item) return; const partes = item.split('|'); const row = document.createElement('tr'); row.innerHTML = '<td>' + (partes[0] || '') + '</td><td>' + (partes[1] || '') + '</td><td>' + (partes[2] || '') + '</td>'; logsBody.appendChild(row); }); }");
	client.println("}");
	client.println("async function acionarPorta(){ await fetch('/mudarestadoporta'); atualizarPagina(); }");
	client.println("async function cadastrarTag(){");
	client.println("const campoNome = document.getElementById('campoNomeCadastro');");
	client.println("const nome = encodeURIComponent(campoNome.value.trim());");
	client.println("if (!nome) return;");
	client.println("await fetch('/cadastrotag?nome=' + nome);");
	client.println("campoNome.value = '';");
	client.println("atualizarPagina();");
	client.println("}");
	client.println("async function limparTags(){ await fetch('/limpartags'); atualizarPagina(); }");
	client.println("window.addEventListener('load', function(){ atualizarPagina(); setInterval(atualizarPagina, 3000); });");
	client.println("</script></head>");
	client.println("<body><h1>ESP32 Web Server</h1>");
	if (!horarioDefinido) {
		client.println("<p><strong>Horario nao sincronizado.</strong> Use o link de sincronizacao de Brasilia.</p>");
	}
	client.println("<p>Horario: <span id=\"horario\">");
	client.println(obterDataHoraAtual());
	client.println("</span></p>");
	client.println("<p>Porta comandada: <span id=\"porta\">");
	client.println(portaEstaFechada ? "fechada" : "aberta");
	client.println("</span></p>");
	client.println("<p>Reed switch: <span id=\"reed\">");
	client.println(portaFechadaPeloReed ? "fechada" : "aberta");
	client.println("</span></p>");
	client.println("<p>Tags autorizadas: <span id=\"totaltags\">");
	client.println(quantidadeTagsAutorizadas);
	client.println("</span></p>");
	client.println("<p id=\"modoCadastro\" style=\"display:");
	client.println(modoCadastroTag ? "block" : "none");
	client.println(";\">Modo de cadastro ativo para: <span id=\"nomeCadastro\">");
	client.println(nomeCadastroPendente);
	client.println("</span></p>");
	client.println("<p id=\"modoCadastroAviso\" style=\"display:");
	client.println(modoCadastroTag ? "block" : "none");
	client.println(";\">Aproxime a tag para vincular esse nome.</p>");
	client.println("<p><button type=\"button\" onclick=\"acionarPorta()\">Acionar porta</button></p>");
	client.println("<p><a href=\"#\" onclick=\"window.location='/sethora?valor='+encodeURIComponent(new Date().toLocaleString('sv-SE',{timeZone:'America/Sao_Paulo',hour12:false})); return false;\">Sincronizar hora de Brasilia</a></p>");
	client.println("<p><input id=\"campoNomeCadastro\" type=\"text\" placeholder=\"Nome da pessoa\" required></p>");
	client.println("<p><button type=\"button\" onclick=\"cadastrarTag()\">Cadastrar proxima tag</button></p>");
	client.println("<p><button type=\"button\" onclick=\"limparTags()\">Limpar tags salvas</button></p>");
	client.println("<h2>Tags salvas</h2>");
	client.println("<table><tr><th>UID</th><th>Nome</th></tr><tbody id=\"tagsBody\">");
	for (int i = 0; i < quantidadeTagsAutorizadas; i++) {
		client.println("<tr><td>" + tagsAutorizadas[i].uid + "</td><td>" + tagsAutorizadas[i].nome + "</td></tr>");
	}
	client.println("</tbody></table>");
	client.println("<h2>Log</h2>");
	client.println("<table><tr><th>Data/Hora</th><th>Nome</th><th>Acao</th></tr><tbody id=\"logsBody\">");
	for (int i = 0; i < quantidadeLogs; i++) {
		client.println("<tr><td>" + logsEventos[i].dataHora + "</td><td>" + logsEventos[i].nome + "</td><td>" + logsEventos[i].acao + "</td></tr>");
	}
	client.println("</tbody></table>");
	client.println("</body></html>");
	client.println();
	client.stop();
	Serial.println("Client disconnected.");
	Serial.println("");
}

