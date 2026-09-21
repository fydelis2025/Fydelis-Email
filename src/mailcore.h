#ifndef MAILCORE_H
#define MAILCORE_H

#include <QString>
#include <QByteArray>
#include <QVariantList>
#include <QSslSocket>
#include <QTimer>
#include <QTextStream>
#include <QObject>       // ✅ Faltava esta linha!
#include <QList>         // ✅ Também faltou para QList<MensagemEmail>

struct MensagemEmail {
    int id;
    QString remetente;
    QString assunto;
    QString data;
    QString corpo;
    bool lida;
};

class FydelisMailCore : public QObject {
    Q_OBJECT
public:
    explicit FydelisMailCore(QObject *pai = nullptr);
    ~FydelisMailCore();

    // ⚙️ Configurações
    void configurarSMTP(const QString &servidor, int porta, const QString &usuario, const QString &senha, bool tls = true);
    void configurarPOP3(const QString &servidor, int porta, const QString &usuario, const QString &senha, bool tls = true);
    void configurarIMAP(const QString &servidor, int porta, const QString &usuario, const QString &senha, bool tls = true);

    // 📤 Enviar
    bool enviarMensagem(const QString &dest, const QString &ass, const QString &corpo);
	
	bool salvarMensagemNaPasta(const MensagemEmail &msg, const QString &nomePasta);
    QList<MensagemEmail> carregarMensagensDaPasta(const QString &nomePasta);

    // 📥 BAIXAR CAIXA DE ENTRADA — REAL
    bool conectarPOP3();
    QVariantList baixarCaixaPOP3(int limite = 20);
    void desconectarPOP3();

    // 🔑 Dados
    QString emailUsuario() const { return popUsuario; }
    bool conectado() const { return popConectado; }

signals:
    void logStatus(const QString &texto);

private:
    QString smtpServidor, smtpUsuario, smtpSenha;
    int smtpPorta;
    bool smtpTLS;

    QString popServidor, popUsuario, popSenha;
    int popPorta;
    bool popTLS, popConectado;

    QSslSocket *socketPOP3;
    QTextStream *fluxoPOP3;
    QList<MensagemEmail> cache;

    QString lerRespostaPOP3();
	QString caminhoRaiz;
};

#endif