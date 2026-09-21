#include "../include/fydelis_mail.h"
#include <QCryptographicHash>

extern "C" {

void fydelis_xor_block(unsigned char* dados, long tamanho, const unsigned char* chave) {
    if (tamanho <= 0) return;
    for (long i = 0; i < tamanho; i++) {
        dados[i] ^= chave[i % 32];
    }
}

unsigned long long fydelis_hash_bloco(const unsigned char* entrada, unsigned char* saida, long tamanho) {
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(reinterpret_cast<const char*>(entrada), tamanho);
    QByteArray res = hash.result();
    for (int i = 0; i < 32 && i < tamanho; i++)
        saida[i] = static_cast<unsigned char>(res[i]);
    return tamanho;
}

}