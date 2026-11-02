#ifndef RECONHECER_H
#define RECONHECER_H

#include "Automato.h"

#include <string>
#include <vector>

bool reconhecer(
    const Automato& afd,
    const string& palavra,
    vector<string>& identificacoes
);

#endif