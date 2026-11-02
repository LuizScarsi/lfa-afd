#include "../include/CompletarAutomato.h"

using namespace std;

// Verifica se existe uma transicao para determinado simbolo
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

// Completa todas as transicoes do AFD
void completarAFD(Automato& afd) {

    set<char> alfabeto = afd.getAlfabeto();

    int quantidadeEstados =
        static_cast<int>(afd.getEstados().size());

    bool precisaEstadoErro = false;

    // Verifica se existem transicoes faltando
    for (int estado = 0; estado < quantidadeEstados; estado++) {

        for (char simbolo : alfabeto) {

            if (!possuiTransicao(afd, estado, simbolo)) {
                precisaEstadoErro = true;
            }
        }
    }

    // Se todas as transicoes existem, nao precisamos criar erro
    if (!precisaEstadoErro) {
        return;
    }

    // Cria o estado de erro
    int estadoErro = afd.adicionarEstado();

    // Completa as transicoes dos estados originais
    for (int estado = 0; estado < quantidadeEstados; estado++) {

        for (char simbolo : alfabeto) {

            if (!possuiTransicao(afd, estado, simbolo)) {

                afd.adicionarTransicao(
                    estado,
                    simbolo,
                    estadoErro
                );
            }
        }
    }

    // O estado de erro retorna para si mesmo
    for (char simbolo : alfabeto) {

        afd.adicionarTransicao(
            estadoErro,
            simbolo,
            estadoErro
        );
    }
}