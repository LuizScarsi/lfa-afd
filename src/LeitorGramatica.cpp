#include "../include/LeitorGramatica.h"
#include "../include/LeitorArquivo.h"

// Mantem compatibilidade com codigo que utilize LeitorGramatica isoladamente
bool carregarGramaticaRegular(
    const string& caminho,
    Automato& automato,
    int estadoInicial
) {
    (void)estadoInicial;
    return carregarArquivo(caminho, automato);
}
