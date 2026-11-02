# Gerador de Autômato Finito Determinístico (AFD) Mínimo

**Universidade Federal da Fronteira Sul (UFFS)**  
**Campus Chapecó**  
**CCR:** Linguagens Formais e Autômatos  
**Docente:** Prof. Braulio Mello  

---

## 🎯 Objetivo do Projeto

Construir uma aplicação robusta em C++ capaz de carregar definições léxicas e sintáticas (palavras reservadas, operadores, símbolos especiais e Gramáticas Regulares em notação BNF) a partir de um arquivo texto e convertê-las em um **Autômato Finito Determinístico (AFD) mínimo sem a aplicação de classes de equivalência entre estados**, com **estado de erro** e **livre de estados inalcançáveis e mortos**.

---

## 📐 Conformidade Rigorosa com os Requisitos

O projeto segue fielmente o enunciado oficial da disciplina:

| Etapa | Requisito do Enunciado | Implementação no Projeto |
|---|---|---|
| **1. Carga & AFND** | Compartilhamento **apenas do estado inicial** entre diferentes tokens/gramáticas. Transições exclusivas para cada símbolo subsequente (sem reúso de estados intermediários). | [`src/LeitorArquivo.cpp`](src/LeitorArquivo.cpp): O estado inicial `S` é compartilhado. Para cada token, criam-se novos estados exclusivos ($A, B, C\dots$). Para a GR, o não-terminal inicial mapeia para `S` e os demais recebem estados próprios ($M$). |
| **2. Indeterminismo** | Surgimento de indeterminismo quando dois tokens ou sentenças de GR iniciam pelo mesmo símbolo. | Demonstrado na carga: do estado inicial $S$, com entrada `'s'` vai para $A$ e $H$; com entrada `'e'` vai para $C$ e $M$. |
| **3. Determinização** | Aplicação do **Teorema de Determinização** (construção de subconjuntos com fecho-$\varepsilon$). | [`src/Determinizar.cpp`](src/Determinizar.cpp): Transforma o AFND em AFD gerando os subconjuntos determinísticos e herdando os rótulos de tokens. |
| **4. Minimização** | Submeter o AFD ao processo de minimização **SEM aplicar classes de equivalência**. Livre de inalcançáveis e mortos. | [`src/Minimizar.cpp`](src/Minimizar.cpp): Realiza busca em largura (BFS) a partir do estado inicial para eliminar **inalcançáveis** e BFS reversa a partir dos estados finais para eliminar **mortos**, sem uso de partições/tabela de equivalência. |
| **5. Estado de Erro** | **Ao final da minimização**, acrescentar o estado de erro. Células não mapeadas apontam para o erro. Auto-laço no erro para todos os símbolos do alfabeto. | [`src/CompletarAutomato.cpp`](src/CompletarAutomato.cpp): Inserido exatamente após a etapa 4, completando toda a matriz de transição. |
| **6. Saída e Apresentação** | Visualização clara do autômato e validação. | [`src/Automato.cpp`](src/Automato.cpp): Impressão no formato de **Matriz/Tabela de Transição** (`->`, `*`, colunas por símbolo do alfabeto, lista de tokens), idêntica ao exemplo do PDF. |
| **7. Teste de Palavras** | Demonstrar a operação prática. | [`src/Reconhecer.cpp`](src/Reconhecer.cpp): Simulador interativo com rastreamento passo a passo do caminho percorrido ($q_0 \xrightarrow{s} q_1 \dots$) e token identificado. |

---

## 📂 Estrutura de Diretórios e Arquivos

```text
├── Makefile                   # Script de automacao para compilacao no Linux/WSL
├── README.md                  # Documentacao completa do projeto
├── exemplos/
│   ├── professor.txt          # Arquivo identico ao exemplo fornecido no PDF
│   └── entrada.txt            # Arquivo de teste alternativo
├── include/                   # Cabecalhos (headers C++)
│   ├── Automato.h             # Estrutura do automato, estados, transicoes e tabela
│   ├── LeitorArquivo.h        # Parser universal de tokens e gramaticas BNF
│   ├── LeitorGramatica.h      # Interface de compatibilidade para gramaticas
│   ├── Determinizar.h         # Algoritmo de construcao de subconjuntos
│   ├── Minimizar.h            # Minimizacao sem classes de equivalencia
│   ├── CompletarAutomato.h    # Adicao do estado de erro e completude
│   ├── VerificarEstados.h     # Diagnostico de alcancaveis, inalcancaveis e mortos
│   └── Reconhecer.h           # Simulador e rastreador de sentencas no AFD
└── src/                       # Implementacoes C++ (.cpp)
    ├── Automato.cpp
    ├── LeitorArquivo.cpp
    ├── LeitorGramatica.cpp
    ├── Determinizar.cpp
    ├── Minimizar.cpp
    ├── CompletarAutomato.cpp
    ├── VerificarEstados.cpp
    ├── Reconhecer.cpp
    └── main.cpp               # Ponto de entrada com fluxo das 5 etapas
```

---

## 🛠️ Como Compilar e Executar

### No Linux ou WSL (Recomendado na UFFS)

1. **Compilar via `make`:**
   ```bash
   make
   ```
   *Ou diretamente via `g++`:*
   ```bash
   g++ -std=c++17 -Wall -Wextra -O2 src/*.cpp -Iinclude -o projeto
   ```

2. **Executar com o arquivo de exemplo padrão:**
   ```bash
   ./projeto exemplos/professor.txt
   ```
   *Ou utilizando o alvo do Makefile:*
   ```bash
   make run
   ```

### No Windows (PowerShell / CMD com MinGW ou Clang)

```powershell
g++ -std=c++17 -Wall -Wextra -O2 src/*.cpp -Iinclude -o projeto.exe
.\projeto.exe exemplos\professor.txt
```

---

## 📝 Formato do Arquivo de Entrada

O parser aceita o formato nativo ilustrado no enunciado sem necessidade de tags obrigatórias:

```text
se
entao
senao
<S> ::= a<A> | e<A> | i<A> | o<A> | u<A>
<A> ::= a<A> | e<A> | i<A> | o<A> | u<A> | ε
```

- **Palavras / Tokens:** Qualquer linha sem o operador `::=` é interpretada como cadeia terminal/palavra reservada.
- **Gramática Regular (GR):** Linhas contendo `::=` são interpretadas como regras BNF. O símbolo vazio $\varepsilon$ pode ser escrito como `ε`, `&`, `eps` ou `E`.
- **Comentários:** Linhas iniciadas com `#` ou `//` são ignoradas.
- **Identificação de Categoria:** Por padrão, a gramática identifica a categoria `variavel`. É possível especificar nomes de seções com colchetes opcionais (ex: `[identificador]`) se desejado.

---

## 🎓 Roteiro para a Apresentação e Arguição Oral

Durante a apresentação com o professor, o terminal exibe cada etapa didaticamente:

1. **Etapa 1 (AFND):** Mostra a tabela com os estados $S, A, B, C, D, E, F, G, H, I, J, K, L, M$.
   - **Explicar:** O estado inicial $S$ é compartilhado. Para `se`, cria-se $A$ e $B$ ($*B$ é final). Para `senao`, cria-se $H, I, J, K, L$ ($*L$ é final). Note que com `'s'` em $S$ há transição para $A$ e para $H$ ao mesmo tempo $\implies$ **Indeterminismo gerado na carga**.
2. **Etapa 2 (Determinização):** Mostra como cada subconjunto virou um novo estado ($D_0, D_1 \dots$).
   - **Explicar:** Fecho-$\varepsilon$ do estado inicial cria $D_0 = \{S\}$. A partir daí, cada transição calcula os destinos de todos os membros do subconjunto, eliminando ambiguidades e transferindo os rótulos de tokens dos estados finais originais.
3. **Etapa 3 (Minimização sem Classes de Equivalência):**
   - **Explicar:** O professor exigiu explicitamente que **não** fossem usadas classes de equivalência (tabela de pares distinguíveis / algoritmo de Moore). Portanto, a minimização teórica consiste na identificação e eliminação estrita de **estados inalcançáveis** (via busca direta a partir do inicial) e **estados mortos** (via busca reversa a partir dos finais).
4. **Etapa 4 (Estado de Erro):**
   - **Explicar:** Adicionado como último estado do autômato após a minimização. Todas as células não mapeadas na matriz passam a apontar para o estado `Erro`, que possui laços para si mesmo em todos os caracteres do alfabeto.
5. **Etapa 5 (Simulador Interativo):**
   - Digitar `se` $\rightarrow$ Caminho percorrido até o estado final com token `se`.
   - Digitar `aeiou` $\rightarrow$ Caminho percorrido até o estado final com token `variavel`.
   - Digitar `sen` ou `qualquer_coisa` $\rightarrow$ Mostra a transição sendo desviada para o `[Estado de Erro]`, rejeitando a entrada.
