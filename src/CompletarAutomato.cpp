#include "../include/CompletarAutomato.h"

#include <iostream>
#include <set>

using namespace std;

void completarAFD(Automato& afd) {
    set<char> alfabeto = afd.getAlfabeto();
    int qtdEstados = static_cast<int>(afd.getEstados().size());

    cout << "\n--- Inclusao do Estado de Erro e Completude do AFD ---\n";

    // Conforme o professor: "Ao final da minimizacao, acrescentar um ultimo estado...
    // Este sera o estado de erro. Todas as celulas do AFD nao mapeadas devem ser
    // preenchidas com estado de erro. Todas as transicoes a partir do estado de erro
    // permanecem no estado de erro."
    
    int idErro = afd.adicionarEstado("Erro");
    afd.definirEstadoErro(idErro);

    int transicoesPreenchidas = 0;

    // 1. Redireciona celulas nao mapeadas dos estados existentes para o estado de erro
    for (int s = 0; s < qtdEstados; ++s) {
        for (char c : alfabeto) {
            if (!afd.temTransicao(s, c)) {
                afd.adicionarTransicao(s, c, idErro);
                transicoesPreenchidas++;
            }
        }
    }

    // 2. Transicoes a partir do estado de erro permanecem no estado de erro
    for (char c : alfabeto) {
        afd.adicionarTransicao(idErro, c, idErro);
    }

    cout << "Estado de erro criado: " << afd.getEstados()[idErro].nome 
         << " (id: " << idErro << ")\n";
    cout << "Total de celulas nao mapeadas direcionadas ao erro: " 
         << transicoesPreenchidas << "\n";
    cout << "Auto-lacos no estado de erro criados para todos os " 
         << alfabeto.size() << " simbolos do alfabeto.\n";
}
