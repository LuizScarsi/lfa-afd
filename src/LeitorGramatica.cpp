#include "../include/LeitorGramatica.h"

#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>

using namespace std;

// Remove espacos do inicio e do final
string trim(const string& texto) {

    size_t inicio = texto.find_first_not_of(" \t\r");

    if (inicio == string::npos) {
        return "";
    }

    size_t fim = texto.find_last_not_of(" \t\r");

    return texto.substr(inicio, fim - inicio + 1);
}

// Representa uma producao da gramatica
struct Producao {
    string esquerda;
    vector<string> alternativas;
};

bool carregarGramaticaRegular(
    const string& caminho,
    Automato& automato,
    int estadoInicial
) {
    ifstream arquivo(caminho);

    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << caminho << "\n";
        return false;
    }

    vector<Producao> producoes;

    string linha;
    bool lerGramatica = false;
    bool encontrouGramatica = false;

    // 1. Le as producoes da gramatica
    while (getline(arquivo, linha)) {

        linha = trim(linha);

        if (linha == "[GRAMATICA]") {
            lerGramatica = true;
            encontrouGramatica = true;
            continue;
        }

        if (!linha.empty() && linha[0] == '[') {
            lerGramatica = false;
            continue;
        }

        if (!lerGramatica || linha.empty()) {
            continue;
        }

        size_t separador = linha.find("::=");

        if (separador == string::npos) {
            cerr << "Producao invalida: " << linha << "\n";
            return false;
        }

        Producao producao;

        producao.esquerda = trim(
            linha.substr(0, separador)
        );

        string direita = linha.substr(separador + 3);

        // Divide as alternativas pelo simbolo |
        size_t inicio = 0;

        while (inicio <= direita.size()) {

            size_t fim = direita.find('|', inicio);

            string alternativa;

            if (fim == string::npos) {
                alternativa = direita.substr(inicio);
            } else {
                alternativa = direita.substr(
                    inicio,
                    fim - inicio
                );
            }

            alternativa = trim(alternativa);

            if (!alternativa.empty()) {
                producao.alternativas.push_back(alternativa);
            }

            if (fim == string::npos) {
                break;
            }

            inicio = fim + 1;
        }

        producoes.push_back(producao);
    }

    arquivo.close();

    if (!encontrouGramatica) {
        return true;
    }

    if (producoes.empty()) {
        cerr << "A secao da gramatica esta vazia.\n";
        return false;
    }

    // 2. Identifica todos os nao terminais
    set<string> naoTerminais;

    for (const Producao& p : producoes) {

        naoTerminais.insert(p.esquerda);
    }

    // 3. O primeiro nao terminal sera o inicial
    string inicialGramatica = producoes[0].esquerda;

    map<string, int> estadosGramatica;

    estadosGramatica[inicialGramatica] = estadoInicial;

    // Cria os estados dos demais nao terminais
    for (const string& naoTerminal : naoTerminais) {

        if (naoTerminal == inicialGramatica) {
            continue;
        }

        int novoEstado = automato.adicionarEstado();

        estadosGramatica[naoTerminal] = novoEstado;
    }

    // 4. Converte as producoes em transicoes
    for (const Producao& p : producoes) {

        int origem = estadosGramatica[p.esquerda];

        for (const string& alternativa : p.alternativas) {

            // Producao epsilon: o proprio estado e final
            if (alternativa == "ε") {

                automato.marcarFinal(origem, "GRAMATICA");
                continue;
            }

            // Exige pelo menos um simbolo terminal
            if (alternativa.empty()) {
                continue;
            }

            char simbolo = alternativa[0];

            // Producao do tipo a<A>
            if (alternativa.size() >= 4 &&
                alternativa[1] == '<' &&
                alternativa.back() == '>') {

                string destinoNome = alternativa.substr(
                    1,
                    alternativa.size() - 1
                );

                if (estadosGramatica.find(destinoNome) ==
                    estadosGramatica.end()) {

                    cerr << "Nao terminal nao definido: "
                         << destinoNome << "\n";

                    return false;
                }

                int destino = estadosGramatica[destinoNome];

                automato.adicionarTransicao(
                    origem,
                    simbolo,
                    destino
                );
            }

            // Producao do tipo a
            else if (alternativa.size() == 1) {

                int destino = automato.adicionarEstado();

                automato.adicionarTransicao(
                    origem,
                    simbolo,
                    destino
                );

                automato.marcarFinal(destino, "GRAMATICA");
            }

            else {

                cerr << "Alternativa invalida: "
                     << alternativa << "\n";

                return false;
            }
        }
    }

    return true;
}