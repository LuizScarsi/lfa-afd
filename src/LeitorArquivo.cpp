#include "../include/LeitorArquivo.h"

#include <fstream>
#include <iostream>

using namespace std;

bool carregarTokens(
    const string& caminho,
    Automato& automato,
    int estadoInicial
) {
    ifstream arquivo(caminho);

    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << caminho << "\n";
        return false;
    }

    string linha;
    bool lerTokens = false;
    bool possuiSecoes = false;

    while (getline(arquivo, linha)) {

        // Identifica as secoes do arquivo
        if (linha == "[TOKENS]") {
            lerTokens = true;
            possuiSecoes = true;
            continue;
        }

        if (linha == "[GRAMATICA]") {
            lerTokens = false;
            possuiSecoes = true;
            continue;
        }

        // Sem secoes, considera todas as linhas como tokens
        if (possuiSecoes && !lerTokens) {
            continue;
        }

        string token = linha;

        size_t inicio = token.find_first_not_of(" \t\r");

        if (inicio == string::npos) {
            continue;
        }

        size_t fim = token.find_last_not_of(" \t\r");

        token = token.substr(inicio, fim - inicio + 1);

        int estadoAtual = estadoInicial;

        for (char simbolo : token) {

            int novoEstado = automato.adicionarEstado();

            automato.adicionarTransicao(
                estadoAtual,
                simbolo,
                novoEstado
            );

            estadoAtual = novoEstado;
        }

        automato.marcarFinal(estadoAtual, token);
    }

    return true;
}