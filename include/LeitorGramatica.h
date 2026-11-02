#ifndef LEITOR_GRAMATICA_H
#define LEITOR_GRAMATICA_H

#include "Automato.h"
#include <string>

using namespace std;

bool carregarGramaticaRegular(
    const string& caminho,
    Automato& automato,
    int estadoInicial
);

#endif
