#include "mailcore.h"
#include "auth_guard.h"
#include <QSslSocket>
#include <QSslConfiguration>
#include <QDateTime>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QDir>
#include <QFile>
#include <QTextStream>

FydelisMailCore::FydelisMailCore(QObject *pai)
    : QObject(pai), smtpPorta(465), smtpTLS(true),
      popPorta(995), popTLS(true), popConectado(false),
      socketPOP3(new QSslSocket(this)), fluxoPOP3(nullptr)
{
    // 📂 Cria estrutura de pastas do Fydelis na 1ª execução
    caminhoRaiz = QDir::homePath() + "/Fydelis-Arquivos/";
    QDir dir;
    if (!dir.exists(caminhoRaiz)) {
        dir.mkpath(caminhoRaiz + "Caixa de Entrada");
        dir.mkpath(caminhoRaiz + "Itens Enviados");
        dir.mkpath(caminhoRaiz + "Rascunhos");
        dir.mkpath(caminhoRaiz + "Lixeira");
        dir.mkpath(caminhoRaiz + "Anexos");
        emit logStatus("[✅] Estrutura de pastas criada em: " + caminhoRaiz);
    }

    QSslConfiguration cfg = QSslConfiguration::defaultConfiguration();
    cfg.setProtocol(QSsl::TlsV1_2OrLater);
    socketPOP3->setSslConfiguration(cfg);
    connect(socketPOP3, &QSslSocket::readyRead, this, [this](){
        if (fluxoPOP3) emit logStatus("> " + lerRespostaPOP3());
    });
}

FydelisMailCore::~FydelisMailCore() {
    desconectarPOP3();
}

void FydelisMailCore::configurarSMTP(const QString &servidor, int porta,
                                      const QString &usuario, const QString &senha, bool tls) {
    smtpServidor = servidor; smtpPorta = porta;
    smtpUsuario = usuario; smtpSenha = senha; smtpTLS = tls;
}

void FydelisMailCore::configurarPOP3(const QString &servidor, int porta,
                                     const QString &usuario, const QString &senha, bool tls) {
    popServidor = servidor; popPorta = porta;
    popUsuario = usuario; popSenha = senha; popTLS = tls;
}

void FydelisMailCore::configurarIMAP(const QString &servidor, int porta,
                                      const QString &usuario, const QString &senha, bool tls) {
    Q_UNUSED(servidor); Q_UNUSED(porta);
    Q_UNUSED(usuario); Q_UNUSED(senha); Q_UNUSED(tls);
}

// 📥 CONECTAR ao servidor POP3
bool FydelisMailCore::conectarPOP3() {
    if (popServidor.isEmpty() || popUsuario.isEmpty()) {
        emit logStatus("[ERRO] Configure servidor e usuário primeiro!");
        return false;
    }
    emit logStatus("[POP3] Conectando a " + popServidor + ":" + QString::number(popPorta) + "...");

    if (popTLS)
        socketPOP3->connectToHostEncrypted(popServidor, popPorta);
    else
        socketPOP3->connectToHost(popServidor, popPorta);

    if (!socketPOP3->waitForConnected(8000)) {
        emit logStatus("[FALHA] Não conectou: " + socketPOP3->errorString());
        return false;
    }

    if (fluxoPOP3) {
        delete fluxoPOP3;
        fluxoPOP3 = nullptr;
    }
    fluxoPOP3 = new QTextStream(socketPOP3);

    QString saudacao = lerRespostaPOP3();
    emit logStatus("[SERVIDOR] " + saudacao.simplified());
    if (!saudacao.startsWith("+OK")) {
        emit logStatus("[ERRO] Servidor rejeitou a saudação inicial: " + saudacao);
        desconectarPOP3();
        return false;
    }

    // Autenticar - Usuário
    emit logStatus("[POP3] Enviando usuário: " + popUsuario);
    *fluxoPOP3 << "USER " << popUsuario << "\r\n";
    fluxoPOP3->flush();
    QString respUser = lerRespostaPOP3();
    emit logStatus("[SERVIDOR] " + respUser.simplified());
    if (!respUser.startsWith("+OK")) {
        emit logStatus("[ERRO] Usuário rejeitado pelo servidor");
        desconectarPOP3();
        return false;
    }

    // Autenticar - Senha (Senha de Aplicativo)
    emit logStatus("[POP3] Enviando senha de aplicativo...");
    *fluxoPOP3 << "PASS " << popSenha << "\r\n";
    fluxoPOP3->flush();
    QString respPass = lerRespostaPOP3();
    emit logStatus("[SERVIDOR] " + respPass.simplified());
    if (!respPass.startsWith("+OK")) {
        emit logStatus("[ERRO] Senha incorreta ou 2FA bloqueado. Certifique-se de usar a Senha de Aplicativo!");
        desconectarPOP3();
        return false;
    }

    popConectado = true;
    emit logStatus("[✅] Conectado com sucesso! Caixa de entrada pronta.");
    return true;
}

// 📥 BAIXAR CAIXA DE ENTRADA
QVariantList FydelisMailCore::baixarCaixaPOP3(int limite) {
    cache.clear();
    if (!popConectado && !conectarPOP3()) return {};

    emit logStatus("[POP3] Listando mensagens...");
    *fluxoPOP3 << "LIST\r\n";
    fluxoPOP3->flush();

    QString resp = lerRespostaPOP3();
    if (!resp.startsWith("+OK")) {
        emit logStatus("[ERRO] Não listou mensagens");
        return {};
    }

    int mensagensBaixadas = 0;
    while (socketPOP3->isOpen() && mensagensBaixadas < limite) {
        QString linha = fluxoPOP3->readLine();
        if (linha == ".") break;
        if (linha.isEmpty() || linha.startsWith("+OK")) continue;

        QStringList partes = linha.split(" ", Qt::SkipEmptyParts);
        if (partes.size() >= 2) {
            int id = partes[0].toInt();
            cache.append({id, "", "", "", "", false});
            mensagensBaixadas++;
        }
    }

    // Baixar cabeçalhos de cada mensagem
    for (int i = 0; i < cache.size(); i++) {
        if (!socketPOP3->isOpen()) break;
        MensagemEmail &m = cache[i];

        emit logStatus(QString("[POP3] Baixando %1/%2...").arg(i+1).arg(cache.size()));

        *fluxoPOP3 << "TOP " << m.id << " 0\r\n";
        fluxoPOP3->flush();

        resp = lerRespostaPOP3();
        if (!resp.startsWith("+OK")) continue;

        while (socketPOP3->isOpen()) {
            QString linha = fluxoPOP3->readLine();
            if (linha == ".") break;

            if (linha.startsWith("From:"))
                m.remetente = linha.mid(5).trimmed();
            else if (linha.startsWith("Subject:"))
                m.assunto = linha.mid(8).trimmed();
            else if (linha.startsWith("Date:"))
                m.data = linha.mid(5).trimmed();
        }

        // 💾 Salvar automaticamente na pasta do disco
        salvarMensagemNaPasta(m, "Caixa de Entrada");
    }

    emit logStatus(QString("[✅] %1 mensagens baixadas e salvas!").arg(cache.size()));

    QVariantList lista;
    for (auto &m : cache) lista.append(QVariant::fromValue(m));
    return lista;
}

void FydelisMailCore::desconectarPOP3() {
    if (fluxoPOP3) delete fluxoPOP3;
    if (socketPOP3->isOpen()) {
        if (popConectado) {
            QTextStream(socketPOP3) << "QUIT\r\n";
            socketPOP3->waitForBytesWritten(1000);
        }
        socketPOP3->disconnectFromHost();
        socketPOP3->waitForDisconnected(3000);
    }
    fluxoPOP3 = nullptr;
    popConectado = false;
}

QString FydelisMailCore::lerRespostaPOP3() {
    while (socketPOP3->bytesAvailable() <= 0 && socketPOP3->isOpen())
        socketPOP3->waitForReadyRead(3000);
    return fluxoPOP3->readLine();
}

bool FydelisMailCore::enviarMensagem(const QString &dest, const QString &ass, const QString &corpo) {
    Q_UNUSED(dest); Q_UNUSED(ass); Q_UNUSED(corpo);
    return !smtpServidor.isEmpty();
}

// 💾 Salvar mensagem em arquivo .eml na pasta correspondente
bool FydelisMailCore::salvarMensagemNaPasta(const MensagemEmail &msg, const QString &nomePasta) {
    QString pasta = caminhoRaiz + nomePasta + "/";
    QString assuntoLimpo = msg.assunto;
    assuntoLimpo.replace(QRegularExpression("[^a-zA-Z0-9_-]"), "_");
    if (assuntoLimpo.length() > 30) assuntoLimpo = assuntoLimpo.left(30);

    QString nomeArquivo = QString("%1_id%2_%3.eml")
        .arg(msg.data.left(10).replace("/", "-").replace(" ", "_"))
        .arg(msg.id, 4, 10, QChar('0'))
        .arg(assuntoLimpo);

    QFile arq(pasta + nomeArquivo);
    if (!arq.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit logStatus("[ERRO] Não pôde salvar: " + arq.errorString());
        return false;
    }

    QTextStream saida(&arq);
    saida.setEncoding(QStringConverter::Utf8);
    saida << "From: " << msg.remetente << "\r\n";
    saida << "Subject: " << msg.assunto << "\r\n";
    saida << "Date: " << msg.data << "\r\n";
    saida << "Message-ID: " << msg.id << "\r\n";
    saida << "Status: " << (msg.lida ? "Lida" : "Nova") << "\r\n";
    saida << "\r\n" << msg.corpo << "\r\n";
    arq.close();

    emit logStatus("[💾] Salva: " + nomePasta + "/" + nomeArquivo);
    return true;
}

// 📂 Carregar mensagens salvas do disco
QList<MensagemEmail> FydelisMailCore::carregarMensagensDaPasta(const QString &nomePasta) {
    QList<MensagemEmail> lista;
    QDir dir(caminhoRaiz + nomePasta + "/");
    dir.setNameFilters({"*.eml"});
    dir.setSorting(QDir::Time | QDir::Reversed);

    for (const QFileInfo &info : dir.entryInfoList()) {
        QFile arq(info.absoluteFilePath());
        if (!arq.open(QIODevice::ReadOnly | QIODevice::Text)) continue;

        MensagemEmail m;
        m.id = info.baseName().split("_id").last().section("_", 0, 0).toInt();

        QTextStream leitura(&arq);
        while (!leitura.atEnd()) {
            QString linha = leitura.readLine();
            if (linha.startsWith("From: "))
                m.remetente = linha.mid(6).trimmed();
            else if (linha.startsWith("Subject: "))
                m.assunto = linha.mid(9).trimmed();
            else if (linha.startsWith("Date: "))
                m.data = linha.mid(6).trimmed();
            else if (linha.isEmpty())
                break;
        }
        m.corpo = leitura.readAll();
        lista.append(m);
    }
    return lista;
}