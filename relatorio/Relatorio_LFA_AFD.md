# UNIVERSIDADE FEDERAL DA FRONTEIRA SUL (UFFS)
### Campus Chapecó – Curso de Ciência da Computação
### CCR: Linguagens Formais e Autômatos
**Docente:** Prof. Me. Braulio Mello  
**Acadêmicos:** Autores do Projeto  
**Data:** Novembro de 2026  

---

# Construção e Validação Linear de Autômato Finito Determinístico Mínimo com Estado de Erro a partir de Especificações Léxicas e Gramáticas Regulares

### *Relatório Técnico-Científico de Concepção, Algoritmos e Implementação em C++*

---

## Resumo
Este trabalho apresenta o projeto, a especificação formal e a implementação em linguagem C++17 de uma ferramenta computacional para conversão integrada de palavras reservadas (tokens literais) e Gramáticas Regulares (GR em notação BNF) em um Autômato Finito Determinístico (AFD) mínimo, provido de estado de erro e estritamente livre de estados inalcançáveis e mortos, sem o emprego de partição por classes de equivalência. A arquitetura do sistema foi estruturada em um pipeline linear de cinco etapas sequenciais: (1) instanciação do Autômato Finito Não-Determinístico (AFND) com unicidade estrita do estado inicial $S$ e caminhos exclusivos para cada token; (2) determinização clássica por construção de subconjuntos com cálculo de fecho-$\varepsilon$; (3) minimização estrutural sem classes de equivalência por eliminação de estados inalcançáveis (busca em largura direta) e estados mortos (busca em largura reversa no grafo transposto); (4) totalização da função de transição pela anexação final de um estado de erro sumidouro dotado de auto-laços reflexivos; e (5) verificação estrutural e simulação de reconhecimento de sentenças em tempo linear $O(n)$ com rastreamento completo de transições e tokens identificados. A validação experimental sobre o caso base do CCR demonstrou a progressão quantitativa de estados ($14 \to 12 \to 12 \to 13$), comprovando a robustez dos algoritmos e a aderência integral às diretrizes curriculares estabelecidas.

**Palavras-chave:** Linguagens Formais; Autômatos Finitos Determinísticos; Gramáticas Regulares; Determinização por Subconjuntos; Minimização Estrutural; Estado de Erro; Análise Léxica.

---

## Abstract
This paper presents the formal design and C++17 implementation of a computational tool for transforming reserved words (literal tokens) and Regular Grammars (RG in BNF notation) into a minimal Deterministic Finite Automaton (DFA) equipped with a sink error state and strictly free of unreachable and dead states, without applying equivalence class partitioning. The architecture is structured into a linear five-stage pipeline: (1) instantiation of a Non-Deterministic Finite Automaton (NFA) with a single shared initial state $S$ and exclusive paths for each token; (2) classical subset construction determinization with epsilon-closure evaluation; (3) structural minimization without equivalence classes by eliminating unreachable states (forward BFS) and dead states (reverse BFS on the transposed graph); (4) transition function totalization by appending a final sink error state; and (5) structural verification and linear-time $O(n)$ sentence recognition with full path tracing. Experimental verification on benchmark instances confirmed the state progression ($14 \to 12 \to 12 \to 13$), demonstrating correctness and total compliance with formal curriculum requirements.

**Keywords:** Formal Languages; Deterministic Finite Automata; Regular Grammars; Subset Construction; Structural Minimization; Error State; Lexical Analysis.

---

## 1. Introdução

Na arquitetura clássica de processadores de linguagens e compiladores, a análise léxica (ou *scanner*) atua como a etapa inicial do *front-end*, responsável pela leitura do fluxo contínuo de caracteres de entrada e pelo seu agrupamento em unidades léxicas elementares denominadas *tokens*. Enquanto as palavras reservadas e os padrões sintáticos de identificadores e numerais são naturalmente especificados por regras de Gramáticas Regulares (GR) lineares à direita ou listas de sentenças literais, sua transcrição direta para máquinas de estado finito resulta habitualmente em Autômatos Finitos Não-Determinísticos (AFND).

Embora o modelo não-determinístico possua o mesmo poder expressivo do modelo determinístico para a classe das linguagens regulares, sua execução em computadores digitais convencionais exigiria algoritmos de retrocesso (*backtracking*) ou paralelismo simulado, implicando custos proibitivos de processamento. Torna-se mandatório, portanto, converter o AFND em um Autômato Finito Determinístico (AFD) equivalente, no qual a leitura de cada caractere determine um estado de destino unívoco, assegurando tempo de reconhecimento estritamente linear em função do comprimento da cadeia de entrada ($O(n)$).

No âmbito pedagógico e técnico do CCR de Linguagens Formais e Autômatos da UFFS, a especificação curricular estabelece três diretrizes estruturais de projeto:
1. **Minimização Estrutural Específica:** O autômato gerado deve ser mínimo estritamente sob o critério de **ausência de estados inalcançáveis e mortos**, vetando expressamente o emprego de particionamento por classes de equivalência entre estados indistinguíveis;
2. **Função de Transição Total:** O autômato deve ser completo, de modo que qualquer combinação de estado e símbolo não prevista pela linguagem seja direcionada para um **estado de erro** (sumidouro ou *sink*);
3. **Ordem Rígida de Execução:** O estado de erro deve ser anexado **ao final do processo de minimização**, evitando distorções conceituais, uma vez que o estado de erro, por definição formal, é incapaz de alcançar qualquer estado final de aceitação.

Este trabalho tem como objetivo descrever a formalização teórica, a metodologia linear e a implementação desenvolvida em C++17, demonstrando passo a passo as etapas de transformação e a validação experimental completa da ferramenta sobre os casos de teste especificados.

---

## 2. Fundamentação Teórica e Modelo Formal

O desenvolvimento fundamenta-se nos conceitos consolidados da teoria de autômatos finitos e linguagens formais regulares.

### 2.1 Gramáticas Regulares e Sentenças Literais
Uma Gramática Regular à direita (tipo 3 na Hierarquia de Chomsky) é formalmente uma quádrupla:
$$G = (V_N, V_T, P, S)$$
onde:
- $V_N$ é o conjunto finito de variáveis não-terminais;
- $V_T$ é o conjunto finito de símbolos terminais, sendo $V_N \cap V_T = \emptyset$;
- $S \in V_N$ é o símbolo não-terminal inicial;
- $P$ é o conjunto de produções, cujas regras admitem estritamente as formas $A \to aB$, $A \to a$ ou $A \to \varepsilon$, com $A, B \in V_N$, $a \in V_T$ e $\varepsilon$ representando a cadeia vazia.

Tokens literais (como `se`, `entao`, `senao`) correspondem a palavras finitas $w = c_1 c_2 \dots c_k \in V_T^*$. Cada cadeia é assimilada pelo autômato como uma cadeia linear estrita de derivação $S \xrightarrow{c_1} A_1 \xrightarrow{c_2} \dots \xrightarrow{c_k} A_k$, onde $A_k$ é marcado como estado de aceitação portador do nome do token.

### 2.2 Autômato Finito Não-Determinístico (AFND)
O AFND gerado na primeira etapa é definido formalmente pela 5-upla:
$$M_{AFND} = (Q, \Sigma, \delta, S, F)$$
onde:
- $Q$ é o conjunto finito de estados;
- $\Sigma$ é o alfabeto de símbolos de entrada;
- $\delta: Q \times (\Sigma \cup \{\varepsilon\}) \to \mathcal{P}(Q)$ é a função de transição que mapeia um par estado-símbolo para um subconjunto de estados possíveis;
- $S \in Q$ é o estado inicial compartilhado;
- $F \subseteq Q$ é o conjunto de estados finais de aceitação.

O não-determinismo surge quando existem múltiplos caminhos para um mesmo símbolo ($\|\delta(q, a)\| > 1$) ou transições espontâneas por cadeia vazia $\varepsilon$.

### 2.3 Teorema da Determinização por Construção de Subconjuntos
Conforme a teoria clássica de autômatos, todo AFND possui um AFD equivalente $M_{AFD} = (Q_D, \Sigma, \delta_D, q_{D0}, F_D)$ que aceita exatamente a mesma linguagem formal.
- Cada estado $D_i \in Q_D$ representa um subconjunto de estados do AFND original ($D_i \subseteq Q$);
- O estado inicial corresponde ao fecho-$\varepsilon$ do estado inicial: $q_{D0} = \text{fecho-}\varepsilon(\{S\})$;
- A função de transição determinística é computada por:
  $$\delta_D(D_i, a) = \text{fecho-}\varepsilon\left(\bigcup_{q \in D_i} \delta(q, a)\right), \quad \forall a \in \Sigma$$
- Um estado $D_i$ é final se contiver ao menos um estado aceitador do AFND:
  $$D_i \in F_D \iff D_i \cap F \neq \emptyset$$

### 2.4 Minimização Estrutural sem Classes de Equivalência
Na teoria tradicional, a minimização plena frequentemente utiliza algoritmos de partição para unificar estados indistinguíveis. No entanto, sob a especificação curricular sob análise, a minimização resume-se estritamente à detecção e remoção de duas categorias de estados supérfluos:
1. **Estados Inalcançáveis:** Estados $q \in Q_D$ para os quais não existe qualquer palavra $w \in \Sigma^*$ tal que $\delta_D^*(q_{D0}, w) = q$. São detectados em tempo linear mediante uma busca em largura (BFS) direta a partir de $q_{D0}$.
2. **Estados Mortos:** Estados $q \in Q_D$ dos quais é impossível alcançar qualquer estado aceitador de $F_D$ ($\{w \in \Sigma^* \mid \delta_D^*(q, w) \in F_D\} = \emptyset$). São detectados em tempo linear por uma busca em largura (BFS) reversa a partir dos estados finais sobre o grafo com arestas invertidas (grafo transposto $G^T$).

A minimização preserva exclusivamente os estados que satisfazem:
$$\text{Manter}(q) \iff (\text{Alcancavel}(q) \land \text{Vivo}(q)) \lor (q = q_{D0})$$

### 2.5 Totalidade da Função de Transição via Estado de Erro
Para converter a função de transição parcial em uma função total $\delta: Q \times \Sigma \to Q$, anexa-se um nó terminal sumidouro $q_{\text{erro}} \notin F_D$ dotado de auto-laços $\delta(q_{\text{erro}}, a) = q_{\text{erro}}$ para todo $a \in \Sigma$. Todas as transições indefinidas dos estados válidos convergem para $q_{\text{erro}}$, assegurando a completude da máquina.

---

## 3. Metodologia e Pipeline Linear da Aplicação

A arquitetura do software foi implementada em C++17 modular, estruturada em um pipeline linear unidirecional no qual cada etapa recebe um objeto da classe `Automato` e produz a estrutura subsequente.

```
+--------------------------------------------------------------------------+
|                  ARQUIVO DE ENTRADA (professor.txt)                      |
|                  Tokens literais e Gramática BNF                         |
+--------------------------------------------------------------------------+
                                     |
                                     v
+--------------------------------------------------------------------------+
| [ETAPA 1] LeitorArquivo.cpp -> Construção do AFND                        |
| - Estado inicial S único e compartilhado                                 |
| - Caminhos exclusivos para cada token -> Indeterminismo nativo           |
| - Resultado: 14 estados (S, A..M), alfabeto com 8 símbolos               |
+--------------------------------------------------------------------------+
                                     |
                                     v
+--------------------------------------------------------------------------+
| [ETAPA 2] Determinizar.cpp -> Teorema de Subconjuntos                    |
| - Mapeamento de subconjuntos de estados em novos nós Dn                  |
| - Resolução de indeterminações e herança de rótulos de tokens            |
| - Resultado: 12 estados determinados (D0 a D11)                          |
+--------------------------------------------------------------------------+
                                     |
                                     v
+--------------------------------------------------------------------------+
| [ETAPA 3] Minimizar.cpp -> Minimização sem Classes de Equivalência       |
| - BFS direta a partir de D0 (identifica estados alcançáveis)             |
| - BFS reversa no grafo transposto a partir de F (identifica vivos)       |
| - Poda de estados mortos e inalcançáveis -> Renumeração (M0..M11)        |
+--------------------------------------------------------------------------+
                                     |
                                     v
+--------------------------------------------------------------------------+
| [ETAPA 4] CompletarAutomato.cpp -> Estado de Erro e Função Total         |
| - Anexação do nó Erro como último estado                                 |
| - Preenchimento de 72 células vazias com transição para Erro             |
| - 8 auto-laços reflexivos no estado de erro                              |
| - Resultado: 13 estados (M0..M11 + Erro)                                 |
+--------------------------------------------------------------------------+
                                     |
                                     v
+--------------------------------------------------------------------------+
| [ETAPA 5] Reconhecer.cpp / VerificarEstados.cpp -> Simulação Linear O(n) |
| - Diagnóstico estrutural de alcance e vivacidade                         |
| - Reconhecimento com rastro detalhado do caminho de transição            |
+--------------------------------------------------------------------------+
```

### 3.1 Arquitetura de Pastas e Módulos do Código-Fonte

O repositório do projeto organiza-se estritamente conforme a arquitetura modular:
- `include/`: Contém as interfaces e definições de tipos (`.h`);
- `src/`: Contém as implementações dos algoritmos (`.cpp`);
- `exemplos/`: Arquivos de teste (`professor.txt`, `exemplo1_operadores_digitos.txt`, etc.);
- `relatorio/`: Documentação técnica e relatórios acadêmicos;
- `Makefile`: Script de compilação automatizada com `g++ -std=c++17 -Wall -Wextra -O2`.

| Arquivo de Cabeçalho | Arquivo Fonte | Responsabilidade Central |
|---|---|---|
| `include/Automato.h` | `src/Automato.cpp` | Modela estados, transições, alfabeto (`std::set<char>`), tabela de transição alinhada e identificadores de estado inicial, final e de erro. |
| `include/LeitorArquivo.h` | `src/LeitorArquivo.cpp` | Realiza o *parsing* integrado do arquivo `.txt`, separando tokens literais e produções BNF (`::=`), construindo o AFND inicial. |
| `include/LeitorGramatica.h` | `src/LeitorGramatica.cpp` | Módulo de compatibilidade que delega a leitura ao `LeitorArquivo`. |
| `include/Determinizar.h` | `src/Determinizar.cpp` | Executa o algoritmo de determinação por construção de subconjuntos com cálculo de fecho-$\varepsilon$. |
| `include/Minimizar.h` | `src/Minimizar.cpp` | Executa a minimização sem classes de equivalência, removendo nós inalcançáveis e mortos. |
| `include/CompletarAutomato.h` | `src/CompletarAutomato.cpp` | Acrescenta o estado de erro e preenche as lacunas da matriz para tornar a função de transição total. |
| `include/VerificarEstados.h` | `src/VerificarEstados.cpp` | Audita o autômato final confirmando a ausência de inalcançáveis e mortos. |
| `include/Reconhecer.h` | `src/Reconhecer.cpp` | Motor de reconhecimento e simulação interativa de sentenças com impressão de rastro computacional. |
| — | `src/main.cpp` | Orquestrador do pipeline de execução e interface de linha de comando. |

### 3.2 Complexidade Assintótica de Tempo e Espaço

A Tabela 1 apresenta a análise de complexidade computacional para cada componente do pipeline:

| Módulo | Algoritmo | Complexidade de Tempo | Complexidade de Espaço |
|---|---|---|---|
| `LeitorArquivo` | Parsing e construção linear do AFND | $O(N_{\text{linhas}} \cdot L_{\text{media}})$ | $O(\|Q_{\text{AFND}}\| + \|\delta_{\text{AFND}}\|)$ |
| `Determinizar` | Construção de subconjuntos com BFS | $O(\|\Sigma\| \cdot 2^{\|Q_{\text{AFND}}\|})$ pior caso; linear no caso prático | $O(\|Q_{\text{AFD}}\| \cdot \|\Sigma\|)$ |
| `Minimizar` | BFS direta + BFS reversa em grafo transposto | $O(\|Q_{\text{AFD}}\| + \|\delta_{\text{AFD}}\|)$ | $O(\|Q_{\text{AFD}}\|)$ |
| `CompletarAutomato` | Varredura e totalização da matriz | $O(\|Q_{\text{AFD}}\| \cdot \|\Sigma\|)$ | $O(\|Q_{\text{AFD}}\| \cdot \|\Sigma\|)$ |
| `Reconhecer` | Simulação determinística de palavras | $O(\|w\| \cdot \|\delta_{\text{AFD}}\|)$ | $O(\|w\|)$ |

*Tabela 1: Complexidade computacional assintótica dos módulos do sistema.*

---

## 4. Estudo de Caso Experimental e Validação dos Resultados

A validação foi conduzida sobre a especificação oficial contida no arquivo `exemplos/professor.txt`:

```text
se
entao
senao
<S> ::= a<A> | e<A> | i<A> | o<A> | u<A>
<A> ::= a<A> | e<A> | i<A> | o<A> | u<A> | ε
```

### 4.1 Evolução Sequencial do Autômato
1. **Etapa 1 (AFND Inicial):** 14 estados ($S, A, B, C, D, E, F, G, H, I, J, K, L, M$). O alfabeto detectado contém 8 símbolos terminais: $\Sigma = \{a, e, i, n, o, s, t, u\}$. Identificam-se as indeterminações: $\delta(S, s) = \{A, H\}$ e $\delta(S, e) = \{C, M\}$.
2. **Etapa 2 (AFD Determinado):** 12 estados determinizados ($D_0$ a $D_{11}$). A construção de subconjuntos agrupou os estados equivalentes concorrentes:
   - $D_0 = \{S\}$ (Inicial);
   - $D_1 = \{M\}$ (Final: `variavel`);
   - $D_2 = \{C, M\}$ (Final: `variavel`);
   - $D_3 = \{A, H\}$;
   - $D_4 = \{D\}$;
   - $D_5 = \{B, I\}$ (Final: `se`);
   - $D_6 = \{E\}$;
   - $D_7 = \{J\}$;
   - $D_8 = \{F\}$;
   - $D_9 = \{K\}$;
   - $D_{10} = \{G\}$ (Final: `entao`);
   - $D_{11} = \{L\}$ (Final: `senao`).
3. **Etapa 3 (AFD Minimizado):** 12 estados preservados ($M_0$ a $M_{11}$). Todos os nós mostraram-se simultaneamente alcançáveis a partir do estado inicial e vivos em relação a algum estado aceitador, mantendo o quantitativo inalterado.
4. **Etapa 4 (AFD Total Completo):** 13 estados ($M_0$ a $M_{11}$ acrescidos do estado `Erro` com identificador numérico 12). Foram identificadas e preenchidas exatamente 72 transições ausentes com apontamento para `Erro`, além de 8 auto-laços reflexivos.

### 4.2 Matriz de Transição do AFD Completo

A Tabela 2 apresenta a matriz de transições completa gerada pela aplicação ao término da Etapa 4:

| Estado ($\delta$) | a | e | i | n | o | s | t | u | Tokens Identificados |
|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---|
| **-> M0** | M1 | M2 | M1 | Erro | Erro | M3 | Erro | M1 | — |
| *** M1** | M1 | M1 | M1 | Erro | Erro | Erro | Erro | M1 | variavel |
| *** M2** | M1 | M1 | M1 | M4 | Erro | Erro | Erro | M1 | variavel |
| **M3** | Erro | M5 | Erro | Erro | Erro | Erro | Erro | Erro | — |
| **M4** | Erro | Erro | Erro | Erro | Erro | Erro | M6 | Erro | — |
| *** M5** | Erro | Erro | Erro | M7 | Erro | Erro | Erro | Erro | se |
| **M6** | M8 | Erro | Erro | Erro | Erro | Erro | Erro | Erro | — |
| **M7** | M9 | Erro | Erro | Erro | Erro | Erro | Erro | Erro | — |
| **M8** | Erro | Erro | Erro | Erro | M10 | Erro | Erro | Erro | — |
| **M9** | Erro | Erro | Erro | Erro | M11 | Erro | Erro | Erro | — |
| *** M10** | Erro | Erro | Erro | Erro | Erro | Erro | Erro | Erro | entao |
| *** M11** | Erro | Erro | Erro | Erro | Erro | Erro | Erro | Erro | senao |
| **E Erro** | Erro | Erro | Erro | Erro | Erro | Erro | Erro | Erro | [ERRO] |

*Tabela 2: Matriz de transição do AFD completo. Legenda: -> Inicial; * Final; E Erro.*

### 4.3 Reconhecimento de Sentenças no Simulador Interativo

A Tabela 3 documenta a avaliação experimental de sentenças no simulador:

| Palavra de Entrada | Rastro da Computação no AFD | Status | Token Retornado | Justificativa Teórica |
|---|---|:---:|:---:|---|
| `se` | $M_0 \xrightarrow{s} M_3 \xrightarrow{e} M_5$ | **ACEITA** | `se` | Termina no estado final aceitador $M_5$. |
| `entao` | $M_0 \xrightarrow{e} M_2 \xrightarrow{n} M_4 \xrightarrow{t} M_6 \xrightarrow{a} M_8 \xrightarrow{o} M_{10}$ | **ACEITA** | `entao` | Derivação estrita com terminação em $M_{10}$. |
| `senao` | $M_0 \xrightarrow{s} M_3 \xrightarrow{e} M_5 \xrightarrow{n} M_7 \xrightarrow{a} M_9 \xrightarrow{o} M_{11}$ | **ACEITA** | `senao` | Derivação estrita com terminação em $M_{11}$. |
| `e` | $M_0 \xrightarrow{e} M_2$ | **ACEITA** | `variavel` | $M_2 = \{C, M\}$ herda a condição aceitadora de $M$. |
| `aeiou` | $M_0 \xrightarrow{a} M_1 \xrightarrow{e} M_1 \xrightarrow{i} M_1 \xrightarrow{o} M_1 \xrightarrow{u} M_1$ | **ACEITA** | `variavel` | Auto-laços sobre o estado aceitador de vogais. |
| `sen` | $M_0 \xrightarrow{s} M_3 \xrightarrow{e} M_5 \xrightarrow{n} M_7$ | **REJEITADA** | — | Prefixo válido, porém $M_7$ não é estado final. |
| `st` | $M_0 \xrightarrow{s} M_3 \xrightarrow{t} \text{Erro}$ | **REJEITADA** | — | A transição $\delta(M_3, t)$ converge para o estado sumidouro. |
| `xyz` | $M_0 \xrightarrow{x} \text{[Sem transicao]}$ | **REJEITADA** | — | Caractere `'x'` não integra o alfabeto formal $\Sigma$. |

*Tabela 3: Rastreamento do reconhecimento de palavras no AFD.*

### 4.4 Validação Ativa de Inúteis (`exemplo3_inalcancavel_e_morto.txt`)
O caso de teste anômalo contendo a regra morta `<M> ::= m<M>` e a regra inalcançável `<Z> ::= z<Z> | ε` comprovou empiricamente que o módulo `Minimizar.cpp` elimina estados mortos e componentes desconexas, remapeando os nós de forma contígua e sem lacunas.

---

## 5. Diretrizes para Arguição Oral e Decisões de Projeto

1. **Reconhecimento da cadeia `'e'` como `variavel`:** No AFND inicial, o símbolo `'e'` a partir de $S$ atinge o estado $C$ (início do token `entao`) e o estado $M$ (produção da GR de vogais). Na determinização, é formado o estado $D_2 = \{C, M\}$. Como $M$ é aceitador pela produção vazia $\varepsilon$, $D_2$ herda legitimamente o rótulo de aceitação `variavel`.
2. **Precedência da Minimização sobre o Estado de Erro:** O estado de erro é, formalmente, um estado morto (dele não parte nenhuma transição para estados finais). Se fosse introduzido previamente à minimização, a busca em largura reversa a partir de $F$ o classificaria e expurgaria como morto, violando a exigência de completude da matriz.
3. **Poda Topológica vs. Classes de Equivalência:** O método clássico de Hopcroft unifica estados indistinguíveis pelo cálculo de partições iterativas. A minimização requisitada opera como uma poda topológica sobre grafos direcionados, removendo exclusivamente nós mortos e inalcançáveis, preservando a identidade estrutural dos estados úteis.
4. **Eficiência da BFS Reversa:** A construção prévia da lista de adjacências invertida (grafo transposto $G^T$) permite identificar a totalidade dos estados vivos em uma única varredura linear de complexidade $O(\|V\| + \|E\|)$ a partir dos estados finais.

---

## 6. Conclusão

A ferramenta desenvolvida atendeu com rigor a todos os requisitos matemáticos e funcionais prescritos pelo CCR de Linguagens Formais e Autômatos da UFFS. O pipeline linear implementado assegurou transparência e rastreabilidade total de cada transformação teórica. Os resultados experimentais comprovaram a integridade das matrizes de transição e a precisão do motor de reconhecimento, fornecendo um modelo computacional robusto para o estudo prático da análise léxica em compiladores.

---

## 7. Referências Bibliográficas

1. MENEZES, P. B. **Linguagens Formais e Autômatos**. 6. ed. Porto Alegre: Bookman, 2011.
2. AHO, A. V.; LAM, M. S.; SETHI, R.; ULLMAN, J. D. **Compiladores: Princípios, Técnicas e Ferramentas**. 2. ed. São Paulo: Pearson/Prentice Hall, 2008.
3. DIVERIO, T. A.; MENEZES, P. B. **Teoria da Computação: Máquinas Universais e Computabilidade**. 3. ed. Porto Alegre: Bookman, 2011.
4. DAVIS, M. D.; SIGAL, Ron; WEYUKER, E. J. **Computability, Complexity, and Languages: Fundamentals of Theoretical Computer Science**. 2. ed. Boston: Academic Press, Harcourt, Brace, 1994.
5. FURTADO, O. J. V. **Apostila de Linguagens Formais e Compiladores**. [Florianópolis]: UFSC, 2002. Disponível em: <www.inf.ufsc.br/~olinto/apostila-lfc.doc>.
6. HOPCROFT, J. E.; MOTWANI, R.; ULLMAN, J. D. **Introdução à Teoria dos Autômatos, Linguagens e Computação**. Rio de Janeiro: Campus, 2003.
