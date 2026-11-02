#ifndef MINIMIZAR_H
#define MINIMIZAR_H

#include "Automato.h"

// Minimizacao conforme explicitamente exigido pelo professor:
// "minimo SEM a aplicacao de classes de equivalencia entre estados, livre de estados inalcancaveis e mortos."
// O algoritmo elimina estados inalcancaveis (a partir do estado inicial) e
// estados mortos (que nao alcancam nenhum estado de aceitacao).
Automato minimizarAFD(const Automato& afd);

#endif
