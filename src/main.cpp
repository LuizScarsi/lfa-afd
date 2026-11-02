#include "../include/Automato.h"
#include "../include/LeitorArquivo.h"
#include "../include/LeitorGramatica.h"
#include "../include/Determinizar.h"
#include "../include/CompletarAutomato.h"
#include "../include/Minimizar.h"
#include "../include/Reconhecer.h"
#include "../include/VerificarEstados.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
    Automato afnd;

    int inicial = afnd.adicionarEstado();
    afnd.definirEstadoInicial(inicial);

    if (!carregarTokens("exemplos/entrada.txt", afnd, inicial)) {
        return 1;
    }

    if (!carregarGramaticaRegular(
            "exemplos/entrada.txt",
            afnd,
            inicial)) {
        return 1;
    }

    cout << "\n========== AFND ==========\n";
    afnd.imprimir();

    cout << "\n========== DETERMINIZACAO ==========\n";
    Automato afd = determinizar(afnd);

    completarAFD(afd);

    cout << "\n========== AFD MINIMIZADO ==========\n";
    Automato minimizado = minimizarAFD(afd);
    minimizado.imprimir();

    // Verifica o resultado da minimizacao.
    verificarEstados(minimizado);

    // Remove estados desnecessarios, preservando o estado de erro.
    Automato afdFinal = removerEstadosInuteis(minimizado);

    cout << "\n========== AFD FINAL ==========\n";
    afdFinal.imprimir();

    verificarEstados(afdFinal);

    // Testa palavras no AFD final.
    cout << "\n========== TESTE DE PALAVRAS ==========\n";

    string palavra;

    while (true) {
        cout << "\nDigite uma palavra (ou 'sair' para encerrar): ";
        getline(cin, palavra);

        if (palavra == "sair") {
            break;
        }

        vector<string> identificacoes;

        if (reconhecer(afdFinal, palavra, identificacoes)) {
            cout << "Palavra reconhecida!\n";
            cout << "Identificacao: ";

            for (const string& token : identificacoes) {
                cout << token << " ";
            }

            cout << "\n";
        } else {
            cout << "Palavra nao reconhecida!\n";
        }
    }

    return 0;
}
