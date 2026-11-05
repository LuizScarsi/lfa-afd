import os
import sys
import re
from docx import Document
from docx.shared import Inches, Pt, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.table import WD_TABLE_ALIGNMENT
from docx.oxml import OxmlElement, parse_xml
from docx.oxml.ns import nsdecls, qn

from reportlab.lib import colors
from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.lib.units import cm
from reportlab.platypus import (
    SimpleDocTemplate, Paragraph, Spacer, Table, TableStyle, PageBreak, KeepTogether
)
from reportlab.pdfgen import canvas

# ---------------------------------------------------------------------------
# METADADOS INSTITUCIONAIS
# ---------------------------------------------------------------------------
INSTITUICAO = "UNIVERSIDADE FEDERAL DA FRONTEIRA SUL (UFFS)"
CAMPUS = "CAMPUS CHAPECÓ – CURSO DE CIÊNCIA DA COMPUTAÇÃO"
DISCIPLINA = "CCR: LINGUAGENS FORMAIS E AUTÔMATOS"
PROFESSOR = "Prof. Braulio Mello"
TITULO = "Construção e Validação Linear de Autômato Finito Determinístico Mínimo com Estado de Erro a partir de Especificações Léxicas e Gramáticas Regulares"
SUBTITULO = "Relatório Técnico-Científico de Concepção, Algoritmos e Implementação em C++"
AUTORES = "Acadêmicos do CCR de Linguagens Formais e Autômatos"
EMAIL = "Matheus Lazarotto, Luiz Augusto Scarsi"
CIDADE_ANO = "Chapecó – SC, Novembro de 2026"

RESUMO_TEXTO = (
    "Este trabalho apresenta o projeto, a especificação formal e a implementação em linguagem C++17 de uma ferramenta "
    "computacional para conversão integrada de palavras reservadas (tokens literais) e Gramáticas Regulares (GR em notação BNF) "
    "em um Autômato Finito Determinístico (AFD) mínimo, provido de estado de erro e estritamente livre de estados inalcançáveis "
    "e mortos, sem o emprego de partição por classes de equivalência. A arquitetura do sistema foi estruturada em um pipeline "
    "linear de cinco etapas sequenciais: (1) instanciação do Autômato Finito Não-Determinístico (AFND) com unicidade estrita do "
    "estado inicial S e caminhos exclusivos para cada token; (2) determinização clássica por construção de subconjuntos com cálculo "
    "de fecho-ε; (3) minimização estrutural sem classes de equivalência por eliminação de estados inalcançáveis (busca em largura direta) "
    "e estados mortos (busca em largura reversa no grafo transposto); (4) totalização da função de transição pela anexação final de um "
    "estado de erro sumidouro dotado de auto-laços reflexivos; e (5) verificação estrutural e simulação de reconhecimento de sentenças "
    "em tempo linear O(n) com rastreamento completo de transições e tokens identificados. A validação experimental sobre o caso base do CCR "
    "demonstrou a progressão quantitativa de estados (14 -> 12 -> 12 -> 13), comprovando a robustez dos algoritmos e a aderência "
    "integral às diretrizes curriculares estabelecidas."
)

PALAVRAS_CHAVE = "Linguagens Formais; Autômatos Finitos Determinísticos; Gramáticas Regulares; Determinização por Subconjuntos; Minimização Estrutural; Estado de Erro; Análise Léxica."

ABSTRACT_TEXTO = (
    "This paper presents the formal design and C++17 implementation of a computational tool for transforming reserved words "
    "(literal tokens) and Regular Grammars (RG in BNF notation) into a minimal Deterministic Finite Automaton (DFA) equipped with a "
    "sink error state and strictly free of unreachable and dead states, without applying equivalence class partitioning. The architecture "
    "is structured into a linear five-stage pipeline: (1) instantiation of a Non-Deterministic Finite Automaton (NFA) with a single "
    "shared initial state S and exclusive paths for each token; (2) classical subset construction determinization with epsilon-closure "
    "evaluation; (3) structural minimization without equivalence classes by eliminating unreachable states (forward BFS) and dead "
    "states (reverse BFS on the transposed graph); (4) transition function totalization by appending a final sink error state; and (5) "
    "structural verification and linear-time O(n) sentence recognition with full path tracing. Experimental verification on benchmark "
    "instances confirmed the state progression (14 -> 12 -> 12 -> 13), demonstrating correctness and total compliance with formal "
    "curriculum requirements."
)

KEYWORDS = "Formal Languages; Deterministic Finite Automata; Regular Grammars; Subset Construction; Structural Minimization; Error State; Lexical Analysis."

# ---------------------------------------------------------------------------
# ESTRUTURA DO TEXTO EM PRETO E BRANCO (LINEAR E SEM CITACOES INVASIVAS NO TEXTO)
# ---------------------------------------------------------------------------
SECOES = [
    ("1. Introdução", [
        "Na arquitetura clássica de processadores de linguagens e compiladores, a análise léxica (ou scanner) atua como a etapa inicial do front-end, responsável pela leitura do fluxo contínuo de caracteres de entrada e pelo seu agrupamento em unidades léxicas elementares denominadas tokens. Enquanto as palavras reservadas e os padrões sintáticos de identificadores e numerais são naturalmente especificados por regras de Gramáticas Regulares (GR) lineares à direita ou listas de sentenças literais, sua transcrição direta para máquinas de estado finito resulta habitualmente em Autômatos Finitos Não-Determinísticos (AFND).",
        "Embora o modelo não-determinístico possua o mesmo poder expressivo do modelo determinístico para a classe das linguagens regulares, sua execução em computadores digitais convencionais exigiria algoritmos de retrocesso (backtracking) ou paralelismo simulado, implicando custos proibitivos de processamento. Torna-se mandatório, portanto, converter o AFND em um Autômato Finito Determinístico (AFD) equivalente, no qual a leitura de cada caractere determine um estado de destino unívoco, assegurando tempo de reconhecimento estritamente linear em função do comprimento da cadeia de entrada (O(n)).",
        "No âmbito pedagógico e técnico do CCR de Linguagens Formais e Autômatos da UFFS, a especificação curricular estabelece três diretrizes estruturais de projeto:\n"
        "• Minimização Estrutural Específica: O autômato gerado deve ser mínimo estritamente sob o critério de ausência de estados inalcançáveis e mortos, vetando expressamente o emprego de particionamento por classes de equivalência entre estados indistinguíveis;\n"
        "• Função de Transição Total: O autômato deve ser completo, de modo que qualquer combinação de estado e símbolo não prevista pela linguagem seja direcionada para um estado de erro (sumidouro ou sink);\n"
        "• Ordem Rígida de Execução: O estado de erro deve ser anexado ao final do processo de minimização, evitando distorções conceituais, uma vez que o estado de erro, por definição formal, é incapaz de alcançar qualquer estado final de aceitação.",
        "Este trabalho tem como objetivo descrever a formalização teórica, a metodologia linear e a implementação desenvolvida em C++17, demonstrando passo a passo as etapas de transformação e a validação experimental completa da ferramenta sobre os casos de teste especificados."
    ]),
    ("2. Fundamentação Teórica e Modelo Formal", [
        "2.1 Gramáticas Regulares e Sentenças Literais: Uma Gramática Regular à direita (tipo 3 na Hierarquia de Chomsky) é formalmente uma quádrupla G = (V_N, V_T, P, S), onde V_N representa o conjunto finito de variáveis não-terminais, V_T o conjunto finito de símbolos terminais, S o símbolo inicial e P as produções restritas às formas A -> aB, A -> a ou A -> ε. Tokens literais correspondem a palavras finitas tratadas isomorphicamente como cadeias lineares estritas de derivação.",
        "2.2 Autômato Finito Não-Determinístico (AFND): O AFND gerado na primeira etapa é definido pela 5-upla M = (Q, Σ, δ, S, F), onde Q é o conjunto de estados, Σ o alfabeto, δ a função de transição com múltiplos destinos, S o estado inicial compartilhado e F o conjunto de estados aceitadores.",
        "2.3 Teorema da Determinização por Construção de Subconjuntos: Todo AFND possui um AFD equivalente onde cada estado D_i representa um subconjunto de estados do AFND original. A transição determinística é dada por δ_AFD(D_i, a) = fecho-ε( ∪_{q ∈ D_i} δ(q, a) ), e um estado D_i é aceitador se contiver ao menos um estado aceitador do AFND.",
        "2.4 Minimização Estrutural sem Classes de Equivalência: A minimização consiste estritamente na remoção de duas anomalias estruturais:\n"
        "• Estados Inalcançáveis: Nós q ∈ Q_AFD inacessíveis a partir do estado inicial (detectados em tempo linear por BFS direta a partir de q_0);\n"
        "• Estados Mortos: Nós q ∈ Q_AFD incapazes de atingir qualquer estado final de aceitação (detectados em tempo linear por BFS reversa no grafo transposto G^T a partir dos estados finais F).",
        "2.5 Totalidade da Função de Transição via Estado de Erro: Para satisfazer a condição de função total δ: Q × Σ -> Q, anexa-se um nó terminal sumidouro q_erro dotado de auto-laços reflexivos para todo o alfabeto. Todas as células indefinidas da matriz de transição convergem para q_erro."
    ]),
    ("3. Metodologia e Pipeline Linear da Aplicação", [
        "A arquitetura do software organiza-se modularmente em C++17 (pastas include/ e src/), dividindo o processamento em um pipeline linear de cinco etapas sequenciais:",
        "3.1 Modelo de Dados Central (include/Automato.h e src/Automato.cpp): Encapsula estados, transições, alfabeto dinâmico e estado de erro. O identificador numérico de cada nó coincide com seu índice vetorial (acesso em O(1)). O método imprimirTabela() exibe a matriz de transições formatada com alinhamento tabular e legendas para estado inicial (->), final (*) e erro (E).",
        "3.2 Etapa 1: Carga e Construção do AFND (src/LeitorArquivo.cpp): Realiza o parsing simultâneo de tokens e gramáticas BNF. Unicidade estrita do estado inicial S; cada token recebe caminhos privativos com estados novos, gerando indeterminismo natural em S (ex: 'se' e 'senao' possuem transições distintas sob 's'). Para a GR, o não-terminal inicial vincula-se a S e alternativas vazias (ε) marcam o estado como aceitador ('variavel').",
        "3.3 Etapa 2: Determinização Algorítmica (src/Determinizar.cpp): Emprega construção de subconjuntos com std::set<int>, tabela de busca std::map<std::set<int>, int> e fila std::queue. Herda cumulativamente os nomes de tokens dos estados componentes.",
        "3.4 Etapa 3: Minimização Estrutural de Inúteis (src/Minimizar.cpp): Realiza a poda de estados inalcançáveis por busca em largura para frente e de estados mortos por busca em largura retrógrada no grafo com arestas invertidas. Os estados sobreviventes são renumerados de modo contíguo (M0, M1, ...).",
        "3.5 Etapa 4: Adição do Estado de Erro e Matriz Total (src/CompletarAutomato.cpp): Inserido obrigatoriamente após a minimização para evitar que o próprio estado de erro seja eliminado como morto. Preenche todas as células vazias da tabela e estabelece laços reflexivos completos.",
        "3.6 Etapa 5: Diagnóstico e Reconhecimento com Rastreio (src/Reconhecer.cpp e src/VerificarEstados.cpp): O motor de reconhecimento avalia sentenças caractere a caractere em O(n), imprimindo o caminho percorrido e os tokens associados. O módulo VerificarEstados audita a vivacidade e alcance da máquina final."
    ]),
    ("4. Estudo de Caso Experimental e Validação dos Resultados", [
        "A validação empírica teve como base o arquivo de teste fornecido pelo docente (exemplos/professor.txt), composto pelos tokens 'se', 'entao', 'senao' e a Gramática Regular de vogais (<S> ::= a<A>|e<A>|i<A>|o<A>|u<A> e <A> ::= a<A>|e<A>|i<A>|o<A>|u<A>|ε).",
        "4.1 Evolução Sequencial de Estados no Pipeline:\n"
        "• AFND Inicial (Etapa 1): 14 estados (S, A..M). Indeterminismo nos símbolos 's' (destinos A, H) e 'e' (destinos C, M).\n"
        "• AFD Determinado (Etapa 2): 12 estados determinizados (D0 a D11). Resolução de subconjuntos com herança de tokens (D1={M} -> variavel, D2={C,M} -> variavel, D5={B,I} -> se, D10={G} -> entao, D11={L} -> senao).\n"
        "• AFD Minimizado (Etapa 3): 12 estados mantidos (M0 a M11). Diagnosticou 0 inalcançáveis e 0 mortos, confirmando que o AFD gerado já era enxuto.\n"
        "• AFD Final Completo (Etapa 4): 13 estados (M0 a M11 acrescidos de Erro). Preenchimento exato de 72 transições ausentes direcionadas ao Erro e 8 auto-laços reflexivos.",
        "4.2 Validação de Sentenças no Reconhecedor:\n"
        "• 'se': M0 --s--> M3 --e--> M5 [ACEITA: se]\n"
        "• 'entao': M0 --e--> M2 --n--> M4 --t--> M6 --a--> M8 --o--> M10 [ACEITA: entao]\n"
        "• 'senao': M0 --s--> M3 --e--> M5 --n--> M7 --a--> M9 --o--> M11 [ACEITA: senao]\n"
        "• 'e': M0 --e--> M2 [ACEITA: variavel] (D2={C,M} herda a aceitação da vogal M)\n"
        "• 'aeiou': M0 --a--> M1 --e--> M1 --i--> M1 --o--> M1 --u--> M1 [ACEITA: variavel]\n"
        "• 'sen': M0 --s--> M3 --e--> M5 --n--> M7 [REJEITADA: estado M7 não-final]\n"
        "• 'st': M0 --s--> M3 --t--> Erro [REJEITADA: transição direcionada ao estado de erro]\n"
        "• 'xyz': [REJEITADA: símbolo 'x' não pertence ao alfabeto]",
        "4.3 Validação Estrutural com Inúteis (exemplos/exemplo3_inalcancavel_e_morto.txt):\n"
        "Submissão de regras anômalas com laço estéril <M> ::= m<M> e símbolo desconectado <Z> ::= z<Z>|ε. A Etapa 3 detectou e expurgou com exatidão o estado morto e a componente inalcançável, confirmando a robustez da poda de grafos."
    ]),
    ("5. Diretrizes para Arguição Oral e Decisões de Projeto", [
        "1. Reconhecimento da cadeia 'e' como variável: No AFND inicial, 'e' transita simultaneamente para o início de 'entao' (estado C) e para a gramática de vogais (estado aceitador M). A determinização gera o subconjunto D2 = {C, M}, que herda a condição final de M e valida 'e' como variável com correção formal.\n"
        "2. Precedência da Minimização sobre o Estado de Erro: O estado de erro é um sorvedouro que jamais alcança um estado final. Se inserido antes da minimização, a busca em largura reversa o classificaria e eliminaria sumariamente como estado morto.\n"
        "3. Minimização sem Classes de Equivalência: Ao contrário de técnicas que particionam estados em blocos indistinguíveis, o método implementado opera como poda topológica de grafo, expurgando unicamente componentes inalcançáveis e mortas.\n"
        "4. Eficiência Linear da Detecção de Mortos: Ao inverter as arestas (grafo transposto G^T) e disparar uma BFS única a partir dos estados finais, obtém-se o conjunto de estados vivos em tempo O(|V| + |E|)."
    ]),
    ("6. Conclusão", [
        "A aplicação computacional desenvolvida atendeu com fidelidade e rigor científico a todos os requisitos formulados para o CCR de Linguagens Formais e Autômatos da UFFS. O fluxo linear de dados assegurou total transparência em cada estágio de processamento, consolidando uma ferramenta computacional robusta para o estudo prático dos mecanismos de análise léxica em compiladores."
    ]),
    ("7. Referências Bibliográficas", [
        "[1] MENEZES, P. B. Linguagens Formais e Autômatos. 6. ed. Porto Alegre: Bookman, 2011.",
        "[2] AHO, A. V.; LAM, M. S.; SETHI, R.; ULLMAN, J. D. Compiladores: Princípios, Técnicas e Ferramentas. 2. ed. São Paulo: Pearson/Prentice Hall, 2008.",
        "[3] DIVERIO, T. A.; MENEZES, P. B. Teoria da Computação: Máquinas Universais e Computabilidade. 3. ed. Porto Alegre: Bookman, 2011.",
        "[4] DAVIS, M. D.; SIGAL, Ron; WEYUKER, E. J. Computability, Complexity, and Languages: Fundamentals of Theoretical Computer Science. 2. ed. Boston: Academic Press, Harcourt, Brace, 1994.",
        "[5] FURTADO, O. J. V. Apostila de Linguagens Formais e Compiladores. [Florianópolis]: UFSC, 2002. Disponível em: www.inf.ufsc.br/~olinto/apostila-lfc.doc.",
        "[6] HOPCROFT, J. E.; MOTWANI, R.; ULLMAN, J. D. Introdução à Teoria dos Autômatos, Linguagens e Computação. Rio de Janeiro: Campus, 2003."
    ])
]

# ---------------------------------------------------------------------------
# GERADOR WORD (.DOCX) - 100% PRETO E BRANCO
# ---------------------------------------------------------------------------
def gerar_docx(caminho_arquivo):
    doc = Document()

    for section in doc.sections:
        section.top_margin = Inches(1.0)
        section.bottom_margin = Inches(1.0)
        section.left_margin = Inches(1.0)
        section.right_margin = Inches(1.0)

    # Cabecalho Institucional (Preto)
    p_inst = doc.add_paragraph()
    p_inst.alignment = WD_ALIGN_PARAGRAPH.CENTER
    r_inst = p_inst.add_run(f"{INSTITUICAO}\n{CAMPUS}\n{DISCIPLINA}\nDocente: {PROFESSOR}\n")
    r_inst.font.name = "Arial"
    r_inst.font.size = Pt(10)
    r_inst.font.bold = True
    r_inst.font.color.rgb = RGBColor(0, 0, 0)

    # Titulo (Preto)
    p_tit = doc.add_paragraph()
    p_tit.alignment = WD_ALIGN_PARAGRAPH.CENTER
    r_tit = p_tit.add_run(TITULO)
    r_tit.font.name = "Arial"
    r_tit.font.size = Pt(15)
    r_tit.font.bold = True
    r_tit.font.color.rgb = RGBColor(0, 0, 0)

    # Subtitulo (Preto italico)
    p_sub = doc.add_paragraph()
    p_sub.alignment = WD_ALIGN_PARAGRAPH.CENTER
    r_sub = p_sub.add_run(SUBTITULO)
    r_sub.font.name = "Arial"
    r_sub.font.size = Pt(10.5)
    r_sub.font.italic = True
    r_sub.font.color.rgb = RGBColor(0, 0, 0)

    # Autores (Preto)
    p_aut = doc.add_paragraph()
    p_aut.alignment = WD_ALIGN_PARAGRAPH.CENTER
    r_aut = p_aut.add_run(f"{AUTORES}\n{EMAIL}\n{CIDADE_ANO}\n")
    r_aut.font.name = "Arial"
    r_aut.font.size = Pt(9.5)
    r_aut.font.bold = True
    r_aut.font.color.rgb = RGBColor(0, 0, 0)

    # Linha divisoria (Preto)
    p_div = doc.add_paragraph()
    p_div.alignment = WD_ALIGN_PARAGRAPH.CENTER
    r_div = p_div.add_run("―" * 55)
    r_div.font.color.rgb = RGBColor(0, 0, 0)

    # Resumo
    p_res = doc.add_paragraph()
    p_res.alignment = WD_ALIGN_PARAGRAPH.JUSTIFY
    r_res_tit = p_res.add_run("Resumo — ")
    r_res_tit.font.name = "Arial"
    r_res_tit.font.size = Pt(9.5)
    r_res_tit.font.bold = True
    r_res_tit.font.color.rgb = RGBColor(0, 0, 0)
    r_res_corpo = p_res.add_run(RESUMO_TEXTO)
    r_res_corpo.font.name = "Arial"
    r_res_corpo.font.size = Pt(9)
    r_res_corpo.font.color.rgb = RGBColor(0, 0, 0)

    p_kw = doc.add_paragraph()
    p_kw.alignment = WD_ALIGN_PARAGRAPH.JUSTIFY
    r_kw_tit = p_kw.add_run("Palavras-chave: ")
    r_kw_tit.font.name = "Arial"
    r_kw_tit.font.size = Pt(9)
    r_kw_tit.font.bold = True
    r_kw_tit.font.color.rgb = RGBColor(0, 0, 0)
    r_kw_corpo = p_kw.add_run(PALAVRAS_CHAVE)
    r_kw_corpo.font.name = "Arial"
    r_kw_corpo.font.size = Pt(9)
    r_kw_corpo.font.color.rgb = RGBColor(0, 0, 0)

    # Abstract
    p_abs = doc.add_paragraph()
    p_abs.alignment = WD_ALIGN_PARAGRAPH.JUSTIFY
    r_abs_tit = p_abs.add_run("Abstract — ")
    r_abs_tit.font.name = "Arial"
    r_abs_tit.font.size = Pt(9.5)
    r_abs_tit.font.bold = True
    r_abs_tit.font.color.rgb = RGBColor(0, 0, 0)
    r_abs_corpo = p_abs.add_run(ABSTRACT_TEXTO)
    r_abs_corpo.font.name = "Arial"
    r_abs_corpo.font.size = Pt(9)
    r_abs_corpo.font.color.rgb = RGBColor(0, 0, 0)

    p_akw = doc.add_paragraph()
    p_akw.alignment = WD_ALIGN_PARAGRAPH.JUSTIFY
    r_akw_tit = p_akw.add_run("Keywords: ")
    r_akw_tit.font.name = "Arial"
    r_akw_tit.font.size = Pt(9)
    r_akw_tit.font.bold = True
    r_akw_tit.font.color.rgb = RGBColor(0, 0, 0)
    r_akw_corpo = p_akw.add_run(KEYWORDS)
    r_akw_corpo.font.name = "Arial"
    r_akw_corpo.font.size = Pt(9)
    r_akw_corpo.font.color.rgb = RGBColor(0, 0, 0)

    doc.add_paragraph()

    # Secoes (Fonte Preta)
    for titulo_sec, paragrafos in SECOES:
        p_sec = doc.add_paragraph()
        p_sec.paragraph_format.space_before = Pt(14)
        p_sec.paragraph_format.space_after = Pt(4)
        p_sec.paragraph_format.keep_with_next = True
        r_sec = p_sec.add_run(titulo_sec)
        r_sec.font.name = "Arial"
        r_sec.font.size = Pt(12)
        r_sec.font.bold = True
        r_sec.font.color.rgb = RGBColor(0, 0, 0)

        for p_texto in paragrafos:
            p_corpo = doc.add_paragraph()
            p_corpo.paragraph_format.space_after = Pt(6)
            p_corpo.paragraph_format.line_spacing = 1.15
            p_corpo.alignment = WD_ALIGN_PARAGRAPH.JUSTIFY

            partes = p_texto.split(":")
            if len(partes) > 1 and len(partes[0]) < 65 and not p_texto.startswith("["):
                r_tit_item = p_corpo.add_run(partes[0] + ":")
                r_tit_item.font.name = "Arial"
                r_tit_item.font.size = Pt(10)
                r_tit_item.font.bold = True
                r_tit_item.font.color.rgb = RGBColor(0, 0, 0)
                
                r_resto = p_corpo.add_run("".join(partes[1:]))
                r_resto.font.name = "Arial"
                r_resto.font.size = Pt(10)
                r_resto.font.color.rgb = RGBColor(0, 0, 0)
            else:
                r_p = p_corpo.add_run(p_texto)
                r_p.font.name = "Arial"
                r_p.font.size = Pt(10)
                r_p.font.color.rgb = RGBColor(0, 0, 0)

    doc.save(caminho_arquivo)
    print(f"[DOCX] Documento gerado com sucesso em P&B: {caminho_arquivo}")

# ---------------------------------------------------------------------------
# GERADOR PDF (REPORTLAB) - 100% PRETO E BRANCO
# ---------------------------------------------------------------------------
def limpar_para_reportlab(txt):
    def rep(m):
        conteudo = m.group(0)
        if conteudo.lower() in ['<b>', '</b>', '<i>', '</i>', '<br/>', '<br>']:
            return conteudo
        return conteudo.replace('<', '&lt;').replace('>', '&gt;')
    txt = re.sub(r'<[^>]+>', rep, txt)
    return txt.replace("\n", "<br/>")

class NumberedCanvasPB(canvas.Canvas):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._saved_page_states = []

    def showPage(self):
        self._saved_page_states.append(dict(self.__dict__))
        self._startPage()

    def save(self):
        num_pages = len(self._saved_page_states)
        for state in self._saved_page_states:
            self.__dict__.update(state)
            self.draw_page_number(num_pages)
            super().showPage()
        super().save()

    def draw_page_number(self, page_count):
        self.saveState()
        self.setFont("Helvetica", 8.5)
        self.setFillColor(colors.black)
        
        # Cabecalho superior em preto
        if self._pageNumber > 1:
            self.drawString(2 * cm, 28.2 * cm, "UFFS - Ciência da Computação | LFA - Prof. Braulio Mello | Geração Linear de AFD")
            self.setStrokeColor(colors.black)
            self.setLineWidth(0.5)
            self.line(2 * cm, 28.0 * cm, 19 * cm, 28.0 * cm)

        # Rodape em preto
        texto_rodape = f"Página {self._pageNumber} de {page_count}"
        self.drawRightString(19 * cm, 1.2 * cm, texto_rodape)
        self.drawString(2 * cm, 1.2 * cm, "Relatório Técnico-Científico - Linguagens Formais e Autômatos")
        self.setStrokeColor(colors.black)
        self.setLineWidth(0.5)
        self.line(2 * cm, 1.5 * cm, 19 * cm, 1.5 * cm)
        self.restoreState()

def gerar_pdf(caminho_arquivo):
    doc = SimpleDocTemplate(
        caminho_arquivo,
        pagesize=A4,
        leftMargin=2 * cm,
        rightMargin=2 * cm,
        topMargin=2.2 * cm,
        bottomMargin=2.2 * cm
    )

    styles = getSampleStyleSheet()

    estilo_inst = ParagraphStyle(
        'Instituicao',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=9,
        leading=12,
        alignment=1,
        textColor=colors.black,
        spaceAfter=10
    )

    estilo_titulo = ParagraphStyle(
        'TituloArtigo',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=13.5,
        leading=17.5,
        alignment=1,
        textColor=colors.black,
        spaceAfter=6
    )

    estilo_subtitulo = ParagraphStyle(
        'SubtituloArtigo',
        parent=styles['Normal'],
        fontName='Helvetica-Oblique',
        fontSize=9.5,
        leading=13,
        alignment=1,
        textColor=colors.black,
        spaceAfter=10
    )

    estilo_autores = ParagraphStyle(
        'AutoresArtigo',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=9,
        leading=12.5,
        alignment=1,
        textColor=colors.black,
        spaceAfter=12
    )

    estilo_secao = ParagraphStyle(
        'Secao',
        parent=styles['Heading1'],
        fontName='Helvetica-Bold',
        fontSize=11.5,
        leading=15,
        textColor=colors.black,
        spaceBefore=14,
        spaceAfter=6,
        keepWithNext=True
    )

    estilo_corpo = ParagraphStyle(
        'Corpo',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=9,
        leading=13,
        alignment=4,
        textColor=colors.black,
        spaceAfter=5
    )

    estilo_resumo = ParagraphStyle(
        'Resumo',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=8.5,
        leading=11.5,
        alignment=4,
        textColor=colors.black,
        spaceAfter=5
    )

    story = []

    # Cabecalho
    story.append(Paragraph(f"{INSTITUICAO}<br/>{CAMPUS}<br/>{DISCIPLINA} — {PROFESSOR}", estilo_inst))
    story.append(Paragraph(TITULO, estilo_titulo))
    story.append(Paragraph(SUBTITULO, estilo_subtitulo))
    story.append(Paragraph(f"<b>{AUTORES}</b><br/>{EMAIL}<br/>{CIDADE_ANO}", estilo_autores))

    # Resumo e Abstract no padrao ABNT continuo (sem borda/quadro que desformata)
    story.append(Spacer(1, 4))
    story.append(Paragraph(f"<b>Resumo —</b> {RESUMO_TEXTO}", estilo_resumo))
    story.append(Paragraph(f"<b>Palavras-chave:</b> {PALAVRAS_CHAVE}", estilo_resumo))
    story.append(Spacer(1, 4))
    story.append(Paragraph(f"<b>Abstract —</b> {ABSTRACT_TEXTO}", estilo_resumo))
    story.append(Paragraph(f"<b>Keywords:</b> {KEYWORDS}", estilo_resumo))
    story.append(Spacer(1, 8))

    # Secoes em Preto e Branco
    for titulo_s, paras in SECOES:
        story.append(Paragraph(titulo_s, estilo_secao))
        for p_txt in paras:
            p_formatado = limpar_para_reportlab(p_txt)
            story.append(Paragraph(p_formatado, estilo_corpo))
        story.append(Spacer(1, 4))

    doc.build(story, canvasmaker=NumberedCanvasPB)
    print(f"[PDF] Documento gerado com sucesso em P&B: {caminho_arquivo}")

if __name__ == "__main__":
    os.makedirs("relatorio", exist_ok=True)
    caminho_docx = os.path.join("relatorio", "Relatorio_LFA_AFD.docx")
    caminho_pdf = os.path.join("relatorio", "Relatorio_LFA_AFD.pdf")

    gerar_docx(caminho_docx)
    gerar_pdf(caminho_pdf)
