#ifndef DETERMINIZAR_H
#define DETERMINIZAR_H

#include "Automato.h"
#include <set>

using namespace std;

// Calcula o fecho-epsilon de um conjunto de estados
set<int> fechoEpsilon(const Automato& automato, const set<int>& conjunto);

// Converte um AFND em um AFD pelo teorema de determinizacao (construcao de subconjuntos)
Automato determinizar(const Automato& afnd);

#endif
