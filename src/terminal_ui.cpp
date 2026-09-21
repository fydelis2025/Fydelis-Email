#include "../include/fydelis_mail.h"
#include <QTextStream>
#include <QDateTime>

void exibirBannerFydelis() {
    QTextStream saida(stdout);
    saida << "\033[38;5;46m\n";
    saida << "╔══════════════════════════════════════════════════╗\n";
    saida << "║  F Y D E L I S - M A I L  |  FIREBIRD SECURE v1.0 ║\n";
    saida << "║  Correio Criptografado — Rede FydelisTech OS      ║\n";
    saida << "║  Criptografia nativa · Verificação FydelisGuard   ║\n";
    saida << "╚══════════════════════════════════════════════════╝\033[0m\n";
}

void menuTerminal(FydelisMailCore &servico) {
    QTextStream io(stdin, stdout);
    while (true) {
        io << "\n\033[38;5;39m[ Fydelis-Mail Terminal ]\033[0m\n";
        io << " 1 → Conectar Conta\n";
        io << " 2 → Enviar Mensagem Criptografada\n";
        io << " 3 → Listar Caixa de Entrada\n";
        io << " 4 → Gerar Par de Chaves\n";
        io << " 0 → Sair\n> ";
        io.flush();

        QString opcao = io.readLine();
        if (opcao == "1") {
            io << "Servidor SMTP: "; QString srv = io.readLine();
            io << "Porta: "; int p = io.readLine().toInt();
            io << "Usuário: "; QString u = io.readLine();
            io << "Senha: "; QString senha = io.readLine();
            if (servico.conectarSMTP(srv, p, u, senha))
                io << "\033[38;5;46m[+] Conectado com TLS 1.3 ativo\033[0m\n";
            else
                io << "\033[38;5;196m[!] Falha na conexão\033[0m\n";
        }
        else if (opcao == "2") {
            io << "Destinatário: "; QString dest = io.readLine();
            io << "Assunto: "; QString ass = io.readLine();
            io << "Corpo (fim com .):\n";
            QString corpo, linha;
            while ((linha = io.readLine()) != ".") corpo += linha + "\n";
            bool ok = servico.enviarMensagemCriptografada(dest, ass, corpo);
            io << (ok ? "\033[38;5;46m[✓] Mensagem enviada e assinada\033[0m\n"
                       : "\033[38;5;196m[✗] Falha no envio\033[0m\n");
        }
        else if (opcao == "0") break;
    }
}