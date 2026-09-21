# 🔥 Fydelis-Mail — Firebird Secure v1.0.0

> **Correio Eletrônico Criptografado & Cliente de E-Mail de Alta Performance**  
> *Parte do ecossistema de segurança e sistemas operacionais FydelisTech.*

---

## 🚀 Sobre o Projeto

O **Fydelis-Mail (Firebird Secure)** é um cliente de e-mail moderno desenvolvido em **C++ com o framework Qt (Qt5/Qt6)**, projetado para oferecer uma interface limpa, rápida e segura. Ele integra protocolos padrão da indústria (POP3/SMTP sobre SSL/TLS) com rotinas criptográficas de baixo nível para garantir total privacidade e controle sobre as mensagens locais.

---

## 🛡️ Principais Funcionalidades

- **Criptografia Fim-a-Fim & Armazenamento Local:** As mensagens sincronizadas são estruturadas e armazenadas localmente em formato `.eml` organizado por diretórios dedicados (`~/Fydelis-Arquivos/`).
- **Comunicação Segura (SSL/TLS):** Suporte nativo a sockets criptografados (`QSslSocket`) para conexões blindadas com servidores de e-mail corporativos e pessoais (Gmail, Outlook, etc.)[cite: 6].
- **Sincronização POP3 / SMTP Real:** Motor robusto de conexão capaz de listar caixas de entrada remotas, baixar cabeçalhos/corpos e despachar correspondências criptografadas[cite: 6].
- **Suporte a 2FA via Senhas de Aplicativo:** Compatibilidade total com provedores modernos que exigem validação em duas etapas através de tokens de aplicativo dedicados.
- **Interface Gráfica Glassmorphism / Dark Theme:** Visual imersivo em modo escuro com layout triplo de alta produtividade (Pastas, Lista de Mensagens e Leitor/Editor).

---

## 📂 Arquitetura do Repositório

```text
Fydelis-Mail/
├── include/              # Cabeçalhos globais e definições de classes
├── src/                  # Código-fonte principal em C++
│   ├── main.cpp          # Ponto de entrada, interface Qt e diálogos de configuração
│   └── mailcore.cpp      # Núcleo de rede, sockets POP3/SMTP e manipulação de arquivos
├── crypto/               # Módulos de criptografia e rotinas nativas (C/Assembly)
└── Fydelis-Arquivos/     # Diretório local gerado para cache e persistência (.eml)
