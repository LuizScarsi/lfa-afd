#include "../include/CompletarAutomato.h"

#include <set>

using namespace std;

bool possuiTransicao(
    const Automato& automato,
    int estado,
    char simbolo
) {
    for (const Transicao& t : automato.getTransicoes()) {
        if (t.origem == estado &&
            !t.epsilon &&
            t.simbolo == simbolo) {
            return true;
        }
    }

    return false;
}

void completarAFD(Automato& afd) {
    set<char> alfabeto = afd.getAlfabeto();

    int quantidadeEstados =
        static_cast<int>(afd.getEstados().size());

    bool precisaEstadoErro = false;

    for (int estado = 0; estado < quantidadeEstados; estado++) {
        for (char simbolo : alfabeto) {
            if (!possuiTransicao(afd, estado, simbolo)) {
                precisaEstadoErro = true;
            }
        }
    }

    if (!precisaEstadoErro) {
        return;
    }

    int estadoErro = afd.adicionarEstado();
    afd.definirEstadoErro(estadoErro);

    for (int estado = 0; estado < quantidadeEstados; estado++) {
        for (char simbolo : alfabeto) {
            if (!possuiTransicao(afd, estado, simbolo)) {
                afd.adicionarTransicao(estado, simbolo, estadoErro);
            }
        }
    }

    for (char simbolo : alfabeto) {
        afd.adicionarTransicao(estadoErro, simbolo, estadoErro);
    }
}
