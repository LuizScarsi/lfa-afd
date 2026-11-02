#include "../include/Determinizar.h"

#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <vector>

using namespace std;

// Calcula o fecho-epsilon de um conjunto de estados
set<int> fechoEpsilon(
    const Automato& automato,
    const set<int>& conjunto
) {
    set<int> resultado = conjunto;
    vector<int> pilha;

    for (int estado : conjunto) {
        pilha.push_back(estado);
    }

    while (!pilha.empty()) {
        int atual = pilha.back();
        pilha.pop_back();

        for (const Transicao& t : automato.getTransicoes()) {

            if (t.origem == atual && t.epsilon) {

                if (resultado.insert(t.destino).second) {
                    pilha.push_back(t.destino);
                }
            }
        }
    }

    return resultado;
}

// Transforma um AFND em AFD
Automato determinizar(const Automato& afnd) {

    Automato afd;

    // Relaciona cada conjunto de estados do AFND
    // com um estado correspondente no AFD
    map<set<int>, int> estadosCriados;

    // Guarda os conjuntos que ainda precisam ser processados
    queue<set<int>> fila;

    // Cria um estado do AFD para um conjunto do AFND
    auto criarEstado = [&](const set<int>& conjunto) {

        int novoEstado = afd.adicionarEstado();

        estadosCriados[conjunto] = novoEstado;
        fila.push(conjunto);

        cout << "Q" << novoEstado << " representa {";

        bool primeiro = true;
        bool final = false;

        for (int estado : conjunto) {

            if (!primeiro) {
                cout << ", ";
            }

            cout << "q" << estado;
            primeiro = false;

            const Estado& original = afnd.getEstados()[estado];

            if (original.final) {
                final = true;

                for (const string& token : original.tokens) {
                    afd.marcarFinal(novoEstado, token);
                }
            }
        }

        cout << "}\n";

        // Se for final, mas não possuir nome de token,
        // ainda assim precisa ser marcado como final
        if (final) {
            afd.marcarFinal(novoEstado);
        }

        return novoEstado;
    };

    // O estado inicial do AFD é o fecho-epsilon
    // do estado inicial do AFND
    set<int> inicial = fechoEpsilon(
        afnd,
        {afnd.getEstadoInicial()}
    );

    int estadoInicial = criarEstado(inicial);

    afd.definirEstadoInicial(estadoInicial);

    // Processa os conjuntos enquanto houver novos estados
    while (!fila.empty()) {

        set<int> atual = fila.front();
        fila.pop();

        int origem = estadosCriados[atual];

        // Testa cada símbolo do alfabeto
        for (char simbolo : afnd.getAlfabeto()) {

            set<int> destinos;

            // Descobre todos os destinos possíveis
            for (int estado : atual) {

                for (const Transicao& t : afnd.getTransicoes()) {

                    if (t.origem == estado &&
                        !t.epsilon &&
                        t.simbolo == simbolo) {

                        destinos.insert(t.destino);
                    }
                }
            }

            // Não cria transição para conjunto vazio.
            // O estado de erro será adicionado depois.
            if (destinos.empty()) {
                continue;
            }

            // Considera também as transições epsilon
            destinos = fechoEpsilon(afnd, destinos);

            int destino;

            // Verifica se o conjunto já foi criado
            auto encontrado = estadosCriados.find(destinos);

            if (encontrado != estadosCriados.end()) {

                destino = encontrado->second;

            } else {

                destino = criarEstado(destinos);
            }

            // Adiciona a transição determinística
            afd.adicionarTransicao(origem, simbolo, destino);
        }
    }

    return afd;
}