#include "../include/Automato.h"

#include <algorithm>
#include <iostream>

using namespace std;

Automato::Automato() {
    estadoInicial = -1;
    estadoErro = -1;
}

int Automato::adicionarEstado() {
    int id = static_cast<int>(estados.size());

    Estado novoEstado;
    novoEstado.id = id;
    novoEstado.final = false;

    estados.push_back(novoEstado);

    return id;
}

void Automato::definirEstadoInicial(int estado) {
    if (estado >= 0 &&
        estado < static_cast<int>(estados.size())) {
        estadoInicial = estado;
    }
}

void Automato::definirEstadoErro(int estado) {
    if (estado >= 0 &&
        estado < static_cast<int>(estados.size())) {
        estadoErro = estado;
    } else {
        estadoErro = -1;
    }
}

void Automato::marcarFinal(int estado, string token) {
    if (estado < 0 ||
        estado >= static_cast<int>(estados.size())) {
        return;
    }

    estados[estado].final = true;

    if (!token.empty()) {
        const vector<string>& tokens = estados[estado].tokens;

        if (find(tokens.begin(), tokens.end(), token) == tokens.end()) {
            estados[estado].tokens.push_back(token);
        }
    }
}

void Automato::adicionarTransicao(
    int origem,
    char simbolo,
    int destino
) {
    if (origem < 0 ||
        destino < 0 ||
        origem >= static_cast<int>(estados.size()) ||
        destino >= static_cast<int>(estados.size())) {
        return;
    }

    Transicao nova;
    nova.origem = origem;
    nova.simbolo = simbolo;
    nova.destino = destino;
    nova.epsilon = false;

    transicoes.push_back(nova);
    alfabeto.insert(simbolo);
}

void Automato::adicionarTransicaoEpsilon(int origem, int destino) {
    if (origem < 0 ||
        destino < 0 ||
        origem >= static_cast<int>(estados.size()) ||
        destino >= static_cast<int>(estados.size())) {
        return;
    }

    Transicao nova;
    nova.origem = origem;
    nova.simbolo = '\0';
    nova.destino = destino;
    nova.epsilon = true;

    transicoes.push_back(nova);
}

void Automato::imprimir() const {
    cout << "\nESTADOS:\n";

    for (const Estado& estado : estados) {
        cout << "q" << estado.id;

        if (estado.id == estadoInicial) {
            cout << " (Inicial)";
        }

        if (estado.final) {
            cout << " (Final)";

            if (!estado.tokens.empty()) {
                cout << " [";

                for (size_t i = 0; i < estado.tokens.size(); i++) {
                    cout << estado.tokens[i];

                    if (i + 1 < estado.tokens.size()) {
                        cout << ", ";
                    }
                }

                cout << "]";
            }
        }

        if (estado.id == estadoErro) {
            cout << " (Erro)";
        }

        cout << "\n";
    }

    cout << "\nTRANSICOES:\n";

    for (const Transicao& transicao : transicoes) {
        cout << "q" << transicao.origem << " --";

        if (transicao.epsilon) {
            cout << "ε";
        } else {
            cout << transicao.simbolo;
        }

        cout << "--> q" << transicao.destino << "\n";
    }

    cout << "\nALFABETO: ";

    for (char simbolo : alfabeto) {
        cout << simbolo << " ";
    }

    cout << "\n";
}

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

int Automato::getEstadoErro() const {
    return estadoErro;
}
