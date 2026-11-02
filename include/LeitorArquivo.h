#ifndef LEITOR_ARQUIVO_H
#define LEITOR_ARQUIVO_H

#include <string>
#include "Automato.h"

bool carregarTokens(
    const string& caminho,
    Automato& automato,
    int estadoInicial
);

#endif