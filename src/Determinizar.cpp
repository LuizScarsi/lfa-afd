#include "../include/Determinizar.h"

#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <vector>

using namespace std;

set<int> fechoEpsilon(const Automato& automato, const set<int>& conjunto) {
    set<int> resultado = conjunto;
    vector<int> pilha(conjunto.begin(), conjunto.end());

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

Automato determinizar(const Automato& afnd) {
    Automato afd;

    map<set<int>, int> subconjuntoParaId;
    queue<set<int>> fila;

    int contadorAFD = 0;

    auto criarEstadoAFD = [&](const set<int>& subconjunto) -> int {
        string nomeAFD = "D" + to_string(contadorAFD++);
        int novoId = afd.adicionarEstado(nomeAFD);
        subconjuntoParaId[subconjunto] = novoId;
        fila.push(subconjunto);

        // Verifica se algum estado do subconjunto e final e transfere os tokens
        bool ehFinal = false;
        vector<string> tokensColetados;

        for (int idOrig : subconjunto) {
            const Estado& estOrig = afnd.getEstados()[idOrig];
            if (estOrig.final) {
                ehFinal = true;
                for (const string& tok : estOrig.tokens) {
                    afd.marcarFinal(novoId, tok);
                    tokensColetados.push_back(tok);
                }
            }
        }

        if (ehFinal) {
            afd.marcarFinal(novoId);
        }

        // Imprime a correspondencia pedagógica
        cout << "  " << nomeAFD << " = { ";
        bool primeiro = true;
        for (int idOrig : subconjunto) {
            if (!primeiro) cout << ", ";
            cout << afnd.getEstados()[idOrig].nome;
            primeiro = false;
        }
        cout << " }";

        if (ehFinal) {
            cout << "  [FINAL";
            if (!tokensColetados.empty()) {
                cout << ": ";
                for (size_t i = 0; i < tokensColetados.size(); ++i) {
                    cout << (i > 0 ? ", " : "") << tokensColetados[i];
                }
            }
            cout << "]";
        }
        cout << "\n";

        return novoId;
    };

    cout << "\n--- Correspondencia de Estados (Subconjuntos -> AFD) ---\n";

    set<int> inicialFecho = fechoEpsilon(afnd, {afnd.getEstadoInicial()});
    int idInicialAFD = criarEstadoAFD(inicialFecho);
    afd.definirEstadoInicial(idInicialAFD);

    while (!fila.empty()) {
        set<int> atual = fila.front();
        fila.pop();

        int origemAFD = subconjuntoParaId[atual];

        for (char c : afnd.getAlfabeto()) {
            set<int> alcancaveis;

            for (int idOrig : atual) {
                vector<int> dests = afnd.getDestinos(idOrig, c);
                for (int d : dests) {
                    alcancaveis.insert(d);
                }
            }

            if (alcancaveis.empty()) {
                continue;
            }

            set<int> proximoSubconjunto = fechoEpsilon(afnd, alcancaveis);
            int destinoAFD;

            auto it = subconjuntoParaId.find(proximoSubconjunto);
            if (it != subconjuntoParaId.end()) {
                destinoAFD = it->second;
            } else {
                destinoAFD = criarEstadoAFD(proximoSubconjunto);
            }

            afd.adicionarTransicao(origemAFD, c, destinoAFD);
        }
    }

    return afd;
}
