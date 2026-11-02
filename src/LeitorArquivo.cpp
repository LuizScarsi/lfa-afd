#include "../include/LeitorArquivo.h"
#include "../include/LeitorGramatica.h"

#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>

using namespace std;

// Remove espacos e quebras de linha das extremidades
static string trimString(const string& texto) {
    size_t inicio = texto.find_first_not_of(" \t\r\n");
    if (inicio == string::npos) return "";
    size_t fim = texto.find_last_not_of(" \t\r\n");
    return texto.substr(inicio, fim - inicio + 1);
}

// Verifica se a string representa a palavra vazia (epsilon)
static bool isEpsilon(const string& str) {
    return (str == "ε" || 
            str == "\xce\xb5" || 
            str == "&" || 
            str == "eps" || 
            str == "epsilon" || 
            str == "E");
}

// Representa uma producao da gramatica regular
struct ProducaoGR {
    string naoTerminal;
    vector<string> alternativas;
};

// Gerador de nomes de estados amigaveis (S para inicial, depois A, B, C... M...)
class GeradorNomes {
private:
    char proximaLetra;
    int contador;
public:
    GeradorNomes() : proximaLetra('A'), contador(0) {}

    string proximo() {
        if (proximaLetra == 'S') {
            // 'S' e reservado para o inicial
            proximaLetra++;
        }
        if (proximaLetra <= 'Z') {
            string nome(1, proximaLetra++);
            return nome;
        }
        contador++;
        return "q" + to_string(contador);
    }
};

bool carregarArquivo(
    const string& caminho,
    Automato& automato,
    const string& tokenPadraoGramatica
) {
    ifstream arquivo(caminho);
    if (!arquivo.is_open()) {
        cerr << "Erro: Nao foi possivel abrir o arquivo: " << caminho << "\n";
        return false;
    }

    vector<string> tokens;
    vector<ProducaoGR> producoes;
    string tokenGramatica = tokenPadraoGramatica;

    string linha;
    while (getline(arquivo, linha)) {
        linha = trimString(linha);

        // Ignora linhas vazias e comentarios
        if (linha.empty() || linha[0] == '#' || (linha.size() >= 2 && linha[0] == '/' && linha[1] == '/')) {
            continue;
        }

        // Ignora marcadores de secao opcionais como [TOKENS] ou [GRAMATICA]
        if (linha[0] == '[' && linha.back() == ']') {
            string secao = linha.substr(1, linha.size() - 2);
            if (secao != "TOKENS" && secao != "GRAMATICA" && secao != "GR") {
                tokenGramatica = secao;
            }
            continue;
        }

        // Identifica se e uma regra de Gramatica Regular (BNF contem '::=')
        size_t separador = linha.find("::=");
        if (separador != string::npos) {
            ProducaoGR p;
            p.naoTerminal = trimString(linha.substr(0, separador));
            string direita = linha.substr(separador + 3);

            size_t inicio = 0;
            while (inicio <= direita.size()) {
                size_t fim = direita.find('|', inicio);
                string alt;
                if (fim == string::npos) {
                    alt = trimString(direita.substr(inicio));
                    if (!alt.empty()) p.alternativas.push_back(alt);
                    break;
                } else {
                    alt = trimString(direita.substr(inicio, fim - inicio));
                    if (!alt.empty()) p.alternativas.push_back(alt);
                    inicio = fim + 1;
                }
            }
            producoes.push_back(p);
        } else {
            // Caso contrario, e um token (cadeia/palavra reservada)
            tokens.push_back(linha);
        }
    }
    arquivo.close();

    GeradorNomes gerador;

    // 1. Cria o estado inicial compartilhado S
    int estadoInicial = automato.adicionarEstado("S");
    automato.definirEstadoInicial(estadoInicial);

    // 2. Carga dos tokens:
    // Conforme o professor: "No AF, apenas o estado inicial e compartilhado entre diferentes tokens/gramaticas.
    // Os demais estados sao exclusivos para as transicoes dos demais simbolos dos tokens..."
    for (const string& tok : tokens) {
        int atual = estadoInicial;
        for (size_t i = 0; i < tok.size(); ++i) {
            char c = tok[i];
            int novoEstado = automato.adicionarEstado(gerador.proximo());
            automato.adicionarTransicao(atual, c, novoEstado);
            atual = novoEstado;
        }
        automato.marcarFinal(atual, tok);
    }

    // 3. Carga da Gramatica Regular (GR):
    if (!producoes.empty()) {
        map<string, int> estadosGR;

        // O simbolo nao-terminal inicial da primeira producao compartilha o estado inicial S
        string naoTerminalInicial = producoes[0].naoTerminal;
        estadosGR[naoTerminalInicial] = estadoInicial;

        // Cria estados exclusivos para os demais nao-terminais
        for (const ProducaoGR& p : producoes) {
            if (estadosGR.find(p.naoTerminal) == estadosGR.end()) {
                estadosGR[p.naoTerminal] = automato.adicionarEstado(gerador.proximo());
            }
        }

        // Mapeia todas as transicoes e regras
        for (const ProducaoGR& p : producoes) {
            int origem = estadosGR[p.naoTerminal];

            for (const string& alt : p.alternativas) {
                // Epsilon: o proprio estado e final
                if (isEpsilon(alt)) {
                    automato.marcarFinal(origem, tokenGramatica);
                    continue;
                }

                // Regra do tipo: a<A> (simbolo terminal seguido de nao-terminal entre <>)
                size_t posChevrons = alt.find('<');
                if (posChevrons != string::npos && alt.back() == '>') {
                    char terminal = alt[0];
                    string destinoNome = alt.substr(posChevrons);

                    if (estadosGR.find(destinoNome) == estadosGR.end()) {
                        estadosGR[destinoNome] = automato.adicionarEstado(gerador.proximo());
                    }

                    int destino = estadosGR[destinoNome];
                    automato.adicionarTransicao(origem, terminal, destino);
                }
                // Regra do tipo: apenas terminal (ex: a)
                else if (alt.size() == 1) {
                    char terminal = alt[0];
                    int novoFinal = automato.adicionarEstado(gerador.proximo());
                    automato.adicionarTransicao(origem, terminal, novoFinal);
                    automato.marcarFinal(novoFinal, tokenGramatica);
                }
            }
        }
    }

    return true;
}

bool carregarTokens(const string& caminho, Automato& automato, int estadoInicial) {
    return carregarArquivo(caminho, automato);
}

bool carregarGramaticaRegular(const string& caminho, Automato& automato, int estadoInicial, const string& nomeToken) {
    return carregarArquivo(caminho, automato, nomeToken);
}
