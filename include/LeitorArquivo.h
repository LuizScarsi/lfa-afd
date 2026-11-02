#ifndef LEITOR_ARQUIVO_H
#define LEITOR_ARQUIVO_H

#include "Automato.h"
#include <string>

using namespace std;

// Carrega tanto tokens quanto gramaticas regulares a partir do mesmo arquivo fonte,
// exatamente conforme o formato especificado pelo professor (com ou sem marcadores de secao).
bool carregarArquivo(
    const string& caminho,
    Automato& automato,
    const string& tokenPadraoGramatica = "variavel"
);

// Funcoes de compatibilidade caso chamadas isoladamente
bool carregarTokens(
    const string& caminho,
    Automato& automato,
    int estadoInicial
);

bool carregarGramaticaRegular(
    const string& caminho,
    Automato& automato,
    int estadoInicial,
    const string& nomeToken = "variavel"
);

#endif
