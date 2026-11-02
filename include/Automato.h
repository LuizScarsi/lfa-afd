#ifndef AUTOMATO_H
#define AUTOMATO_H

#include <vector>
#include <set>
#include <string>

using namespace std;

// Representa um estado do automato.
struct Estado {
    int id;
    string nome;
    bool final;
    vector<string> tokens;
    bool isErro;

    Estado(int _id = -1, string _nome = "", bool _final = false, bool _isErro = false)
        : id(_id), nome(_nome), final(_final), isErro(_isErro) {}
};

// Representa uma transicao.
struct Transicao {
    int origem;
    char simbolo;
    int destino;
    bool epsilon;

    Transicao(int _origem = -1, char _simbolo = '\0', int _destino = -1, bool _epsilon = false)
        : origem(_origem), simbolo(_simbolo), destino(_destino), epsilon(_epsilon) {}
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

    int adicionarEstado(const string& nome = "");

    void definirEstadoInicial(int estado);
    void definirEstadoErro(int estado);

    void marcarFinal(int estado, const string& token = "");

    void adicionarTransicao(int origem, char simbolo, int destino);
    void adicionarTransicaoEpsilon(int origem, int destino);

    // Impressao formatada em tabela e em lista
    void imprimirTabela(const string& titulo = "") const;
    void imprimirLista(const string& titulo = "") const;
    void imprimir() const; // compatibilidade

    const vector<Estado>& getEstados() const;
    const vector<Transicao>& getTransicoes() const;
    const set<char>& getAlfabeto() const;

    int getEstadoInicial() const;
    int getEstadoErro() const;

    // Metodos auxiliares de consulta
    vector<int> getDestinos(int origem, char simbolo) const;
    int getDestinoAFD(int origem, char simbolo) const;
    bool temTransicao(int origem, char simbolo) const;
};

#endif
