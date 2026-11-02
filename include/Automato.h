#ifndef AUTOMATO_H
#define AUTOMATO_H

#include <vector>
#include <set>
#include <string>

using namespace std;

// Identificam as posicoes do automato
struct Estado {
    int id;
    bool final;
    // guardamos os nomes dos tokens pois o projeto deve identificar qual token foi reconhecido
    vector<string> tokens;
};

// Indica pra qual estado vamos ao ler determinado simbolo
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

public:
    Automato();

    int adicionarEstado();

    void definirEstadoInicial(int estado);

    void marcarFinal(int estado, string token = "");

    void adicionarTransicao(int origem, char simbolo, int destino);

    void adicionarTransicaoEpsilon(int origem, int destino);

    void imprimir();

    // Acessores para os algoritmos que implementaremos depois
    const vector<Estado>& getEstados() const;
    const vector<Transicao>& getTransicoes() const;
    const set<char>& getAlfabeto() const;
    int getEstadoInicial() const;
};

#endif