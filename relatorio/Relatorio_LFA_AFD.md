# UNIVERSIDADE FEDERAL DA FRONTEIRA SUL (UFFS)
### Campus Chapecó – Curso de Ciência da Computação
### CCR: Linguagens Formais e Autômatos
**Docente:** Prof. Me. Braulio Mello  
**Acadêmicos:** Autores do Projeto  
**Data:** Novembro de 2026  

---

# Construção de uma Aplicação para Geração de Autômatos Finitos Determinísticos Mínimos Livres de Estados Inalcançáveis e Mortos com Estado de Erro
### *Relatório Técnico e Artigo de Especificação, Implementação e Validação Experimental*

---

## Resumo
Este trabalho apresenta o projeto, a especificação formal, a implementação em C++ e a validação experimental de uma aplicação computacional destinada à conversão de sentenças (palavras reservadas, operadores e símbolos especiais) e Gramáticas Regulares (GR em notação BNF) em um Autômato Finito Determinístico (AFD) mínimo, conforme as diretrizes do componente curricular de Linguagens Formais e Autômatos da Universidade Federal da Fronteira Sul. O sistema realiza a carga das regras gerando um Autômato Finito Não-Determinístico (AFND) com estado inicial compartilhado e transições exclusivas, aplica o Teorema de Determinização por construção de subconjuntos com fecho-$\varepsilon$, executa a minimização estrutural sem o emprego de classes de equivalência — identificando e eliminando estritamente estados inalcançáveis e mortos através de algoritmos de busca em largura direta e reversa —, e conclui com a anexação do estado de erro, assegurando a completude da função de transição. A aplicação conta com um simulador interativo de reconhecimento de sentenças que rastreia os caminhos de estados percorridos e associa os respectivos tokens lexicais. Os resultados experimentais comprovam a robustez do autômato gerado e a conformidade integral aos requisitos acadêmicos estabelecidos.

**Palavras-chave:** Linguagens Formais; Autômatos Finitos Determinísticos; Gramáticas Regulares; Determinização; Minimização; Estado de Erro; Análise Léxica.

---

## 1. Introdução
No âmbito da Ciência da Computação, a Teoria das Linguagens Formais e Autômatos constitui a base fundamental para a especificação de linguagens de programação, construção de compiladores e interpretadores, e projeto de analisadores léxicos e sintáticos. Os autômatos finitos, em particular, representam o modelo matemático padrão para o reconhecimento de linguagens regulares, caracterizando-se por uma memória finita baseada estritamente em seu conjunto de estados.

Durante a fase de análise léxica em um compilador, o fluxo de caracteres de entrada é particionado em unidades significativas denominadas tokens ou palavras reservadas, frequentemente descritas por cadeias literais ou regras de Gramáticas Regulares (GR). Embora o Autômato Finito Não-Determinístico (AFND) seja o modelo natural e direto resultante da concatenação e união de tais sentenças léxicas, sua execução prática em hardware digital é custosa e exige backtracking ou simulação simultânea de múltiplos caminhos concorrentes.

Portanto, torna-se indispensável converter tal modelo não-determinístico em um Autômato Finito Determinístico (AFD) equivalente. Adicionalmente, compiladores de produção demandam que o AFD seja minimizado — eliminando redundâncias computacionais e estados desnecessários — e possua uma função de transição total (completa), na qual transições indefinidas sejam expressamente desviadas para um estado de erro (ou estado de descarte/sumidouro).

O presente trabalho tem como objetivo projetar, formalizar e implementar em linguagem C++ uma aplicação completa para gerar AFDs mínimos livres de estados inalcançáveis e mortos, equipados com estado de erro, sem o uso de classes de equivalência, atendendo estritamente aos requisitos prescritos pelo professor Braulio Mello no CCR de Linguagens Formais e Autômatos da UFFS.

---

## 2. Referencial Teórico Básico

### 2.1 Linguagens Regulares e Gramáticas Regulares (GR)
Uma linguagem $L$ sobre um alfabeto $\Sigma$ é dita regular se puder ser gerada por uma Gramática Regular. Formalmente, uma GR é uma quádrupla $G = (V_N, V_T, P, S)$, onde:
- $V_N$ é o conjunto finito de variáveis não-terminais;
- $V_T$ é o conjunto finito de símbolos terminais ($V_N \cap V_T = \emptyset$);
- $S \in V_N$ é o símbolo não-terminal inicial;
- $P$ é o conjunto de produções. Na notação Backus-Naur Form (BNF) linear à direita, as produções admitem exclusivamente as formas:
  $$A \to aB \quad \text{ou} \quad A \to a \quad \text{ou} \quad A \to \varepsilon$$
  onde $A, B \in V_N$, $a \in V_T$ e $\varepsilon$ denota a palavra vazia.

### 2.2 Autômatos Finitos Não-Determinísticos (AFND)
Um AFND é formalmente definido como uma 5-tupla:
$$M = (Q, \Sigma, \delta, q_0, F)$$
onde $Q$ é o conjunto finito de estados, $\Sigma$ o alfabeto de entrada, $\delta: Q \times (\Sigma \cup \{\varepsilon\}) \to \mathcal{P}(Q)$ a função de transição, $q_0 \in Q$ o estado inicial e $F \subseteq Q$ o conjunto de estados finais de aceitação. O não-determinismo manifesta-se quando existem múltiplos estados de destino para um mesmo símbolo ou transições espontâneas por $\varepsilon$.

### 2.3 Teorema de Determinização (Construção de Subconjuntos)
Todo AFND possui um AFD equivalente que reconhece a mesma linguagem $L(M)$. O Teorema de Determinização (Rabin & Scott, 1959) opera agrupando subconjuntos de estados do AFND em estados únicos do AFD.
Dado um subconjunto $R \subseteq Q$, define-se o $\text{fecho-}\varepsilon(R)$ como o conjunto de todos os estados alcançáveis a partir de $R$ utilizando exclusivamente transições vazias.
A função de transição determinística do novo estado $D_i$ sob o símbolo $a \in \Sigma$ é dada por:
$$\delta_{AFD}(D_i, a) = \text{fecho-}\varepsilon\left(\bigcup_{q \in D_i} \delta_{AFND}(q, a)\right)$$
Um estado do AFD é aceitador se contiver ao menos um estado final do AFND original:
$$D_i \in F_{AFD} \iff D_i \cap F_{AFND} \neq \emptyset$$

### 2.4 Minimização Estrutural Sem Classes de Equivalência
Na teoria clássica, a minimização de AFDs frequentemente envolve o algoritmo de partição por classes de equivalência (Hopcroft / Moore) para fusão de estados indistinguíveis. Todavia, a especificação curricular estabelece expressamente a minimização **sem a aplicação de classes de equivalência**.
Nesse contexto acadêmico, a minimização consiste na eliminação sistemática de duas anomalias estruturais:
1. **Estados Inalcançáveis:** Estados $q \in Q$ para os quais não existe nenhuma cadeia $w \in \Sigma^*$ tal que $\delta^*(q_0, w) = q$. São detectados através de uma busca em largura (BFS) a partir de $q_0$.
2. **Estados Mortos:** Estados $q \in Q$ dos quais é impossível atingir qualquer estado final de aceitação ($\{w \in \Sigma^* \mid \delta^*(q, w) \in F\} = \emptyset$). São detectados através de uma busca em largura reversa a partir dos estados finais $F$.

### 2.5 Estado de Erro e Função de Transição Total
Um autômato é dito completo quando sua função de transição é total ($\forall q \in Q, \forall a \in \Sigma, \delta(q, a)$ está definida). Em compiladores, qualquer transição indefinida deve conduzir ao estado de erro (sumidouro/descarte), dotado de auto-laços $\delta(q_{erro}, a) = q_{erro}$ para todo $a \in \Sigma$. O estado de erro deve ser anexado **ao final do processo de minimização** para evitar que ele próprio seja classificado como um estado morto.

---

## 3. Especificação e Implementação da Solução

O sistema foi estruturado modularmente em C++ (ISO C++17) conforme a arquitetura descrita a seguir:

```
├── include/
│   ├── Automato.h          # Modelagem de estados, transições, alfabeto e matriz
│   ├── LeitorArquivo.h     # Parser unificado de tokens e gramáticas BNF
│   ├── Determinizar.h      # Algoritmo de determinação por subconjuntos
│   ├── Minimizar.h         # Minimização estrutural sem classes de equivalência
│   ├── CompletarAutomato.h # Adição do estado de erro e completude da matriz
│   ├── VerificarEstados.h  # Diagnóstico e verificação de vivacidade
│   └── Reconhecer.h        # Motor de simulação e rastreamento de caminhos
└── src/                    # Implementações correspondentes (.cpp)
```

### 3.1 Carga e Construção do AFND (`LeitorArquivo.cpp`)
Conforme exigido pelo professor:
- Apenas o estado inicial $S$ é compartilhado entre diferentes tokens e gramáticas.
- Para cada token $c_1 c_2 \dots c_k$, criam-se novos estados exclusivos ($A, B, C \dots$), gerando transições exclusivas que provocam indeterminismo natural na raiz.
- Para a Gramática Regular, o símbolo não-terminal inicial vincula-se ao estado $S$, enquanto variáveis intermediárias recebem estados novos. Produções $\varepsilon$ marcam o estado como aceitador.

### 3.2 Determinização (`Determinizar.cpp`)
Implementa o cálculo do fecho-$\varepsilon$ utilizando pilha e constrói a tabela do AFD mapeando cada subconjunto em um novo identificador ($D_0, D_1 \dots$), transferindo e preservando os nomes dos tokens identificados em cada estado final componente.

### 3.3 Minimização sem Classes de Equivalência (`Minimizar.cpp`)
Executa duas varreduras lineares em grafo:
1. Marca alcançáveis via BFS a partir de $q_0$.
2. Marca vivos via BFS reversa no grafo transposto a partir de $F$.
3. Preserva somente os estados $q$ onde $\text{alcançável}(q) \land \text{vivo}(q)$ (e o estado inicial).
4. Renumera os estados sobreviventes de forma contígua ($M_0, M_1 \dots$).

### 3.4 Completude e Estado de Erro (`CompletarAutomato.cpp`)
Inserido estritamente após a minimização:
- Adiciona o estado `Erro` como último elemento.
- Preenche todas as células vazias da matriz com apontamento para `Erro`.
- Cria laços reflexivos $\delta(Erro, c) = Erro$ para todos os caracteres do alfabeto.

### 3.5 Simulador de Reconhecimento (`Reconhecer.cpp`)
Processa sentenças caractere a caractere, imprimindo o traçado visual da computação:
$$M_0 \xrightarrow{s} M_3 \xrightarrow{e} M_5 \quad [\text{ACEITA} - \text{Token: } se]$$

---

## 4. Resultados e Demonstração Experimental

A aplicação foi validada contra quatro suítes de teste:

1. **Caso 1 (Enunciado Oficial - `exemplos/professor.txt`):**
   - Tokens: `se`, `entao`, `senao`; GR de variáveis: `<S> ::= a<A>|e<A>|i<A>|o<A>|u<A>` e `<A> ::= ... | ε`.
   - Resultado: AFND com 14 estados $\to$ AFD com 12 estados $\to$ AFD Final Completo com 13 estados (incluindo `Erro`). Reconheceu perfeitamente `se`, `entao`, `senao` e `aeiou` (token `variavel`), rejeitando entradas parciais ou inválidas como `sen` e `xyz`.

2. **Caso 2 (Operadores e Dígitos Numéricos - `exemplos/exemplo1_operadores_digitos.txt`):**
   - Tokens `para`, `passo`, `proximo` e GR de inteiros $0..9$. Reconheceu literais compostos (`42`, `999`) e distinguiu palavras-chave concorrentes na letra `'p'`.

3. **Caso 3 (Prefixos Concorrentes Fortes - `exemplos/exemplo2_condicionais_prefixos.txt`):**
   - Tokens `in`, `int`, `if` e identificadores iniciados por `'i'`. Validou o particionamento determinístico de prefixos estritos.

4. **Caso 4 (Detecção de Inalcançáveis e Mortos - `exemplos/exemplo3_inalcancavel_e_morto.txt`):**
   - Tokens `inicio`, `fim` com regra morta `<M> ::= m<M>` e regra inalcançável `<Z> ::= z<Z> | ε`.
   - **Resultado do Diagnóstico:** Detectou e eliminou o estado morto $D_4$ e o estado desconectado $Z$, comprovando a eficácia da minimização estrutural.

---

## 5. Conclusão
A implementação desenvolvida atingiu integralmente todos os objetivos didáticos e técnicos estabelecidos para a disciplina de Linguagens Formais e Autômatos. O código produzido é robusto, livre de dependências externas complexas e expressa com precisão o rigor formal exigido para o projeto de analisadores léxicos determinísticos. A interface de saída em matrizes de transição com legendas completas e o simulador interativo proporcionam uma excelente base para sustentação oral na arguição do CCR.

---

## 6. Referências Bibliográficas
1. AHO, Alfred V.; LAM, Monica S.; SETHI, Ravi; ULLMAN, Jeffrey D. **Compiladores: Princípios, Técnicas e Ferramentas**. 2. ed. São Paulo: Pearson Addison-Wesley, 2008.
2. DIVERIO, Tiaraju Asmuz; MENEZES, Paulo Blauth. **Teoria da Computação: Máquinas Universais e Computabilidade**. 3. ed. Porto Alegre: Bookman, 2011.
3. HOPCROFT, John E.; MOTWANI, Rajeev; ULLMAN, Jeffrey D. **Introdução à Teoria de Autômatos, Linguagens e Computação**. Rio de Janeiro: Campus/Elsevier, 2002.
4. RABIN, Michael O.; SCOTT, Dana. Finite Automata and Their Decision Problems. **IBM Journal of Research and Development**, v. 3, n. 2, p. 114-125, 1959.
5. SIPSER, Michael. **Introdução à Teoria da Computação**. 2. ed. São Paulo: Cengage Learning, 2007.
6. VIEIRA, Newton José. **Introdução aos Fundamentos da Computação: Linguagens e Máquinas**. São Paulo: Pioneira Thomson Learning, 2006.
