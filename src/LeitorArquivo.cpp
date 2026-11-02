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

    string token;

    while (getline(arquivo, token)) {

        // Remove espaços no inicio e no final
        size_t inicio = token.find_first_not_of(" \t\r");

        if (inicio == string::npos) {
            continue;
        }

        size_t fim = token.find_last_not_of(" \t\r");

        token = token.substr(inicio, fim - inicio + 1);

        int estadoAtual = estadoInicial;

        // Cria uma transicao para cada caractere
        for (char simbolo : token) {

            int novoEstado = automato.adicionarEstado();

            automato.adicionarTransicao(
                estadoAtual,
                simbolo,
                novoEstado
            );

            estadoAtual = novoEstado;
        }

        // O ultimo estado representa o token completo
        automato.marcarFinal(estadoAtual, token);
    }

    arquivo.close();

    return true;
}