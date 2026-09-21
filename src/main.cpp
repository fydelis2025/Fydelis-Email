#include "../include/fydelis_mail.h"
#include "mailcore.h"
#include <QApplication>
#include <QMainWindow>
#include <QSplitter>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStatusBar>
#include <QMenuBar>
#include <QTimer>
#include <QDialog>
#include <QFormLayout>
#include <QSpinBox>
#include <QCheckBox>

class DialogoConfig : public QDialog {
public:
    DialogoConfig(QWidget *pai = nullptr) : QDialog(pai) {
        setWindowTitle("⚙️ Configuração de Conta");
        resize(520, 500);
        auto *form = new QFormLayout(this);

        edtSMTP_srv = new QLineEdit("smtp.gmail.com");
        edtSMTP_prt = new QSpinBox; edtSMTP_prt->setRange(1,65535); edtSMTP_prt->setValue(465);
        edtPOP_srv = new QLineEdit("pop.gmail.com");
        edtPOP_prt = new QSpinBox; edtPOP_prt->setRange(1,65535); edtPOP_prt->setValue(995);
        edtUsuario = new QLineEdit;
        edtSenha = new QLineEdit; edtSenha->setEchoMode(QLineEdit::Password);
        avisoSenha = new QLabel("💡 Com verificação em 2 etapas → use <b>Senha de Aplicativo</b>");
        avisoSenha->setStyleSheet("color:#FFCC00; font-size:9pt;");
        avisoSenha->setWordWrap(true);

        form->addRow("<b>📤 SMTP — Envio</b>", new QLabel);
        form->addRow("Servidor:", edtSMTP_srv);
        form->addRow("Porta:", edtSMTP_prt);
        form->addRow("<b>📥 POP3 — Caixa de Entrada</b>", new QLabel);
        form->addRow("Servidor:", edtPOP_srv);
        form->addRow("Porta:", edtPOP_prt);
        form->addRow("<b>🔐 Acesso</b>", new QLabel);
        form->addRow("E-mail:", edtUsuario);
        form->addRow("Senha:", edtSenha);
        form->addRow("", avisoSenha);

        auto *botoes = new QHBoxLayout;
        auto *btnOK = new QPushButton("✅ Salvar e Conectar");
        auto *btnCanc = new QPushButton("❌ Cancelar");
        botoes->addStretch();
        botoes->addWidget(btnOK);
        botoes->addWidget(btnCanc);
        form->addRow(botoes);

        connect(btnOK, &QPushButton::clicked, this, &QDialog::accept);
        connect(btnCanc, &QPushButton::clicked, this, &QDialog::reject);
    }

    void aplicar(FydelisMailCore *c) {
        c->configurarSMTP(edtSMTP_srv->text(), edtSMTP_prt->value(),
                          edtUsuario->text(), edtSenha->text());
        c->configurarPOP3(edtPOP_srv->text(), edtPOP_prt->value(),
                          edtUsuario->text(), edtSenha->text());
    }

    QLineEdit *edtSMTP_srv, *edtPOP_srv, *edtUsuario, *edtSenha;
    QSpinBox *edtSMTP_prt, *edtPOP_prt;
    QLabel *avisoSenha;
};

#include "../include/fydelis_mail.h"
#include "mailcore.h"
#include <QApplication>
#include <QMainWindow>
#include <QSplitter>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStatusBar>
#include <QMenuBar>
#include <QTimer>
#include <QDialog>
#include <QFormLayout>
#include <QSpinBox>
#include <QCheckBox>

// === MANTENHA AQUI A CLASSE DialogoConfig que já temos ===

class JanelaFirebird : public QMainWindow {
public:
    explicit JanelaFirebird(FydelisMailCore *nucleo, QWidget *pai = nullptr)
        : QMainWindow(pai), core(nucleo) {
        setWindowTitle("Fydelis-Email — Firebird Secure");
        resize(1280, 750);

        // Conectar status
        connect(core, &FydelisMailCore::logStatus, this, [this](const QString &msg){
            statusBar()->showMessage(msg, 5000);
        });

        // 🎨 TEMA — igual ao original
        setStyleSheet(R"(
            QMainWindow { background: #0A100A; }
            QWidget { background: #0A100A; color: #E0FFE0; font-family: "Segoe UI", sans-serif; font-size: 10pt; }
            QMenuBar { background: #070F07; border-bottom: 1px solid #1A4A1A; padding: 4px; }
            QMenuBar::item:selected { background: #1A4A1A; color: #88FF88; }
            QListWidget#pastas {
                background: #0C180C; border: none; border-right: 1px solid #1A3A1A; padding: 8px;
            }
            QListWidget#pastas::item {
                padding: 12px; margin: 2px; border-radius: 6px;
            }
            QListWidget#pastas::item:selected {
                background: #1A4A1A; color: #FFFFFF; font-weight: bold;
            }
            QListWidget#mensagens {
                background: #0E1C0E; border: none; border-right: 1px solid #1A3A1A; padding: 8px;
            }
            QListWidget#mensagens::item {
                padding: 10px; border-bottom: 1px solid #1A3A1A;
            }
            QListWidget#mensagens::item:selected {
                background: #1A4A1A; color: #FFFFFF;
            }
            QTextEdit { background: #0E1C0E; border: none; padding: 16px; font-size: 11pt; }
            QLineEdit { background: #0E1C0E; border: 1px solid #2A5F2A;
                border-radius: 4px; padding: 8px 12px; color: #FFFFFF; }
            QLineEdit:focus { border-color: #00FF66; background: #122212; }
            QPushButton {
                background: qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 #2A662A,stop:1 #1A441A);
                color: #FFFFFF; border: 1px solid #3A883A;
                border-radius: 6px; padding: 10px 18px; font-weight: bold;
            }
            QPushButton:hover {
                background: qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 #3A883A,stop:1 #2A662A);
                border-color: #00FF66;
            }
            QLabel#titulo { color: #FFCC00; font-size: 14pt; font-weight: bold; }
            QStatusBar { background: #070F07; border-top: 1px solid #1A3A1A; color: #66CC66; }
            QSplitter::handle { background: #1A3A1A; width: 3px; }
        )");

        // 📋 Menu
        QMenu *arquivo = menuBar()->addMenu("&Arquivo");
        arquivo->addAction("⚙️ Configurar Conta", this, &JanelaFirebird::abrirConfig);
        arquivo->addSeparator();
        arquivo->addAction("Sair");
        QMenu *ajuda = menuBar()->addMenu("&Ajuda");

        // 🧱 LAYOUT TRIPLO — IGUAL AO ORIGINAL!
        QSplitter *principal = new QSplitter(Qt::Horizontal, this);

        // ⬅️ PAINEL ESQUERDO — Pastas
        QWidget *painelEsq = new QWidget;
        QVBoxLayout *layEsq = new QVBoxLayout(painelEsq);
        layEsq->setContentsMargins(12, 15, 12, 15);

        QLabel *logo = new QLabel("<h2 style='color:#FFCC00; margin:0;'>🔥 FYDELIS</h2>"
                                   "<p style='color:#88CC88; margin:4px 0;'>EMAIL SEGURO</p>");
        logo->setObjectName("titulo");
        layEsq->addWidget(logo);

        pastas = new QListWidget;
        pastas->setObjectName("pastas");
        pastas->addItem("📥 Caixa de Entrada");
        pastas->addItem("📤 Itens Enviados");
        pastas->addItem("📁 Rascunhos");
        pastas->addItem("🗑️  Lixeira");
        pastas->addItem("🔑 Chaves de Segurança");
        pastas->setCurrentRow(0);
        layEsq->addWidget(pastas);

        QPushButton *btnBaixar = new QPushButton("📥 BAIXAR CAIXA");
        QPushButton *btnNovo = new QPushButton("✉️  NOVA MENSAGEM");
        layEsq->addWidget(btnBaixar);
        layEsq->addWidget(btnNovo);
        layEsq->addStretch();

        // 📄 PAINEL CENTRAL — Lista de Mensagens
        mensagens = new QListWidget;
        mensagens->setObjectName("mensagens");
        mensagens->addItem("ℹ️ Configure sua conta → clique em 'Baixar Caixa'");
        mensagens->addItem("   para carregar mensagens reais do servidor POP3");

        // ➡️ PAINEL DIREITO — Escrever/Ver Mensagem
        QWidget *painelDir = new QWidget;
        QVBoxLayout *layDir = new QVBoxLayout(painelDir);
        layDir->setContentsMargins(15, 15, 15, 15);

        layDir->addWidget(new QLabel("Para:"));
        destino = new QLineEdit;
        destino->setPlaceholderText("voce@exemplo.com");
        layDir->addWidget(destino);

        layDir->addWidget(new QLabel("Assunto:"));
        assunto = new QLineEdit;
        assunto->setPlaceholderText("Assunto da mensagem");
        layDir->addWidget(assunto);

        layDir->addWidget(new QLabel("Mensagem:"));
        texto = new QTextEdit;
        texto->setPlaceholderText("Escreva sua mensagem...\nToda comunicação é criptografada 🔒");
        texto->setMinimumHeight(250);
        layDir->addWidget(texto);

        QHBoxLayout *botoes = new QHBoxLayout;
        QPushButton *btnEnviar = new QPushButton("🚀 ENVIAR");
        QPushButton *btnLimpar = new QPushButton("🗑️  Limpar");
        botoes->addWidget(btnEnviar);
        botoes->addStretch();
        botoes->addWidget(btnLimpar);
        layDir->addLayout(botoes);

        // 🔗 Conectar botões
        connect(btnBaixar, &QPushButton::clicked, this, &JanelaFirebird::baixarCaixa);
        connect(btnNovo, &QPushButton::clicked, this, &JanelaFirebird::limparCampos);
        connect(btnEnviar, &QPushButton::clicked, this, &JanelaFirebird::enviarMensagem);
        connect(btnLimpar, &QPushButton::clicked, this, &JanelaFirebird::limparCampos);
        connect(pastas, &QListWidget::itemClicked, this, [this](QListWidgetItem *item){
            if (item->text().startsWith("📥")) statusBar()->showMessage("Selecionada: Caixa de Entrada", 3000);
        });

        // 📐 Montar layout triplo
        principal->addWidget(painelEsq);
        principal->addWidget(mensagens);
        principal->addWidget(painelDir);
        principal->setStretchFactor(0, 1);
        principal->setStretchFactor(1, 2);
        principal->setStretchFactor(2, 3);

        setCentralWidget(principal);
        statusBar()->showMessage("🔥 Fydelis-Email Firebird | Configure em Arquivo → Configurar Conta");

        // ==================================================
        // ✅ VERIFICAÇÃO AUTOMÁTICA AO ABRIR O PROGRAMA
        // ==================================================
        QTimer::singleShot(600, this, [this](){
            verificarEInicializar();
        });
    }

private slots:
    // ✅ NOVA FUNÇÃO — Executa ao abrir
    void verificarEInicializar() {
        mensagens->clear();

        // 1️⃣ Primeira vez: sem configuração
        if (core->emailUsuario().isEmpty()) {
            mensagens->addItem("👋 Bem-vindo ao Fydelis-Email Firebird!");
            mensagens->addItem("");
            mensagens->addItem("→ Vá em: Arquivo → Configurar Conta");
            mensagens->addItem("   para inserir seus dados de e-mail");
            mensagens->addItem("");
            mensagens->addItem("💡 Com verificação em 2 etapas → use Senha de Aplicativo");
            statusBar()->showMessage("Bem-vindo! Configure sua conta para começar", 10000);
            return;
        }

        // 2️⃣ Já configurado → carrega mensagens salvas localmente
        mensagens->addItem(QString("✅ Conta: %1").arg(core->emailUsuario()));
        mensagens->addItem("📂 Carregando mensagens salvas...");
        statusBar()->showMessage("Conta detectada — carregando...", 5000);

        // Carrega do disco primeiro (rápido e offline)
        QList<MensagemEmail> locais = core->carregarMensagensDaPasta("Caixa de Entrada");
        if (!locais.isEmpty()) {
            mensagens->clear();
            for (const auto &m : locais) {
                QString exib = QString("[💾] %1 — %2")
                    .arg(m.assunto.isEmpty() ? "(sem assunto)" : m.assunto.left(45))
                    .arg(m.remetente.isEmpty() ? "?" : m.remetente.left(25));
                mensagens->addItem(exib);
            }
            mensagens->addItem("");
            mensagens->addItem(QString("   ✅ %1 mensagens carregadas do disco").arg(locais.size()));
        }

        // 3️⃣ Depois verifica online em segundo plano
        QTimer::singleShot(1000, this, [this](){
            statusBar()->showMessage("📥 Verificando servidor por novas mensagens...", 4000);
            QVariantList novas = core->baixarCaixaPOP3(20);
            if (!novas.isEmpty()) {
                mensagens->clear();
                for (int i = 0; i < novas.size(); i++) {
                    MensagemEmail m = novas[i].value<MensagemEmail>();
                    QString exib = QString("%1 — %2")
                        .arg(m.assunto.isEmpty() ? "(sem assunto)" : m.assunto.left(45))
                        .arg(m.remetente.isEmpty() ? "?" : m.remetente.left(25));
                    mensagens->addItem(exib);
                }
                statusBar()->showMessage(QString("✅ Atualizado: %1 mensagens").arg(novas.size()), 5000);
            }
        });
    }

    void abrirConfig() {
        DialogoConfig dlg(this);
        if (dlg.exec() == QDialog::Accepted) {
            dlg.aplicar(core);
            statusBar()->showMessage("✅ Conta configurada!", 3000);
            // Recarrega após configurar
            QTimer::singleShot(300, this, [this](){ verificarEInicializar(); });
        }
    }

    void baixarCaixa() {
        mensagens->clear();
        statusBar()->showMessage("📥 Conectando ao servidor POP3...");

        QVariantList lista = core->baixarCaixaPOP3(20);

        if (lista.isEmpty()) {
            mensagens->addItem("⚠️ Sem conexão ou sem mensagens novas");
            mensagens->addItem("   → Carregando mensagens salvas localmente...");

            QList<MensagemEmail> locais = core->carregarMensagensDaPasta("Caixa de Entrada");
            if (!locais.isEmpty()) {
                for (const auto &m : locais) {
                    QString exib = QString("[💾] %1 — %2")
                        .arg(m.assunto.isEmpty() ? "(sem assunto)" : m.assunto.left(45))
                        .arg(m.remetente.isEmpty() ? "?" : m.remetente.left(25));
                    mensagens->addItem(exib);
                }
                mensagens->addItem("");
                mensagens->addItem(QString("   ✅ %1 mensagens carregadas do disco").arg(locais.size()));
                statusBar()->showMessage(QString("📂 %1 mensagens locais carregadas").arg(locais.size()), 5000);
            } else {
                mensagens->addItem("   Verifique: servidor, porta, usuário, senha");
                mensagens->addItem("   💡 Com 2FA → use Senha de Aplicativo (16 caracteres)");
                statusBar()->showMessage("⚠️ Nenhuma mensagem encontrada", 5000);
            }
        } else {
            for (int i = 0; i < lista.size(); i++) {
                MensagemEmail m = lista[i].value<MensagemEmail>();
                QString exib = QString("%1 — %2")
                    .arg(m.assunto.isEmpty() ? "(sem assunto)" : m.assunto.left(45))
                    .arg(m.remetente.isEmpty() ? "?" : m.remetente.left(25));
                mensagens->addItem(exib);
            }
            statusBar()->showMessage(QString("✅ %1 mensagens baixadas e salvas!").arg(lista.size()), 5000);
        }
    }

    void enviarMensagem() {
        if (destino->text().isEmpty()) {
            statusBar()->showMessage("⚠️ Preencha o destinatário!", 3000);
            return;
        }
        statusBar()->showMessage("📤 Enviando mensagem...");
        QTimer::singleShot(1200, [this](){
            bool ok = core->enviarMensagem(destino->text(), assunto->text(), texto->toPlainText());
            if (ok) {
                statusBar()->showMessage("✅ Mensagem enviada com sucesso! 🔒", 5000);
                mensagens->insertItem(0, "✉️  Enviado → " + destino->text());
                limparCampos();
            } else {
                statusBar()->showMessage("❌ Falha — verifique configurações SMTP", 5000);
            }
        });
    }

    void limparCampos() {
        destino->clear(); assunto->clear(); texto->clear();
        destino->setFocus();
    }

private:
    FydelisMailCore *core;
    QListWidget *pastas;
    QListWidget *mensagens;
    QLineEdit *destino, *assunto;
    QTextEdit *texto;
};

// ✅ Classe principal
FydelisMail::FydelisMail(QObject *pai)
    : QObject(pai), core(new FydelisMailCore) {}

FydelisMail::~FydelisMail() { delete core; }
FydelisMailCore* FydelisMail::nucleo() const { return core; }

void FydelisMail::iniciarInterface() {
    static JanelaFirebird janela(core);
    janela.show();
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QApplication::setApplicationName("Fydelis-Email — Firebird");
    QApplication::setApplicationVersion("1.0.0-firebird");

    FydelisMail app;
    app.iniciarInterface();
    return a.exec();
}