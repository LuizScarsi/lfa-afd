#ifndef RECONHECER_H
#define RECONHECER_H

#include "Automato.h"
#include <string>
#include <vector>

using namespace std;

// Reconhece uma palavra no AFD e preenche os tokens identificados caso seja aceita.
bool reconhecer(
    const Automato& afd,
    const string& palavra,
    vector<string>& identificacoes
);

// Versao com rastreamento completo do caminho percorrido para apresentacao
bool reconhecerComRastro(
    const Automato& afd,
    const string& palavra,
    vector<string>& identificacoes,
    string& rastro
);

#endif
