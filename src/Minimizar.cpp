#include "../include/Minimizar.h"

#include <algorithm>
#include <set>
#include <vector>

using namespace std;

bool mesmosTokens(const Estado& a, const Estado& b) {
    set<string> tokensA(a.tokens.begin(), a.tokens.end());
    set<string> tokensB(b.tokens.begin(), b.tokens.end());

    return tokensA == tokensB;
}

int encontrarDestino(
    const Automato& afd,
    int estado,
    char simbolo
) {
    for (const Transicao& t : afd.getTransicoes()) {
        if (t.origem == estado &&
            !t.epsilon &&
            t.simbolo == simbolo) {
            return t.destino;
        }
    }

    return -1;
}

bool estaMarcado(
    const vector<vector<bool>>& tabela,
    int a,
    int b
) {
    if (a == b) {
        return false;
    }

    if (a > b) {
        swap(a, b);
    }

    return tabela[a][b];
}

Automato minimizarAFD(const Automato& afd) {
    const vector<Estado>& estados = afd.getEstados();

    int n = static_cast<int>(estados.size());

    vector<vector<bool>> tabela(n, vector<bool>(n, false));

    // Estados finais e nao finais sao distinguiveis.
    // Tambem preservamos as identificacoes dos tokens.
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (estados[i].final != estados[j].final ||
                !mesmosTokens(estados[i], estados[j])) {
                tabela[i][j] = true;
            }
        }
    }

    bool houveMudanca = true;

    while (houveMudanca) {
        houveMudanca = false;

        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                if (tabela[i][j]) {
                    continue;
                }

                for (char simbolo : afd.getAlfabeto()) {
                    int destinoA = encontrarDestino(afd, i, simbolo);
                    int destinoB = encontrarDestino(afd, j, simbolo);

                    if (destinoA != destinoB &&
                        (destinoA == -1 ||
                         destinoB == -1 ||
                         estaMarcado(tabela, destinoA, destinoB))) {

                        tabela[i][j] = true;
                        houveMudanca = true;
                        break;
                    }
                }
            }
        }
    }

    Automato minimizado;

    vector<int> novoEstado(n, -1);
    vector<int> representantes;

    // Cria um estado para cada grupo de estados equivalentes.
    for (int i = 0; i < n; i++) {
        int equivalente = -1;

        for (int j = 0; j < i; j++) {
            if (!estaMarcado(tabela, j, i)) {
                equivalente = j;
                break;
            }
        }

        if (equivalente != -1) {
            novoEstado[i] = novoEstado[equivalente];
        } else {
            int id = minimizado.adicionarEstado();

            novoEstado[i] = id;
            representantes.push_back(i);

            if (estados[i].final) {
                minimizado.marcarFinal(id);

                for (const string& token : estados[i].tokens) {
                    minimizado.marcarFinal(id, token);
                }
            }
        }
    }

    minimizado.definirEstadoInicial(
        novoEstado[afd.getEstadoInicial()]
    );

    // Preserva a identificacao do estado de erro apos a minimizacao.
    int erroAntigo = afd.getEstadoErro();

    if (erroAntigo != -1) {
        minimizado.definirEstadoErro(novoEstado[erroAntigo]);
    }

    // Copia as transicoes usando os novos IDs.
    for (int representante : representantes) {
        int origemNova = novoEstado[representante];

        for (const Transicao& t : afd.getTransicoes()) {
            if (t.origem != representante) {
                continue;
            }

            int destinoNovo = novoEstado[t.destino];

            if (t.epsilon) {
                minimizado.adicionarTransicaoEpsilon(
                    origemNova,
                    destinoNovo
                );
            } else {
                minimizado.adicionarTransicao(
                    origemNova,
                    t.simbolo,
                    destinoNovo
                );
            }
        }
    }

    return minimizado;
}
