#ifndef FYDELIS_MAIL_H
#define FYDELIS_MAIL_H

#include <QObject>

class FydelisMailCore;

class FydelisMail : public QObject {
    Q_OBJECT
public:
    explicit FydelisMail(QObject *pai = nullptr);
    ~FydelisMail();
    FydelisMailCore* nucleo() const;

public slots:
    void iniciarInterface();

signals:
    void logMensagem(const QString &texto);

private:
    FydelisMailCore *core;
};

#endif