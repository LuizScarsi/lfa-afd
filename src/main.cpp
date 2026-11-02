#include "../include/Automato.h"
#include "../include/LeitorArquivo.h"

#include <iostream>

using namespace std;

int main() {
    Automato afnd;

    // Cria o estado inicial compartilhado
    int inicial = afnd.adicionarEstado();

    afnd.definirEstadoInicial(inicial);

    // Carrega os tokens do arquivo
    if (!carregarTokens("exemplos/entrada.txt", afnd, inicial)) {
        return 1;
    }

    // Exibe o resultado
    afnd.imprimir();

    return 0;
}