#include "../include/VerificarEstados.h"

#include <iostream>
#include <queue>
#include <vector>

using namespace std;

// Marca os estados alcancaveis a partir do estado inicial.
vector<bool> encontrarAlcancaveis(const Automato& automato) {
    int quantidade = static_cast<int>(automato.getEstados().size());

    vector<bool> alcancavel(quantidade, false);
    queue<int> fila;

    int inicial = automato.getEstadoInicial();

    if (inicial < 0 || inicial >= quantidade) {
        return alcancavel;
    }

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

// Marca os estados que conseguem chegar a algum estado final.
// Para isso, percorre as transicoes no sentido inverso.
vector<bool> encontrarEstadosQueChegamAoFinal(
    const Automato& automato
) {
    int quantidade = static_cast<int>(automato.getEstados().size());

    vector<bool> chegaFinal(quantidade, false);
    vector<vector<int>> reversas(quantidade);
    queue<int> fila;

    for (const Transicao& t : automato.getTransicoes()) {
        reversas[t.destino].push_back(t.origem);
    }

    for (int i = 0; i < quantidade; i++) {
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

    int quantidade = static_cast<int>(estados.size());
    int estadoErro = automato.getEstadoErro();

    vector<bool> alcancavel = encontrarAlcancaveis(automato);
    vector<bool> chegaFinal =
        encontrarEstadosQueChegamAoFinal(automato);

    cout << "\n========== VERIFICACAO DE ESTADOS ==========\n";

    cout << "\nEstados alcancaveis:\n";

    for (int i = 0; i < quantidade; i++) {
        if (alcancavel[i]) {
            cout << "q" << i << " ";
        }
    }

    cout << "\n\nEstados inalcançaveis:\n";

    bool encontrouInalcancavel = false;

    for (int i = 0; i < quantidade; i++) {
        if (!alcancavel[i]) {
            cout << "q" << i << " ";
            encontrouInalcancavel = true;
        }
    }

    if (!encontrouInalcancavel) {
        cout << "Nenhum";
    }

    cout << "\n\nEstados mortos comuns:\n";

    bool encontrouMorto = false;

    for (int i = 0; i < quantidade; i++) {
        if (!chegaFinal[i] && i != estadoErro) {
            cout << "q" << i << " ";
            encontrouMorto = true;
        }
    }

    if (!encontrouMorto) {
        cout << "Nenhum";
    }

    cout << "\n\nEstado de erro preservado:\n";

    if (estadoErro != -1) {
        cout << "q" << estadoErro;
    } else {
        cout << "Nenhum estado de erro registrado";
    }

    cout << "\n";
}

Automato removerEstadosInuteis(const Automato& automato) {
    const vector<Estado>& estados = automato.getEstados();
    const vector<Transicao>& transicoes = automato.getTransicoes();

    int quantidade = static_cast<int>(estados.size());
    int inicial = automato.getEstadoInicial();
    int erro = automato.getEstadoErro();

    // Sem estado de erro, nao temos para onde redirecionar
    // transicoes que chegariam a estados mortos.
    // Nesse caso, mantemos o automato original.
    if (erro == -1) {
        cout << "\nNao foi registrado um estado de erro. "
             << "Nenhum estado foi removido.\n";
        return automato;
    }

    vector<bool> alcancavel = encontrarAlcancaveis(automato);
    vector<bool> chegaFinal =
        encontrarEstadosQueChegamAoFinal(automato);

    vector<bool> manter(quantidade, false);

    // Mantem estados alcancaveis que chegam a um estado final.
    // Preserva tambem o estado de erro e o estado inicial.
    for (int i = 0; i < quantidade; i++) {
        if (alcancavel[i] &&
            (chegaFinal[i] || i == erro || i == inicial)) {
            manter[i] = true;
        }
    }

    // O estado de erro deve continuar presente.
    if (erro >= 0 && erro < quantidade) {
        manter[erro] = true;
    }

    // Cria um novo automato e relaciona os IDs antigos aos novos.
    Automato resultado;
    vector<int> novoId(quantidade, -1);

    for (int i = 0; i < quantidade; i++) {
        if (!manter[i]) {
            continue;
        }

        int id = resultado.adicionarEstado();
        novoId[i] = id;

        if (estados[i].final) {
            resultado.marcarFinal(id);

            for (const string& token : estados[i].tokens) {
                resultado.marcarFinal(id, token);
            }
        }
    }

    if (inicial >= 0 && inicial < quantidade && novoId[inicial] != -1) {
        resultado.definirEstadoInicial(novoId[inicial]);
    }

    resultado.definirEstadoErro(novoId[erro]);

    // Copia as transicoes. Se uma transicao apontava para um
    // estado morto removido, ela passa a apontar para o estado de erro.
    for (const Transicao& t : transicoes) {
        if (!manter[t.origem]) {
            continue;
        }

        int novaOrigem = novoId[t.origem];
        int novoDestino = novoId[t.destino];

        if (novoDestino == -1) {
            novoDestino = novoId[erro];
        }

        if (t.epsilon) {
            resultado.adicionarTransicaoEpsilon(
                novaOrigem,
                novoDestino
            );
        } else {
            resultado.adicionarTransicao(
                novaOrigem,
                t.simbolo,
                novoDestino
            );
        }
    }

    cout << "\n========== REMOCAO DE ESTADOS ==========\n";

    bool removeu = false;

    for (int i = 0; i < quantidade; i++) {
        if (!manter[i]) {
            cout << "Estado removido: q" << i << "\n";
            removeu = true;
        }
    }

    if (!removeu) {
        cout << "Nenhum estado precisou ser removido.\n";
    }

    return resultado;
}
