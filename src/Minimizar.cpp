#include "../include/Minimizar.h"

#include <algorithm>
#include <set>
#include <vector>

using namespace std;

// Compara os tokens reconhecidos por dois estados
bool mesmosTokens(const Estado& a, const Estado& b) {

    set<string> tokensA(a.tokens.begin(), a.tokens.end());
    set<string> tokensB(b.tokens.begin(), b.tokens.end());

    return tokensA == tokensB;
}

// Encontra o destino de uma transicao
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

// Verifica se um par de estados ja foi marcado
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

// Realiza a minimizacao pelo metodo da tabela
Automato minimizarAFD(const Automato& afd) {

    const vector<Estado>& estados = afd.getEstados();

    int n = static_cast<int>(estados.size());

    vector<vector<bool>> tabela(
        n,
        vector<bool>(n, false)
    );

    // 1. Marca pares inicialmente distinguiveis
    for (int i = 0; i < n; i++) {

        for (int j = i + 1; j < n; j++) {

            if (estados[i].final != estados[j].final ||
                !mesmosTokens(estados[i], estados[j])) {

                tabela[i][j] = true;
            }
        }
    }

    // 2. Analisa os destinos das transicoes
    bool houveMudanca = true;

    while (houveMudanca) {

        houveMudanca = false;

        for (int i = 0; i < n; i++) {

            for (int j = i + 1; j < n; j++) {

                if (tabela[i][j]) {
                    continue;
                }

                for (char simbolo : afd.getAlfabeto()) {

                    int destinoA = encontrarDestino(
                        afd, i, simbolo
                    );

                    int destinoB = encontrarDestino(
                        afd, j, simbolo
                    );

                    if (destinoA != destinoB &&
                        estaMarcado(tabela, destinoA, destinoB)) {

                        tabela[i][j] = true;
                        houveMudanca = true;

                        break;
                    }
                }
            }
        }
    }

    // 3. Cria o AFD minimizado
    Automato minimizado;

    vector<int> novoEstado(n, -1);
    vector<int> representantes;

    for (int i = 0; i < n; i++) {

        // Procura um estado anterior indistinguivel
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

            // Preserva a condicao de estado final e seus tokens
            if (estados[i].final) {

                minimizado.marcarFinal(id);

                for (const string& token : estados[i].tokens) {
                    minimizado.marcarFinal(id, token);
                }
            }
        }
    }

    // Define o estado inicial
    minimizado.definirEstadoInicial(
        novoEstado[afd.getEstadoInicial()]
    );

    // 4. Reconstrói as transições
    for (int representante : representantes) {

        int origemNova = novoEstado[representante];

        for (const Transicao& t : afd.getTransicoes()) {

            if (t.origem == representante && !t.epsilon) {

                int destinoNovo = novoEstado[t.destino];

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