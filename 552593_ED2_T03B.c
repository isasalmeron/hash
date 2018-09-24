/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 03B - Hashing com encadeamento
 *
 * RA: 552593
 * Aluno: Isabela Salmeron Boschi
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

/* Tamanho dos campos dos registros */
#define	TAM_PRIMARY_KEY	13
#define	TAM_NOME		38
#define	TAM_TIPO		38
#define TAM_CP			8
#define	TAM_DATA		9
#define	TAM_HORA		6
#define	TAM_NIVEL		4

#define TAM_REGISTRO    192
#define MAX_REGISTROS   5000
#define TAM_ARQUIVO     (MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA 			"Opcao invalida!\n\n"
#define REGISTRO_N_ENCONTRADO 	"Registro nao encontrado!\n\n"
#define CAMPO_INVALIDO 			"Campo invalido! Informe novamente.\n\n"
#define ERRO_PK_REPETIDA 		"ERRO: Ja existe um registro com a chave primaria: %s.\n\n"
#define REGISTRO_INSERIDO 		"Registro %s inserido com sucesso.\n\n"

/* Registro do Pokémon */
typedef struct {
	char primary_key[TAM_PRIMARY_KEY];
	char nome_pokemon[TAM_NOME];
	char tipo_pokemon[TAM_TIPO];
	char combat_points[TAM_CP];
	char data_captura[TAM_DATA]; 	// DD/MM/AA
	char hora_captura[TAM_HORA];	// HH:MM
	char nome_treinador[TAM_NOME];
	char nivel_treinador[TAM_NIVEL];
	char nome_equipe[TAM_NOME];
} Pokemon;

/* Registro da Tabela Hash
 * Contém a chave primária, o RRN do registro atual e o ponteiro para o próximo
 * registro. */
typedef struct chave Chave;
struct chave {
	char pk[TAM_PRIMARY_KEY];
	int rrn;
	Chave *prox;
};

/* Estrutura da Tabela Hash */
typedef struct {
  int tam;
  Chave **v;
} Hashtable;

/* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
int nregistros;

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Descarta o que estiver no buffer de entrada */
void ignore();

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo();

/* Exibe o jogador */
void exibir_registro(int rrn);

Pokemon recuperar_registro(int rrn);
int prox_primo(int tam);
int verificaPrimo(int num);
void criar_tabela(Hashtable *tabela, int tam);
int funcaoHash(Hashtable tabela, char aux[]);
void carregar_tabela(Hashtable *tabela);
void cadastrar(Hashtable *tabela);
int buscaHash(Hashtable tabela, char chave[], int *rrn);
void buscar(Hashtable tabela);
void alterar(Hashtable tabela);
void remover(Hashtable *tabela);
void imprimir_tabela(Hashtable tabela);
void liberar_tabela(Hashtable *tabela);
Chave *criaNo(char chave[], int rrn);

/* ---------- FUNÇÕES DE VERIFICAÇÕES ---------- */

int verificaNome(char nome[]);
void converteMaiusculo(char str[]);
int verificaTipo(char nome[]);
int verificaCp(char str[]);
int verificaData(int dia, int mes, int ano);
int verificaHora(int hora, int minuto);
int verificaNivel(char str[]);
int verificaNomeEquipe(char nome[]);
int verificaNomeTreinador(char nome[]);
void chavePrimaria(Pokemon *pokemon);


/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main() {

	/* Arquivo */
	int carregarArquivo = 0;
	scanf("%d\n", &carregarArquivo); // 1 (sim) | 0 (nao)
	if (carregarArquivo) carregar_arquivo();



	/* Tabela Hash */
	int tam;
	scanf("%d", &tam);
	tam = prox_primo(tam);

	Hashtable tabela;
	criar_tabela(&tabela, tam);
	if (carregarArquivo) carregar_tabela(&tabela);



	/* Execução do programa */
	int opcao = 0;
	while(opcao != 6) {
		scanf("%d", &opcao);
		switch(opcao) {

		case 1:
			getchar();
			cadastrar(&tabela);
			break;
		case 2:
			getchar();
			alterar(tabela);
			break;
		case 3:
			getchar();
			buscar(tabela);
			break;
		case 4:
		    getchar();
			remover(&tabela);
			break;
		case 5:
			imprimir_tabela(tabela);
			break;
		case 6:
			liberar_tabela(&tabela);
			break;

		case 10:
			printf("%s\n", ARQUIVO);
			break;

		default:
			ignore();
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return 0;
}



/* ==========================================================================
 * ================================= FUNÇÕES ================================
 * ========================================================================== */

/* Descarta o que estiver no buffer de entrada */
void ignore() {
	char c;
	while ((c = getchar()) != '\n' && c != EOF);
}

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo() {
	scanf("%[^\n]\n", ARQUIVO);
}

/* Exibe o Pokémon */
void exibir_registro(int rrn) {

	Pokemon p = recuperar_registro(rrn);

	printf("%s\n", p.primary_key);
	printf("%s\n", p.nome_pokemon);
	printf("%s\n", p.tipo_pokemon);
	printf("%s\n", p.combat_points);
	printf("%s\n", p.data_captura);
	printf("%s\n", p.hora_captura);
	printf("%s\n", p.nome_treinador);
	printf("%s\n", p.nivel_treinador);
	printf("%s\n", p.nome_equipe);
	printf("\n");
	
}

//Função que encontra um determinado registro no arquivo
Pokemon recuperar_registro(int rrn){
	Pokemon pokemon;
	char *p = ARQUIVO + (rrn * 192);

	sscanf(p,"%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@", pokemon.primary_key, pokemon.nome_pokemon, pokemon.tipo_pokemon, pokemon.combat_points, pokemon.data_captura, pokemon.hora_captura, pokemon.nome_treinador, pokemon.nivel_treinador, pokemon.nome_equipe);

	return pokemon;
}

//Função que verifica se um número é primo, se não for, retorna o próximo
int prox_primo(int tam){
	int i;

	for(i = tam; verificaPrimo(i) == 0; i++);

	return i;
}

//Função que verifica se um número é primo ou não
int verificaPrimo(int num){
	int i;

	for(i = 2; i <= sqrt(num); i++){
		if(num % i == 0){
			return 0;
		}
	}

	return 1;
}

//Função que aloca espaço para a tabela e inicializa as variáveis
void criar_tabela(Hashtable *tabela, int tam){
	int i;

	tabela->v = (Chave **) malloc(sizeof(Chave *) * tam);
	tabela->tam = tam;

	for(i = 0; i < tam; i++){
		tabela->v[i] = (Chave *) malloc(sizeof(Chave));
		tabela->v[i]->prox = NULL;
	}
}

//Função que retorna a posição na tabela hash que a nova chave deve ser inserida
int funcaoHash(Hashtable tabela, char aux[]){
	int i, posicao = 0;

	for(i = 0; i < 12; i++){
		posicao = posicao + ((i+1) * aux[i]);
	}

	return posicao % tabela.tam;
}

Chave *criaNo(char chave[], int rrn){
	Chave *novaChave = (Chave *) malloc(sizeof(Chave));

	strcpy(novaChave->pk, chave);
	novaChave->rrn = rrn;

	return novaChave;
}

//Função que percorre o arquivo de dados e salva na tabela hash a chave primária e o rrn
void carregar_tabela(Hashtable *tabela){
	int i, posicao;
	char *p = ARQUIVO, chave[TAM_PRIMARY_KEY];
	Chave *aux, *novo;

	//Percorre o arquivo de dados
	for(i = 0; ARQUIVO[i*192] != '\0'; i++){
		p = ARQUIVO + (i * 192);
		sscanf(p, "%[^@]@", chave);

		posicao = funcaoHash(*tabela, chave);	//Verifica em que posição da tabela hash a chave deve ser inserida
		novo = criaNo(chave, i);

		//Se a posicao ainda estiver vazia, insere nela mesmo
		if(tabela->v[posicao]->prox == NULL){
			novo->prox = NULL;
			tabela->v[posicao]->prox = novo;
		}
		else{
			aux = tabela->v[posicao];
			//Procura o fim da lista ou a posição que deve ser inserido
			while((aux->prox != NULL) && (strcmp(aux->prox->pk, chave) < 0)){
				aux = aux->prox;
			}

			novo->prox = aux->prox;
			aux->prox = novo;
		}
	}

	nregistros = i;
}

//Função que recebe os dados e faz as verificações necessárias, se estiver tudo correto, insere
void cadastrar(Hashtable *tabela){
	char *aux, sdia[3], smes[3], sano[3], shora[3], sminuto[3], registro[193], complemento[192];
	Pokemon pokemon;
	int dia, mes, ano, hora, minuto, tam, i, posicao;
	Chave *novo, *aux2;

	aux = (char *) malloc(100 * sizeof(char));
	//Recebe o nome do pokemon
	scanf("%s", aux);
	ignore();
	//Enquanto não for um nome válido, pede um novo nome
	while(!verificaNome(aux)){
		printf(CAMPO_INVALIDO);
		free(aux);
		aux = (char *) malloc(100 * sizeof(char));
		scanf("%s", aux);	
		ignore();
	}
	converteMaiusculo(aux);	//Converte os caracteres em letras maiúsculas
	strcpy(pokemon.nome_pokemon, aux); //Salva o nome do pokemon

	free(aux);
	aux = (char *) malloc(100 * sizeof(char));
	//Recebe o tipo do pokemon
	scanf("%s", aux);	
	ignore();
	//Enquanto não for um tipo válido, pede um novo tipo
	while(!verificaTipo(aux)){
		printf(CAMPO_INVALIDO);
		free(aux);
		aux = (char *) malloc(100 * sizeof(char));
		scanf("%s", aux);	
		ignore();
	}
	converteMaiusculo(aux);	//Converte os caracteres em letras maiúsculas
	strcpy(pokemon.tipo_pokemon, aux); //Salva o tipo do pokemon

	free(aux);
	aux = (char *) malloc(100 * sizeof(char));
	//Recebe o cp do pokemon
	scanf("%s", aux);
	ignore();	
	//Enquanto não for um cp válido, pede um novo cp
	while(!verificaCp(aux)){
		printf(CAMPO_INVALIDO);
		free(aux);
		aux = (char *) malloc(100 * sizeof(char));
		scanf("%s", aux);	
		ignore();
	}
	strcpy(pokemon.combat_points, aux); //Salva o cp do pokemon

	//Recebe a data de captura do pokemon
	scanf("%d/%d/%d", &dia, &mes, &ano);
	ignore();
	//Enquanto não for uma data válida, pede uma nova data
	while(!verificaData(dia, mes, ano)){
		printf(CAMPO_INVALIDO);
		scanf("%d/%d/%d", &dia, &mes, &ano);
		ignore();
	}
	//Converte os inteiros em string
	sprintf(sdia, "%02d", dia);
	sprintf(smes, "%02d", mes);
	sprintf(sano, "%02d", ano);

	//Salva no registro pokemon
	strcpy(pokemon.data_captura, sdia);
	strcat(pokemon.data_captura, "/");
	strcat(pokemon.data_captura, smes);
	strcat(pokemon.data_captura, "/");
	strcat(pokemon.data_captura, sano);
  	pokemon.data_captura[TAM_DATA-1] = '\0';

	//Recebe a hora de captura do pokemon
	scanf("%d:%d", &hora, &minuto);
	ignore();
	//Enquanto não for um horário válid0, pede um novo horário
	while(!verificaHora(hora, minuto)){
		printf(CAMPO_INVALIDO);
		scanf("%d:%d", &hora, &minuto);
		ignore();
	}
	//Converte os inteiros em string
	sprintf(shora, "%02d", hora);
	sprintf(sminuto, "%02d", minuto);

	//Salva no registro pokemon
	strcpy(pokemon.hora_captura, shora);
	strcat(pokemon.hora_captura, ":");
	strcat(pokemon.hora_captura, sminuto);
   	pokemon.hora_captura[TAM_HORA-1] = '\0';

   	free(aux);
	aux = (char *) malloc(100 * sizeof(char));
	//Recebe o nome do treinador
	scanf("%s", aux);
	ignore();
	//Enquanto não for um nome válido, pede um novo nome
	while(!verificaNomeTreinador(aux)){
		printf(CAMPO_INVALIDO);
		free(aux);
		aux = (char *) malloc(100 * sizeof(char));
		scanf("%s", aux);	
		ignore();
	}
	converteMaiusculo(aux);	//Converte os caracteres em letras maiúsculas
	strcpy(pokemon.nome_treinador, aux); //Salva o nome do treinador

	free(aux);
	aux = (char *) malloc(100 * sizeof(char));
	//Recebe o nível do treinador
	scanf("%s", aux);
	ignore();
	//Enquanto não for um nível válido, pede um novo nível
	while(verificaNivel(aux) == 0){
		printf(CAMPO_INVALIDO);
		free(aux);
		aux = (char *) malloc(100 * sizeof(char));
		scanf("%s", aux);	
		ignore();
	}
	strcpy(pokemon.nivel_treinador, aux);	//Salva o nível do treinador

	free(aux);
	aux = (char *) malloc(100 * sizeof(char));
	//Recebe o nome da equipe
	scanf("%s", aux);
	ignore();
	//Enquanto não for um nome válido, pede um novo nome
	while(!verificaNomeEquipe(aux)){
		printf(CAMPO_INVALIDO);
		free(aux);
		aux = (char *) malloc(100 * sizeof(char));
		scanf("%s", aux);
		ignore();
	}
	strcpy(pokemon.nome_equipe, aux); //Salva o nome da equipe

	//Cria a chave primária
   	chavePrimaria(&pokemon);

   	// Se a chave primária não existe na tabela hash
   	if(buscaHash(*tabela, pokemon.primary_key, &tam) == -1){
   		posicao = funcaoHash(*tabela, pokemon.primary_key);	//Verifica em que posição da tabela hash a chave deve ser inserida
		novo = criaNo(pokemon.primary_key, nregistros);

		//Se a posicao ainda estiver vazia, insere nela mesmo
		if(tabela->v[posicao]->prox == NULL){
			novo->prox = NULL;
			tabela->v[posicao]->prox = novo;
		}
		else{			
			aux2 = tabela->v[posicao];
			//Procura o fim da lista ou a posição que deve ser inserido
			while((aux2->prox != NULL) && (strcmp(aux2->prox->pk, pokemon.primary_key) < 0)){
				aux2 = aux2->prox;
			}

			novo->prox = aux2->prox;
			aux2->prox = novo;

		}
		nregistros++;

		//Calcula o tamanho do registro
   		tam = 44 + strlen(pokemon.nome_pokemon) + strlen(pokemon.tipo_pokemon) + strlen(pokemon.nome_treinador) + strlen(pokemon.nome_equipe);
   		tam = 192 - tam; //Calcula a quantidade de # que será necessários para preencher o registro no arquivo de dados
   		for(i = 0; i < tam; i++){
   			complemento[i] = '#';
   		}
   		complemento[i] = '\0';

		//Salva os dados no arquivo
		sprintf(registro, "%s@%s@%s@%s@%s@%s@%s@%s@%s@%s", pokemon.primary_key, pokemon.nome_pokemon, pokemon.tipo_pokemon, pokemon.combat_points, pokemon.data_captura, pokemon.hora_captura, pokemon.nome_treinador, pokemon.nivel_treinador,  pokemon.nome_equipe, complemento);
		strcat(ARQUIVO, registro);
		printf("%s\n", pokemon.primary_key);
		printf(REGISTRO_INSERIDO, pokemon.primary_key);
   	}
   	else{
    	printf(ERRO_PK_REPETIDA, pokemon.primary_key);
   	}
}

//Função que procura por uma chave na tabela, retorna a posicao em que está na tabela(rrn), -1 caso não encontre
int buscaHash(Hashtable tabela, char chave[], int *rrn){
	int posicao;
	Chave *aux;

	posicao = funcaoHash(tabela, chave);

	//Se a posição não tiver nenhuma chave
	if(tabela.v[posicao] == NULL){
		return -1;
	}

	//Percorre a lista
	aux = tabela.v[posicao];
	while(aux != NULL){
		if((strcmp(aux->pk, chave) == 0)){
			*rrn = aux->rrn;	//Se encontrou
			return posicao;
		}
		aux = aux->prox;
	}

	return -1;
}

//Função que dada uma chave primária, se ela existir, altera o cp do pokemon no arquivo de dados
void alterar(Hashtable tabela){
	char chave[TAM_PRIMARY_KEY], nome[TAM_NOME], tipo[TAM_TIPO], *cp, *p;
	int tam, rrn, posicao;

	cp = (char *) malloc(TAM_CP * sizeof(char));

	scanf("%[^\n]s", chave);	//Recebe a chave primária
	ignore();
	converteMaiusculo(chave);	//Converte os caracteres em letras maiúsculas

	posicao = buscaHash(tabela, chave, &rrn);
	//Se a chave primária não existir
	if(posicao == -1){
    	printf(REGISTRO_N_ENCONTRADO);
	}
	//Se existir
	else{
		scanf("%[^\n]s", cp);	//Recebe o novo cp
		ignore();

		//Enquanto não for um cp válido, pede um novo cp
		while(!verificaCp(cp)){
			printf(CAMPO_INVALIDO);
			free(cp);
			cp = (char *) malloc(TAM_CP * sizeof(char));
			cp[TAM_CP-1] = '\0';
			scanf("%s", cp);		
			ignore();		
		}

		p = ARQUIVO + (rrn * 192) + 13;	//Vai até o registro já no campo tipo do pokemon
		sscanf(p, "%[^@]@%[^@]", nome, tipo);	//Lê o nome e o tipo do pokemon
		tam = strlen(nome) + strlen(tipo) + 2;	
		p = ARQUIVO + (rrn * 192) + 13 + tam;	//Vai até o campo cp do registro
		
		//Altera o arquivo
		sprintf(p, "%s", cp);
		p = p + TAM_CP - 1;
		*p = '@';
	}
}

//Função que dada uma chave, se ela existir, remove do arquivo de dados e tabela hash
void remover(Hashtable *tabela){
	char aux[TAM_PRIMARY_KEY-1], *p;
	int rrn, posicao;
	Chave *aux2, *anterior;


	scanf("\n%[^\n]s", aux);
	ignore();
	aux[12] = '\0';
	converteMaiusculo(aux);

	posicao = buscaHash(*tabela, aux, &rrn);	//Busca na tabela hash
	if(posicao == -1){
		printf(REGISTRO_N_ENCONTRADO);
	}
	else{
		//Remove da tabela hash
		aux2 = tabela->v[posicao];
		//Se a lista tiver só um elemento e for ele a ser removido
		if(strcmp(aux2->pk, aux) == 0){
			tabela->v[posicao] = aux2->prox;	//Lista vazia
			free(aux2);	//Remove o elemento
		}
		else{
			anterior = aux2;
			aux2 = aux2->prox;
			//Percorre a lista
			while(aux2 != NULL){
				if((strcmp(aux2->pk, aux) == 0)){
					anterior->prox = aux2->prox;
					free(aux2);
					break;
				}
				anterior = aux2;
				aux2 = aux2->prox;
			}
		}		

		//Remove do arquivo de dados
		p = ARQUIVO + (rrn * 192);	//Vai até o registro
		sprintf(p, "*|");
	}
}

//Função que dado uma chave, busca o pokemon na tabela
void buscar(Hashtable tabela){
	char *aux;
	int rrn, posicao;

	aux = (char *) malloc(13 * sizeof(char));
	scanf("%s", aux);
	ignore();

	converteMaiusculo(aux);

	posicao = buscaHash(tabela, aux, &rrn);	//Busca na tabela hash

	//Caso não encontre
	if(posicao == -1){
		printf(REGISTRO_N_ENCONTRADO);
	}
	else{
		exibir_registro(rrn);	//Imprime o registro
	}
	free(aux);
}	

//Função que imprime a tabela hash
void imprimir_tabela(Hashtable tabela){
	int i;
	Chave *aux;

	for(i = 0; i < tabela.tam; i++){
		printf("[%d]", i);

		aux = tabela.v[i]->prox;
		while(aux != NULL){
			printf(" %s", aux->pk);
			aux = aux->prox;
		}
		printf("\n");
	}
	printf("\n");
}

//Função que libera a memória alocada para a tabela hash
void liberar_tabela(Hashtable *tabela){
	free(tabela->v);
}

/* ---------- FUNÇÕES DE VERIFICAÇÕES ---------- */

//Função que verifica se o nome do pokemon é válido, ou seja, se contém apenas letras
int verificaNome(char nome[]){
	int i = 0;

   	//Percorre a string char a char, se encontrar um caractere que não seja letra, sai da função
   	while(nome[i] != '\0'){
      	if(!((nome[i] >= 'A' && nome[i] <= 'Z') || (nome[i] >= 'a' && nome[i] <= 'z'))){
         	return 0;
      	}
      	i++;
   	}

   return 1;
}

//Função que converte string para caracteres maiusculos
void converteMaiusculo(char str[]){
   	int i = 0;

   	while(str[i] != '\0'){
      	str[i] = toupper(str[i]);
      	i++;
   	}
}

//Função que verifica se o tipo do pokemon é válido
int verificaTipo(char nome[]){
	int i = 0, flag = 0;

   	//Percorre a string char a char, se encontrar um caractere que não seja letra ou '/', sai da função
   	while(nome[i] != '\0'){
      	if((i != 0 && nome[i+1] != '\0') && nome[i] == '/'){
      		flag++;
         	i++;
      	}
      	else if(!((nome[i] >= 'A' && nome[i] <= 'Z') || (nome[i] >= 'a' && nome[i] <= 'z')) || flag == 2){
         	return 0;
      	}
      	else{
         	i++;
      	}
   	}

   	return 1;
}

//Função que verifica se o cp do pokemon é válido
int verificaCp(char str[]){
   	int i = 0, flag = 0;

   	if(strlen(str) != TAM_CP-1){
   		return 0;
   	}

   	while(str[i] != '\0'){
      	if(!((str[i] >= '0' && str[i] <= '9') || str[i] == '.')){
         	return 0;
      	}
      	if(str[i] == '.'){
         	flag = 1;
      	}
      	if(str[i] == '.' && i != 4){
         	return 0;
      	}
      	i++;
   }

   	if(flag == 0){
      	return 0;
   	}
   	return 1;
}

//Função que verifica se a data de captura é válida
int verificaData(int dia, int mes, int ano){

   if(ano < 16 || dia < 1 || mes < 1 || mes > 12){
      return 0;
   }
   if((mes == 1 || mes == 3 || mes == 5 || mes == 7 || mes == 8 || mes == 10 || mes == 12) && (dia > 31)){
      return 0;
   }
   if((mes == 4 || mes == 6 || mes == 9 || mes == 11) && (dia > 30)){
      return 0;
   }
   //Se o ano for bissexto, o mês for fevereiro e o dia for maior que 29
   if((ano % 4 == 0) && (mes == 2) && (dia > 29)){
      return 0;
   }
   //Se o ano não for bissexto, o mês for fevereiro e o dia for maior que 28
   if((ano % 4 != 0) && (mes == 2) && (dia > 28)){
      return 0;
   }

   return 1;
}

//Função que verifica se o horário de captura é válido
int verificaHora(int hora, int minuto){
   	if((hora < 0 || hora > 23) || (minuto < 0 || minuto > 59)){
      	return 0;
   	}

   	return 1;
}

//Função que verifica se o nível do treinador é válido
int verificaNivel(char str[]){
   	int num;

   	//Calcula a quantidade de bytes do numero, se não for 3 é inválido
   	if(strlen(str) != 3){
      	return 0;
   	}

   	num = atoi(str);  //Converte a string para int 
   	//Se não for um número entre 1 e 100, é inválido
   	if(num < 1 || num > 100){
      	return 0;
   	}

   	return 1;
}

//Função que verifica se o nome da equipe é válido
int verificaNomeEquipe(char nome[]){
   	converteMaiusculo(nome);   //Converte para maiusculo
   	if(strcmp(nome, "VALOR") == 0){
      	return 1;
   	}
   	if(strcmp(nome, "INSTINCT") == 0){
      	return 1;
   	}
   	if(strcmp(nome, "MYSTIC") == 0){
      	return 1;
   	}

   return 0;
}

//Função que verifica se o nome do treinador é válido
int verificaNomeTreinador(char nome[]){
	int i = 0;

   	//Percorre a string char a char, se encontrar um caractere que não seja letra, sai da função
   	while(nome[i] != '\0'){
      	if(!((nome[i] >= 'A' && nome[i] <= 'Z') || (nome[i] >= 'a' && nome[i] <= 'z') || (nome[i] >= '0' && nome[i] <= '9'))){
         	return 0;
      	}
      	i++;
   	}

   return 1;
}

//Função que cria a chave primária
void chavePrimaria(Pokemon *pokemon){

   pokemon->primary_key[0] = pokemon->nome_equipe[0]; //Primeira letra do nome da equipe
   pokemon->primary_key[1] = pokemon->nome_treinador[0]; //Primeira letra do nome do treinador
   pokemon->primary_key[2] = pokemon->nome_pokemon[0];   //Primeira letra do nome do pokemon
   pokemon->primary_key[3] = pokemon->nome_pokemon[1];   //Segunda letra do nome do pokemon
   pokemon->primary_key[4] = pokemon->data_captura[0];	//Primeiro dígito do dia
   pokemon->primary_key[5] = pokemon->data_captura[1];	//Segundo dígito do dia
   pokemon->primary_key[6] = pokemon->data_captura[3];	//Primeiro digito do mês
   pokemon->primary_key[7] = pokemon->data_captura[4];	//Segundo dígito do mês
   pokemon->primary_key[8] = pokemon->hora_captura[0];	//Primeiro dígito da hora
   pokemon->primary_key[9] = pokemon->hora_captura[1];	//Segundo dígito da hora
   pokemon->primary_key[10] = pokemon->hora_captura[3];	//Primeiro dígito do minuto
   pokemon->primary_key[11] = pokemon->hora_captura[4];	//Segundo dígito minuto
   pokemon->primary_key[12] = '\0';
}