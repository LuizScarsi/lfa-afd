#include "../include/Automato.h"

#include <iostream>
#include <stdexcept>

Automato::Automato() {
    estadoInicial = -1;
}

// Cria um estado e retorna seu identificador
int Automato::adicionarEstado() {
    int id = estados.size();

    Estado novo;
    novo.id = id;
    novo.final = false;

    estados.push_back(novo);

    return id;
}

// Define o estado inicial
void Automato::definirEstadoInicial(int estado) {
    if (estado < 0 || estado >= estados.size()) {
        throw invalid_argument("Estado inicial invalido");
    }

    estadoInicial = estado;
}

// Marca um estado como final
void Automato::marcarFinal(int estado, string token) {
    if (estado < 0 || estado >= estados.size()) {
        throw invalid_argument("Estado final invalido");
    }

    estados[estado].final = true;

    if (!token.empty()) {
        estados[estado].tokens.push_back(token);
    }
}

// Adiciona uma transicao comum
void Automato::adicionarTransicao(
    int origem, char simbolo, int destino
) {
    if (origem < 0 || origem >= estados.size() ||
        destino < 0 || destino >= estados.size()) {
        throw invalid_argument("Estado invalido na transicao");
    }

    Transicao nova;
    nova.origem = origem;
    nova.simbolo = simbolo;
    nova.destino = destino;
    nova.epsilon = false;

    transicoes.push_back(nova);
    alfabeto.insert(simbolo);
}

// Adiciona uma transicao epsilon
void Automato::adicionarTransicaoEpsilon(
    int origem, int destino
) {
    if (origem < 0 || origem >= estados.size() ||
        destino < 0 || destino >= estados.size()) {
        throw invalid_argument("Estado invalido na transicao");
    }

    Transicao nova;
    nova.origem = origem;
    nova.simbolo = '\0';
    nova.destino = destino;
    nova.epsilon = true;

    transicoes.push_back(nova);
}

// Imprime o automato
void Automato::imprimir() {
    cout << "\nESTADOS:\n";

    for (const Estado& estado : estados) {
        cout << "q" << estado.id;

        if (estado.id == estadoInicial) {
            cout << " (Inicial)";
        }

        if (estado.final) {
            cout << " (Final)";
        }

        cout << "\n";
    }

    cout << "\nTRANSICOES:\n";

    for (const Transicao& t : transicoes) {
        cout << "q" << t.origem << " --";

        if (t.epsilon) {
            cout << "epsilon";
        } else {
            cout << t.simbolo;
        }

        cout << "--> q" << t.destino << "\n";
    }

    cout << "\nALFABETO: ";

    for (char simbolo : alfabeto) {
        cout << simbolo << " ";
    }

    cout << "\n";
}

// Acessores
const vector<Estado>& Automato::getEstados() const {
    return estados;
}

const vector<Transicao>& Automato::getTransicoes() const {
    return transicoes;
}

const set<char>& Automato::getAlfabeto() const {
    return alfabeto;
}

int Automato::getEstadoInicial() const {
    return estadoInicial;
}