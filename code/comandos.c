#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "erro.h"
#include "main.h"
#include "listas_ligadas.h"

/**Função que identifica qual o comando a ser executado.
 *@param comando Recebe a variável comando dada pela função comando do main.c
 *@return 1 a 7 guarda a flag correspondente a cada comando na variável comando.
 *@return 0 Dá erro de não existe comando.
 */
int identifica_comando(char * comando)
{
  if(strcmp(comando, "q") == 0){return 1;}
  if(strcmp(comando, "cr") == 0){return 2;}
  if(strcmp(comando, "gr") == 0){return 3;}
  if(strcmp(comando, "jg") == 0){return 4;}
  if(strcmp(comando, "est1") == 0){return 5;}
  if(strcmp(comando, "est2") == 0){return 6;}
  if(strcmp(comando, "mc") == 0){return 7;}
  if(strcmp(comando, "est3") == 0){return 8;}
  if(strcmp(comando, "an") == 0){return 9;}
  if(strcmp(comando, "est4") == 0){return 10;}
  if(strcmp(comando, "est5") == 0){return 11;}
  if(strcmp(comando, "rsv") == 0){return 12;}   
  return 0;
}

/**Função que executa o comando dado o valor retornado na função que identifica que comando foi introduzido pelo utilizador.
 *@param jogo Referencia-se a onde se encontra guardado as dimensões do tabuleiro.
 *@param arg1 Variável onde está guardada o primeiro argumento dado pelo utilizador aquando a introdução do comando.
 *@param arg2 Variável onde está guardada o segundo argumento dado pelo utilizador aquando a introdução do comando.
 *@param comando Refere-se à opção que vai ser executada pelo utilizador.
 *@param argumentos Número de argumentos introduzidos pelo utilizador.
 *@return 1 a 7. Execução de comandos bem sucedida.
 *@return 0. Execução de comandos mal sucedida.
 */
int executa_comando(Tabuleiro jogo, int comando, int argumentos, char * arg1, char * arg2)
{
  switch(comando)
    {
    case 2: /*Se for 2 usa o carrega*/
      if(argumentos == 2) /*Recebe 2 args pois recebe o cr e o tabuleiro tipo ex1(Exemplificar: cr ex1)*/
      {
	 		carrega_tab(jogo, arg1); /*Um argumento pois só precisa do ex1, por exemplo*/
	  		return 0;
      }
      else
      {
	  		return mensagem_de_erro(E_ARGS);
      }
      break;
      
    case 3: /*Se for 3 usa o comando grava*/
      if(argumentos == 2) /*Recebe o gr e o tabuleiro que o utilizador quer*/
      {
	  		grava_tab(jogo, arg1);
	  		return 0;
      }
      else
		{
	  		return mensagem_de_erro(E_ARGS);
		}
      break;
      		
    case 4:
      if(argumentos == 3)
		{
	  		if(testa_inteiro(arg1) && testa_inteiro(arg2))
	    	{
	      	joga(jogo, atoi(arg1), atoi(arg2));
	      	jogo->n_comando++;
	    	}
	  		else
	    	{
	      	mensagem_de_erro(E_INVARGS);
	    	}
		}
      else
		{
	  		mensagem_de_erro(E_ARGS);
		}
      break;
    
    case 5:
      if(argumentos == 1)
		{
	  		estrategia1(jogo);
	  		jogo->n_comando++;
	  		impressao_ecra(jogo);
		}
      else
      {
	  		mensagem_de_erro(E_ARGS);
		}
      break;
      	
    case 6:
      if(argumentos == 1)
		{
	  		estrategia2(jogo);
	  		jogo->n_comando++;
	  		impressao_ecra(jogo);
		}
      else
		{
	  		mensagem_de_erro(E_ARGS);
		}	
      break;
      	
    case 7:
      if(argumentos == 3)
		{
	  		if(testa_inteiro(arg1) && testa_inteiro(arg2))
	    	{
	      	marca(jogo, atoi(arg1), atoi(arg2)); /*atoi significa converter uma string em inteiros*/
	      	jogo->n_comando++;
	    	}
	  		else
	    	{
	      	mensagem_de_erro(E_INVARGS);
	    	}
		}
      else
		{
	  		mensagem_de_erro(E_ARGS);
		}
      break;
      	
    case 8:
      if(argumentos == 1)
		{
	  		estrategia3(jogo);
	  		jogo->n_comando++;
	  		impressao_ecra(jogo);
		}
      else
		{
	  		mensagem_de_erro(E_ARGS);
		}	
      break;
      	
    case 9:
      if(argumentos == 1)
		{
	  		anula(jogo);
		}
      else
		{
	  		mensagem_de_erro(E_ARGS);
		}	
      break;
      	
    case 10:
      if(argumentos == 1)
		{
	  		estrategia4(jogo);
	  		jogo->n_comando++;
	  		impressao_ecra(jogo);
		}
      else
		{
	  		mensagem_de_erro(E_ARGS);
		}		
      break;
      	
    case 11:
      if(argumentos == 1)
		{
	  		estrategia5(jogo);
	  		jogo->n_comando++;
	  		impressao_ecra(jogo);
		}
      else
		{
	  		mensagem_de_erro(E_ARGS);
		}	
      break;
    
    case 12:
      if(argumentos == 1)
		{
	  		resolve(jogo);
	  		jogo->n_comando++;
	  		impressao_ecra(jogo);
		}
      else
		{
	  		mensagem_de_erro(E_ARGS);
		}	
      break;
    }
  return 0;
}

/**Função auxiliar da função executa_comando que verifica se na função joga e marca, os argumentos dados pelo utilizador que são guardados numa string, são dígitos.
 *@param a Argumento dado pelo utilizador na função marca,  ou joga.
 *@return flag Retorna 1 ou 0 conforme se é digito ou não.
 */
int testa_inteiro(char * a)
{
  int flag = 1, i = 0;
  while(a[i])
    {
      flag = isdigit(a[i++]);
    }
  return flag;
}
