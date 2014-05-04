#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <ctype.h>
#include "cores.h"
#include "erro.h"
#include "main.h"
#include "listas_ligadas.h"

#define valor(c) (c-'0')

/**Função principal, que executa o prompt, e a cada comando executa a acção correspondente.
*@return 1 O programa vai continuar a ser executado, enquanto não for retornado 99 ao r.
*/
int main()
{
  Tabuleiro jogo = (Tabuleiro)malloc(sizeof(tab));
  int r = 0;
  jogo -> tabuleiro = NULL;
  jogo -> colunas = 0;
  jogo -> linhas = 0;
  textcolor(RESET, BLACK, WHITE2);
  jogo->n_comando = 0;
  jogo->jogadas = NULL;
  do
    {
      r = comando(jogo, prompt());
    }
  while(r != 99);
  return 1;
}

/**Função auxiliar de carrega_tab, que inicia o tabuleiro.
*@param colunas Indica o número de colunas.
*@param linhas Indica o número de linhas.
*@param jogo Referencia-se a onde se encontra guardado as dimensões do tabuleiro.
*@return 1 Se correu tudo bem.
*@return 0 Se ocorreram problemas na alocação de memória.
*/
int inicia_tab(Tabuleiro jogo, int colunas, int linhas)
{
  	int i, coluna, linha;
  	jogo->marcas = (int **) malloc(colunas * sizeof(int *));
  	jogo->tabuleiro = (char **) malloc(colunas * sizeof(char *));
  	if(!jogo->tabuleiro || !jogo->marcas) 
    	return 0;
  	for(i = 0; i < colunas; i++)
   {
   	jogo->tabuleiro[i] = (char *) malloc(linhas * sizeof(char));
      jogo->marcas[i] = (int *) malloc(linhas * sizeof(int));
      if(!jogo->tabuleiro[i] || !jogo->marcas)
      	return 0;
   }
   /*Nesta fase, mete-se o tabuleiro das marcas inicializado todo a 1.*/
   for(linha = 0; linha < jogo->linhas; linha++)
  	{
  		for(coluna = 0; coluna < jogo->colunas; coluna++)
  		{
  			jogo->marcas[coluna][linha] = 1;
  		}
  	}
  	return 1;
} 

/**Função que limpa o tabuleiro e que liberta a respectiva memória.
*@param jogo Recebe o tabuleiro.
*/
void destroi_tab(Tabuleiro jogo)
{
  int i;
  Jogadas aux;
  while(jogo->jogadas) 
    {
      aux=jogo->jogadas->seg;
      free(jogo->jogadas);
      jogo->jogadas=aux;
    }
  for(i = 0; i < jogo->colunas; i++)
    {
      free(jogo->tabuleiro[i]);
      free(jogo->marcas[i]);
    }
  free(jogo->tabuleiro);
  free(jogo->marcas);
  jogo->colunas = 0;
  jogo->linhas = 0;
  jogo->n_comando = 0;
}

/**Função que carrega o tabuleiro do ficheiro para a memória.
*@param ficheiro Indica o nome do ficheiro a abrir.
*@param jogo Tabuleiro que vai receber.
*@return 1 Se correu tudo bem.
*/
int carrega_tab(Tabuleiro jogo, char * ficheiro)
{
	int n = 0, limpa = 0, col, lin, casas_pintadas=0, casas_pintadas2=0 , flag=1;
  	char linha[200], * limpa2 = NULL; /*200, para ter a certeza que cabe*/
  	FILE * file;
  	file = fopen(strcat(ficheiro, ".ill"), "r");
  	if(!file)
  	{
  		return mensagem_de_erro(E_NO_FILE);
   }
  	else
   { 
   	if(verifica_tab(jogo)){
			destroi_tab(jogo);
     	}      
      limpa = fscanf(file,"%d %d%*[\n]", &jogo->colunas, &jogo->linhas); limpa++; /*Onde tem [\n] significa que vai remover todos os quebra-linhas do tabuleiro*/
      if(!inicia_tab(jogo, jogo->colunas, jogo->linhas)){
      	destroi_tab(jogo);
			return mensagem_de_erro(E_MEM);
      }
      while(!feof(file)) /*Enquanto não chegar ao final do ficheiro*/
		{
			limpa2 = fgets(linha, 199, file); limpa2++;
	  		if(!feof(file))
	    	{
				trata_linha(jogo, linha, n++);
	    	}
		}
      fclose(file);
      /*Depois de carregado o tabuleiro, verifica-se a seguir quais são os pontos que são iluminados*/
      casas_pintadas=alteracoes(jogo);
		while(flag != 0)
		{  
      	for(lin=0; lin<jogo->linhas; lin++)
      	{
      		for(col=0; col<jogo->colunas; col++)
      		{
      			if(jogo->tabuleiro[col][lin]=='@'){
      				identifica_iluminados(jogo, col, lin);
	      		}
	      	}	
	   	}
	   casas_pintadas2 = alteracoes(jogo);
   	flag=casas_pintadas2-casas_pintadas;
   	casas_pintadas=casas_pintadas2;
		}
		/*Limpar a stack, depois de ter identificado quais é que eram iluminados.*/
		while(jogo->jogadas){
			Jogadas aux = jogo->jogadas->seg;
			free(jogo->jogadas);
			jogo->jogadas=aux;
		}
      impressao_ecra(jogo);
      return 1;   
    }
}

/**Função auxiliar da função carrega_tab, que atribui a cada célula do tabuleiro o seu caractere. Ou seja, vai passar todos os elementos de uma linha para um array.
*@param linha Recebe a linha da função carrega_tab.
*@param n Número da linha.
*@param jogo Tabuleiro recebido. 
*/
void trata_linha(Tabuleiro jogo, char * linha, int n)
{
  int i, j = 0;
  for(i = 0; linha[i]; i += 2)
  {
  		jogo->tabuleiro[j][n] = linha[i];
  		j++;
  }
}

/**Função que executa o prompt, o qual guarda em cmd o introduzido pelo utilizador e passa-o também para o histórico do interpretador.
*@return cmd+i Para guardar o introduzido pelo utilizador na variavél para uso posterior na função comando.
*/
char * prompt()
{
  int i = 0;
  char * cmd = NULL;
  cmd = readline("Illuminatus>");
  while(isspace(cmd[i])) i++;    
  add_history(cmd + i);
  return cmd + i;
}

/**Pega no comando que recebe da função do prompt e separa-os pelos espaços, identifica-o e depois executa-o.
*@param jogo Recebe o tabuleiro.
*@param cmd Recebe o comando e os argumentos dado pelo utilizador no prompt.
*@return 99 Se o utilizador introduzir o comando "q" retorna 99 que é enviado ao main, que faz com que saia do programa. 
*/
int comando(Tabuleiro jogo, char * cmd)
{
  int r = 0, comando;
  char * token;
  char * arg1=NULL, * arg2=NULL, * arg3=NULL;
  if(strcmp(cmd, ""))
    {
      token = strtok(cmd, " ");
      while(token != NULL)
      {
      	switch(r) /*Pode receber até três argumentos*/
      	{
      		case 0:
	      		arg1 = token;
	      		break;
	  			case 1:
	  				arg2 = token;
	  				break;
	  			case 2:
	  				arg3 = token;
	  				break;
	  		}
	  		token = strtok(NULL, " ");
	  		r++;
	   }
      comando = identifica_comando(arg1);
      if(!comando)
      {
      	return mensagem_de_erro(E_COMMAND);
	   }
      if(comando == 1)
      {
	  		return 99;
		}
      	executa_comando(jogo, comando, r, arg2, arg3);
      	return 0;
    }
  return 0;
}

/**Função responsável pela impressão no ecrã do estado actual do tabuleiro numa dada altura.
*@param jogo Recebe estado do tabuleiro no momento.
*/
void impressao_ecra(Tabuleiro jogo)
{
	int linha, coluna;
  	printf("   ");
  	textcolor(BRIGHT, RED, WHITE2);
  	for(coluna = 0; coluna < jogo -> colunas; coluna++) printf("%2d ", coluna + 1);
  	printf("\n");
  	for(linha = 0; linha < jogo -> linhas; linha++)
   {
   	textcolor(BRIGHT, RED, WHITE2);
      printf("%2d ", linha + 1);
      for(coluna = 0; coluna < jogo -> colunas; coluna++)
      {
      	if(jogo -> tabuleiro[coluna][linha] == '@') textcolor(RESET, RED, YELLOW);
         if(jogo -> tabuleiro[coluna][linha] == '-') textcolor(RESET, BLACK, WHITE2);
         if(jogo -> tabuleiro[coluna][linha] == '.' || jogo -> tabuleiro[coluna][linha] == '-'){
         	if(jogo -> marcas[coluna][linha] == 1){
         		textcolor(RESET, BLACK, WHITE);
          	}
          	else{
          		textcolor(RESET, WHITE, YELLOW);
          	}
        }
        if(isdigit(jogo -> tabuleiro[coluna][linha]) || jogo -> tabuleiro[coluna][linha] == 'x') textcolor(BRIGHT, WHITE, BLACK);
        printf(" %c ", jogo -> tabuleiro[coluna][linha]);
        textcolor(RESET, BLACK, WHITE2);
      }
  		printf("\n");
  	}
   
}

/**Função que grava o estado actual do tabuleiro numa determinada altura.
*@param ficheiro Recebe o nome do ficheiro a gravar.
*@param jogo Recebe o estado do tabuleiro.
*@return 1 Se tudo correu bem.
*/
int grava_tab(Tabuleiro jogo, char * ficheiro)
{
  int i, j;
  FILE * file;
  if(!verifica_tab(jogo))
  		return mensagem_de_erro(E_NO_BOARD);
  file = fopen(strcat(ficheiro, ".ill"), "w");
  if(!file)
    {
      return mensagem_de_erro(E_SAVE);
    }
  else
    {
      fprintf(file, "%d %d\n", jogo->colunas, jogo->linhas);
      for(i = 0; i < jogo->linhas; i++)
      {
      	for(j = 0; j < jogo->colunas; j++)
	    	{
	      	if(j == (jogo->colunas - 1))
				{
		  			fprintf(file, "%c", jogo->tabuleiro[j][i]);
				}
	      	else
				{
		  			fprintf(file, "%c ", jogo->tabuleiro[j][i]);
				}
	    	}
	  		fprintf(file, "\n");
		}
      fclose(file);
      return 1;
    }
}

/**Função que verifica se existe um tabuleiro criado ou não.
*@param jogo Recebe o tabuleiro carregado pelo utilizador.
*@return 1 Se verificar que o tabuleiro existe.
*@return 0 Se verificar que não existe tabuleiro.
*/
int verifica_tab(Tabuleiro jogo)
{
  return (jogo->tabuleiro != NULL) ? 1 : 0;
}

/**Função que coloca uma lâmpada numa determinada posição dada pelo utilizador.
*@param jogo Recebe o tabuleiro criado.
*@param coluna Corresponde á coluna introduzida pelo utilizador.
*@param linha Corresponde á linha introduzida pelo utilizador.
*@return 1 Se tudo correu bem.
*/
int joga(Tabuleiro jogo, int coluna, int linha)
{
	if(!verifica_tab(jogo)){ 
		return mensagem_de_erro(E_NO_BOARD);
	}
  	coluna -= 1;
  	linha -= 1;
  	if(coluna < 0 || linha < 0 || coluna >= jogo->colunas || linha >= jogo->linhas)
  	{
  		return mensagem_de_erro(E_COORDS);
  	}
  	if(jogo->tabuleiro[coluna][linha] == '-' || jogo->tabuleiro[coluna][linha] == '.')
   {
   	trabalho_joga(jogo, coluna, linha);
      impressao_ecra(jogo);
   }
  	else
  	{
  		if(jogo->tabuleiro[coluna][linha] == '@')
    	{
    		insere_Jogadas(jogo, coluna, linha, '@', 0);
    		jogo->tabuleiro[coluna][linha] = '-';
      	jogo->marcas[coluna][linha] = 1;
      	impressao_ecra(jogo);
     	}
      else 
      {
      	impressao_ecra(jogo);
      	mensagem_de_erro(E_BLOC);
      }
   }
  	return 1;
}

/**Função auxiliar do joga, que executa o seu trabalho.
*@param jogo Recebe o tabuleiro criado.
*@param coluna Corresponde á coluna introduzida pelo utilizador.
*@param linha Corresponde á linha introduzida pelo utilizador.
*/
void trabalho_joga(Tabuleiro jogo, int coluna, int linha)
{
	if(jogo->tabuleiro[coluna][linha] == '-' && jogo->marcas[coluna][linha] == 1) insere_Jogadas(jogo, coluna, linha, '-', 1);
   if(jogo->tabuleiro[coluna][linha] == '-' && jogo->marcas[coluna][linha] == 0) insere_Jogadas(jogo, coluna, linha, '-', 0);
   if(jogo->tabuleiro[coluna][linha] == '.' && jogo->marcas[coluna][linha] == 1) insere_Jogadas(jogo, coluna, linha, '.', 1);
   if(jogo->tabuleiro[coluna][linha] == '-' && jogo->marcas[coluna][linha] == 0) insere_Jogadas(jogo, coluna, linha, '.', 0);
   jogo->tabuleiro[coluna][linha] = '@';
   jogo->marcas[coluna][linha] = 0;
}

/**Função auxiliar da função estrategia2 que passa todos os elementos á direita da posição actual, que sejam traços, em pontos. Se já for ponto fica igual. 
*@param jogo Recebe estado actual do tabuleiro no momento quando a função é usada.
*@param pos_actual Posição actual dada pela função estrategia2.
*@param linha Linha a iluminar dada pela estrategia2.
*/
void ilumina_dir(Tabuleiro jogo, int pos_actual, int linha)
{
  int i;
  for(i = pos_actual; (i < jogo->colunas && (jogo->tabuleiro[i][linha] == '-' || jogo->tabuleiro[i][linha] == '.')); i++){
    if(jogo->tabuleiro[i][linha] == '-' && jogo->marcas[i][linha] == 1){
      insere_Jogadas(jogo, i, linha, '-', 1); 
      jogo->tabuleiro[i][linha] = '.';
    }
    else
    		if(jogo->tabuleiro[i][linha] == '-' && jogo->marcas[i][linha] == 0){
      		insere_Jogadas(jogo, i, linha, '-', 0);
      		jogo->tabuleiro[i][linha] = '.';
      	}
    jogo->marcas[i][linha] = 0;
  }
}

/**Passa todos os elementos á esquerda da posição actual, que sejam traços, em pontos. Se já for ponto fica igual.
*@param jogo Recebe estado actual do tabuleiro no momento quando a função é usada.
*@param pos_actual Posição actual dada pela função estrategia2.
*@param linha Linha a iluminar dada pela estrategia2.
*/
void ilumina_esq(Tabuleiro jogo, int pos_actual, int linha)
{
	int i;
	for(i = pos_actual; (i >= 0 && (jogo->tabuleiro[i][linha] == '-' || jogo->tabuleiro[i][linha] == '.')); i--){
		if(jogo->tabuleiro[i][linha] == '-' && jogo->marcas[i][linha] == 1){
      	insere_Jogadas(jogo, i, linha, '-', 1);
      	jogo->tabuleiro[i][linha] = '.';
    	}
    	else
    		if(jogo->tabuleiro[i][linha] == '-' && jogo->marcas[i][linha] == 0){
      		insere_Jogadas(jogo, i, linha, '-', 0);
      		jogo->tabuleiro[i][linha] = '.';
      	}
		jogo->marcas[i][linha] = 0;
  	}
}

/**Passa todos os elementos que estejam acima da posição actual, que sejam traços, em pontos. Se já for ponto fica igual.
*@param jogo Recebe estado actual do tabuleiro no momento quando a função é usada.
*@param pos_actual Posição actual dada pela função estrategia2.
*@param coluna Coluna a iluminar dada pela estrategia2.
*/
void ilumina_cima(Tabuleiro jogo, int pos_actual, int coluna)
{
  int i;
  for(i = pos_actual; (i >= 0 && (jogo->tabuleiro[coluna][i] == '-' || jogo->tabuleiro[coluna][i] == '.')); i--){
    if(jogo->tabuleiro[coluna][i] == '-' && jogo->marcas[coluna][i] == 1){
      insere_Jogadas(jogo, coluna, i, '-', 1);		
      jogo->tabuleiro[coluna][i] = '.';
    }
    else
    		if(jogo->tabuleiro[coluna][i] == '-' && jogo->marcas[coluna][i] == 0){
      		insere_Jogadas(jogo, coluna, i, '-', 0);
      		jogo->tabuleiro[coluna][i] = '.';
      	}
    jogo->marcas[coluna][i] = 0;
  }	
}

/**Passa todos os elementos que estejam abaixo da posição actual, que sejam traços, em pontos. Se já for ponto fica igual.
*@param jogo Recebe estado actual do tabuleiro no momento quando a função é usada.
*@param pos_actual Posição actual dada pela função estrategia2.
*@param coluna Coluna a iluminar dada pela estrategia2.
*/
void ilumina_baixo(Tabuleiro jogo, int pos_actual, int coluna)
{
  int i;
  for(i = pos_actual; (i <= jogo->linhas && (jogo->tabuleiro[coluna][i] == '-' || jogo->tabuleiro[coluna][i] == '.')); i++){
    if(jogo->tabuleiro[coluna][i] == '-' && jogo->marcas[coluna][i] == 1){
      insere_Jogadas(jogo, coluna, i, '-', 1); 
      jogo->tabuleiro[coluna][i] = '.';
    }
    else
    		if(jogo->tabuleiro[coluna][i] == '-' && jogo->marcas[coluna][i] == 0){
      		insere_Jogadas(jogo, coluna, i, '-', 0);
      		jogo->tabuleiro[coluna][i] = '.';
      	}
    jogo->marcas[coluna][i] = 0;
  }
}

/**Aplica a estratégia 2 referida no enunciado a todo o tabuleiro
*@param jogo Recebe o tabuleiro.
*/
int estrategia2(Tabuleiro jogo)
{
  int coluna, linha;
  if(!verifica_tab(jogo)){ 
  	return mensagem_de_erro(E_NO_BOARD);
  }
  for(linha = 0; linha < jogo->linhas; linha++)
    {
      for(coluna = 0; coluna < jogo->colunas; coluna++)
      {
      	if(jogo->tabuleiro[coluna][linha] == '@')
      	{
	      	ilumina_dir(jogo, coluna + 1, linha);
	      	ilumina_esq(jogo, coluna - 1, linha);
	      	ilumina_baixo(jogo, linha + 1, coluna);
	      	ilumina_cima(jogo, linha - 1, coluna);
	    	}
		}
    }
  return 0;
}

/**Função que marca uma determinada posição com um ponto.
*@param jogo Recebe o tabuleiro criado.
*@param coluna Corresponde á coluna introduzida pelo utilizador.
*@param linha Corresponde á linha introduzida pelo utilizador.
*@return 1 Se tudo correu bem.
*/
int marca(Tabuleiro jogo, int coluna, int linha)
{
  if(!verifica_tab(jogo)){ 
  		return mensagem_de_erro(E_NO_BOARD);
  }
  coluna -= 1;
  linha -= 1;
  if(coluna < 0 || linha < 0 || coluna >= jogo->colunas || linha >= jogo->linhas)
  { 
  		return mensagem_de_erro(E_COORDS);
  }
  if(jogo->tabuleiro[coluna][linha] == '-')
    {
    	insere_Jogadas(jogo, coluna, linha, '-', 1);
      jogo->tabuleiro[coluna][linha] = '.';
      jogo->marcas[coluna][linha] = 1;
      impressao_ecra(jogo);
    }
  else
  { 
  		if(jogo->tabuleiro[coluna][linha] == '.' )
    	{
    		insere_Jogadas(jogo, coluna, linha, '.', 1);
      	jogo->tabuleiro[coluna][linha] = '-';
			jogo->marcas[coluna][linha] = 1;
      	impressao_ecra(jogo);
      }
      else 
      {
      	impressao_ecra(jogo);
      	mensagem_de_erro(E_BLOC);
      }
   }
  return 1;
}

/**Função que implementa a estratégia 1 do enunciado.
*@param jogo Recebe o tabuleiro.
*/
int estrategia1(Tabuleiro jogo)
{
  int coluna, linha;
  char celula;
  if(!verifica_tab(jogo)){ 
  	return mensagem_de_erro(E_NO_BOARD);
  }
  for(linha = 0; linha < jogo->linhas; linha++)
    {
      for(coluna = 0; coluna < jogo->colunas; coluna++)
      {
			celula=jogo->tabuleiro[coluna][linha];     	
      	if(isdigit(celula) && valor(celula) > 0)
      	{
	      	if(valor(celula) == (posicoeslivres(jogo, coluna, linha) + conta_lamp(jogo, coluna, linha)))
	      		poe_lamp(jogo, coluna, linha);
	    	}
		}
    }
  return 0;
}

/**Função auxiliar da função estrategia1 que verifica as posições livres ortogonais (ou seja, que contém traços) imediatamente ao lado da posição actual.
*@param jogo Recebe o tabuleiro.
*@param coluna Coluna dada pela função estrategia1.
*@param linha Linha dada pela função estrategia1.
*@return res Variavél onde é guardada o numero de posições livres.
*/
int posicoeslivres(Tabuleiro jogo, int coluna, int linha)
{
	int res = 0; 
	if(coluna<jogo->colunas-1 && (jogo->tabuleiro[coluna+1][linha]=='-')){
		res++;
	}
	if(coluna>0 && (jogo->tabuleiro[coluna-1][linha] == '-')){
		res++;
	}
	if(linha<jogo->linhas-1 && (jogo->tabuleiro[coluna][linha+1] == '-')){
		res++;
	}
	if(linha>0 && (jogo->tabuleiro[coluna][linha-1] == '-')){
		res++;
	}
	return res;
}

/**Função auxiliar da função estrategia1 que verificada a condição da estrategia1 põe uma lâmpada onde existir uma posição livre (ou seja, onde existir um traço).
*@param jogo Recebe o tabuleiro.
*@param coluna Coluna dada pela função estrategia1.
*@param linha Linha dada pela função estrategia1.
*/	
int poe_lamp(Tabuleiro jogo, int coluna, int linha)
{	
	if(coluna<jogo->colunas-1 && (jogo->tabuleiro[coluna+1][linha] == '-')){
		insere_Jogadas(jogo, coluna+1, linha, '-', 1);		
		jogo->tabuleiro[coluna+1][linha] = '@';
		jogo->marcas[coluna+1][linha] = 0;
	}
	if(coluna>0 && (jogo->tabuleiro[coluna-1][linha] =='-')){
		insere_Jogadas(jogo, coluna-1, linha, '-', 1);
		jogo->tabuleiro[coluna-1][linha]='@';
		jogo->marcas[coluna-1][linha] = 0;
	}
	if(linha<jogo->linhas-1 && (jogo->tabuleiro[coluna][linha+1] == '-')){
		insere_Jogadas(jogo, coluna, linha+1, '-', 1);
		jogo->tabuleiro[coluna][linha+1]='@';
		jogo->marcas[coluna][linha+1] = 0;
	}
	if(linha>0 && (jogo->tabuleiro[coluna][linha-1] == '-')){
		insere_Jogadas(jogo, coluna, linha-1, '-', 1);
		jogo->tabuleiro[coluna][linha-1]='@';
		jogo->marcas[coluna][linha-1] = 0;
	}
	return 0;
}

/**Aplica a estratégia 3 referida no enunciado a todo o tabuleiro, e a estratégia 3 vai continuar a ser executada até não existir alterações no tabuleiro.
*@param jogo Recebe o tabuleiro.
*return 0 Tudo ocorreu conforme esperado.
*/
int estrategia3(Tabuleiro jogo)
{
	int coluna, linha, casas_pintadas=0, casas_pintadas2=0 , flag=1;
	char celula;
	if(!verifica_tab(jogo)){ 
  		return mensagem_de_erro(E_NO_BOARD);
  	}
  	casas_pintadas=alteracoes(jogo);
	while(flag != 0)
	{  		
  		for(linha = 0; linha < jogo->linhas; linha++)
  		{
  			for(coluna = 0; coluna < jogo->colunas; coluna++)
  			{
  				celula=jogo->tabuleiro[coluna][linha];      	
      		if(isdigit(celula))
      		{
	      		marca_orto(jogo, coluna, linha);
	      		marca_diag(jogo, coluna, linha);
      		}
  			}
   	}
   	casas_pintadas2 = alteracoes(jogo);
   	flag=casas_pintadas2-casas_pintadas;
   	casas_pintadas=casas_pintadas2;
   }
  	return 0;
}

/**Função auxiliar da função estrategia3 que verifica quantos espaços em branco existem no tabuleiro numa determinada altura.
*@param jogo Recebe o tabuleiro criado.
*@return conta Variável onde é guardada a contagem dos traços, isto é, dos espaços brancos.
*/
int alteracoes(Tabuleiro jogo)
{
	int coluna, linha, conta=0;
	for(linha = 0; linha < jogo->linhas; linha++)
  		{
  			for(coluna = 0; coluna < jogo->colunas; coluna++)
  			{
      		if(jogo->tabuleiro[coluna][linha]=='-')
      			conta++;
      	}
      }
	return conta;
}

/**Função auxiliar da função estrategia3 que marca as posições ortogonais com um ponto.
*@param jogo Recebe o tabuleiro criado.
*@param coluna Corresponde á coluna dada pela função estrategia3.
*@param linha Corresponde á linha dada pela função estrategia3.
*@return 0 Se correu tudo bem.
*/
int marca_orto(Tabuleiro jogo, int coluna, int linha)
{
	char celula;
	celula=jogo->tabuleiro[coluna][linha];
  	if(valor(celula) == conta_lamp(jogo, coluna, linha) || valor(celula) == 0)
  	{ 
  		poe_ponto_orto(jogo, coluna, linha);
  	}
  	return 0;
}

/**Função auxiliar da função marca_orto que verifica quantas lâmpadas se encontram á volta duma dada coordenada.
*@param jogo Recebe o tabuleiro.
*@param coluna Coluna dada pela função marca_orto.
*@param linha Linha dada pela função marca_orto.
*@return res Variável onde é guardada o numero de lâmpadas.
*/
int conta_lamp(Tabuleiro jogo, int coluna, int linha)
{
	int res = 0; 
	if(coluna<jogo->colunas-1 && (jogo->tabuleiro[coluna+1][linha]=='@')){
		res++;
	}
	if(coluna>0 && (jogo->tabuleiro[coluna-1][linha] == '@')){
		res++;
	}
	if(linha<jogo->linhas-1 && (jogo->tabuleiro[coluna][linha+1] == '@')){
		res++;
	}
	if(linha>0 && (jogo->tabuleiro[coluna][linha-1] == '@')){
		res++;
	}
	return res;
}


/**Função auxiliar da função marca_orto que verificada a condição de marca_orto põe um ponto onde existir uma posição livre (ou seja, onde existir um traço).
*@param jogo Recebe o tabuleiro.
*@param coluna Coluna dada pela pela função marca_orto.
*@param linha Linha dada pela pela função marca_orto.
*/	
void poe_ponto_orto(Tabuleiro jogo, int coluna, int linha)
{	
	if(coluna<jogo->colunas-1 && (jogo->tabuleiro[coluna+1][linha] == '-')){
		trabalho_marcas(jogo, coluna+1, linha);
	}
	if(coluna>0 && (jogo->tabuleiro[coluna-1][linha] =='-')){
		trabalho_marcas(jogo, coluna-1, linha);
	}
	if(linha<jogo->linhas-1 && (jogo->tabuleiro[coluna][linha+1] == '-')){
		trabalho_marcas(jogo, coluna, linha+1);
	}
	if(linha>0 && (jogo->tabuleiro[coluna][linha-1] == '-')){
		trabalho_marcas(jogo, coluna, linha-1);
	}
}

/**Função auxiliar da função estrategia3 que marca as posições diagonais com um ponto.
*@param jogo Recebe o tabuleiro criado.
*@param coluna Corresponde á coluna dada pela função estrategia3.
*@param linha Corresponde á linha dada pela função estrategia3.
*/
void marca_diag(Tabuleiro jogo, int coluna, int linha)
{
  	marca_lamp(jogo, coluna, linha);
  	marca_cantos(jogo, coluna, linha);
}

/**Função auxiliar da função marca_diag que se encontrar uma lâmpada, marca as posições á esquerda é á direita se a lâmpada estiver em cima ou em baixo e marca as posições em cima e em baixo se a lâmpada estiver á esquerda ou á direita.
*@param jogo Recebe o tabuleiro criado.
*@param coluna Corresponde á coluna dada pela função marca_diag.
*@param linha Corresponde á linha dada pela função marca_diag.
*/
void marca_lamp(Tabuleiro jogo, int coluna, int linha)
{
	/*No caso de as lâmpadas estarem nas casas ortogonais*/
	if(coluna<jogo->colunas-1 && (jogo->tabuleiro[coluna+1][linha] == '@')){
	  if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && (jogo->tabuleiro[coluna+1][linha+1]=='-')){
	  	 trabalho_marcas(jogo, coluna+1, linha+1);
	  }
	  if(coluna<jogo->colunas-1 && linha>0 && (jogo->tabuleiro[coluna+1][linha-1]=='-')){
	  	 trabalho_marcas(jogo, coluna+1, linha-1);
	  }
	}
	if(coluna>0 && (jogo->tabuleiro[coluna-1][linha] =='@')){
	  if(coluna>0 && linha<jogo->linhas-1 && (jogo->tabuleiro[coluna-1][linha+1]=='-')){
	  	 trabalho_marcas(jogo, coluna-1, linha+1);
	  }
	  if(linha>0 && coluna>0 && (jogo->tabuleiro[coluna-1][linha-1]=='-')){
	  	 trabalho_marcas(jogo, coluna-1, linha-1);
	  }
	}
	if(linha<jogo->linhas-1 && (jogo->tabuleiro[coluna][linha+1] =='@')){
	  if(linha<jogo->linhas-1 && coluna<jogo->colunas-1 && (jogo->tabuleiro[coluna+1][linha+1]=='-')){
	  	 trabalho_marcas(jogo, coluna+1, linha+1);
	  }
	  if(linha<jogo->linhas-1 && coluna>0 && (jogo->tabuleiro[coluna-1][linha+1]=='-')){
	  	 trabalho_marcas(jogo, coluna-1, linha+1);
	  }
	}
	if(linha>0 && (jogo->tabuleiro[coluna][linha-1] =='@')){
	  if(linha>0 && coluna<jogo->colunas-1 && (jogo->tabuleiro[coluna+1][linha-1]=='-')){
	  	 trabalho_marcas(jogo, coluna+1, linha-1);
	  }
	  if(linha>0 && coluna>0 && (jogo->tabuleiro[coluna-1][linha-1]=='-')){
	  	 trabalho_marcas(jogo, coluna-1, linha-1);
	  }
	}
	/*No caso de as lâmpadas estarem nos cantos*/
	if(linha>0 && coluna>0 && (jogo->tabuleiro[coluna-1][linha-1] == '@')){
	  if(linha>0 && (jogo->tabuleiro[coluna][linha-1]=='-')){
	  	 trabalho_marcas(jogo, coluna, linha-1);
	  }
	  if(linha>0 && coluna<jogo->colunas-1 && (jogo->tabuleiro[coluna+1][linha-1]=='-')){
	  	 trabalho_marcas(jogo, coluna+1, linha-1);
	  }
	  if(coluna>0 && (jogo->tabuleiro[coluna-1][linha]=='-')){
	  	 trabalho_marcas(jogo, coluna-1, linha);
	  }
	  if(coluna>0 && linha<jogo->linhas-1 && (jogo->tabuleiro[coluna-1][linha+1]=='-')){
	  	 trabalho_marcas(jogo, coluna-1, linha+1);
	  }
	}
	
	if(linha>0 && coluna<jogo->colunas-1 && (jogo->tabuleiro[coluna+1][linha-1] =='@')){
	  if(linha>0 && (jogo->tabuleiro[coluna][linha-1]=='-')){
		 trabalho_marcas(jogo, coluna, linha-1);
	  }
	  if(linha>0 && coluna>0 && (jogo->tabuleiro[coluna-1][linha-1]=='-')){
		 trabalho_marcas(jogo, coluna-1, linha-1);
	  }
	  if(coluna<jogo->colunas-1 && (jogo->tabuleiro[coluna+1][linha]=='-')){
	  	 trabalho_marcas(jogo, coluna+1, linha);
	  }
	  if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && (jogo->tabuleiro[coluna+1][linha+1]=='-')){
	  	 trabalho_marcas(jogo, coluna+1, linha+1);
	  }
	}
	
	if(coluna<jogo->colunas-1 && coluna<jogo->colunas-1 && (jogo->tabuleiro[coluna+1][linha+1] =='@')){
	  if(linha<jogo->linhas-1 && (jogo->tabuleiro[coluna][linha+1]=='-')){
		 trabalho_marcas(jogo, coluna, linha+1); 
	  }
	  if(linha>0 && coluna<jogo->colunas-1 && (jogo->tabuleiro[coluna+1][linha-1]=='-')){
		 trabalho_marcas(jogo, coluna+1, linha-1);
	  }
	  if(coluna<jogo->colunas-1 && (jogo->tabuleiro[coluna+1][linha]=='-')){
		 trabalho_marcas(jogo, coluna+1, linha);
	  }
	  if(coluna>0 && linha<jogo->linhas-1 && (jogo->tabuleiro[coluna-1][linha+1]=='-')){
		 trabalho_marcas(jogo, coluna-1, linha+1);
	  }
	}
	
	if(coluna>0 && linha<jogo->linhas-1 && (jogo->tabuleiro[coluna-1][linha+1] =='@')){
	  if(linha<jogo->linhas-1 && (jogo->tabuleiro[coluna][linha+1]=='-')){
		 trabalho_marcas(jogo, coluna, linha+1);
	  }
	  if(linha<jogo->linhas-1 && coluna<jogo->colunas-1 && (jogo->tabuleiro[coluna+1][linha+1]=='-')){
		 trabalho_marcas(jogo, coluna+1, linha+1);
	  }
	  if(coluna>0 && (jogo->tabuleiro[coluna-1][linha]=='-')){
		 trabalho_marcas(jogo, coluna-1, linha);
	  }
	  if(coluna>0 && linha>0 && (jogo->tabuleiro[coluna-1][linha-1]=='-')){
		 trabalho_marcas(jogo, coluna-1, linha-1);
	  }
	}
}

/**Função auxiliar da função marca_diag que vai marcar os cantos de um número depois de verificada certos testes.
*@param jogo Recebe o tabuleiro criado.
*@param coluna Corresponde á coluna dada pela função marca_diag.
*@param linha Corresponde á linha dada pela função marca_diag.
*/
void marca_cantos(Tabuleiro jogo, int coluna, int linha)
{
	int soma;
	char celula;
	celula=jogo->tabuleiro[coluna][linha];	
	soma=(conta_lamp(jogo, coluna, linha) + posicoeslivres(jogo, coluna, linha));
	if((soma - diagonal_se(jogo, coluna, linha)) < valor(celula)){
	  if(linha>0 && coluna>0 && (jogo->tabuleiro[coluna-1][linha-1]=='-')){
	  	 trabalho_marcas(jogo, coluna-1, linha-1);
	  }
	}
	if((soma - diagonal_sd(jogo, coluna, linha)) < valor(celula)){
	  if(coluna<jogo->colunas-1 && linha>0 && (jogo->tabuleiro[coluna+1][linha-1]=='-')){
	  	 trabalho_marcas(jogo, coluna+1, linha-1);
	  }	
	}
		
	if((soma - diagonal_ie(jogo, coluna, linha)) < valor(celula)){
	  if(coluna>0 && linha<jogo->linhas-1 && (jogo->tabuleiro[coluna-1][linha+1]=='-')){
	    trabalho_marcas(jogo, coluna-1, linha+1);
	  }
	}
	if((soma - diagonal_id(jogo, coluna, linha)) < valor(celula)){
		if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && (jogo->tabuleiro[coluna+1][linha+1]=='-')){
			trabalho_marcas(jogo, coluna+1, linha+1);
	  }
	}
}

/**Função auxiliar das funções que executa o trabalho das mesmas, ou seja, marcam pontos no tabuleiro.
*@param jogo Recebe o tabuleiro criado.
*@param coluna Corresponde á coluna dada pela função marca_diag.
*@param linha Corresponde á linha dada pela função marca_diag.
*/
void trabalho_marcas(Tabuleiro jogo, int coluna, int linha)
{
	insere_Jogadas(jogo, coluna, linha, '-', 1);
	jogo->tabuleiro[coluna][linha] = '.';
	jogo->marcas[coluna][linha] = 1;
}

/**Função auxiliar da função marca_cantos que verifica se as coordenadas ortogonais da zona superior esquerdo estão livres e se estiverem conta-as.
*@param jogo Recebe o tabuleiro criado.
*@param coluna Corresponde á coluna dada pela função marca_cantos.
*@param linha Corresponde á linha dada pela função marca_cantos.
*@return conta Variável onde é guardada a contagem das posições livres
*/
int diagonal_se(Tabuleiro jogo, int coluna, int linha)
{
	int conta=0;
	if(coluna>0 && jogo->tabuleiro[coluna-1][linha]=='-')
		conta++;
	if(linha>0 && jogo->tabuleiro[coluna][linha-1]=='-')
		conta++;	
	return conta;
}

/**Função auxiliar da função marca_cantos que verifica se as coordenadas ortogonais da zona superior direito estão livres e se estiverem conta-as.
*@param jogo Recebe o tabuleiro criado.
*@param coluna Corresponde á coluna dada pela função marca_cantos.
*@param linha Corresponde á linha dada pela função marca_cantos.
*@return conta Variável onde é guardada a contagem das posições livres
*/
int diagonal_sd(Tabuleiro jogo, int coluna, int linha)
{
	int conta=0;
	if(coluna<jogo->colunas-1 && jogo->tabuleiro[coluna+1][linha]=='-')
		conta++;
	if(linha>0 && jogo->tabuleiro[coluna][linha-1]=='-')
		conta++;	
	return conta;
}

/**Função auxiliar da função marca_cantos que verifica se as coordenadas ortogonais da zona inferior esquerdo estão livres e se estiverem conta-as.
*@param jogo Recebe o tabuleiro criado.
*@param coluna Corresponde á coluna dada pela função marca_cantos.
*@param linha Corresponde á linha dada pela função marca_cantos.
*@return conta Variável onde é guardada a contagem das posições livres
*/
int diagonal_ie(Tabuleiro jogo, int coluna, int linha)
{
	int conta=0;
	if(coluna>0 && jogo->tabuleiro[coluna-1][linha]=='-')
		conta++;
	if(linha<jogo->linhas-1 && jogo->tabuleiro[coluna][linha+1]=='-')
		conta++;	
	return conta;
}

/**Função auxiliar da função marca_cantos que verifica se as coordenadas ortogonais da zona inferior direito estão livres e se estiverem conta-as.
*@param jogo Recebe o tabuleiro criado.
*@param coluna Corresponde á coluna dada pela função marca_cantos.
*@param linha Corresponde á linha dada pela função marca_cantos.
*@return conta Variável onde é guardada a contagem das posições livres
*/
int diagonal_id(Tabuleiro jogo, int coluna, int linha)
{
	int conta=0;
	if(coluna<jogo->colunas-1 && jogo->tabuleiro[coluna+1][linha]=='-')
		conta++;
	if(linha<jogo->linhas-1 && jogo->tabuleiro[coluna][linha+1]=='-')
		conta++;	
	return conta;
}

/**Função responsável por executar a estratégia 4 do enunciado.
*@param jogo Recebe o tabuleiro.
*@return 0 Tudo correu conforme o esperado.
*/
int estrategia4(Tabuleiro jogo)
{
	int coluna, linha;
	char celula;
	if(!verifica_tab(jogo)){ 
  		return mensagem_de_erro(E_NO_BOARD);
  	}
  	for(linha = 0; linha < jogo->linhas; linha++)
  	{
  		for(coluna = 0; coluna < jogo->colunas; coluna++)
  		{
 			celula=jogo->tabuleiro[coluna][linha];
      	if(isdigit(celula))
      	{
	      	if(verifica_diag_esq(jogo, coluna, linha)==1)
	      	{
					switch(celula){
						case '0': trata0_esq(jogo, coluna, linha);break;
						case '1': trata1_esq(jogo, coluna, linha);break;
						case '2': trata2_esq(jogo, coluna, linha);break;
						case '3': trata3_esq(jogo, coluna, linha);break;
						case '4': trata4_esq(jogo, coluna, linha);break;
					}
	      	}
	      	if(verifica_diag_dir(jogo, coluna, linha)==1)
	      	{
					switch(celula){
						case '0': trata0_dir(jogo, coluna, linha);break;
						case '1': trata1_dir(jogo, coluna, linha);break;
						case '2': trata2_dir(jogo, coluna, linha);break;
						case '3': trata3_dir(jogo, coluna, linha);break;
						case '4': trata4_dir(jogo, coluna, linha);break;
					}
	     		}
      	}
  		}
   }
  	return 0;
}

/**Função responsável por verificar se na diagonal da esquerda contém um digito.
*@param jogo Recebe o tabuleiro.
*@param coluna Variável que contém a coluna dada pela função estrategia4. 
*@param linha Variável que contém a linha dada pela função estrategia4.
*return res Variável onde é guardada o resultado, se der 1 significa que se encontra um dígito na sua diagonal inferior esquerda.
*/
int verifica_diag_esq(Tabuleiro jogo, int coluna, int linha)
{
	int res;
	if(coluna>0 && linha<jogo->linhas-1 && isdigit(jogo->tabuleiro[coluna-1][linha+1])){
		res=1;
	}
	return res;
}

/**Função responsável por verificar se na diagonal da direita contém um digito.
*@param jogo Recebe o tabuleiro.
*@param coluna Variável que contém a coluna dada pela função estrategia4. 
*@param linha Variável que contém a linha dada pela função estrategia4.
*return res Variável onde é guardada o resultado, se der 1 significa que se encontra um dígito na sua diagonal inferior direita.
*/
int verifica_diag_dir(Tabuleiro jogo, int coluna, int linha)
{
	int res;
	if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && isdigit(jogo->tabuleiro[coluna+1][linha+1])){
		res=1;
	}
	return res;
}

/**Função responsável de quando encontrar um 0, tratar todos os casos de quando tiver um digito na célula diagonal esquerda, pondo lâmpadas dependendo da situação.
*@param jogo Recebe o tabuleiro.
*@param coluna Variável que contém a coluna dada pela função estrategia4. 
*@param linha Variável que contém a linha dada pela função estrategia4.
*/
void trata0_esq(Tabuleiro jogo, int coluna, int linha)
{
	/*No caso de a célula diagonal for 1.*/
	if(coluna>0 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-1][linha+1]=='1'){
		 if((diagonal_ie(jogo, coluna-1, linha+1) == 1) && (conta_lamp(jogo, coluna-1, linha+1) == 0)){
		 	if(coluna>1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-2][linha+1]=='-'){
		 		trabalho_tratas(jogo, coluna-2, linha+1);
			}	
			if(coluna>0 && linha<jogo->linhas-2 && jogo->tabuleiro[coluna-1][linha+2]=='-'){
				trabalho_tratas(jogo, coluna-1, linha+2);
			}
		}
	}
	/*No caso de a célula diagonal ser 2.*/
	if(coluna>0 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-1][linha+1]=='2'){
		if(coluna>1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-2][linha+1]=='-'){
			trabalho_tratas(jogo, coluna-2, linha+1);
		}
		if(coluna>0 && linha<jogo->linhas-2 && jogo->tabuleiro[coluna-1][linha+2]=='-'){
			trabalho_tratas(jogo, coluna-1, linha+2);
		}
	}	
}

/**Função responsável de quando encontrar um 1, tratar todos os casos de quando tiver um digito na célula diagonal esquerda, pondo lâmpadas dependendo da situação.
*@param jogo Recebe o tabuleiro.
*@param coluna Variável que contém a coluna dada pela função estrategia4. 
*@param linha Variável que contém a linha dada pela função estrategia4.
*/
void trata1_esq(Tabuleiro jogo, int coluna, int linha)
{
	if(coluna>0 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-1][linha+1]=='0'){
		 if((diagonal_sd(jogo, coluna, linha) == 1) && (conta_lamp(jogo, coluna, linha) == 0)){
		 	if(linha>0 && jogo->tabuleiro[coluna][linha-1]=='-'){
		 		trabalho_tratas(jogo, coluna, linha-1);
			}	
			if(coluna<jogo->colunas-1 && jogo->tabuleiro[coluna+1][linha]=='-'){
				trabalho_tratas(jogo, coluna+1, linha);
			}
		}
	}
	/*No caso de a célula diagonal ser 1.*/
	if(coluna>0 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-1][linha+1]=='1'){
		caso_geral(jogo, coluna, linha);
		caso_geral(jogo, coluna-1, linha+1);
		if(coluna<jogo->colunas-1 && linha>0 && ((jogo->tabuleiro[coluna+1][linha]=='@') || (jogo->tabuleiro[coluna][linha-1]=='@'))){ 
			if((diagonal_ie(jogo, coluna-1, linha+1) == 1) && (conta_lamp(jogo, coluna-1, linha+1) == 0)){
				if(coluna>1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-2][linha+1]=='-'){
					trabalho_tratas(jogo, coluna-2, linha+1);
				}	
				if(coluna>0 && linha<jogo->linhas-2 && jogo->tabuleiro[coluna-1][linha+2]=='-'){
					trabalho_tratas(jogo, coluna-1, linha+2);
				}
			}
		}
		if(coluna>1 && linha<jogo->linhas-2 && ((jogo->tabuleiro[coluna-2][linha+1]=='@') || (jogo->tabuleiro[coluna-1][linha+2]=='@'))){
			if(diagonal_sd(jogo, coluna, linha) == 1 && conta_lamp(jogo, coluna, linha) == 0){
				if(linha>0 && jogo->tabuleiro[coluna][linha-1]=='-'){
					trabalho_tratas(jogo, coluna, linha-1);
				}
				if(coluna<jogo->colunas-1 && jogo->tabuleiro[coluna+1][linha]=='-'){
					trabalho_tratas(jogo, coluna+1, linha);
				}
			}
		}
	}
	/*No caso de a célula diagonal ser 2.*/
	if(coluna>0 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-1][linha+1]=='2'){
		caso_geral(jogo, coluna, linha);
		caso_geral(jogo, coluna-1, linha+1);
		if(coluna<jogo->colunas-1 && linha>0 && ((jogo->tabuleiro[coluna+1][linha]=='@') || (jogo->tabuleiro[coluna][linha-1]=='@'))){
			if(coluna>1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-2][linha+1]=='-'){
				trabalho_tratas(jogo, coluna-2, linha+1);
			}	
			if(coluna>0 && linha<jogo->linhas-2 && jogo->tabuleiro[coluna-1][linha+2]=='-'){
				trabalho_tratas(jogo, coluna-1, linha+2);
			}
		}
		if(coluna>1 && linha<jogo->linhas-2 && jogo->tabuleiro[coluna-2][linha+1]=='@' && jogo->tabuleiro[coluna-1][linha+2]=='@'){
			if(diagonal_sd(jogo, coluna, linha) == 1 && conta_lamp(jogo, coluna, linha) == 0){
				if(linha>0 && jogo->tabuleiro[coluna][linha-1]=='-'){
					trabalho_tratas(jogo, coluna, linha-1);
				}
				if(coluna<jogo->colunas-1 && jogo->tabuleiro[coluna+1][linha]=='-'){
					trabalho_tratas(jogo, coluna+1, linha);
				}
			}
		}	
	}
	/*No caso de a célula diagonal ser 3.*/
	if(coluna>0 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-1][linha+1]=='3'){
		caso_geral(jogo, coluna, linha);
		caso_geral(jogo, coluna-1, linha+1);
		if(coluna>1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-2][linha+1]=='-'){
			trabalho_tratas(jogo, coluna-2, linha+1);
		}	
		if(coluna>0 && linha<jogo->linhas-2 && jogo->tabuleiro[coluna-1][linha+2]=='-'){
			trabalho_tratas(jogo, coluna-1, linha+2);
		}
	}
}

/**Função responsável de quando encontrar um 2, tratar todos os casos de quando tiver um digito na célula diagonal esquerda, pondo lâmpadas dependendo da situação.
*@param jogo Recebe o tabuleiro.
*@param coluna Variável que contém a coluna dada pela função estrategia4. 
*@param linha Variável que contém a linha dada pela função estrategia4.
*/
void trata2_esq(Tabuleiro jogo, int coluna, int linha)
{
	/*No caso de a célula diagonal ser 4.*/
	if(coluna>0 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-1][linha+1]=='4')
		poe_lamp(jogo, coluna-1, linha+1);
	/*No caso de a célula diagonal ser 0.*/
	if(coluna>0 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-1][linha+1]=='0'){
		if(coluna<jogo->colunas-1 && jogo->tabuleiro[coluna+1][linha]=='-'){
			trabalho_tratas(jogo, coluna+1, linha);
		}	
		if(linha>0 && jogo->tabuleiro[coluna][linha-1]=='-'){
			trabalho_tratas(jogo, coluna, linha-1);
		}
	}
	/*No caso de a célula diagonal ser 2.*/
	if(coluna>0 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-1][linha+1]=='2'){
		caso_geral(jogo, coluna, linha);
		caso_geral(jogo, coluna-1, linha+1);
		if(coluna<jogo->colunas-1 && linha>0 && jogo->tabuleiro[coluna+1][linha]=='@' && jogo->tabuleiro[coluna][linha-1]=='@'){
			if(coluna>1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-2][linha+1]=='-'){
				trabalho_tratas(jogo, coluna-2, linha+1);
			}	
			if(coluna>0 && linha<jogo->linhas-2 && jogo->tabuleiro[coluna-1][linha+2]=='-'){
				trabalho_tratas(jogo, coluna-1, linha+2);
			}
		}
		if(coluna>1 && linha<jogo->linhas-2 && jogo->tabuleiro[coluna-2][linha+1]=='@' && jogo->tabuleiro[coluna-1][linha+2]=='@'){
			if(coluna<jogo->colunas-1 && jogo->tabuleiro[coluna+1][linha]=='-'){
				trabalho_tratas(jogo, coluna+1, linha);
			}	
			if(linha>0 && jogo->tabuleiro[coluna][linha-1]=='-'){
				trabalho_tratas(jogo, coluna, linha-1);
			}
		}
	}
	/*No caso de a célula diagonal ser 3.*/
	if(coluna>0 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-1][linha+1]=='3'){
		caso_geral(jogo, coluna, linha);
		caso_geral(jogo, coluna-1, linha+1);
		if(linha>0 && coluna<jogo->colunas-1 && (conta_lamp(jogo, coluna, linha) == 1) && (jogo->tabuleiro[coluna+1][linha]=='@' || jogo->tabuleiro[coluna][linha-1]=='@')){
			if(coluna>1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-2][linha+1]=='-'){
				trabalho_tratas(jogo, coluna-2, linha+1);
			}
			if(coluna>0 && linha<jogo->linhas-2 && jogo->tabuleiro[coluna-1][linha+2]=='-'){
				trabalho_tratas(jogo, coluna-1, linha+2);
			}
		}
	}
	/*No caso de a célula diagonal ser 1.*/
	if(coluna>0 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-1][linha+1]=='1'){
		caso_geral(jogo, coluna, linha);
		caso_geral(jogo, coluna-1, linha+1);
		if(coluna<jogo->colunas-1 && linha>0 && jogo->tabuleiro[coluna+1][linha]=='@' && jogo->tabuleiro[coluna][linha-1]=='@'){
			if(diagonal_ie(jogo, coluna-1, linha+1) == 1 && conta_lamp(jogo, coluna-1, linha+1) == 0){
				if(coluna>1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-2][linha+1]=='-'){
					trabalho_tratas(jogo, coluna-2, linha+1);
				}
				if(coluna>0 && linha<jogo->linhas-2 && jogo->tabuleiro[coluna-1][linha+2]=='-'){
					trabalho_tratas(jogo, coluna-1, linha+2);
				}
			}
		}
		
		if(diagonal_sd(jogo, coluna, linha) == 1)
		{
			if(coluna<jogo->colunas-1 && jogo->tabuleiro[coluna+1][linha]=='-'){
				trabalho_tratas(jogo, coluna+1, linha);
			}
			if(linha>0 && jogo->tabuleiro[coluna][linha-1]=='-'){
				trabalho_tratas(jogo, coluna, linha-1);
			}	
		}
		
		if(linha<jogo->linhas-2 && coluna>1 && conta_lamp(jogo, coluna-1, linha+1)==1 && (jogo->tabuleiro[coluna-2][linha+1]=='@' || jogo->tabuleiro[coluna-1][linha+2]=='@')){
			if(coluna<jogo->colunas-1 && jogo->tabuleiro[coluna+1][linha]=='-'){
				trabalho_tratas(jogo, coluna+1, linha);
			}
			if(linha>0 && jogo->tabuleiro[coluna][linha-1]=='-'){
				trabalho_tratas(jogo, coluna, linha-1);
			}
		}
	}
}

/**Função responsável de quando encontrar um 3, tratar todos os casos de quando tiver um digito na célula diagonal esquerda, pondo lâmpadas dependendo da situação.
*@param jogo Recebe o tabuleiro.
*@param coluna Variável que contém a coluna dada pela função estrategia4. 
*@param linha Variável que contém a linha dada pela função estrategia4.
*/
void trata3_esq(Tabuleiro jogo, int coluna, int linha)
{
	/*No caso de a célula diagonal ser 1.*/
	if(coluna>0 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-1][linha+1]=='1'){
		caso_geral(jogo, coluna, linha);
		caso_geral(jogo, coluna-1, linha+1);
		if(linha>0 && jogo->tabuleiro[coluna][linha-1]=='-'){
			trabalho_tratas(jogo, coluna, linha-1);
		}
		if(coluna<jogo->colunas-1 && jogo->tabuleiro[coluna+1][linha]=='-'){
			trabalho_tratas(jogo, coluna+1, linha);
		}
	}
	/*No caso da célula diagonal ser 4.*/
	if(coluna>0 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-1][linha+1]=='4'){
		poe_lamp(jogo, coluna-1, linha+1);   	
		caso_geral(jogo, coluna, linha);	
	}
	/*No caso da célula diagonal ser 3.*/
	if(coluna>0 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-1][linha+1]=='3'){
		caso_geral(jogo, coluna, linha);
		caso_geral(jogo, coluna-1, linha+1);
		if(linha>0 && coluna<jogo->colunas-1 && jogo->tabuleiro[coluna][linha-1]=='@' && jogo->tabuleiro[coluna+1][linha]=='@'){
			if(coluna>0 && linha<jogo->linhas-2 && jogo->tabuleiro[coluna-1][linha+2]=='-'){
				trabalho_tratas(jogo, coluna-1, linha+2);
			}
			if(coluna>1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-2][linha+1]=='-'){
				trabalho_tratas(jogo, coluna-2, linha+1);
			}
		}
		if(coluna>1 && linha<jogo->linhas-2 && jogo->tabuleiro[coluna-2][linha+1]=='@' && jogo->tabuleiro[coluna-1][linha+2]=='@'){
			if(coluna<jogo->colunas-1 && jogo->tabuleiro[coluna+1][linha]=='-'){
				trabalho_tratas(jogo, coluna+1, linha);
			}
			if(linha>0 && jogo->tabuleiro[coluna][linha-1]=='-'){
				trabalho_tratas(jogo, coluna, linha-1);
			}
		}
	}
	/*No caso da célula diagonal ser 2.*/				
	if(coluna>0 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-1][linha+1]=='2'){
		caso_geral(jogo, coluna, linha);
		caso_geral(jogo, coluna-1, linha+1);
		if(coluna>0 && linha<jogo->linhas-1 && (conta_lamp(jogo, coluna-1, linha+1) == 1) && (jogo->tabuleiro[coluna-2][linha+1]=='@' || jogo->tabuleiro[coluna-1][linha+2]=='@')){
			if(coluna<jogo->colunas-1 && jogo->tabuleiro[coluna+1][linha]=='-'){
				trabalho_tratas(jogo, coluna+1, linha);
			}
			if(linha>0 && jogo->tabuleiro[coluna][linha-1]=='-'){
				trabalho_tratas(jogo, coluna, linha-1);
			}
		}
	}
}

/**Função responsável de quando encontrar um 4, tratar todos os casos de quando tiver um digito na célula diagonal esquerda, pondo lâmpadas dependendo da situação.
*@param jogo Recebe o tabuleiro.
*@param coluna Variável que contém a coluna dada pela função estrategia4. 
*@param linha Variável que contém a linha dada pela função estrategia4.
*/
void trata4_esq(Tabuleiro jogo, int coluna, int linha)
{
	/*No caso de a célula diagonal for 2.*/
	if(coluna>0 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-1][linha+1]=='2')
		poe_lamp(jogo, coluna, linha);
	/*No caso de a célula diagonal for 4.*/
	if(coluna>0 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-1][linha+1]=='4'){
		poe_lamp(jogo, coluna, linha);
		poe_lamp(jogo, coluna-1, linha+1);
	}
	/*No caso de a célula diagonal for 3.*/
	if(coluna>0 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna-1][linha+1]=='3'){
		poe_lamp(jogo, coluna, linha);
		caso_geral(jogo, coluna-1, linha+1);
	} 
}

/**Função responsável de quando encontrar um 0, tratar todos os casos de quando tiver um digito na célula diagonal direita, pondo lâmpadas dependendo da situação.
*@param jogo Recebe o tabuleiro.
*@param coluna Variável que contém a coluna dada pela função estrategia4. 
*@param linha Variável que contém a linha dada pela função estrategia4.
*/
void trata0_dir(Tabuleiro jogo, int coluna, int linha)
{
	/*No caso de a célula diagonal for 1.*/
	if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+1][linha+1]=='1'){
		if(diagonal_id(jogo, coluna+1, linha+1) == 1 && conta_lamp(jogo, coluna, linha) == 0){
			if(coluna<jogo->colunas-2 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+2][linha+1]=='-'){
				trabalho_tratas(jogo, coluna+2, linha+1);
			}	
			if(coluna<jogo->colunas-1 && linha<jogo->linhas-2 && jogo->tabuleiro[coluna+1][linha+2]=='-'){
				trabalho_tratas(jogo, coluna+1, linha+2);
			}
		}
	}
	/*No caso de a célula diagonal ser 2.*/
	if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+1][linha+1]=='2'){
		if(coluna<jogo->colunas-2 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+2][linha+1]=='-'){
			trabalho_tratas(jogo, coluna+2, linha+1);
		}
		if(coluna<jogo->colunas-1 && linha<jogo->linhas-2 && jogo->tabuleiro[coluna+1][linha+2]=='-'){
			trabalho_tratas(jogo, coluna+1, linha+2);
		}
	}
}

/**Função responsável de quando encontrar um 1, tratar todos os casos de quando tiver um digito na célula diagonal direita, pondo lâmpadas dependendo da situação.
*@param jogo Recebe o tabuleiro.
*@param coluna Variável que contém a coluna dada pela função estrategia4. 
*@param linha Variável que contém a linha dada pela função estrategia4.
*/
void trata1_dir(Tabuleiro jogo, int coluna, int linha)
{
	/*No caso de a célula diagonal ser 0.*/
	if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+1][linha+1]=='0'){
		 if(diagonal_se(jogo, coluna, linha) == 1 && conta_lamp(jogo, coluna, linha) == 0){
		 	if(linha>0 && jogo->tabuleiro[coluna][linha-1]=='-'){
		 		trabalho_tratas(jogo, coluna, linha-1);
			}	
			if(coluna>0 && jogo->tabuleiro[coluna-1][linha]=='-'){
				trabalho_tratas(jogo, coluna-1, linha);
			}
		}
	}
	/*No caso de a célula diagonal ser 1.*/
	if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+1][linha+1]=='1'){
		caso_geral(jogo, coluna, linha);
		caso_geral(jogo, coluna+1, linha+1);
		if(coluna>0 && linha>0 && (jogo->tabuleiro[coluna-1][linha]=='@' || jogo->tabuleiro[coluna][linha-1]=='@')){
			if(diagonal_id(jogo, coluna+1, linha+1) == 1 && conta_lamp(jogo, coluna+1, linha+1) == 0){
				if(linha<jogo->linhas-1 && coluna<jogo->colunas-2 && jogo->tabuleiro[coluna+2][linha+1]=='-'){
					trabalho_tratas(jogo, coluna+2, linha+1);
				}	
				if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+1][linha+2]=='-'){
					trabalho_tratas(jogo, coluna+1, linha+2);
				}
			}
		}
		if(coluna<jogo->colunas-2 && linha<jogo->linhas-2 && ((jogo->tabuleiro[coluna+2][linha+1]=='@') || (jogo->tabuleiro[coluna+1][linha+2]=='@'))){
			if(diagonal_se(jogo, coluna, linha) == 1 && conta_lamp(jogo, coluna, linha) == 0){
				if(linha>0 && jogo->tabuleiro[coluna][linha-1]=='-'){
					trabalho_tratas(jogo, coluna, linha-1);
				}
				if(coluna>0 && jogo->tabuleiro[coluna-1][linha]=='-'){
					trabalho_tratas(jogo, coluna-1, linha);
				}
			}
		}
	}
	/*No caso de a célula diagonal ser 2.*/
	if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+1][linha+1]=='2'){
		caso_geral(jogo, coluna, linha);
		caso_geral(jogo, coluna+1, linha+1);
		if(coluna>0 && linha>0 && ((jogo->tabuleiro[coluna-1][linha]=='@') || (jogo->tabuleiro[coluna][linha-1]=='@'))){
			if(linha<jogo->linhas-1 && coluna<jogo->colunas-2 && jogo->tabuleiro[coluna+2][linha+1]=='-'){
				trabalho_tratas(jogo, coluna+2, linha+1);
			}	
			if(coluna<jogo->colunas-1 && linha<jogo->linhas-2 && jogo->tabuleiro[coluna+1][linha+2]=='-'){
				trabalho_tratas(jogo, coluna+1, linha+2);
			}
		}
		if(coluna<jogo->colunas-2 && linha<jogo->linhas-2 && jogo->tabuleiro[coluna+2][linha+1]=='@' && jogo->tabuleiro[coluna+1][linha+2]=='@'){
			if(diagonal_se(jogo, coluna, linha) == 1 && conta_lamp(jogo, coluna, linha) == 0){
				if(linha>0 && jogo->tabuleiro[coluna][linha-1]=='-'){
					trabalho_tratas(jogo, coluna, linha-1);
				}
				if(coluna>0 && jogo->tabuleiro[coluna-1][linha]=='-'){
					trabalho_tratas(jogo, coluna-1, linha);
				}
			}
		}
	}
	/*No caso de a célula diagonal ser 3.*/
	if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+1][linha+1]=='3'){
		caso_geral(jogo, coluna, linha);
		caso_geral(jogo, coluna+1, linha+1);
		if(linha<jogo->linhas-1 && coluna<jogo->colunas-2 && jogo->tabuleiro[coluna+2][linha+1]=='-'){
			trabalho_tratas(jogo, coluna+2, linha+1);
		}	
		if(coluna<jogo->colunas-1 && linha<jogo->linhas-2 && jogo->tabuleiro[coluna+1][linha+2]=='-'){
			trabalho_tratas(jogo, coluna+1, linha+2);
		}
	}
}

/**Função responsável de quando encontrar um 2, tratar todos os casos de quando tiver um digito na célula diagonal direita, pondo lâmpadas dependendo da situação.
*@param jogo Recebe o tabuleiro.
*@param coluna Variável que contém a coluna dada pela função estrategia4. 
*@param linha Variável que contém a linha dada pela função estrategia4.
*/
void trata2_dir(Tabuleiro jogo, int coluna, int linha)
{
	/*No caso de a célula diagonal ser 4.*/
	if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+1][linha+1]=='4')
		poe_lamp(jogo, coluna+1, linha+1);
	/*No caso de a célula diagonal ser 0.*/			
	if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+1][linha+1]=='0'){
		if(coluna>0 && jogo->tabuleiro[coluna-1][linha]=='-'){
			trabalho_tratas(jogo, coluna-1, linha);
		}	
		if(linha>0 && jogo->tabuleiro[coluna][linha-1]=='-'){
			trabalho_tratas(jogo, coluna, linha-1);
		}
	}
	/*No caso de a célula diagonal ser 2.*/                       
	if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+1][linha+1]=='2'){
		caso_geral(jogo, coluna, linha);
		caso_geral(jogo, coluna+1, linha+1);
		if(coluna>0 && linha>0 && jogo->tabuleiro[coluna-1][linha]=='@' && jogo->tabuleiro[coluna][linha-1]=='@'){
			if(coluna<jogo->colunas-2 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+2][linha+1]=='-'){
				trabalho_tratas(jogo, coluna+2, linha+1);
			}	
			if(coluna<jogo->colunas-1 && linha<jogo->linhas-2 && jogo->tabuleiro[coluna+1][linha+2]=='-'){
				trabalho_tratas(jogo, coluna+1, linha+2);
			}
		}
		if(coluna<jogo->colunas-2 && linha<jogo->linhas-2 && jogo->tabuleiro[coluna+2][linha+1]=='@' && jogo->tabuleiro[coluna+1][linha+2]=='@'){
			if(coluna>0 && jogo->tabuleiro[coluna-1][linha]=='-'){
				trabalho_tratas(jogo, coluna-1, linha);
			}	
			if(linha>0 && jogo->tabuleiro[coluna][linha-1]=='-'){
				trabalho_tratas(jogo, coluna, linha-1);
			}
		}
	}
	/*No caso de a célula diagonal ser 3.*/
	if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+1][linha+1]=='3'){
		caso_geral(jogo, coluna, linha);
		caso_geral(jogo, coluna+1, linha+1);
		if(linha>0 && coluna>0 && (conta_lamp(jogo, coluna, linha) == 1) && (jogo->tabuleiro[coluna-1][linha]=='@' || jogo->tabuleiro[coluna][linha-1]=='@')){
			if(coluna<jogo->colunas-2 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+2][linha+1]=='-'){
				trabalho_tratas(jogo, coluna+2, linha+1);
			}
			if(coluna<jogo->colunas-1 && linha<jogo->linhas-2 && jogo->tabuleiro[coluna+1][linha+2]=='-'){
				trabalho_tratas(jogo, coluna+1, linha+2);
			}
		}
	}
	/*No caso de a célula diagonal ser 1.*/
	if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+1][linha+1]=='1'){
		caso_geral(jogo, coluna, linha);
		caso_geral(jogo, coluna+1, linha+1);
		if(coluna>0 && linha>0 && jogo->tabuleiro[coluna-1][linha]=='@' && jogo->tabuleiro[coluna][linha-1]=='@'){
			if(diagonal_id(jogo, coluna+1, linha+1) == 1 && conta_lamp(jogo, coluna+1, linha+1) == 0){
				if(coluna<jogo->colunas-2 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+2][linha+1]=='-'){
					trabalho_tratas(jogo, coluna+2, linha+1);
				}
				if(coluna<jogo->colunas-1 && linha<jogo->linhas-2 && jogo->tabuleiro[coluna+1][linha+2]=='-'){
					trabalho_tratas(jogo, coluna+1, linha+2);
				}
			}
		}	
		
		if(linha<jogo->linhas-2 && coluna<jogo->colunas-2 && conta_lamp(jogo, coluna+1, linha+1)==1 && (jogo->tabuleiro[coluna+2][linha+1]=='@' || jogo->tabuleiro[coluna+1][linha+2]=='@')){
			if(coluna>0 && jogo->tabuleiro[coluna-1][linha]=='-'){
				trabalho_tratas(jogo, coluna-1, linha);
			}
			if(linha>0 && jogo->tabuleiro[coluna][linha-1]=='-'){
				trabalho_tratas(jogo, coluna, linha-1);
			}
		}
	}
}

/**Função responsável de quando encontrar um 3, tratar todos os casos de quando tiver um digito na célula diagonal direita, pondo lâmpadas dependendo da situação.
*@param jogo Recebe o tabuleiro.
*@param coluna Variável que contém a coluna dada pela função estrategia4. 
*@param linha Variável que contém a linha dada pela função estrategia4.
*/
void trata3_dir(Tabuleiro jogo, int coluna, int linha)
{
	/*No caso de a célula diagonal ser 1.*/
	if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+1][linha+1]=='1'){
		caso_geral(jogo, coluna, linha);
		caso_geral(jogo, coluna+1, linha+1);
		if(linha>0 && jogo->tabuleiro[coluna][linha-1]=='-'){
			trabalho_tratas(jogo, coluna, linha-1);
		}
		if(coluna>0 && jogo->tabuleiro[coluna-1][linha]=='-'){
			trabalho_tratas(jogo, coluna-1, linha);
		}
	}
	/*No caso da célula diagonal ser 4.*/
	if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+1][linha+1]=='4'){
		poe_lamp(jogo, coluna+1, linha+1);
		caso_geral(jogo, coluna, linha);
	}
	/*No caso da célula diagonal ser 3.*/
	if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+1][linha+1]=='3'){
		caso_geral(jogo, coluna, linha);
		caso_geral(jogo, coluna+1, linha+1);
	}
	/*No caso da célula diagonal ser 2.*/				
	if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+1][linha+1]=='2'){
		caso_geral(jogo, coluna, linha);
		caso_geral(jogo, coluna+1, linha+1);
		if(coluna<jogo->colunas-2 && linha<jogo->linhas-2 && (conta_lamp(jogo, coluna+1, linha+1) == 1) && (jogo->tabuleiro[coluna+2][linha+1]=='@' || jogo->tabuleiro[coluna+1][linha+2]=='@')){
			if(coluna>0 && jogo->tabuleiro[coluna-1][linha]=='-'){
				trabalho_tratas(jogo, coluna-1, linha);
			}
			if(linha>0 && jogo->tabuleiro[coluna][linha-1]=='-'){
				trabalho_tratas(jogo, coluna, linha-1);
			}
		}
	}				
}

/**Função responsável de quando encontrar um 4, tratar todos os casos de quando tiver um digito na célula diagonal direita, pondo lâmpadas dependendo da situação.
*@param jogo Recebe o tabuleiro.
*@param coluna Variável que contém a coluna dada pela função estrategia4. 
*@param linha Variável que contém a linha dada pela função estrategia4.
*/
void trata4_dir(Tabuleiro jogo, int coluna, int linha)
{
	/*No caso de a célula diagonal for 2.*/
	if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+1][linha+1]=='2')
		poe_lamp(jogo, coluna, linha);
	/*No caso de a célula diagonal for 4.*/
	if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+1][linha+1]=='4'){
		poe_lamp(jogo, coluna, linha);
		poe_lamp(jogo, coluna+1, linha+1);
	}
	/*No caso de a célula diagonal for 3.*/
	if(coluna<jogo->colunas-1 && linha<jogo->linhas-1 && jogo->tabuleiro[coluna+1][linha+1]=='3'){
		poe_lamp(jogo, coluna, linha);
		caso_geral(jogo, coluna+1, linha+1);
	}
}

/**Função auxiliar das funções trata(dígito)_esq/dir que executa o trabalho necessário para a estratégia 4.
*@param jogo Recebe o tabuleiro.
*@param coluna Variável que contém a coluna dada pelas funções dos trata(dígito)_esq/dir. 
*@param linha Variável que contém a linha dada pelas funções dos trata(dígito)_esq/dir.
*/
void trabalho_tratas(Tabuleiro jogo, int coluna, int linha)
{
	insere_Jogadas(jogo, coluna, linha, '-', 1);
	jogo->tabuleiro[coluna][linha] = '@';
	jogo->marcas[coluna][linha] = 0;
}

/**Função responsável por executar um caso geral parecido com a estrategia 1 para ser executado nas funções trata(dígito)_esq/dir.
*@param jogo Recebe o tabuleiro.
*@param coluna Variável que contém a coluna dada pelas funções de trata(dígito)_esq/dir. 
*@param linha Variável que contém a linha dada pelas funções de trata(dígito)_esq/dir..
*/
void caso_geral(Tabuleiro jogo, int coluna, int linha)
{
	char celula;
	celula=jogo->tabuleiro[coluna][linha]; 	
	if(valor(celula) == (posicoeslivres(jogo, coluna, linha) + conta_lamp(jogo, coluna, linha)))
		poe_lamp(jogo, coluna, linha);
}

/**Função responsável por executar a estratégia 5 do enunciado.
*@param jogo Recebe o tabuleiro.
*@return 0 Tudo correu conforme o esperado.
*/
int estrategia5(Tabuleiro jogo)
{
	int coluna, linha, casas_pintadas=0, casas_pintadas2=0 , flag=1;
  	if(!verifica_tab(jogo)){ 
    	return mensagem_de_erro(E_NO_BOARD);
  	}
  	casas_pintadas=alteracoes(jogo);
	while(flag != 0)
	{  	
  		for(linha = 0; linha < jogo->linhas; linha++)
   	{
   		for(coluna = 0; coluna < jogo->colunas; coluna++)
			{
				/*1 é o ponto não-iluminado*/
	  			if(jogo->tabuleiro[coluna][linha] == '.' && jogo->marcas[coluna][linha] == 1 && livres_orto(jogo, coluna, linha) == 1 && lamp_orto(jogo, coluna, linha) == 0){
	 				insere_Jogadas(jogo, coluna, linha, '.', 1);
	 				poe_lamp_est5(jogo, coluna, linha);
	  			}
	  			if(jogo->tabuleiro[coluna][linha] == '-' && jogo->marcas[coluna][linha] == 1 && livres_orto(jogo, coluna, linha) == 0 && lamp_orto(jogo, coluna, linha) == 0){
	    			poe_lamp_est5_aux(jogo, coluna, linha);
	  			}
			}
		}
		casas_pintadas2 = alteracoes(jogo);
   	flag=casas_pintadas2-casas_pintadas;
   	casas_pintadas=casas_pintadas2;
	}
  	return 0;
}

/**Função auxiliar de carrega_tab. Passa todos os elementos que estejam á direita, á esquerda, acima e abaixo de uma certa posição actual, que sejam pontos ou traços, em 0's para serem guardados numa matriz á parte onde vão ser guardadas os pontos que são iluminados e os não-iluminados, onde a lâmpada também será posta a 0.
*@param jogo Recebe o estado actual do tabuleiro no momento quando a função é usada.
*@param coluna Coluna dada pela função carrega_tab.
*@param linha Linha dada pela função carrega_tab.
*/
void identifica_iluminados(Tabuleiro jogo, int coluna, int linha)
{
	int i;
	jogo->marcas[coluna][linha] = 0;
	
	for(i = coluna+1; i < jogo->colunas && (jogo->tabuleiro[i][linha] == '.' || jogo->tabuleiro[i][linha] == '-'); i++){
		if(jogo->tabuleiro[i][linha] == '.' && jogo->marcas[i][linha] == 1){ 
			insere_Jogadas(jogo, i, linha, '.', 1);
			jogo->marcas[i][linha] = 0;
		}
		if(jogo->tabuleiro[i][linha] == '-' && jogo->marcas[i][linha] == 1){ 
			insere_Jogadas(jogo, i, linha, '-', 1);
			jogo->marcas[i][linha] = 0;
		}
   }
   
   for(i = coluna-1; i >= 0 && (jogo->tabuleiro[i][linha] == '.' || jogo->tabuleiro[i][linha] == '-'); i--){
   	if(jogo->tabuleiro[i][linha] == '.' && jogo->marcas[i][linha] == 1){ 
   		insere_Jogadas(jogo, i, linha, '.', 1);
   		jogo->marcas[i][linha] = 0;
   	}
		if(jogo->tabuleiro[i][linha] == '-' && jogo->marcas[i][linha] == 1){ 
			insere_Jogadas(jogo, i, linha, '-', 1);
			jogo->marcas[i][linha] = 0;
		}
   }
   
   for(i = linha-1; i >= 0 && (jogo->tabuleiro[coluna][i] == '.' || jogo->tabuleiro[coluna][i] == '-'); i--){
   	if(jogo->tabuleiro[coluna][i] == '.' && jogo->marcas[coluna][i] == 1){ 
   		insere_Jogadas(jogo, coluna, i, '.', 1);
   		jogo->marcas[coluna][i] = 0;
   	}
		if(jogo->tabuleiro[coluna][i] == '-' && jogo->marcas[coluna][i] == 1){ 
			insere_Jogadas(jogo, coluna, i, '-', 1);
    		jogo->marcas[coluna][i] = 0;
    	}
  	}
  
  	for(i = linha+1; i <= jogo->linhas && (jogo->tabuleiro[coluna][i] == '.' || jogo->tabuleiro[coluna][i] == '-'); i++){
  		if(jogo->tabuleiro[coluna][i] == '.' && jogo->marcas[coluna][i] == 1){ 
  			insere_Jogadas(jogo, coluna, i, '.', 1);
  			jogo->marcas[coluna][i] = 0;
  		}
		if(jogo->tabuleiro[coluna][i] == '-' && jogo->marcas[coluna][i] == 1){ 
			insere_Jogadas(jogo, coluna, i, '-', 1);
    		jogo->marcas[coluna][i] = 0;
    	}
  	}
}

/**Função auxiliar da estrategia5, que procura ao longo das coordenadas ortogonais quantas posições livres tem.
*@param jogo Recebe o estado actual do tabuleiro no momento quando a função é usada.
*@param coluna Posição actual dada pela função estrategia5.
*@param linha Linha dada pela função estrategia5.
*/
int livres_orto(Tabuleiro jogo, int coluna, int linha)
{
	int i, conta=0;
	for(i = coluna+1; i < jogo->colunas && (jogo->tabuleiro[i][linha] == '-' || jogo->tabuleiro[i][linha] == '.'); i++){
		if(jogo->tabuleiro[i][linha] == '-' && jogo->marcas[i][linha] == 1){
			conta++;
		}
   }

   for(i = coluna-1; i >= 0  && (jogo->tabuleiro[i][linha] == '-' || jogo->tabuleiro[i][linha] == '.'); i--){
		if(jogo->tabuleiro[i][linha] == '-' && jogo->marcas[i][linha] == 1){
			conta++;
		}
   }
   
   for(i = linha-1; i >=0 && (jogo->tabuleiro[coluna][i] == '-' || jogo->tabuleiro[coluna][i] == '.'); i--){
		if(jogo->tabuleiro[coluna][i] == '-' && jogo->marcas[coluna][i] == 1){
			conta++;
		}
   }
   
   for(i = linha+1; i <= jogo->linhas && (jogo->tabuleiro[coluna][i] == '-' || jogo->tabuleiro[coluna][i] == '.'); i++){
		if(jogo->tabuleiro[coluna][i] == '-' && jogo->marcas[coluna][i] == 1){
			conta++;
		}
   }
   
	return conta;
}

/**Função auxiliar da função estrategia5, que dada uma certa coordenada, procura uma posição-livre e não-iluminada ao longo das suas coordenadas ortogonais e depois mete lâmpada e identificas os pontos iluminados ao lango dela.
*@param jogo Recebe o estado actual do tabuleiro no momento quando a função é usada.
*@param linha Linha dada pela função estrategia5.
*@param coluna Coluna dada pela função estrategia5.
*/
void poe_lamp_est5(Tabuleiro jogo, int coluna, int linha)
{
	int i;
	for(i = linha+1; i <= jogo->linhas && (jogo->tabuleiro[coluna][i] == '-' || jogo->tabuleiro[coluna][i] == '.'); i++){
		if(jogo->tabuleiro[coluna][i] == '-' && jogo->marcas[coluna][i] == 1){
			poe_lamp_est5_aux(jogo, coluna, i);
		}
   }
   
   for(i = linha-1; i >= 0 && (jogo->tabuleiro[coluna][i] == '-' || jogo->tabuleiro[coluna][i] == '.'); i--){
		if(jogo->tabuleiro[coluna][i] == '-' && jogo->marcas[coluna][i] == 1){
			poe_lamp_est5_aux(jogo, coluna, i);
		}
   }
	
	for(i = coluna-1; i >= 0 && (jogo->tabuleiro[i][linha] == '-' || jogo->tabuleiro[i][linha] == '.'); i--){
		if(jogo->tabuleiro[i][linha] == '-' && jogo->marcas[i][linha] == 1){
			poe_lamp_est5_aux(jogo, i, linha);
		}
   }
   
   for(i = coluna+1; i < jogo->colunas && (jogo->tabuleiro[i][linha] == '-' || jogo->tabuleiro[i][linha] == '.'); i++){
		if(jogo->tabuleiro[i][linha] == '-' && jogo->marcas[i][linha] == 1){
			poe_lamp_est5_aux(jogo, i, linha);
		}
   }
}

/**Função auxiliar da função estrategia5 e da poe_lamp_est5, que executa o trabalho necessário para cada sitio onde esteja colocada.
*@param jogo Recebe o estado actual do tabuleiro no momento quando a função é usada.
*@param linha Linha dada pela função estrategia5 ou poe_lamp_est5.
*@param coluna Coluna dada pela função estrategia5 ou poe_lamp_est5.
*/
void poe_lamp_est5_aux(Tabuleiro jogo, int coluna, int linha)
{
	insere_Jogadas(jogo, coluna, linha, '-', 1);
	jogo->tabuleiro[coluna][linha] = '@';
	identifica_iluminados(jogo, coluna, linha);
}

/**Função auxiliar da estrategia5, que procura ao longo das coordenadas ortogonais quantas lâmpadas tem.
*@param jogo Recebe o estado actual do tabuleiro no momento quando a função é usada.
*@param coluna Posição actual dada pela função estrategia5.
*@param linha Linha dada pela função estrategia5.
*@return conta Variável onde é guardada a contagem.
*/
int lamp_orto(Tabuleiro jogo, int coluna, int linha)
{
	int i, conta=0;
	for(i = coluna+1; i < jogo->colunas && (jogo->tabuleiro[i][linha] == '-' || jogo->tabuleiro[i][linha] == '.' || jogo->tabuleiro[i][linha] == '@'); i++){
		if(jogo->tabuleiro[i][linha] == '@'){
			conta++;
		}
   }
   
   for(i = coluna-1; i >= 0  && (jogo->tabuleiro[i][linha] == '-' || jogo->tabuleiro[i][linha] == '.' || jogo->tabuleiro[i][linha] == '@'); i--){
		if(jogo->tabuleiro[i][linha] == '@'){
			conta++;
		}
   }
   
   for(i = linha-1; i >=0 && (jogo->tabuleiro[coluna][i] == '-' || jogo->tabuleiro[coluna][i] == '.' || jogo->tabuleiro[coluna][i] == '@'); i--){
		if(jogo->tabuleiro[coluna][i] == '@'){
			conta++;
		}
   }
   
   for(i = linha+1; i <= jogo->linhas && (jogo->tabuleiro[coluna][i] == '-' || jogo->tabuleiro[coluna][i] == '.' || jogo->tabuleiro[coluna][i] == '@'); i++){
		if(jogo->tabuleiro[coluna][i] == '@'){
			conta++;
		}
   }
   
	return conta;
}

/**Função que resolve um tabuleiro válido.
*@param jogo Recebe o estado actual do tabuleiro no momento quando a função é usada.
*@return 0 Se tudo correu conforme o esperado.
*/
int resolve(Tabuleiro jogo)
{
  	if(trabalho_rsv(jogo) == 1)
  		return 0;
  	else 
  		return mensagem_de_erro(E_WRONG_SOLUTION);
}

/**Função que trata de fazer o trabalho necessário para a função resolve.
*@param jogo Recebe o estado actual do tabuleiro, quando a função é usada.
*@return 1 Se tudo correu conforme o era esperado.
*@return 0 Se não correu como se esperava.
*/
int trabalho_rsv(Tabuleiro jogo)
{

	if(executa_estrategias(jogo) == 0)
		return 1;
	else
		return 0;
}

/**Função auxiliar de trabalho_rsv, que executa as estratégias todas enquanto houver alterações no tabuleiro.
*@param jogo Recebe estado actual do tabuleiro no momento quando a função é usada.
*@return flag Variável que vai indicar se não houve mais alterações.
*/
int executa_estrategias(Tabuleiro jogo)
{
	int casas_pintadas=0, casas_pintadas2=0 , flag=1;
	casas_pintadas=alteracoes(jogo);
	while(flag != 0)
	{  
		estrategia1(jogo);
		estrategia2(jogo);
		estrategia3(jogo);
		estrategia4(jogo);
		estrategia5(jogo);		
		
		casas_pintadas2 = alteracoes(jogo);
   	flag=casas_pintadas2-casas_pintadas;
   	casas_pintadas=casas_pintadas2;
	}
	return flag;
}

/**Função que conta o número total de lâmpadas do tabuleiro, criada apenas para análise do assembly dele.
*@param jogo Recebe estado actual do tabuleiro no momento quando a função é usada.
*@return conta Variável onde é guardada a contagem.
*/
int assembly_lampadas(Tabuleiro jogo)
{
	int linha, coluna, conta = 0;
	for(linha = 0; linha < jogo->linhas; linha++)
  	{
  		for(coluna = 0; coluna < jogo->colunas; coluna++)
  		{
  			if(jogo->tabuleiro[coluna][linha] == '@')
  				conta++;
  		}
  	}
  	return conta;
}
