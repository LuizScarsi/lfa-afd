#include "../include/Automato.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

Automato::Automato() {
    estadoInicial = -1;
    estadoErro = -1;
}

int Automato::adicionarEstado(const string& nome) {
    int id = static_cast<int>(estados.size());

    Estado novoEstado;
    novoEstado.id = id;
    novoEstado.nome = nome.empty() ? ("q" + to_string(id)) : nome;
    novoEstado.final = false;
    novoEstado.isErro = false;

    estados.push_back(novoEstado);
    return id;
}

void Automato::definirEstadoInicial(int estado) {
    if (estado >= 0 && estado < static_cast<int>(estados.size())) {
        estadoInicial = estado;
    }
}

void Automato::definirEstadoErro(int estado) {
    if (estado >= 0 && estado < static_cast<int>(estados.size())) {
        estadoErro = estado;
        estados[estado].isErro = true;
    } else {
        estadoErro = -1;
    }
}

void Automato::marcarFinal(int estado, const string& token) {
    if (estado < 0 || estado >= static_cast<int>(estados.size())) {
        return;
    }

    estados[estado].final = true;

    if (!token.empty()) {
        vector<string>& tokens = estados[estado].tokens;
        if (find(tokens.begin(), tokens.end(), token) == tokens.end()) {
            tokens.push_back(token);
        }
    }
}

void Automato::adicionarTransicao(int origem, char simbolo, int destino) {
    if (origem < 0 || destino < 0 ||
        origem >= static_cast<int>(estados.size()) ||
        destino >= static_cast<int>(estados.size())) {
        return;
    }

    Transicao nova(origem, simbolo, destino, false);
    transicoes.push_back(nova);
    alfabeto.insert(simbolo);
}

void Automato::adicionarTransicaoEpsilon(int origem, int destino) {
    if (origem < 0 || destino < 0 ||
        origem >= static_cast<int>(estados.size()) ||
        destino >= static_cast<int>(estados.size())) {
        return;
    }

    Transicao nova(origem, '\0', destino, true);
    transicoes.push_back(nova);
}

vector<int> Automato::getDestinos(int origem, char simbolo) const {
    vector<int> dests;
    for (const Transicao& t : transicoes) {
        if (t.origem == origem && !t.epsilon && t.simbolo == simbolo) {
            dests.push_back(t.destino);
        }
    }
    return dests;
}

int Automato::getDestinoAFD(int origem, char simbolo) const {
    for (const Transicao& t : transicoes) {
        if (t.origem == origem && !t.epsilon && t.simbolo == simbolo) {
            return t.destino;
        }
    }
    return -1;
}

bool Automato::temTransicao(int origem, char simbolo) const {
    return getDestinoAFD(origem, simbolo) != -1;
}

void Automato::imprimirTabela(const string& titulo) const {
    if (!titulo.empty()) {
        cout << "\n=== " << titulo << " (Tabela de Transicao) ===\n";
    }

    if (estados.empty()) {
        cout << "(Automato vazio)\n";
        return;
    }

    int colEstadoLargura = 12;
    int colSimboloLargura = 9;
    int colTokenLargura = 24;

    // Ajusta largura se houver muitos tokens
    for (const Estado& e : estados) {
        string tks = "";
        for (size_t i = 0; i < e.tokens.size(); ++i) {
            tks += (i > 0 ? ", " : "") + e.tokens[i];
        }
        if (static_cast<int>(tks.size()) + 2 > colTokenLargura) {
            colTokenLargura = static_cast<int>(tks.size()) + 2;
        }
    }

    auto linhaDivisoria = [&]() {
        cout << "+-" << string(colEstadoLargura, '-') << "-+";
        for (size_t i = 0; i < alfabeto.size(); ++i) {
            cout << string(colSimboloLargura, '-') << "-+";
        }
        cout << string(colTokenLargura, '-') << "-+\n";
    };

    linhaDivisoria();
    cout << "| " << setw(colEstadoLargura) << left << "Estado (delta)";
    for (char c : alfabeto) {
        string simStr(1, c);
        cout << " | " << setw(colSimboloLargura - 2) << left << simStr;
    }
    cout << " | " << setw(colTokenLargura - 2) << left << "Tokens" << " |\n";
    linhaDivisoria();

    for (const Estado& e : estados) {
        string rotulo = "";
        if (e.id == estadoInicial) rotulo += "->";
        if (e.final) rotulo += "*";
        if (e.isErro || e.id == estadoErro) rotulo += "E";

        string nomeDisplay = rotulo + (rotulo.empty() ? "" : " ") + e.nome;
        cout << "| " << setw(colEstadoLargura) << left << nomeDisplay;

        for (char c : alfabeto) {
            vector<int> dests = getDestinos(e.id, c);
            string celula = "";
            if (dests.empty()) {
                celula = "-";
            } else {
                for (size_t i = 0; i < dests.size(); ++i) {
                    if (i > 0) celula += ",";
                    celula += estados[dests[i]].nome;
                }
            }
            cout << " | " << setw(colSimboloLargura - 2) << left << celula;
        }

        string tokensStr = "";
        for (size_t i = 0; i < e.tokens.size(); ++i) {
            if (i > 0) tokensStr += ", ";
            tokensStr += e.tokens[i];
        }
        if (tokensStr.empty() && (e.isErro || e.id == estadoErro)) {
            tokensStr = "[ERRO]";
        }
        cout << " | " << setw(colTokenLargura - 2) << left << tokensStr << " |\n";
    }

    linhaDivisoria();
    cout << "Legenda: '->' Estado Inicial | '*' Estado Final | 'E' Estado de Erro | '-' Sem transicao\n";
}

void Automato::imprimirLista(const string& titulo) const {
    if (!titulo.empty()) {
        cout << "\n=== " << titulo << " (Lista de Estados e Transicoes) ===\n";
    }

    cout << "\nESTADOS (" << estados.size() << "):\n";
    for (const Estado& e : estados) {
        cout << "  " << e.nome << " (id: " << e.id << ")";
        if (e.id == estadoInicial) cout << " [INICIAL]";
        if (e.final) cout << " [FINAL]";
        if (e.isErro || e.id == estadoErro) cout << " [ERRO]";
        if (!e.tokens.empty()) {
            cout << " -> tokens: [";
            for (size_t i = 0; i < e.tokens.size(); ++i) {
                cout << (i > 0 ? ", " : "") << e.tokens[i];
            }
            cout << "]";
        }
        cout << "\n";
    }

    cout << "\nTRANSICOES (" << transicoes.size() << "):\n";
    for (const Transicao& t : transicoes) {
        cout << "  " << estados[t.origem].nome << " --";
        if (t.epsilon) cout << "ε";
        else cout << t.simbolo;
        cout << "--> " << estados[t.destino].nome << "\n";
    }

    cout << "\nALFABETO: { ";
    for (char c : alfabeto) {
        cout << "'" << c << "' ";
    }
    cout << "}\n";
}

void Automato::imprimir() const {
    imprimirTabela();
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
