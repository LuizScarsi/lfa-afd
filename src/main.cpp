#include "../include/Automato.h"
#include "../include/LeitorArquivo.h"
#include "../include/Determinizar.h"
#include "../include/Minimizar.h"
#include "../include/CompletarAutomato.h"
#include "../include/VerificarEstados.h"
#include "../include/Reconhecer.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

void imprimirCabecalho() {
    cout << "====================================================================\n";
    cout << "         UNIVERSIDADE FEDERAL DA FRONTEIRA SUL (UFFS)               \n";
    cout << "             CCR: Linguagens Formais e Automatos                    \n";
    cout << "               Professor: Braulio Mello                             \n";
    cout << "====================================================================\n";
    cout << " PROJETO: Gerador de AFD Minimo (Sem Classes de Equivalencia),      \n";
    cout << "          Livre de Estados Inalcancaveis e Mortos, com Estado Erro  \n";
    cout << "====================================================================\n\n";
}

int main(int argc, char* argv[]) {
    imprimirCabecalho();

    string caminhoArquivo = "exemplos/professor.txt";
    if (argc > 1) {
        caminhoArquivo = argv[1];
    }

    cout << "Arquivo de entrada selecionado: " << caminhoArquivo << "\n\n";

    // =====================================================================
    // ETAPA 1: Carga do Arquivo Fonte e Geracao do AFND
    // =====================================================================
    cout << "--------------------------------------------------------------------\n";
    cout << "[ETAPA 1] Carga de Tokens e Gramatica Regular -> Construcao do AFND\n";
    cout << "--------------------------------------------------------------------\n";
    cout << "- Compartilhamento exclusivo do estado inicial S entre tokens e GR.\n";
    cout << "- Transicoes exclusivas para cada novo simbolo/token na carga.\n";

    Automato afnd;
    if (!carregarArquivo(caminhoArquivo, afnd, "variavel")) {
        // Tenta caminho alternativo de exemplo se nao encontrar
        caminhoArquivo = "exemplos/entrada.txt";
        cout << "Tentando carregar arquivo alternativo: " << caminhoArquivo << "...\n";
        if (!carregarArquivo(caminhoArquivo, afnd, "variavel")) {
            cerr << "Falha ao carregar arquivo de entrada. Encerrando aplicacao.\n";
            return 1;
        }
    }

    afnd.imprimirTabela("AFND Inicial (Apos Carga)");

    // =====================================================================
    // ETAPA 2: Determinizacao (AFND -> AFD)
    // =====================================================================
    cout << "\n--------------------------------------------------------------------\n";
    cout << "[ETAPA 2] Teorema de Determinizacao (Construcao de Subconjuntos)\n";
    cout << "--------------------------------------------------------------------\n";
    cout << "- Mapeamento de subconjuntos de estados do AFND em estados do AFD.\n";
    cout << "- Resolucao de indeterminismos e heranca dos rotulos de tokens.\n";

    Automato afd = determinizar(afnd);
    afd.imprimirTabela("AFD Determinado (Antes da Minimizacao)");

    // =====================================================================
    // ETAPA 3: Minimizacao SEM Classes de Equivalencia
    // =====================================================================
    cout << "\n--------------------------------------------------------------------\n";
    cout << "[ETAPA 3] Minimizacao Sem Classes de Equivalencia\n";
    cout << "--------------------------------------------------------------------\n";
    cout << "- Conforme o enunciado: Minimo sem aplicacao de classes de equivalencia.\n";
    cout << "- Elimina exclusivamente estados inalcancaveis e estados mortos.\n";

    Automato afdMinimizado = minimizarAFD(afd);
    afdMinimizado.imprimirTabela("AFD Minimizado (Livre de Inalcancaveis e Mortos)");

    // =====================================================================
    // ETAPA 4: Estado de Erro e Completude do AFD
    // =====================================================================
    cout << "\n--------------------------------------------------------------------\n";
    cout << "[ETAPA 4] Adicao do Estado de Erro ao Final da Minimizacao\n";
    cout << "--------------------------------------------------------------------\n";
    cout << "- Acrescenta o estado de erro como ultimo estado.\n";
    cout << "- Celulas nao mapeadas do AFD sao preenchidas com o estado de erro.\n";
    cout << "- Transicoes a partir do estado de erro permanecem nele mesmo.\n";

    Automato afdFinal = afdMinimizado;
    completarAFD(afdFinal);

    afdFinal.imprimirTabela("AFD Final Completo (Com Estado de Erro)");

    // Relatorio de confirmacao estrutural
    verificarEstados(afdFinal);

    // =====================================================================
    // ETAPA 5: Reconhecimento Interativo de Sentencas / Palavras
    // =====================================================================
    cout << "\n====================================================================\n";
    cout << "[ETAPA 5] Simulador de Reconhecimento de Sentencas\n";
    cout << "====================================================================\n";
    cout << "Digite palavras para testar no AFD final.\n";
    cout << "Exemplos sugeridos: 'se', 'entao', 'senao', 'a', 'aeiou', 'sen', 'xyz'\n";
    cout << "Digite 'sair' para encerrar o programa.\n";

    string palavra;
    while (true) {
        cout << "\nEntrada > ";
        if (!getline(cin, palavra)) {
            break;
        }

        // Remove quebra de linha estilo Windows (\r) se presente
        while (!palavra.empty() && (palavra.back() == '\r' || palavra.back() == ' ' || palavra.back() == '\t')) {
            palavra.pop_back();
        }
        while (!palavra.empty() && (palavra.front() == ' ' || palavra.front() == '\t')) {
            palavra.erase(palavra.begin());
        }

        if (palavra == "sair" || palavra == "exit" || palavra == ":q") {
            cout << "Encerrando simulador.\n";
            break;
        }

        if (palavra.empty()) {
            cout << "(Palavra vazia)\n";
        }

        vector<string> tokensReconhecidos;
        string rastro;
        bool aceita = reconhecerComRastro(afdFinal, palavra, tokensReconhecidos, rastro);

        cout << "Caminho percorrido: " << rastro << "\n";
        if (aceita) {
            cout << "STATUS: [ACEITA] -> ";
            for (size_t i = 0; i < tokensReconhecidos.size(); ++i) {
                cout << (i > 0 ? ", " : "") << tokensReconhecidos[i];
            }
            cout << "\n";
        } else {
            cout << "STATUS: [REJEITADA]\n";
        }
    }

    return 0;
}
