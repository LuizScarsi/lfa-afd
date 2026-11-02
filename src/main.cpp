#include "../include/Automato.h"

#include <iostream>

int main() {
    Automato afnd;

    // Criando o estado inicial
    int q0 = afnd.adicionarEstado();

    afnd.definirEstadoInicial(q0);

    // Token "ab"
    int q1 = afnd.adicionarEstado();
    int q2 = afnd.adicionarEstado();

    afnd.adicionarTransicao(q0, 'a', q1);
    afnd.adicionarTransicao(q1, 'b', q2);

    afnd.marcarFinal(q2, "AB");

    // Token "ac"
    int q3 = afnd.adicionarEstado();
    int q4 = afnd.adicionarEstado();

    afnd.adicionarTransicao(q0, 'a', q3);
    afnd.adicionarTransicao(q3, 'c', q4);

    afnd.marcarFinal(q4, "AC");

    // Exibindo o automato
    afnd.imprimir();

    return 0;
}