#ifndef COMPLETAR_AUTOMATO_H
#define COMPLETAR_AUTOMATO_H

#include "Automato.h"

// Acrescenta o estado de erro ao final do processo de minimizacao,
// completando todas as celulas nao mapeadas para apontarem para o estado de erro.
// Todas as transicoes a partir do estado de erro permanecem nele mesmo.
void completarAFD(Automato& afd);

#endif
