#include "../include/Reconhecer.h"

#include <sstream>

using namespace std;

bool reconhecerComRastro(
    const Automato& afd,
    const string& palavra,
    vector<string>& identificacoes,
    string& rastro
) {
    identificacoes.clear();
    int estadoAtual = afd.getEstadoInicial();
    int estadoErro = afd.getEstadoErro();
    const vector<Estado>& estados = afd.getEstados();

    if (estadoAtual < 0 || estadoAtual >= static_cast<int>(estados.size())) {
        rastro = "Erro: Automato sem estado inicial valido.";
        return false;
    }

    stringstream ss;
    ss << estados[estadoAtual].nome;

    for (char simbolo : palavra) {
        int proximo = afd.getDestinoAFD(estadoAtual, simbolo);

        if (proximo == -1) {
            ss << " --" << simbolo << "--> [Sem transicao]";
            rastro = ss.str();
            return false;
        }

        estadoAtual = proximo;
        ss << " --" << simbolo << "--> " << estados[estadoAtual].nome;

        if (estadoAtual == estadoErro) {
            ss << " (Estado de Erro)";
            rastro = ss.str();
            return false;
        }
    }

    const Estado& estFinal = estados[estadoAtual];
    if (estFinal.final && estadoAtual != estadoErro) {
        ss << " [ACEITA";
        if (!estFinal.tokens.empty()) {
            ss << " - Token: ";
            for (size_t i = 0; i < estFinal.tokens.size(); ++i) {
                ss << (i > 0 ? ", " : "") << estFinal.tokens[i];
            }
        }
        ss << "]";
        identificacoes = estFinal.tokens;
        rastro = ss.str();
        return true;
    } else {
        ss << " [REJEITADA: Estado nao-final]";
        rastro = ss.str();
        return false;
    }
}

bool reconhecer(
    const Automato& afd,
    const string& palavra,
    vector<string>& identificacoes
) {
    string rastro;
    return reconhecerComRastro(afd, palavra, identificacoes, rastro);
}
