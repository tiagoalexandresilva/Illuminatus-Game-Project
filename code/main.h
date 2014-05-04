#ifndef ___MAIN_H___
#define ___MAIN_H___

/**Estrutura de dados onde é guardada toda a informação sobre o tabuleiro. */
typedef struct sTabuleiro
{
  /**Variavel que guarda a coluna da célula.*/  
  int colunas;
  /**Variavel que guarda a linha da célula.*/
  int linhas;
  /**Array bidimensional onde são guardadas as linhas e colunas*/
  char ** tabuleiro;
  /**Array bidimensional onde são guardadas as identificações se são marcas iluminadas ou não-iluminadas*/
  int ** marcas;
  /**Variável que guarda uma certa identidade de um comando*/
  int n_comando;
  /**Apontador para a estrutura que guarda todas as jogadas efectuadas.*/
  struct sJogadas * jogadas;
} *Tabuleiro, tab;

int carrega_tab(Tabuleiro jogo, char * ficheiro);
char * prompt();
int comando(Tabuleiro jogo, char * cmd);
void trata_linha(Tabuleiro jogo, char * linha, int n);
void impressao_ecra(Tabuleiro jogo);
int grava_tab(Tabuleiro jogo, char * ficheiro);
int verifica_tab(Tabuleiro jogo);
int joga(Tabuleiro jogo, int coluna, int linha);
void ilumina_dir(Tabuleiro jogo, int pos_actual, int linha);
void ilumina_esq(Tabuleiro jogo, int pos_actual, int linha);
void ilumina_cima(Tabuleiro jogo, int pos_actual, int coluna);
void ilumina_baixo(Tabuleiro jogo, int pos_actual, int coluna);
int estrategia2(Tabuleiro jogo);
int inicia_tab(Tabuleiro jogo, int colunas, int linhas);
void destroi_tab(Tabuleiro jogo);
int marca(Tabuleiro jogo, int coluna, int linha);
int estrategia1(Tabuleiro jogo);
int posicoeslivres(Tabuleiro jogo, int coluna, int linha);
int poe_lamp(Tabuleiro jogo, int coluna, int linha);
int identifica_comando(char * comando);
int executa_comando(Tabuleiro jogo, int comando, int argumentos, char* arg1,char * arg2);
int testa_inteiro(char * a);
int estrategia3(Tabuleiro jogo);
int alteracoes(Tabuleiro jogo);
int marca_orto(Tabuleiro jogo, int coluna, int linha);
int conta_lamp(Tabuleiro jogo, int coluna, int linha);
void poe_ponto_orto(Tabuleiro jogo, int coluna, int linha);
void marca_diag(Tabuleiro jogo, int coluna, int linha);
void marca_lamp(Tabuleiro jogo, int coluna, int linha);
void marca_cantos(Tabuleiro jogo, int coluna, int linha);
int diagonal_se(Tabuleiro jogo, int coluna, int linha);
int diagonal_sd(Tabuleiro jogo, int coluna, int linha);
int diagonal_ie(Tabuleiro jogo, int coluna, int linha);
int diagonal_id(Tabuleiro jogo, int coluna, int linha);
int estrategia4(Tabuleiro jogo);
int verifica_diag_esq(Tabuleiro jogo, int coluna, int linha);
int verifica_diag_dir(Tabuleiro jogo, int coluna, int linha);
void trata0_esq(Tabuleiro jogo, int coluna, int linha);
void trata1_esq(Tabuleiro jogo, int coluna, int linha);
void trata2_esq(Tabuleiro jogo, int coluna, int linha);
void trata3_esq(Tabuleiro jogo, int coluna, int linha);
void trata4_esq(Tabuleiro jogo, int coluna, int linha);
void trata0_dir(Tabuleiro jogo, int coluna, int linha);
void trata1_dir(Tabuleiro jogo, int coluna, int linha);
void trata2_dir(Tabuleiro jogo, int coluna, int linha);
void trata3_dir(Tabuleiro jogo, int coluna, int linha);
void trata4_dir(Tabuleiro jogo, int coluna, int linha);
void caso_geral(Tabuleiro jogo, int coluna, int linha);
int estrategia5(Tabuleiro jogo);
void identifica_iluminados(Tabuleiro jogo, int coluna, int linha);
int livres_orto(Tabuleiro jogo, int coluna, int linha);
void poe_lamp_est5(Tabuleiro jogo, int coluna, int linha);
void poe_lamp_est5_aux(Tabuleiro jogo, int coluna, int linha);
int lamp_orto(Tabuleiro jogo, int coluna, int linha);
void trabalho_joga(Tabuleiro jogo, int coluna, int linha);
void trabalho_marcas(Tabuleiro jogo, int coluna, int linha);
void trabalho_tratas(Tabuleiro jogo, int coluna, int linha);
int resolve(Tabuleiro jogo);
int trabalho_rsv(Tabuleiro jogo);
int assembly_lampadas(Tabuleiro jogo);
int executa_estrategias(Tabuleiro jogo);


#endif
