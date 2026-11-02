#ifndef AUTOMATO_H
#define AUTOMATO_H

#include <vector>
#include <set>
#include <string>

using namespace std;

// Representa um estado do automato.
struct Estado {
    int id;
    bool final;
    vector<string> tokens;
};

// Representa uma transicao.
struct Transicao {
    int origem;
    char simbolo;
    int destino;
    bool epsilon;
};

class Automato {
private:
    vector<Estado> estados;
    vector<Transicao> transicoes;
    set<char> alfabeto;

    int estadoInicial;
    int estadoErro;

public:
    Automato();

    int adicionarEstado();

    void definirEstadoInicial(int estado);
    void definirEstadoErro(int estado);

    void marcarFinal(int estado, string token = "");

    void adicionarTransicao(int origem, char simbolo, int destino);
    void adicionarTransicaoEpsilon(int origem, int destino);

    void imprimir() const;

    const vector<Estado>& getEstados() const;
    const vector<Transicao>& getTransicoes() const;
    const set<char>& getAlfabeto() const;

    int getEstadoInicial() const;
    int getEstadoErro() const;
};

#endif
