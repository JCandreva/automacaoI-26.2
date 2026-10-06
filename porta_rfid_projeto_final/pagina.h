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
	client.println("<style>");
	client.println(":root{--bg1:#07111f;--bg2:#12263f;--card:#101b2d;--card2:#16243a;--text:#ecf4ff;--muted:#9fb2cc;--line:rgba(255,255,255,.10);--accent:#57c7ff;--accent2:#7cf29a;--danger:#ff7c7c;--shadow:0 20px 50px rgba(0,0,0,.28);}");
	client.println("*{box-sizing:border-box;}");
	client.println("body{margin:0;min-height:100vh;font-family:system-ui,-apple-system,Segoe UI,Roboto,Arial,sans-serif;background:radial-gradient(circle at top,#17304f 0,#0b1526 38%,#060b14 100%);color:var(--text);}");
	client.println("body:before{content:'';position:fixed;inset:0;background:linear-gradient(135deg,rgba(87,199,255,.10),transparent 35%,rgba(124,242,154,.08));pointer-events:none;}");
	client.println(".page{position:relative;max-width:1120px;margin:0 auto;padding:28px 18px 40px;}");
	client.println(".hero{background:linear-gradient(135deg,rgba(255,255,255,.08),rgba(255,255,255,.03));border:1px solid var(--line);border-radius:24px;padding:24px 22px;box-shadow:var(--shadow);backdrop-filter:blur(10px);}");
	client.println(".eyebrow{display:inline-block;font-size:12px;letter-spacing:.18em;text-transform:uppercase;color:var(--accent);margin-bottom:10px;}");
	client.println("h1,h2{margin:0;}");
	client.println("h1{font-size:clamp(28px,4vw,44px);line-height:1.05;margin-bottom:10px;}");
	client.println(".lead{margin:0;color:var(--muted);max-width:760px;line-height:1.5;}");
	client.println(".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(180px,1fr));gap:14px;margin:18px 0;}");
	client.println(".card{background:linear-gradient(180deg,var(--card2),var(--card));border:1px solid var(--line);border-radius:20px;padding:16px 18px;box-shadow:var(--shadow);}");
	client.println(".card h2{font-size:14px;text-transform:uppercase;letter-spacing:.12em;color:var(--muted);margin-bottom:8px;}");
	client.println(".metric{font-size:28px;font-weight:700;color:var(--text);word-break:break-word;}");
	client.println(".status-open{color:var(--accent2);}");
	client.println(".status-closed{color:var(--accent);}");
	client.println(".status-warn{color:#ffd36e;}");
	client.println(".panel{background:linear-gradient(180deg,rgba(255,255,255,.05),rgba(255,255,255,.02));border:1px solid var(--line);border-radius:24px;padding:18px;box-shadow:var(--shadow);margin-top:18px;}");
	client.println(".panel-head{display:flex;flex-wrap:wrap;gap:10px;align-items:center;justify-content:space-between;margin-bottom:14px;}");
	client.println(".panel-head p{margin:0;color:var(--muted);}");
	client.println(".actions{display:flex;flex-wrap:wrap;gap:12px;align-items:center;margin-top:12px;}");
	client.println("input{background:#0a1220;color:var(--text);border:1px solid var(--line);border-radius:14px;padding:14px 16px;font-size:16px;min-width:240px;outline:none;}");
	client.println("input:focus{border-color:rgba(87,199,255,.7);box-shadow:0 0 0 3px rgba(87,199,255,.15);}");
	client.println("button,a.button{display:inline-flex;align-items:center;justify-content:center;gap:8px;border:none;border-radius:14px;padding:14px 18px;font-size:15px;font-weight:700;cursor:pointer;text-decoration:none;transition:transform .15s ease,filter .15s ease,background .15s ease;}");
	client.println("button:hover,a.button:hover{transform:translateY(-1px);filter:brightness(1.05);}");
	client.println(".primary{background:linear-gradient(135deg,#58c6ff,#377dff);color:white;}");
	client.println(".secondary{background:linear-gradient(135deg,#2d394d,#1a2231);color:var(--text);border:1px solid var(--line);}");
	client.println(".ghost{background:rgba(255,255,255,.04);color:var(--text);border:1px solid var(--line);}");
	client.println(".sync-link{color:var(--accent);}");
	client.println(".split{display:grid;grid-template-columns:repeat(auto-fit,minmax(320px,1fr));gap:18px;margin-top:18px;}");
	client.println("table{width:100%;border-collapse:collapse;margin-top:12px;overflow:hidden;border-radius:16px;}");
	client.println("th,td{padding:12px 14px;text-align:left;border-bottom:1px solid var(--line);vertical-align:top;}");
	client.println("th{font-size:12px;text-transform:uppercase;letter-spacing:.12em;color:var(--muted);background:rgba(255,255,255,.04);}");
	client.println("tr:hover td{background:rgba(255,255,255,.03);}");
	client.println(".empty{color:var(--muted);font-style:italic;padding:12px 2px;}");
	client.println(".badge{display:inline-flex;align-items:center;padding:6px 10px;border-radius:999px;background:rgba(255,255,255,.06);border:1px solid var(--line);font-size:12px;color:var(--muted);}");
	client.println(".stack{display:flex;flex-direction:column;gap:8px;}");
	client.println("@media (max-width:640px){.page{padding:14px 10px 26px;} .hero,.panel,.card{border-radius:18px;} input{width:100%;min-width:0;} button,a.button{width:100%;}}");
	client.println("</style>");
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
	client.println("<body><div class='page'>");
	client.println("<section class='hero'><div class='eyebrow'>Controle de acesso</div><h1>ESP32 Web Server</h1><p class='lead'>Acompanhe a porta em tempo real, sincronize o horario de Brasilia, cadastre tags NFC e veja o historico de eventos sem recarregar a pagina.</p></section>");
	client.println("<div class='grid'>");
	client.println("<div class='card'><h2>Horario</h2><div class='metric' id='horario'>");
	client.println(obterDataHoraAtual());
	client.println("</div></div>");
	client.println("<div class='card'><h2>Porta comandada</h2><div class='metric status-closed' id='porta'>");
	client.println(portaEstaFechada ? "fechada" : "aberta");
	client.println("</div></div>");
	client.println("<div class='card'><h2>Reed switch</h2><div class='metric status-closed' id='reed'>");
	client.println(portaFechadaPeloReed ? "fechada" : "aberta");
	client.println("</div></div>");
	client.println("<div class='card'><h2>Tags autorizadas</h2><div class='metric' id='totaltags'>");
	client.println(quantidadeTagsAutorizadas);
	client.println("</div></div>");
	client.println("</div>");
	client.println("<section class='panel'><div class='panel-head'><div><h2>Acoes</h2><p>Comandos imediatos para abrir a porta, sincronizar o horario e cadastrar tags.</p></div><span class='badge'>Atualizacao automatica a cada 3s</span></div>");
	if (!horarioDefinido) {
		client.println("<p class='badge status-warn'>Horario nao sincronizado. Use a sincronizacao de Brasilia.</p>");
	}
	client.println("<div class='actions'><button type='button' class='primary' onclick='acionarPorta()'>Acionar porta</button><a class='button secondary sync-link' href='#' onclick=\"window.location='/sethora?valor='+encodeURIComponent(new Date().toLocaleString('sv-SE',{timeZone:'America/Sao_Paulo',hour12:false})); return false;\">Sincronizar hora de Brasilia</a></div>");
	client.println("<div class='actions'><input id='campoNomeCadastro' type='text' placeholder='Nome da pessoa' required><button type='button' class='secondary' onclick='cadastrarTag()'>Cadastrar proxima tag</button><button type='button' class='ghost' onclick='limparTags()'>Limpar tags salvas</button></div>");
	client.println("<p id='modoCadastro' class='badge' style='display:");
	client.println(modoCadastroTag ? "inline-flex" : "none");
	client.println(";'>Modo de cadastro ativo para: <strong id='nomeCadastro' style='margin-left:6px;'>");
	client.println(nomeCadastroPendente);
	client.println("</strong></p>");
	client.println("<p id='modoCadastroAviso' class='badge' style='display:");
	client.println(modoCadastroTag ? "inline-flex" : "none");
	client.println(";'>Aproxime a tag para vincular esse nome.</p>");
	client.println("</section>");
	client.println("<div class='split'><section class='panel'><div class='panel-head'><div><h2>Tags salvas</h2><p>UIDs autorizados e seus nomes vinculados.</p></div><span class='badge'>");
	client.println(String(quantidadeTagsAutorizadas));
	client.println(" cadastradas</span></div><table><tr><th>UID</th><th>Nome</th></tr><tbody id='tagsBody'>");
	for (int i = 0; i < quantidadeTagsAutorizadas; i++) {
		client.println("<tr><td>" + tagsAutorizadas[i].uid + "</td><td>" + tagsAutorizadas[i].nome + "</td></tr>");
	}
	if (quantidadeTagsAutorizadas == 0) {
		client.println("<tr><td colspan='2' class='empty'>Nenhuma tag cadastrada.</td></tr>");
	}
	client.println("</tbody></table></section>");
	client.println("<section class='panel'><div class='panel-head'><div><h2>Log</h2><p>Eventos recentes de abertura, fechamento e cadastro.</p></div><span class='badge'>");
	client.println(String(quantidadeLogs));
	client.println(" eventos</span></div><table><tr><th>Data/Hora</th><th>Nome</th><th>Acao</th></tr><tbody id='logsBody'>");
	for (int i = 0; i < quantidadeLogs; i++) {
		client.println("<tr><td>" + logsEventos[i].dataHora + "</td><td>" + logsEventos[i].nome + "</td><td>" + logsEventos[i].acao + "</td></tr>");
	}
	if (quantidadeLogs == 0) {
		client.println("<tr><td colspan='3' class='empty'>Nenhum evento registrado ainda.</td></tr>");
	}
	client.println("</tbody></table></section></div>");
	client.println("</div></body></html>");
	client.println();
	client.stop();
	Serial.println("Client disconnected.");
	Serial.println("");
}

