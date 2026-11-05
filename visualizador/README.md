# Guia do Simulador & Motion Video - LFA (UFFS)

Este visualizador interativo foi desenvolvido especificamente para o seu projeto de **Linguagens Formais e Autômatos (UFFS)**, permitindo demonstrar em tempo real e de forma animada todo o ciclo de execução do código C++, sem modificar nenhum arquivo do seu repositório original.

---

## 🚀 Como Abrir

Não é necessário instalar nenhuma dependência, Node ou servidor web. Basta abrir o arquivo com um duplo-clique:

* Abra o arquivo **`visualizador/index.html`** no seu navegador de preferência (Google Chrome, Microsoft Edge, Firefox, Brave, etc.).
* Ou execute via PowerShell no terminal:
  ```powershell
  Start-Process .\visualizador\index.html
  ```

---

## 🎬 Funcionalidades e Recursos

### 1. Linha do Tempo e Motion das 5 Etapas Canônicas
* **Etapa 1: Carga e AFND**  
  * Criação do estado inicial compartilhado `S`.
  * Criação de estados exclusivos para cada caractere dos tokens (`A`, `B`, `C`...).
  * Destaque em **laranja** quando o indeterminismo surge na tabela de transição (ex: com `s` em `se` e `senao`).
  * Processamento de Gramáticas Regulares em BNF (`<S> ::= a<A>...`).
* **Etapa 2: Determinização (AFND $\to$ AFD)**  
  * Teorema de Construção de Subconjuntos com fecho-$\varepsilon$.
  * Exibição dos subconjuntos $\{S\}$, $\{A, H\}$, etc., convertendo-se nos estados determinísticos `D0`, `D1`...
  * Herança automática dos rótulos de tokens nos estados finais.
* **Etapa 3: Minimização Sem Classes de Equivalência**  
  * Busca em largura (BFS) a partir de `S` para detectar **estados inalcançáveis**.
  * Busca em largura reversa a partir dos finais para detectar **estados mortos**.
  * Destaque em vermelho e eliminação visual suave com risco na tabela.
* **Etapa 4: Estado de Erro e AFD Completo**  
  * Inserção do estado `Erro` como último estado.
  * Todas as células não mapeadas do AFD são preenchidas apontando para o Erro (com animação de highlight).
  * Auto-laço no Erro para todos os símbolos do alfabeto.
* **Etapa 5: Reconhecedor e Fita de Leitura**  
  * Fita animada com cabeçote de leitura (`▲`).
  * Rastreamento passo a passo com iluminação da célula `(estado, símbolo)` na tabela e do nó no grafo.
  * Veredito: **ACEITA** (com os tokens encontrados) ou **REJEITADA** (se cair no Erro).

---

### 2. Controles de Reprodução (Player de Vídeo)
* **Play / Pause (`▶` / `⏸`)**: Executa a animação continuamente de forma automática.
* **Passo Anterior (`◀`) / Próximo Passo (`▶`)**: Permite avançar ou voltar micro-passo a micro-passo para explicar ao professor em detalhes.
* **Saltar Etapas (`1`, `2`, `3`, `4`, `5` ou `⏭`)**: Pula direto para o início de qualquer uma das etapas.
* **Velocidade**: Alterne entre 0.5x (Didático), 1.0x (Normal), 2.0x (Rápido) e 5.0x (Ultra).

---

### 3. Gravação de Vídeo e Exportação
* **Gravar Vídeo (`⏺ Gravar Vídeo`)**:  
  Usa a API nativa do navegador para gravar diretamente o fluxo do autômato e baixar um arquivo `.webm` pronto em alta qualidade, ideal para inserir nos slides ou submeter como anexo de trabalho!
* **Exportar Imagem (`📷 Exportar Imagem`)**:  
  Salva um instantâneo do grafo do autômato em PNG de alta resolução.

---

### 4. Exemplos Integrados
Você pode selecionar qualquer um dos arquivos da pasta `exemplos/` diretamente no menu dropdown da aplicação:
1. `professor.txt`: Exemplo oficial do PDF com `se`, `entao`, `senao` e vogais.
2. `exemplo1_operadores_digitos.txt`: Laços com dígitos numéricos.
3. `exemplo2_condicionais_prefixos.txt`: Prefixos concorrentes fortes (`in`, `int`, `if`).
4. `exemplo3_inalcancavel_e_morto.txt`: Demonstração prática da eliminação de inalcançáveis e mortos.
5. `Personalizado`: Você pode digitar suas próprias palavras e gramática diretamente no editor da página!
