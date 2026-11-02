#include "../include/Automato.h"
#include "../include/LeitorArquivo.h"
#include "../include/Determinizar.h"
#include "../include/CompletarAutomato.h"
#include "../include/Minimizar.h"

#include <iostream>

using namespace std;

int main() {

    Automato afnd;

    // Cria o estado inicial do AFND
    int inicial = afnd.adicionarEstado();

    afnd.definirEstadoInicial(inicial);

    // Carrega os tokens
    if (!carregarTokens("exemplos/entrada.txt", afnd, inicial)) {
        return 1;
    }

    cout << "\n========== AFND ==========\n";

    afnd.imprimir();

    // Realiza a determinização
    cout << "\n========== DETERMINIZACAO ==========\n";

    Automato afd = determinizar(afnd);
    completarAFD(afd);
    Automato minimizado = minimizarAFD(afd);

    cout << "\n========== AFD minimizado ==========\n";

    minimizado.imprimir();

    return 0;
}