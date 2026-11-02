#include "../include/VerificarEstados.h"

#include <iostream>
#include <queue>
#include <vector>

using namespace std;

vector<bool> encontrarAlcancaveis(const Automato& automato) {
    int total = static_cast<int>(automato.getEstados().size());
    vector<bool> alcancavel(total, false);
    int inicial = automato.getEstadoInicial();

    if (inicial < 0 || inicial >= total) {
        return alcancavel;
    }

    queue<int> fila;
    alcancavel[inicial] = true;
    fila.push(inicial);

    while (!fila.empty()) {
        int atual = fila.front();
        fila.pop();

        for (const Transicao& t : automato.getTransicoes()) {
            if (t.origem == atual && !alcancavel[t.destino]) {
                alcancavel[t.destino] = true;
                fila.push(t.destino);
            }
        }
    }

    return alcancavel;
}

vector<bool> encontrarEstadosQueChegamAoFinal(const Automato& automato) {
    int total = static_cast<int>(automato.getEstados().size());
    vector<bool> chegaFinal(total, false);
    vector<vector<int>> reversas(total);
    queue<int> fila;

    for (const Transicao& t : automato.getTransicoes()) {
        reversas[t.destino].push_back(t.origem);
    }

    for (int i = 0; i < total; ++i) {
        if (automato.getEstados()[i].final) {
            chegaFinal[i] = true;
            fila.push(i);
        }
    }

    while (!fila.empty()) {
        int atual = fila.front();
        fila.pop();

        for (int anterior : reversas[atual]) {
            if (!chegaFinal[anterior]) {
                chegaFinal[anterior] = true;
                fila.push(anterior);
            }
        }
    }

    return chegaFinal;
}

void verificarEstados(const Automato& automato) {
    const vector<Estado>& estados = automato.getEstados();
    int total = static_cast<int>(estados.size());
    int estadoErro = automato.getEstadoErro();

    vector<bool> alcancavel = encontrarAlcancaveis(automato);
    vector<bool> chegaFinal = encontrarEstadosQueChegamAoFinal(automato);

    cout << "\n--- Relatorio de Verificacao Estrutural ---\n";
    cout << "Total de Estados: " << total << "\n";

    cout << "Estados Alcancaveis: ";
    for (int i = 0; i < total; ++i) {
        if (alcancavel[i]) cout << estados[i].nome << " ";
    }
    cout << "\n";

    cout << "Estados Inalcancaveis: ";
    bool temInalc = false;
    for (int i = 0; i < total; ++i) {
        if (!alcancavel[i]) {
            cout << estados[i].nome << " ";
            temInalc = true;
        }
    }
    if (!temInalc) cout << "Nenhum (Livre de inalcancaveis)";
    cout << "\n";

    cout << "Estados Vivos (alcancam final): ";
    for (int i = 0; i < total; ++i) {
        if (chegaFinal[i]) cout << estados[i].nome << " ";
    }
    cout << "\n";

    cout << "Estados Mortos: ";
    bool temMorto = false;
    for (int i = 0; i < total; ++i) {
        if (!chegaFinal[i] && i != estadoErro) {
            cout << estados[i].nome << " ";
            temMorto = true;
        }
    }
    if (!temMorto) cout << "Nenhum (Livre de mortos)";
    cout << "\n";

    if (estadoErro != -1) {
        cout << "Estado de Erro Ativo: " << estados[estadoErro].nome << " (id: " << estadoErro << ")\n";
    }
}
