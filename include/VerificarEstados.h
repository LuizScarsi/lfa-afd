#ifndef VERIFICAR_ESTADOS_H
#define VERIFICAR_ESTADOS_H

#include "Automato.h"

void verificarEstados(const Automato& automato);

// Retorna uma copia do automato sem estados inalcançaveis
// e sem estados mortos comuns. O estado de erro e preservado.
Automato removerEstadosInuteis(const Automato& automato);

#endif
