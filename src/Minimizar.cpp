#include "../include/Minimizar.h"

#include <iostream>
#include <queue>
#include <vector>

using namespace std;

Automato minimizarAFD(const Automato& afd) {
    const vector<Estado>& estados = afd.getEstados();
    const vector<Transicao>& transicoes = afd.getTransicoes();
    int total = static_cast<int>(estados.size());
    int inicial = afd.getEstadoInicial();

    cout << "\n--- Processo de Minimizacao (Sem Classes de Equivalencia) ---\n";
    cout << "Objetivo: Eliminar estados inalcancaveis e estados mortos.\n";

    // 1. Identificar estados alcancaveis a partir do estado inicial (BFS direta)
    vector<bool> alcancavel(total, false);
    if (inicial >= 0 && inicial < total) {
        queue<int> fila;
        alcancavel[inicial] = true;
        fila.push(inicial);

        while (!fila.empty()) {
            int atual = fila.front();
            fila.pop();

            for (const Transicao& t : transicoes) {
                if (t.origem == atual && !alcancavel[t.destino]) {
                    alcancavel[t.destino] = true;
                    fila.push(t.destino);
                }
            }
        }
    }

    // 2. Identificar estados vivos que alcancam ao menos um estado final (BFS reversa)
    vector<bool> alcancaFinal(total, false);
    vector<vector<int>> transicoesReversas(total);
    queue<int> filaRev;

    for (const Transicao& t : transicoes) {
        transicoesReversas[t.destino].push_back(t.origem);
    }

    for (int i = 0; i < total; ++i) {
        if (estados[i].final) {
            alcancaFinal[i] = true;
            filaRev.push(i);
        }
    }

    while (!filaRev.empty()) {
        int atual = filaRev.front();
        filaRev.pop();

        for (int anterior : transicoesReversas[atual]) {
            if (!alcancaFinal[anterior]) {
                alcancaFinal[anterior] = true;
                filaRev.push(anterior);
            }
        }
    }

    // 3. Relatorio de diagnostico
    cout << "\nDiagnostico dos Estados:\n";
    vector<int> inalcancaveis;
    vector<int> mortos;
    vector<bool> manter(total, false);

    for (int i = 0; i < total; ++i) {
        bool ok = true;
        if (!alcancavel[i]) {
            inalcancaveis.push_back(i);
            ok = false;
        } else if (!alcancaFinal[i] && i != inicial) {
            mortos.push_back(i);
            ok = false;
        }

        if (ok) {
            manter[i] = true;
        }
    }

    cout << "  - Estados Alcancaveis: ";
    for (int i = 0; i < total; ++i) {
        if (alcancavel[i]) cout << estados[i].nome << " ";
    }
    cout << "\n";

    cout << "  - Estados Inalcancaveis: ";
    if (inalcancaveis.empty()) cout << "Nenhum";
    else {
        for (int id : inalcancaveis) cout << estados[id].nome << " ";
    }
    cout << "\n";

    cout << "  - Estados Vivos (alcancam final): ";
    for (int i = 0; i < total; ++i) {
        if (alcancaFinal[i]) cout << estados[i].nome << " ";
    }
    cout << "\n";

    cout << "  - Estados Mortos: ";
    if (mortos.empty()) cout << "Nenhum";
    else {
        for (int id : mortos) cout << estados[id].nome << " ";
    }
    cout << "\n";

    // 4. Construcao do AFD Minimizado (renumerado e livre de inuteis)
    Automato minimizado;
    vector<int> mapeamento(total, -1);
    int contadorNovo = 0;

    for (int i = 0; i < total; ++i) {
        if (manter[i]) {
            string novoNome = "M" + to_string(contadorNovo++);
            int novoId = minimizado.adicionarEstado(novoNome);
            mapeamento[i] = novoId;

            if (estados[i].final) {
                minimizado.marcarFinal(novoId);
                for (const string& tok : estados[i].tokens) {
                    minimizado.marcarFinal(novoId, tok);
                }
            }
        }
    }

    if (inicial >= 0 && inicial < total && mapeamento[inicial] != -1) {
        minimizado.definirEstadoInicial(mapeamento[inicial]);
    }

    // Copia as transicoes validas
    for (const Transicao& t : transicoes) {
        if (manter[t.origem] && manter[t.destino]) {
            minimizado.adicionarTransicao(
                mapeamento[t.origem],
                t.simbolo,
                mapeamento[t.destino]
            );
        }
    }

    if (inalcancaveis.empty() && mortos.empty()) {
        cout << "Resultado: O AFD ja se encontrava livre de estados inalcancaveis e mortos.\n";
    } else {
        cout << "Resultado: " << (inalcancaveis.size() + mortos.size()) 
             << " estado(s) inutil(eis) removido(s) com sucesso.\n";
    }

    return minimizado;
}
