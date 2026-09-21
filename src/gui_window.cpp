#include "../include/fydelis_mail.h"
#include "mailcore.h"
#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStatusBar>

class JanelaFydelis : public QMainWindow {
public:
    explicit JanelaFydelis(FydelisMailCore *nucleo, QWidget *pai = nullptr)
        : QMainWindow(pai), core(nucleo) {
        setWindowTitle("🔥 Fydelis-Mail — Firebird Secure");
        resize(920, 650);
        setStyleSheet(R"(
            QMainWindow { background: #0a0f0a; }
            QLabel { color: #33ff66; font-family: Consolas, monospace; }
            QTextEdit, QLineEdit {
                background: #0f1a0f; color: #e0ffe0;
                border: 1px solid #2a5f2a; padding: 8px;
                font-family: Consolas; font-size: 11pt;
            }
            QPushButton {
                background: #1a3a1a; color: #80ff80;
                border: 1px solid #3f8f3f; padding: 10px;
                font-weight: bold; font-size: 10pt;
                border-radius: 4px;
            }
            QPushButton:hover { background: #2a5f2a; color: #ffffff; }
            QStatusBar { background: #0a0f0a; color: #66cc66; }
        )");

        QWidget *centro = new QWidget(this);
        QVBoxLayout *raiz = new QVBoxLayout(centro);
        raiz->setSpacing(12); raiz->setContentsMargins(20,20,20,20);

        QLabel *titulo = new QLabel(
"<pre style='font-size:14pt; line-height:1.3;'>"
"╔══════════════════════════════════════════════════════════════╗\n"
"║          🔥 F Y D E L I S - M A I L  ·  F I R E B I R D       ║\n"
"║              Criptografia Fim-a-Fim — v1.0.0                  ║\n"
"╚══════════════════════════════════════════════════════════════╝</pre>");
        raiz->addWidget(titulo);

        log = new QTextEdit;
        log->setReadOnly(true);
        log->setMinimumHeight(150);
        log->setPlaceholderText("Sistema pronto. Configure sua conta e comece...");
        raiz->addWidget(log);

        dest = new QLineEdit;
        dest->setPlaceholderText("📤 Destinatário  |  exemplo@dominio.com");
        raiz->addWidget(dest);

        assunto = new QLineEdit;
        assunto->setPlaceholderText("📝 Assunto");
        raiz->addWidget(assunto);

        corpo = new QTextEdit;
        corpo->setPlaceholderText("✍️  Sua mensagem... será criptografada antes de enviar.");
        corpo->setMinimumHeight(180);
        raiz->addWidget(corpo);

        QHBoxLayout *botoes = new QHBoxLayout;
        QPushButton *btnEnviar = new QPushButton("✓ ENVIAR CRIPTOGRAFADO");
        QPushButton *btnCaixa = new QPushButton("📥 CAIXA DE ENTRADA");
        QPushButton *btnChaves = new QPushButton("🔑 MINHAS CHAVES");
        botoes->addWidget(btnEnviar);
        botoes->addWidget(btnCaixa);
        botoes->addWidget(btnChaves);
        raiz->addLayout(botoes);

        connect(btnEnviar, &QPushButton::clicked, this, &JanelaFydelis::enviar);
        connect(btnCaixa, &QPushButton::clicked, this, &JanelaFydelis::carregarCaixa);
        connect(btnChaves, &QPushButton::clicked, this, &JanelaFydelis::gerarChaves);

        setCentralWidget(centro);
        statusBar()->showMessage("🔥 Fydelis-Mail Firebird — Sistema Ativo");
    }

private slots:
    void enviar() {
        if (!core) { log->append("[!] Núcleo não inicializado"); return; }
        log->append("[~] Criptografando mensagem...");
        bool ok = core->enviarMensagemCriptografada(
            dest->text().trimmed(),
            assunto->text().trimmed(),
            corpo->toPlainText());
        log->append(ok ? "[✓] MENSAGEM ENVIADA COM SEGURANÇA ✅"
                       : "[!] Falha — verifique conexão/configurações ❌");
    }
    void carregarCaixa() {
        if (!core) return;
        log->append("[~] Sincronizando caixa...");
        QVariantList msgs = core->listarCaixaEntrada(10);
        log->append(QString("[+] %1 mensagens carregadas").arg(msgs.size()));
    }
    void gerarChaves() {
        if (!core) return;
        auto par = core->gerarParChaves();
        log->append("[+] NOVO PAR DE CHAVES GERADO 🔑");
        log->append("  Pública:  " + par.first.toBase64().left(42) + "...");
        log->append("  Privada:  " + par.second.toBase64().left(42) + "...");
        log->append("⚠️ GUARDE A CHAVE PRIVADA EM LOCAL SEGURO!");
    }

private:
    FydelisMailCore *core;
    QTextEdit *log, *corpo;
    QLineEdit *dest, *assunto;
};

// ✅ ÚNICA implementação real — SÓ AQUI!
void FydelisMail::iniciarInterface() {
    emit logMensagem("Interface Firebird Iniciada");
    static JanelaFydelis janela(core);
    janela.show();
}