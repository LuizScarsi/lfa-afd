#ifndef VERIFICAR_ESTADOS_H
#define VERIFICAR_ESTADOS_H

#include "Automato.h"
#include <vector>

using namespace std;

// Funcoes de diagnostico dos estados do automato
vector<bool> encontrarAlcancaveis(const Automato& automato);
vector<bool> encontrarEstadosQueChegamAoFinal(const Automato& automato);

void verificarEstados(const Automato& automato);

#endif
