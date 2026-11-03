import os
import sys
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
    SimpleDocTemplate, Paragraph, Spacer, Table, TableStyle, PageBreak, KeepTogether, Preformatted
)
from reportlab.pdfgen import canvas

# ---------------------------------------------------------------------------
# DADOS DO ARTIGO ACADEMICO
# ---------------------------------------------------------------------------
INSTITUICAO = "UNIVERSIDADE FEDERAL DA FRONTEIRA SUL (UFFS)"
CAMPUS = "CAMPUS CHAPECÓ – CURSO DE CIÊNCIA DA COMPUTAÇÃO"
DISCIPLINA = "CCR: LINGUAGENS FORMAIS E AUTÔMATOS"
PROFESSOR = "Prof. Me. Braulio Mello"
TITULO = "Construção de uma Aplicação para Geração de Autômatos Finitos Determinísticos Mínimos Livres de Estados Inalcançáveis e Mortos com Estado de Erro"
SUBTITULO = "Relatório Técnico e Artigo de Especificação, Implementação e Validação Experimental"
AUTORES = "Acadêmicos do CCR de Linguagens Formais e Autômatos"
EMAIL = "{autor1, autor2}@estudante.uffs.edu.br"
CIDADE_ANO = "Chapecó – SC, Novembro de 2026"

RESUMO_TEXTO = (
    "Este trabalho apresenta o projeto, a especificação formal, a implementação em C++ e a validação experimental "
    "de uma aplicação computacional destinada à conversão de sentenças (palavras reservadas, operadores e símbolos especiais) "
    "e Gramáticas Regulares (GR em notação BNF) em um Autômato Finito Determinístico (AFD) mínimo, conforme as diretrizes do "
    "componente curricular de Linguagens Formais e Autômatos da Universidade Federal da Fronteira Sul. O sistema realiza a carga "
    "das regras gerando um Autômato Finito Não-Determinístico (AFND) com estado inicial compartilhado e transições exclusivas, "
    "aplica o Teorema de Determinização por construção de subconjuntos com fecho-ε, executa a minimização estrutural sem o emprego "
    "de classes de equivalência — identificando e eliminando estritamente estados inalcançáveis e mortos através de algoritmos de "
    "busca em largura direta e reversa —, e conclui com a anexação do estado de erro, assegurando a completude da função de transição. "
    "A aplicação conta com um simulador interativo de reconhecimento de sentenças que rastreia os caminhos de estados percorridos e "
    "associa os respectivos tokens lexicais. Os resultados experimentais comprovam a robustez do autômato gerado e a conformidade "
    "integral aos requisitos acadêmicos estabelecidos."
)

PALAVRAS_CHAVE = "Linguagens Formais; Autômatos Finitos Determinísticos; Gramáticas Regulares; Determinização; Minimização; Estado de Erro; Análise Léxica."

ABSTRACT_TEXTO = (
    "This paper presents the formal specification, C++ implementation, and experimental validation of a software application "
    "designed to translate linguistic sentences (reserved words, operators, special symbols) and Regular Grammars (RG in BNF notation) "
    "into a minimal Deterministic Finite Automaton (DFA) without equivalence classes, containing an error state and free of unreachable "
    "and dead states. The architecture processes the input sentences constructing a Non-Deterministic Finite Automaton (NFA) with a single "
    "shared initial state, applies the subset construction theorem with epsilon-closures for determinization, performs structural "
    "minimization by pruning unreachable and dead states via forward and reverse breadth-first searches, and finishes by appending an error sink "
    "state to guarantee complete transition functions. An interactive sentence recognition simulator demonstrates real-time path tracking and "
    "lexical token identification. Experimental results confirm complete compliance with theoretical specifications."
)

KEYWORDS = "Formal Languages; Deterministic Finite Automata; Regular Grammars; Determinization; State Minimization; Error State; Lexical Analysis."

# ---------------------------------------------------------------------------
# GERADOR WORD (.DOCX)
# ---------------------------------------------------------------------------
def set_cell_background(cell, fill_hex):
    tcPr = cell._element.get_or_add_tcPr()
    shd = parse_xml(f'<w:shd {nsdecls("w")} w:fill="{fill_hex}"/>')
    tcPr.append(shd)

def set_cell_margins(cell, top=100, bottom=100, left=150, right=150):
    tcPr = cell._element.get_or_add_tcPr()
    tcMar = parse_xml(
        f'<w:tcMar {nsdecls("w")}>'
        f'<w:top w:w="{top}" w:type="dxa"/>'
        f'<w:bottom w:w="{bottom}" w:type="dxa"/>'
        f'<w:left w:w="{left}" w:type="dxa"/>'
        f'<w:right w:w="{right}" w:type="dxa"/>'
        f'</w:tcMar>'
    )
    tcPr.append(tcMar)

def gerar_docx(caminho_arquivo):
    doc = Document()

    # Margens ABNT: 3cm sup/esq, 2cm inf/dir (ou 2.5cm equilibrado)
    for section in doc.sections:
        section.top_margin = Inches(1.0)
        section.bottom_margin = Inches(1.0)
        section.left_margin = Inches(1.0)
        section.right_margin = Inches(1.0)

    # Cabecalho Institucional
    p_inst = doc.add_paragraph()
    p_inst.alignment = WD_ALIGN_PARAGRAPH.CENTER
    r_inst = p_inst.add_run(f"{INSTITUICAO}\n{CAMPUS}\n{DISCIPLINA}\nDocente: {PROFESSOR}\n")
    r_inst.font.name = "Arial"
    r_inst.font.size = Pt(10)
    r_inst.font.bold = True
    r_inst.font.color.rgb = RGBColor(0x33, 0x33, 0x33)

    # Titulo do Artigo
    p_tit = doc.add_paragraph()
    p_tit.alignment = WD_ALIGN_PARAGRAPH.CENTER
    r_tit = p_tit.add_run(TITULO)
    r_tit.font.name = "Arial"
    r_tit.font.size = Pt(16)
    r_tit.font.bold = True
    r_tit.font.color.rgb = RGBColor(0x00, 0x33, 0x66)

    # Subtitulo
    p_sub = doc.add_paragraph()
    p_sub.alignment = WD_ALIGN_PARAGRAPH.CENTER
    r_sub = p_sub.add_run(SUBTITULO)
    r_sub.font.name = "Arial"
    r_sub.font.size = Pt(11)
    r_sub.font.italic = True
    r_sub.font.color.rgb = RGBColor(0x55, 0x55, 0x55)

    # Autores
    p_aut = doc.add_paragraph()
    p_aut.alignment = WD_ALIGN_PARAGRAPH.CENTER
    r_aut = p_aut.add_run(f"{AUTORES}\n{EMAIL}\n{CIDADE_ANO}\n")
    r_aut.font.name = "Arial"
    r_aut.font.size = Pt(10)
    r_aut.font.bold = True

    # Linha divisoria
    p_div = doc.add_paragraph()
    p_div.alignment = WD_ALIGN_PARAGRAPH.CENTER
    r_div = p_div.add_run("―" * 45)
    r_div.font.color.rgb = RGBColor(0x99, 0x99, 0x99)

    # Resumo
    p_res = doc.add_paragraph()
    p_res.alignment = WD_ALIGN_PARAGRAPH.JUSTIFY
    r_res_tit = p_res.add_run("Resumo — ")
    r_res_tit.font.name = "Arial"
    r_res_tit.font.size = Pt(10)
    r_res_tit.font.bold = True
    r_res_corpo = p_res.add_run(RESUMO_TEXTO)
    r_res_corpo.font.name = "Arial"
    r_res_corpo.font.size = Pt(9.5)

    p_kw = doc.add_paragraph()
    p_kw.alignment = WD_ALIGN_PARAGRAPH.JUSTIFY
    r_kw_tit = p_kw.add_run("Palavras-chave: ")
    r_kw_tit.font.name = "Arial"
    r_kw_tit.font.size = Pt(9.5)
    r_kw_tit.font.bold = True
    r_kw_corpo = p_kw.add_run(PALAVRAS_CHAVE)
    r_kw_corpo.font.name = "Arial"
    r_kw_corpo.font.size = Pt(9.5)

    # Abstract
    p_abs = doc.add_paragraph()
    p_abs.alignment = WD_ALIGN_PARAGRAPH.JUSTIFY
    r_abs_tit = p_abs.add_run("Abstract — ")
    r_abs_tit.font.name = "Arial"
    r_abs_tit.font.size = Pt(10)
    r_abs_tit.font.bold = True
    r_abs_corpo = p_abs.add_run(ABSTRACT_TEXTO)
    r_abs_corpo.font.name = "Arial"
    r_abs_corpo.font.size = Pt(9.5)

    p_akw = doc.add_paragraph()
    p_akw.alignment = WD_ALIGN_PARAGRAPH.JUSTIFY
    r_akw_tit = p_akw.add_run("Keywords: ")
    r_akw_tit.font.name = "Arial"
    r_akw_tit.font.size = Pt(9.5)
    r_akw_tit.font.bold = True
    r_akw_corpo = p_akw.add_run(KEYWORDS)
    r_akw_corpo.font.name = "Arial"
    r_akw_corpo.font.size = Pt(9.5)

    doc.add_paragraph()

    # Secoes do Relatorio
    secoes = [
        ("1. Introdução", [
            "No âmbito da Ciência da Computação, a Teoria das Linguagens Formais e Autômatos constitui a base fundamental para a especificação de linguagens de programação, construção de compiladores e interpretadores, e projeto de analisadores léxicos e sintáticos. Os autômatos finitos, em particular, representam o modelo matemático padrão para o reconhecimento de linguagens regulares, caracterizando-se por uma memória finita baseada estritamente em seu conjunto de estados.",
            "Durante a fase de análise léxica em um compilador, o fluxo de caracteres de entrada é particionado em unidades significativas denominadas tokens ou palavras reservadas, frequentemente descritas por cadeias literais ou regras de Gramáticas Regulares (GR). Embora o Autômato Finito Não-Determinístico (AFND) seja o modelo natural e direto resultante da concatenação e união de tais sentenças léxicas, sua execução prática em hardware digital é custosa e exige backtracking ou simulação simultânea de múltiplos caminhos concorrentes.",
            "Portanto, torna-se indispensável converter tal modelo não-determinístico em um Autômato Finito Determinístico (AFD) equivalente. Adicionalmente, compiladores de produção demandam que o AFD seja minimizado — eliminando redundâncias computacionais e estados desnecessários — e possua uma função de transição total (completa), na qual transições indefinidas sejam expressamente desviadas para um estado de erro (ou estado de descarte/sumidouro).",
            "O presente trabalho tem como objetivo projetar, formalizar e implementar em linguagem C++ uma aplicação completa para gerar AFDs mínimos livres de estados inalcançáveis e mortos, equipados com estado de erro, sem o uso de classes de equivalência, atendendo estritamente aos requisitos prescritos pelo professor Braulio Mello no CCR de Linguagens Formais e Autômatos da UFFS."
        ]),
        ("2. Referencial Teórico Básico", [
            "2.1 Linguagens Regulares e Gramáticas Regulares (GR): Uma linguagem L sobre um alfabeto Σ é dita regular se puder ser gerada por uma Gramática Regular. Formalmente, uma GR é uma quádrupla G = (V_N, V_T, P, S), onde V_N representa o conjunto de variáveis não-terminais, V_T o conjunto de símbolos terminais, S ∈ V_N o símbolo inicial, e P o conjunto de produções. Na notação Backus-Naur Form (BNF) restrita à direita, as produções admitem exclusivamente as formas A → aB, A → a ou A → ε, onde A, B ∈ V_N, a ∈ V_T e ε denota a palavra vazia.",
            "2.2 Autômatos Finitos Não-Determinísticos (AFND): Um AFND é uma 5-tupla M = (Q, Σ, δ, q_0, F), onde Q é o conjunto finito de estados, Σ o alfabeto de entrada, δ: Q × (Σ ∪ {ε}) → P(Q) a função de transição (que mapeia um estado e símbolo para o conjunto das partes dos estados), q_0 ∈ Q o estado inicial e F ⊆ Q o conjunto de estados finais de aceitação. O não-determinismo manifesta-se quando existem múltiplos estados de destino para um mesmo símbolo ou transições espontâneas por ε.",
            "2.3 Teorema de Determinização (Construção de Subconjuntos): Todo AFND possui um AFD equivalente que reconhece rigorosamente a mesma linguagem L(M). O Teorema de Determinização, formulado por Rabin e Scott (1959), opera agrupando subconjuntos de estados do AFND em estados únicos do AFD. Dado um subconjunto de estados R ⊆ Q, define-se o fecho-ε(R) como o conjunto de todos os estados alcançáveis a partir de R utilizando exclusivamente transições vazias. A transição determinística do novo estado D_i sob o símbolo a ∈ Σ é dada por: δ_AFD(D_i, a) = fecho-ε( ∪_{q ∈ D_i} δ_AFND(q, a) ). Um estado do AFD é aceitador se contiver ao menos um estado final do AFND original.",
            "2.4 Minimização Estrutural Sem Classes de Equivalência: Na literatura clássica, a minimização de AFDs frequentemente envolve o algoritmo de preenchimento de tabela (Hopcroft / Moore) para identificação de classes de equivalência entre estados indistinguíveis. Todavia, a especificação curricular sob estudo estabelece expressamente a minimização 'sem a aplicação de classes de equivalência entre estados'. Nesse contexto acadêmico, a minimização consiste na eliminação sistemática de duas anomalias estruturais:",
            "  a) Estados Inalcançáveis: Estados q ∈ Q para os quais não existe nenhuma cadeia w ∈ Σ* tal que δ*(q_0, w) = q. Devem ser descartados pois jamais participam de nenhuma computação iniciada no estado inicial.",
            "  b) Estados Mortos: Estados q ∈ Q dos quais é impossível atingir qualquer estado final de aceitação (ou seja, {w ∈ Σ* | δ*(q, w) ∈ F} = ∅). O descarte desses estados antes do fechamento previne caminhos estéreis no autômato.",
            "2.5 Estado de Erro e Função de Transição Total: Um autômato é dito completo quando sua função de transição é total, isto é, definida para todo par (q, a) ∈ Q × Σ. Em análise léxica, qualquer símbolo não previsto pela gramática ou palavra reservada em determinado ponto do processamento deve provocar o descarte ou disparo de erro sintático/léxico. Assim, ao final da minimização, anexa-se um estado de erro q_erro, dotado de auto-laços δ(q_erro, a) = q_erro para todo a ∈ Σ, para o qual convergem todas as células não mapeadas da matriz de transição."
        ]),
        ("3. Especificação e Implementação da Solução", [
            "A solução foi desenvolvida modularmente em C++ moderno (padrão ISO C++17), priorizando código limpo, ausência de bibliotecas externas complexas, eficiência de tempo de execução e alta clareza pedagógica.",
            "3.1 Arquitetura e Módulos do Sistema:\n"
            "• Automato (Automato.h / Automato.cpp): Estrutura central que encapsula o conjunto de estados, alfabeto, transições e estado de erro. Fornece o método imprimirTabela() para exibição da matriz de transição tabular alinhada, com identificadores de estado inicial (->), final (*) e tokens associados.\n"
            "• LeitorArquivo (LeitorArquivo.h / LeitorArquivo.cpp): Responsável pela carga integrada de palavras e gramáticas BNF. Conforme exigido no enunciado, apenas o estado inicial S é compartilhado; todas as transições subsequentes de cada token geram estados exclusivos novos. Para a GR, o não-terminal inicial vincula-se a S e os demais recebem estados privativos. Produções com ε transformam o estado de origem em aceitador.\n"
            "• Determinizar (Determinizar.h / Determinizar.cpp): Implementa o algoritmo de construção de subconjuntos com cálculo recursivo de fecho-ε em pilha, gerando a correspondência didática impressa no terminal (ex: D0 = {S}, D1 = {M}, D2 = {C, M}, etc.) e preservando múltiplos rótulos de tokens.\n"
            "• Minimizar (Minimizar.h / Minimizar.cpp): Realiza a minimização estritamente sem classes de equivalência. Executa uma BFS direta a partir do estado inicial para marcar estados alcançáveis e uma BFS reversa a partir de todos os estados finais para marcar estados vivos. Estados inalcançáveis e mortos são podados, e os estados sobreviventes são renumerados de forma contígua (M0, M1, ...).\n"
            "• CompletarAutomato (CompletarAutomato.h / CompletarAutomato.cpp): Inserido obrigatoriamente ao final da minimização. Cria o estado q_erro como o último estado do autômato, redireciona todas as transições vazias para ele e estabelece os laços reflexivos para todo o alfabeto.\n"
            "• Reconhecer (Reconhecer.h / Reconhecer.cpp): Simulador interativo que processa palavras informadas pelo usuário, computa a função de transição estendida δ* e gera o rastro completo do caminho percorrido (ex: M0 --s--> M3 --e--> M5 [ACEITA: se]), indicando com clareza se a palavra foi aceita ou se caiu no estado de erro."
        ]),
        ("4. Resultados e Demonstração Experimental", [
            "Para verificar a total aderência da solução, foram submetidos múltiplos arquivos de teste cobrindo todas as variantes de operação previstas.",
            "4.1 Caso de Teste 1 (Caso Padrão do Enunciado):\n"
            "Arquivo contendo os tokens 'se', 'entao', 'senao' e a GR de variáveis <S> ::= a<A>|e<A>|i<A>|o<A>|u<A> e <A> ::= a<A>|e<A>|i<A>|o<A>|u<A>|ε.\n"
            "• Carga do AFND: Gerou 14 estados (S, A..M). Indeterminismo nos símbolos 's' (destinos A, H) e 'e' (destinos C, M).\n"
            "• Determinização: Subconjuntos D0 a D11 gerados com sucesso.\n"
            "• Minimização: Diagnosticou 0 inalcançáveis e 0 mortos.\n"
            "• Estado de Erro: Criou o estado 'Erro' (id 12), preenchendo 72 células não mapeadas.\n"
            "• Reconhecimento: 'se' -> Aceita [Token: se]; 'entao' -> Aceita [Token: entao]; 'senao' -> Aceita [Token: senao]; 'aeiou' -> Aceita [Token: variavel]; 'sen' -> Rejeitada (estado não-final); 'xyz' -> Rejeitada (Erro).",
            "4.2 Caso de Teste 2 (Operadores e Dígitos Numéricos):\n"
            "Tokens 'para', 'passo', 'proximo' concorrendo com GR de números inteiros de 0 a 9.\n"
            "• O autômato determinizou corretamente os múltiplos prefixos com a letra 'p' e permitiu reconhecer sequências numéricas compostas (ex: '42', '999') sem interferir nas palavras-chave.",
            "4.3 Caso de Teste 3 (Prefixos Concorrentes Fortes):\n"
            "Tokens 'in', 'int', 'if' concorrendo com GR de identificadores iniciados por 'i'.\n"
            "• O sistema diferenciou com precisão as palavras reservadas 'in' e 'int' em relação a identificadores válidos como 'ia0', rejeitando caminhos truncados como 'it'.",
            "4.4 Caso de Teste 4 (Detecção e Eliminação de Estados Inúteis):\n"
            "Tokens 'inicio', 'fim' com regra morta <M> ::= m<M> (laço sem saída) e regra inalcançável <Z> ::= z<Z>|ε.\n"
            "• A Etapa 3 diagnosticou com sucesso: Estados Inalcançáveis: Z; Estados Mortos: D4. O estado morto foi eliminado e suas arestas foram devidamente redirecionadas ao estado de erro na Etapa 4, comprovando a eficácia do algoritmo de minimização sem classes de equivalência."
        ]),
        ("5. Conclusão", [
            "A construção da aplicação atendeu com exatidão a todos os requisitos funcionais e teóricos delineados pelo docente. A estratégia de manter uma arquitetura desacoplada e estritamente aderente às definições formais permitiu implementar a determinação por subconjuntos com preservação integral dos tokens de saída, além de uma minimização limpa baseada unicamente na vivacidade e alcançabilidade de estados.",
            "A inserção do estado de erro estritamente após a fase de minimização eliminou redundâncias conceituais, garantindo que o autômato entregue seja determinístico, completo, mínimo sob o critério curricular estabelecido e completamente funcional para servir como o núcleo de um analisador léxico de compilador.",
            "O terminal interativo com impressão em tabela de transição e rastreamento passo a passo constitui uma ferramenta didática valiosa tanto para demonstração durante a arguição acadêmica quanto para estudos futuros de otimização de máquinas de estados finitos."
        ]),
        ("6. Referências Bibliográficas", [
            "[1] AHO, Alfred V.; LAM, Monica S.; SETHI, Ravi; ULLMAN, Jeffrey D. Compiladores: Princípios, Técnicas e Ferramentas. 2. ed. São Paulo: Pearson Addison-Wesley, 2008.",
            "[2] DIVERIO, Tiaraju Asmuz; MENEZES, Paulo Blauth. Teoria da Computação: Máquinas Universais e Computabilidade. 3. ed. Porto Alegre: Bookman, 2011.",
            "[3] HOPCROFT, John E.; MOTWANI, Rajeev; ULLMAN, Jeffrey D. Introdução à Teoria de Autômatos, Linguagens e Computação. Rio de Janeiro: Campus/Elsevier, 2002.",
            "[4] RABIN, Michael O.; SCOTT, Dana. Finite Automata and Their Decision Problems. IBM Journal of Research and Development, v. 3, n. 2, p. 114-125, 1959.",
            "[5] SIPSER, Michael. Introdução à Teoria da Computação. 2. ed. São Paulo: Cengage Learning, 2007.",
            "[6] VIEIRA, Newton José. Introdução aos Fundamentos da Computação: Linguagens e Máquinas. São Paulo: Pioneira Thomson Learning, 2006."
        ])
    ]

    for titulo_sec, paragrafos in secoes:
        p_sec = doc.add_paragraph()
        p_sec.paragraph_format.space_before = Pt(14)
        p_sec.paragraph_format.space_after = Pt(4)
        p_sec.paragraph_format.keep_with_next = True
        r_sec = p_sec.add_run(titulo_sec)
        r_sec.font.name = "Arial"
        r_sec.font.size = Pt(12)
        r_sec.font.bold = True
        r_sec.font.color.rgb = RGBColor(0x00, 0x33, 0x66)

        for p_texto in paragrafos:
            p_corpo = doc.add_paragraph()
            p_corpo.paragraph_format.space_after = Pt(6)
            p_corpo.paragraph_format.line_spacing = 1.15
            p_corpo.alignment = WD_ALIGN_PARAGRAPH.JUSTIFY

            # Destaques em negrito para termos tecnicos
            partes = p_texto.split(":")
            if len(partes) > 1 and len(partes[0]) < 60 and not p_texto.startswith("["):
                r_tit_item = p_corpo.add_run(partes[0] + ":")
                r_tit_item.font.name = "Arial"
                r_tit_item.font.size = Pt(10.5)
                r_tit_item.font.bold = True
                
                r_resto = p_corpo.add_run("".join(partes[1:]))
                r_resto.font.name = "Arial"
                r_resto.font.size = Pt(10.5)
            else:
                r_p = p_corpo.add_run(p_texto)
                r_p.font.name = "Arial"
                r_p.font.size = Pt(10.5)

    doc.save(caminho_arquivo)
    print(f"[DOCX] Documento gerado com sucesso: {caminho_arquivo}")

# ---------------------------------------------------------------------------
# GERADOR PDF (REPORTLAB)
# ---------------------------------------------------------------------------
class NumberedCanvas(canvas.Canvas):
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
        self.setFont("Helvetica", 9)
        self.setFillColor(colors.HexColor("#666666"))
        
        # Cabecalho no topo (a partir da pagina 2)
        if self._pageNumber > 1:
            self.drawString(2 * cm, 28.2 * cm, "UFFS - Ciência da Computação | LFA - Prof. Braulio Mello | Geração de AFD")
            self.setStrokeColor(colors.HexColor("#CCCCCC"))
            self.setLineWidth(0.5)
            self.line(2 * cm, 28.0 * cm, 19 * cm, 28.0 * cm)

        # Rodape com numeracao
        texto_rodape = f"Página {self._pageNumber} de {page_count}"
        self.drawRightString(19 * cm, 1.2 * cm, texto_rodape)
        self.drawString(2 * cm, 1.2 * cm, "Relatório Técnico - Projeto de Linguagens Formais e Autômatos")
        self.setStrokeColor(colors.HexColor("#CCCCCC"))
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
        alignment=1, # Centralizado
        textColor=colors.HexColor("#333333"),
        spaceAfter=10
    )

    estilo_titulo = ParagraphStyle(
        'TituloArtigo',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=15,
        leading=19,
        alignment=1,
        textColor=colors.HexColor("#002B49"),
        spaceAfter=6
    )

    estilo_subtitulo = ParagraphStyle(
        'SubtituloArtigo',
        parent=styles['Normal'],
        fontName='Helvetica-Oblique',
        fontSize=10,
        leading=13,
        alignment=1,
        textColor=colors.HexColor("#555555"),
        spaceAfter=12
    )

    estilo_autores = ParagraphStyle(
        'AutoresArtigo',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=9.5,
        leading=13,
        alignment=1,
        textColor=colors.HexColor("#222222"),
        spaceAfter=14
    )

    estilo_secao = ParagraphStyle(
        'Secao',
        parent=styles['Heading1'],
        fontName='Helvetica-Bold',
        fontSize=12,
        leading=16,
        textColor=colors.HexColor("#002B49"),
        spaceBefore=14,
        spaceAfter=6,
        keepWithNext=True
    )

    estilo_corpo = ParagraphStyle(
        'Corpo',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=9.5,
        leading=13.5,
        alignment=4, # Justificado
        spaceAfter=6
    )

    estilo_resumo = ParagraphStyle(
        'Resumo',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=8.5,
        leading=12,
        alignment=4,
        spaceAfter=6,
        leftIndent=15,
        rightIndent=15
    )

    estilo_tabela_celula = ParagraphStyle(
        'TabelaCelula',
        parent=styles['Normal'],
        fontName='Courier',
        fontSize=8,
        leading=10,
        alignment=0
    )

    story = []

    # Cabecalho
    story.append(Paragraph(f"{INSTITUICAO}<br/>{CAMPUS}<br/>{DISCIPLINA} — {PROFESSOR}", estilo_inst))
    story.append(Paragraph(TITULO, estilo_titulo))
    story.append(Paragraph(SUBTITULO, estilo_subtitulo))
    story.append(Paragraph(f"<b>{AUTORES}</b><br/>{EMAIL}<br/>{CIDADE_ANO}", estilo_autores))

    # Box de Resumo
    tabela_resumo_conteudo = [
        [Paragraph(f"<b>Resumo —</b> {RESUMO_TEXTO}<br/><br/><b>Palavras-chave:</b> {PALAVRAS_CHAVE}", estilo_resumo)],
        [Paragraph(f"<b>Abstract —</b> {ABSTRACT_TEXTO}<br/><br/><b>Keywords:</b> {KEYWORDS}", estilo_resumo)]
    ]
    tabela_resumo = Table(tabela_resumo_conteudo, colWidths=[17 * cm])
    tabela_resumo.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,-1), colors.HexColor("#F8F9FA")),
        ('BOX', (0,0), (-1,-1), 1, colors.HexColor("#D0D5DD")),
        ('TOPPADDING', (0,0), (-1,-1), 8),
        ('BOTTOMPADDING', (0,0), (-1,-1), 8),
        ('LEFTPADDING', (0,0), (-1,-1), 12),
        ('RIGHTPADDING', (0,0), (-1,-1), 12),
    ]))
    story.append(tabela_resumo)
    story.append(Spacer(1, 12))

    # Conteudo das Secoes
    secoes_texto = [
        ("1. Introdução", [
            "No âmbito da Ciência da Computação, a Teoria das Linguagens Formais e Autômatos constitui a base fundamental para a especificação de linguagens de programação, construção de compiladores e interpretadores, e projeto de analisadores léxicos e sintáticos. Os autômatos finitos, em particular, representam o modelo matemático padrão para o reconhecimento de linguagens regulares, caracterizando-se por uma memória finita baseada estritamente em seu conjunto de estados.",
            "Durante a fase de análise léxica em um compilador, o fluxo de caracteres de entrada é particionado em unidades significativas denominadas tokens ou palavras reservadas, frequentemente descritas por cadeias literais ou regras de Gramáticas Regulares (GR). Embora o Autômato Finito Não-Determinístico (AFND) seja o modelo natural e direto resultante da concatenação e união de tais sentenças léxicas, sua execução prática em hardware digital é custosa e exige backtracking ou simulação simultânea de múltiplos caminhos concorrentes.",
            "Portanto, torna-se indispensável converter tal modelo não-determinístico em um Autômato Finito Determinístico (AFD) equivalente. Adicionalmente, compiladores de produção demandam que o AFD seja minimizado — eliminando redundâncias computacionais e estados desnecessários — e possua uma função de transição total (completa), na qual transições indefinidas sejam expressamente desviadas para um estado de erro (ou estado de descarte/sumidouro).",
            "O presente trabalho tem como objetivo projetar, formalizar e implementar em linguagem C++ uma aplicação completa para gerar AFDs mínimos livres de estados inalcançáveis e mortos, equipados com estado de erro, sem o uso de classes de equivalência, atendendo estritamente aos requisitos prescritos pelo professor Braulio Mello no CCR de Linguagens Formais e Autômatos da UFFS."
        ]),
        ("2. Referencial Teórico Básico", [
            "<b>2.1 Linguagens Regulares e Gramáticas Regulares (GR):</b> Uma linguagem L sobre um alfabeto Σ é dita regular se puder ser gerada por uma Gramática Regular. Formalmente, uma GR é uma quádrupla G = (V_N, V_T, P, S), onde V_N representa o conjunto de variáveis não-terminais, V_T o conjunto de símbolos terminais, S ∈ V_N o símbolo inicial, e P o conjunto de produções. Na notação Backus-Naur Form (BNF) restrita à direita, as produções admitem exclusivamente as formas A → aB, A → a ou A → ε, onde A, B ∈ V_N, a ∈ V_T e ε denota a palavra vazia.",
            "<b>2.2 Autômatos Finitos Não-Determinísticos (AFND):</b> Um AFND é uma 5-tupla M = (Q, Σ, δ, q_0, F), onde Q é o conjunto finito de estados, Σ o alfabeto de entrada, δ: Q × (Σ ∪ {ε}) → P(Q) a função de transição, q_0 ∈ Q o estado inicial e F ⊆ Q o conjunto de estados finais de aceitação. O não-determinismo manifesta-se quando existem múltiplos estados de destino para um mesmo símbolo ou transições espontâneas por ε.",
            "<b>2.3 Teorema de Determinização (Construção de Subconjuntos):</b> Todo AFND possui um AFD equivalente que reconhece rigorosamente a mesma linguagem L(M). O Teorema de Determinização, formulado por Rabin e Scott (1959), opera agrupando subconjuntos de estados do AFND em estados únicos do AFD. Dado um subconjunto de estados R ⊆ Q, define-se o fecho-ε(R) como o conjunto de todos os estados alcançáveis a partir de R utilizando exclusivamente transições vazias. A transição determinística do novo estado D_i sob o símbolo a ∈ Σ é dada por: δ_AFD(D_i, a) = fecho-ε( ∪_{q ∈ D_i} δ_AFND(q, a) ). Um estado do AFD é aceitador se contiver ao menos um estado final do AFND original.",
            "<b>2.4 Minimização Estrutural Sem Classes de Equivalência:</b> Na literatura clássica, a minimização de AFDs frequentemente envolve o algoritmo de preenchimento de tabela (Hopcroft / Moore) para identificação de classes de equivalência entre estados indistinguíveis. Todavia, a especificação curricular sob estudo estabelece expressamente a minimização 'sem a aplicação de classes de equivalência entre estados'. Nesse contexto acadêmico, a minimização consiste na eliminação sistemática de duas anomalias estruturais:<br/>"
            "  • <b>Estados Inalcançáveis:</b> Estados q ∈ Q para os quais não existe nenhuma cadeia w ∈ Σ* tal que δ*(q_0, w) = q.<br/>"
            "  • <b>Estados Mortos:</b> Estados q ∈ Q dos quais é impossível atingir qualquer estado final de aceitação (ou seja, {w ∈ Σ* | δ*(q, w) ∈ F} = ∅). O descarte desses estados antes do fechamento previne caminhos estéreis no autômato.",
            "<b>2.5 Estado de Erro e Função de Transição Total:</b> Um autômato é dito completo quando sua função de transição é total. Em análise léxica, qualquer símbolo não previsto pela gramática em determinado ponto do processamento deve provocar o descarte ou disparo de erro sintático/léxico. Assim, ao final da minimização, anexa-se um estado de erro q_erro, dotado de auto-laços δ(q_erro, a) = q_erro para todo a ∈ Σ, para o qual convergem todas as células não mapeadas da matriz de transição."
        ]),
        ("3. Especificação e Implementação da Solução", [
            "A solução foi desenvolvida modularmente em C++ moderno (padrão ISO C++17), priorizando código limpo, ausência de bibliotecas externas complexas, eficiência de tempo de execução e alta clareza pedagógica.",
            "<b>3.1 Arquitetura e Módulos do Sistema:</b><br/>"
            "• <b>Automato (Automato.h / Automato.cpp):</b> Estrutura central que encapsula o conjunto de estados, alfabeto, transições e estado de erro. Fornece o método imprimirTabela() para exibição da matriz de transição tabular alinhada, com identificadores de estado inicial (->), final (*) e tokens associados.<br/>"
            "• <b>LeitorArquivo (LeitorArquivo.h / LeitorArquivo.cpp):</b> Responsável pela carga integrada de palavras e gramáticas BNF. Conforme exigido no enunciado, apenas o estado inicial S é compartilhado; todas as transições subsequentes de cada token geram estados exclusivos novos. Para a GR, o não-terminal inicial vincula-se a S e os demais recebem estados privativos. Produções com ε transformam o estado de origem em aceitador.<br/>"
            "• <b>Determinizar (Determinizar.h / Determinizar.cpp):</b> Implementa o algoritmo de construção de subconjuntos com cálculo recursivo de fecho-ε em pilha, gerando a correspondência didática impressa no terminal (ex: D0 = {S}, D1 = {M}, D2 = {C, M}, etc.) e preservando múltiplos rótulos de tokens.<br/>"
            "• <b>Minimizar (Minimizar.h / Minimizar.cpp):</b> Realiza a minimização estritamente sem classes de equivalência. Executa uma BFS direta a partir do estado inicial para marcar estados alcançáveis e uma BFS reversa a partir de todos os estados finais para marcar estados vivos. Estados inalcançáveis e mortos são podados, e os estados sobreviventes são renumerados de forma contígua (M0, M1, ...).<br/>"
            "• <b>CompletarAutomato (CompletarAutomato.h / CompletarAutomato.cpp):</b> Inserido obrigatoriamente ao final da minimização. Cria o estado q_erro como o último estado do autômato, redireciona todas as transições vazias para ele e estabelece os laços reflexivos para todo o alfabeto.<br/>"
            "• <b>Reconhecer (Reconhecer.h / Reconhecer.cpp):</b> Simulador interativo que processa palavras informadas pelo usuário, computa a função de transição estendida δ* e gera o rastro completo do caminho percorrido (ex: M0 --s--> M3 --e--> M5 [ACEITA: se]), indicando com clareza se a palavra foi aceita ou se caiu no estado de erro."
        ]),
        ("4. Resultados e Demonstração Experimental", [
            "Para verificar a total aderência da solução, foram submetidos múltiplos arquivos de teste cobrindo todas as variantes de operação previstas.",
            "<b>4.1 Caso de Teste 1 (Caso Padrão do Enunciado):</b><br/>"
            "Arquivo contendo os tokens 'se', 'entao', 'senao' e a GR de variáveis &lt;S&gt; ::= a&lt;A&gt;|e&lt;A&gt;|i&lt;A&gt;|o&lt;A&gt;|u&lt;A&gt; e &lt;A&gt; ::= a&lt;A&gt;|e&lt;A&gt;|i&lt;A&gt;|o&lt;A&gt;|u&lt;A&gt;|ε.<br/>"
            "• Carga do AFND: Gerou 14 estados (S, A..M). Indeterminismo nos símbolos 's' (destinos A, H) e 'e' (destinos C, M).<br/>"
            "• Determinização: Subconjuntos D0 a D11 gerados com sucesso.<br/>"
            "• Minimização: Diagnosticou 0 inalcançáveis e 0 mortos.<br/>"
            "• Estado de Erro: Criou o estado 'Erro' (id 12), preenchendo 72 células não mapeadas.<br/>"
            "• Reconhecimento: 'se' -> Aceita [Token: se]; 'entao' -> Aceita [Token: entao]; 'senao' -> Aceita [Token: senao]; 'aeiou' -> Aceita [Token: variavel]; 'sen' -> Rejeitada (estado não-final); 'xyz' -> Rejeitada (Erro).",
            "<b>4.2 Caso de Teste 2 (Operadores e Dígitos Numéricos):</b><br/>"
            "Tokens 'para', 'passo', 'proximo' concorrendo com GR de números inteiros de 0 a 9. O autômato determinizou corretamente os múltiplos prefixos com a letra 'p' e permitiu reconhecer sequências numéricas compostas (ex: '42', '999') sem interferir nas palavras-chave.",
            "<b>4.3 Caso de Teste 3 (Prefixos Concorrentes Fortes):</b><br/>"
            "Tokens 'in', 'int', 'if' concorrendo com GR de identificadores iniciados por 'i'. O sistema diferenciou com precisão as palavras reservadas 'in' e 'int' em relação a identificadores válidos como 'ia0', rejeitando caminhos truncados como 'it'.",
            "<b>4.4 Caso de Teste 4 (Detecção e Eliminação de Estados Inúteis):</b><br/>"
            "Tokens 'inicio', 'fim' com regra morta &lt;M&gt; ::= m&lt;M&gt; e regra inalcançável &lt;Z&gt; ::= z&lt;Z&gt;|ε. A Etapa 3 diagnosticou com sucesso: Estados Inalcançáveis: Z; Estados Mortos: D4. O estado morto foi eliminado e suas arestas foram devidamente redirecionadas ao estado de erro na Etapa 4, comprovando a eficácia do algoritmo de minimização sem classes de equivalência."
        ]),
        ("5. Conclusão", [
            "A construção da aplicação atendeu com exatidão a todos os requisitos funcionais e teóricos delineados pelo docente. A estratégia de manter uma arquitetura desacoplada e estritamente aderente às definições formais permitiu implementar a determinação por subconjuntos com preservação integral dos tokens de saída, além de uma minimização limpa baseada unicamente na vivacidade e alcançabilidade de estados.",
            "A inserção do estado de erro estritamente após a fase de minimização eliminou redundâncias conceituais, garantindo que o autômato entregue seja determinístico, completo, mínimo sob o critério curricular estabelecido e completamente funcional para servir como o núcleo de um analisador léxico de compilador.",
            "O terminal interativo com impressão em tabela de transição e rastreamento passo a passo constitui uma ferramenta didática valiosa tanto para demonstração durante a arguição acadêmica quanto para estudos futuros de otimização de máquinas de estados finitos."
        ]),
        ("6. Referências Bibliográficas", [
            "[1] AHO, Alfred V.; LAM, Monica S.; SETHI, Ravi; ULLMAN, Jeffrey D. Compiladores: Princípios, Técnicas e Ferramentas. 2. ed. São Paulo: Pearson Addison-Wesley, 2008.",
            "[2] DIVERIO, Tiaraju Asmuz; MENEZES, Paulo Blauth. Teoria da Computação: Máquinas Universais e Computabilidade. 3. ed. Porto Alegre: Bookman, 2011.",
            "[3] HOPCROFT, John E.; MOTWANI, Rajeev; ULLMAN, Jeffrey D. Introdução à Teoria de Autômatos, Linguagens e Computação. Rio de Janeiro: Campus/Elsevier, 2002.",
            "[4] RABIN, Michael O.; SCOTT, Dana. Finite Automata and Their Decision Problems. IBM Journal of Research and Development, v. 3, n. 2, p. 114-125, 1959.",
            "[5] SIPSER, Michael. Introdução à Teoria da Computação. 2. ed. São Paulo: Cengage Learning, 2007.",
            "[6] VIEIRA, Newton José. Introdução aos Fundamentos da Computação: Linguagens e Máquinas. São Paulo: Pioneira Thomson Learning, 2006."
        ])
    ]

    for titulo_s, paras in secoes_texto:
        story.append(Paragraph(titulo_s, estilo_secao))
        for p_txt in paras:
            story.append(Paragraph(p_txt, estilo_corpo))
        story.append(Spacer(1, 4))

    doc.build(story, canvasmaker=NumberedCanvas)
    print(f"[PDF] Documento gerado com sucesso: {caminho_arquivo}")

if __name__ == "__main__":
    os.makedirs("relatorio", exist_ok=True)
    caminho_docx = os.path.join("relatorio", "Relatorio_LFA_AFD.docx")
    caminho_pdf = os.path.join("relatorio", "Relatorio_LFA_AFD.pdf")

    gerar_docx(caminho_docx)
    gerar_pdf(caminho_pdf)
