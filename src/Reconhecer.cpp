#include "../include/Reconhecer.h"

using namespace std;

bool reconhecer(
    const Automato& afd,
    const string& palavra,
    vector<string>& identificacoes
) {
    int estadoAtual = afd.getEstadoInicial();

    // Percorre cada simbolo da palavra
    for (char simbolo : palavra) {

        bool encontrou = false;

        // Procura a transicao correspondente
        for (const Transicao& t : afd.getTransicoes()) {

            if (t.origem == estadoAtual &&
                t.simbolo == simbolo &&
                !t.epsilon) {

                estadoAtual = t.destino;
                encontrou = true;
                break;
            }
        }

        // Se nao encontrou transicao, rejeita
        if (!encontrou) {
            return false;
        }
    }

    // Verifica se o estado final da leitura e aceitador
    const Estado& estado = afd.getEstados()[estadoAtual];

    if (!estado.final) {
        return false;
    }

    identificacoes = estado.tokens;

    return true;
}