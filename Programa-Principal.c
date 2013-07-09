//////////////////////////////////////////////////////////////////////////
//					JOGO DA FORCA - Versão 2.2							//
//////////////////////////////////////////////////////////////////////////
//			Desenvolvimento: Rodrigo Passos da Cunha					//
//			e-mail: contato@rodrigop.com.br 							//
//			http://twitter.com/rpc_1910									//
//						Junho de 2011									//
//	Integração para jogar em rede: Agradecimentos a Rodrigo Piovesana	//
//////////////////////////////////////////////////////////////////////////
//				Login: rodrigo			Senha: passos					//
/////////////////////////////////////////////////////////////////////////

#include<stdio.h>
#include<ctype.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>
#include<windows.h>
#include<winsock2.h>

#define MAX 100
#define PORTAESCUTA 666
#define PORTAENVIA 667
#define TAMANHO_BUFFER 128

/*Funções Utilizadas no Programa*/
void risco();
void limpa();
void topo(char frase[]);
void maiusculo(char variavel[]);
void ajuste(int enter, int tab);


int montaBoneco(int x, int y, char boneco1[7]);
int forca(char palavra[], char dica[], char mensagem[]);

/*Funções da área restrita*/
void Ranking();
void ListarPalavra();
void listarRanking();
void ExcluirPalavra();
void CadastrarPalavra();
int palavraRandom();
void cadastrar(char palavraRecebida[], char dicaRecebida[]);

/*----------------------------------------------------*/
/*Funções comunicação*/

void escuta();
void criaConexao();


/*----------------------------------------------------*/
FILE *ponteiro;
int i;
char op;

struct palavras{
	int id, nivel, estado;
	char palavra[MAX], dica[MAX];
}palavra, ler;

struct Ranking {
	char nome[MAX];
	int pontuacao, id;
}jogador, contador;

/*Variaveis - comunicação*/

int soq = 0;
int ligado = 0, desligado = 0;

char recebePalavra[MAX], enviaPalavra[MAX], recebeDica[MAX], enviaDica[MAX];

struct sockaddr_in local, remota;

int tamanhoRemota = sizeof(remota);

struct in_addr ip_jogador1, ip_jogador2;

char broadcast = '1';

WSADATA wsa_data;

main(){
	system("MODE con cols=126 lines=57");
	ShowWindow(GetForegroundWindow(), SW_MAXIMIZE);
	system("title JOGO DA FORCA 2.2 - BY RPC_1910");
	system("color 0e");

	char lerPalavra[MAX], lerDica[MAX], frase[MAX+10];
	int cont, erro=0, z=0, aux;
	
	ajuste(25,6);
	printf("JOGO DA FORCA VER%cAO 2.2\n", 128);
	ajuste(0,7);
	printf("by RPC_1910");
	sleep(1500);
	
	inicioPrograma:
	limpa();
	system("color 0e");
	topo("Escolha sua Opcao");
	ajuste(20,6); 	printf("(A) Single Player");
	ajuste(2,6); 	printf("(B) Multiplayer");
	ajuste(2,6); 	printf("(C) Multiplayer em rede <NEW>");
	ajuste(23,0);
	risco();
	ajuste(1,3);
	printf("(H) Ajuda\t\t\t(R) Ranking\t\t\t(S) Sair");
	
	op=toupper(getch());
	limpa();
	
	switch(op){
		
		case 'A':{	//Singleplayer
			int retorno, referencia;
			jogador.pontuacao=0;
			limpa();
			
			ajuste(25,6);	printf("Digite seu nome: ");
			gets(jogador.nome);
			jogador.nome[0]=toupper(jogador.nome[0]);
			
			strcpy(frase,"");
			strcat(frase,"Jogador - ");
			strcat(frase,jogador.nome);
			
			
			InicioSingle:
			retorno=palavraRandom();
			
			if(retorno!=0){
				referencia=forca(ler.palavra, ler.dica, frase);
			}
			
			ajuste(25,9);	system("pause");
			limpa();
			
			if(referencia==0){
				jogador.pontuacao++;
				Pergunta:
				limpa();
				ajuste(20,6);	printf("PONTUA%cAO: %d", 128, jogador.pontuacao);
				ajuste(3,6);	printf("Deseja continuar %s? (S/N)", jogador.nome);
				op=toupper(getch());
				switch(op){
					case 'S':{
						goto InicioSingle;
						break;
					}
					
					case 'N':{
						break;
					}
					
					default:{
						goto Pergunta;
					}
				}
			}
			
			else{
				ajuste(25,6);	printf("Jogador: %s", jogador.nome);
				ajuste(2,6);	printf("PONTUA%cAO: %d", 128, jogador.pontuacao);
				ajuste(25,9);	system("pause");
			}
			
			jogador.id=1;
			ponteiro=fopen("Ranking.dat","ab+");
			fread(&contador,sizeof(contador),1,ponteiro);
			while(!feof(ponteiro)){
				jogador.id++;
				fread(&contador,sizeof(contador),1,ponteiro);
			}
			
			fwrite(&jogador,sizeof(jogador),1,ponteiro);
			fclose(ponteiro);
			goto inicioPrograma;
			break;
			
		}
		
		
		case 'B':{	//Multiplayer
			limpa();			
			/*Lê a palavra que será utilizada no jogo*/
			topo("Jogador 1");
			ajuste(20,6);	printf("Digite a palavra: ");
			scanf("%s", &lerPalavra);
			ajuste(2,6);	printf("Digite a dica: ");
			fflush(stdin);
			gets(lerDica);
			/*chama a função para converter a palavra*/
			
			forca(lerPalavra, lerDica, "Jogador 2");
			
			ajuste(25,9);
			system("pause");
			goto inicioPrograma;
			break;
		}
		
		
		/*Função Multiplayer online*/
		case 'C': {
			limpa();
			
			topo("Multiplayer em rede (NEW)");
			ajuste(25,6);	printf("Digite seu nome: ");
			gets(jogador.nome);
			jogador.nome[0]=toupper(jogador.nome[0]);
			
			strcpy(frase,"");
			strcat(frase,"Jogador - ");
			strcat(frase,jogador.nome);
			
			inicioRede:
			topo("Multiplayer em rede (NEW)");
			ajuste(20,6); 	printf("(A) Criar jogo");
			ajuste(2,6); 	printf("(B) Aguardar conexao");
			ajuste(2,6); 	printf("(V) Voltar");
			
			op=toupper(getch());
			
			limpa();
			
			switch(op){
				case 'A': {
					topo("Instrucoes");
					system("type instrucaoCria.txt");
					risco();
					ajuste(2,10);	system("pause");
					
					//Inicio da função
					topo("Criando Jogo");
					
					criaConexao();
					//ajuste(20,6);	printf("Ta desenvolvendo manolo....");
					ajuste(20,6);	system("pause");
					
					goto inicioPrograma;
					break;
				}
				
				case 'B': {
					topo("Instrucoes");
					system("type instrucaoAguarda.txt");
					risco();
					ajuste(2,10);	system("pause");
					
					//Inicio da função
					topo("Aguardando conexao");
					escuta();
					
					limpa();
					
					ajuste(20,6);	printf("Carregando jogo...");
					
					//após receber dados, chama função forca
					cadastrar(recebePalavra, recebeDica);
					forca(recebePalavra, recebeDica, frase);
					ajuste(10,10);	system("pause");
					
					goto inicioPrograma;
					break;
				}
				
				case 'V': {
					goto inicioPrograma;
					break;
				}
				
				default: {
					limpa();
					ajuste(25,6);
					printf("Opcao Invalida!!");
					sleep(1000);
					goto inicioRede;
				}
			}
			break;
        }
		
		case 'H':{
			topo("Ajuda");
			system("type manual.txt");
			ajuste(10,0);	risco();
			ajuste(2,9);	system("pause");
			goto inicioPrograma;
			break;
		}
		
		case 'R':{
			listarRanking();
			goto inicioPrograma;
			break;
		}
		
		case 'L':{
			char login[10], senha[10];
			limpa();
			system("color 0f");
			topo("Area Restrita");
			ajuste(20,6);	printf("Login: ");
			gets(login);	//lê o nome de usuario
	
			maiusculo(login);	//deixa o nome de usuario em maiusculo

			/*Loop para ler a senha do usuario*/
			ajuste(2,6);	printf("Senha: ");
			/*Lendo a senha*/
			i=0;
			senha[i]=getch();	//lê o primeiro valor da senha
			do{
				if(senha[i]==8){	//teste para saber se a pessoa digitou backspace, caso sim apaga o ultimo caracter digitado
					if(i!=0){	//caso a variavel i seja 0, o loop continua, porém apenas lê outro valor sem alterar nada na tela
						i--;
						printf("%s", "\b \b");	//apaga o ultimo caracter da tela
					}
				}
				else{
					printf("%c", '*');	//imprime caracter * para a senha
					senha[i]=toupper(senha[i]);	//converte o caracter para maiusculo
					i++;
				}
				senha[i]=getch();
			}while(senha[i]!=13);
			senha[i]='\0';	//insere o finalizador na senha
			
			maiusculo(senha);
			
			limpa();
			
			if(strcmp(login,"RODRIGO")!=0 || strcmp(senha,"PASSOS")!=0){
				ajuste(25,7);	system("echo Dados inválidos!!!");
				Sleep(1000);
			}
			
			else{
				Restrito:
				topo("Seja bem vindo a area restrita");
				ajuste(12,6);	printf("Escolha sua Op%cao", 135);
				ajuste(4,6);	printf("(A) Cadastrar Palavra");
				ajuste(2,6);	printf("(B) Excluir Palavra");
				ajuste(2,6);	printf("(C) Listar Palavras");
				ajuste(2,6);	printf("(D) Ranking");
				ajuste(2,6);	printf("(L) Logout");
				
				ajuste(3,6);	printf("Op%cao: ", 135);
				op=toupper(getch());
				printf("%c", op);
				switch(op){
					case 'A':{
						CadastrarPalavra();
						goto Restrito;
						break;
					}
					
					case 'B':{
						ExcluirPalavra();
						goto Restrito;
						break;
					}
					
					case 'C':{
						ListarPalavra();
						ajuste(5,9);	system("pause");
						goto Restrito;
						break;
					}
					
					case 'D':{
						Ranking();
						goto Restrito;
						break;
					}
					
					case 'L':{
						limpa();
						ajuste(25,6);
						printf("Encerrando Sessao");
						for(i=0;i<5;i++){
							printf(" .");
							sleep(1000);
						}
						break;
					}
					
					default:{
						beep(1000);
						ajuste(6,6);	system("echo Opção Inválida!!");
						sleep(1000);
						goto Restrito;
					}
				}
			}
			
			goto inicioPrograma;
			break;
		}
		
		case 'S':{
			limpa();
			ajuste(25,6);
			printf("Encerrando");
			for(i=0;i<5;i++){
				printf(" .");
				sleep(1000);
			}
			break;
		}
		
		
		default:{
			limpa();
			ajuste(25,6);
			printf("Opcao Invalida!!");
			sleep(1000);
			goto inicioPrograma;
		}
	}
}

/*xxxxxxxxxxxxxxxxxxxxxxxxxxxx		Funções		xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
/*imprime topo*/
void topo(char frase[]){
	limpa();
	ajuste(2,6);
	printf("%s", frase);
	ajuste(2,0);
	risco();
}

/*Funçao para imprimir o risco do topo*/
void risco(){
	for(i=0;i<126;i++){
		printf("_");
	}
}

/*Função para limpar a tela*/
void limpa(){
	fflush(stdin);
	system("cls");
}

void maiusculo(char variavel[]){
	for(i=0;i<strlen(variavel);i++){	//loop para percorrer a string
		variavel[i]=toupper(variavel[i]);	//deixa o caracter em maiusculo
	}
}

/*Função para ajusteizar os textos*/
void ajuste(int enter, int tab){
	for(i=0;i<enter;i++){
		printf("\n");
	}
	
	for(i=0;i<tab;i++){
		printf("\t");
	}
}

/*Função para a criação do boneco*/
int montaBoneco(int x, int y, char boneco1[7]){
	if(x==0){
		y++;
		switch(y){
			case 1:
				boneco1[0]=79;
				break;
			case 2:
				boneco1[1]=124;
				break;
			case 3:
				boneco1[2]=95;
				break;
			case 4:
				boneco1[3]=95;
				break;
			case 5:
				boneco1[4]=47;
				break;
			case 6:
				boneco1[5]=92;
				break;
		}
	}
	return y;
}

/*Função Principal*/
int forca(char palavra[], char dica[], char mensagem[]){
	
	char adivinha[MAX]="", boneco[7]="", letras[24]="", letra;
	int cont, erro, posicaoLetra, aux, ref;
	
	for(i=0;i<strlen(palavra);i++){
		palavra[i]=toupper(palavra[i]);
		adivinha[i]=45;	//atribui o caracter "-"
	}
	
	strcpy(boneco,"       ");
	erro=0;
	posicaoLetra=0;
	/*parte principal do jogo (lê as letras, faz o teste, imprime o boneco, etc...)*/
	
	while(strcmp(adivinha,palavra)!=0 && erro<6){
		fflush(stdin);
		/*Função para imprimir o boneco e letras digitadas*/
		topo(mensagem);
		
		ajuste(10,3);	printf("______");
		ajuste(1,3);	printf("|     |");
		ajuste(1,3);	printf("|    %c%c%c", boneco[2], boneco[0], boneco[3]);
		ajuste(1,3);	printf("|     %c", boneco[1]);
		ajuste(1,3);	printf("|    %c %c ", boneco[4], boneco[5]);
		ajuste(1,3);	printf("|      ");
		ajuste(1,3);	printf("|      ");
	
		fflush(stdin);
		ajuste(5,3);	printf("LETRAS DIGITADAS:");
		for(i=0;i<posicaoLetra;i++){
			if(letras[i]!=' ' && letras[i]!='\0'){
				printf(" %c", letras[i]);
			}
		}
		
		
		
		ajuste(2,3);	printf("Adivinhe a Palavra: %s", adivinha);
		ajuste(0,3);	printf("Dica: %s", dica);
		ajuste(2,3);	printf("Digite a Letra: ");
		
		/*Leitura da letra*/
		fflush(stdin);
		letra=toupper(getch());
		printf("%c", letra);
		
		/*Testando para saber se ja foi digitada*/
		aux=0;
		for(i=0;i<posicaoLetra;i++){
			if(letra==letras[i]){
				aux++;
			}
		}
		
		/*testa a condição da letra digitada e verifica se ela existe na palavra*/
		if(aux==0 && letra>=65 && letra<=90){
			letras[posicaoLetra]=letra;
	
			cont=0;		
			for(i=0;i<strlen(palavra);i++){
				if(letra==palavra[i]){
					adivinha[i]=palavra[i];
					cont++;
				}
			}
	
			/*chama a função para a montagem do boneco da forca*/
			erro=montaBoneco(cont,erro,boneco);
			posicaoLetra++;
		}
		
		/*caso a letra ja tenha sido digitada, ou seja outro caracter, executa-se esse bloco de comando*/
		else if(aux!=0){
			//limpa();
			ajuste(5,3);	printf("LETRA JA DIGITADA!!!\a");
			sleep(1000);
		}
		
		else{
			//limpa();
			ajuste(5,3);	printf("LETRA INVALIDA!!!\a");
			sleep(1000);
		}
	}
	
	/*Encerrando o programa. Exibe o resultado do jogo*/
	limpa();
	ref=0;
	if(erro==6){
		ajuste(25,6);	printf("        Voce perdeu!!!!");
		ajuste(2,6);	printf("    A palavra era \"%s\"", palavra);
		ref++;
	}
	
	else{
		ajuste(25,6);	printf("        PARAB%cNS!!", 144);
		ajuste(2,6);	printf("     Voc%c acertou!!!", 136);
		ajuste(2,6);	printf("    A palavra era \"%s\"", palavra);
	}
	
	return ref;
	
}

/*Funções da área restrita*/
/*Função para cadastrar palavras*/
void CadastrarPalavra(){
	int cont=0;
	topo("Cadastro de Palavra");
	palavra.id=1;
	
	ponteiro=fopen("Palavras.dat","ab+");
	
	fread(&palavra,sizeof(palavra),1,ponteiro);
	while(!feof(ponteiro)){
		palavra.id++;
		fread(&palavra,sizeof(palavra),1,ponteiro);
	}
	
	palavra.estado=1;
	ajuste(10,3);	printf("Coddigo da palavra %d", palavra.id);
	ajuste(2,3);	printf("Digite a Palavra: ");
	gets(palavra.palavra);
	ajuste(2,3);	printf("Digite a dica para a palavra: ");
	gets(palavra.dica);
	
	palavra.nivel=1;	//por padrão, todas as palavras são faceis
	maiusculo(palavra.palavra);
	palavra.dica[0]=toupper(palavra.dica[0]);
	
	struct palavras palavra2;
	
	rewind(ponteiro);
	fread(&palavra2,sizeof(palavra2),1,ponteiro);
	while(!feof(ponteiro)){
		if(strcmp(palavra.palavra,palavra2.palavra)==0){
			cont++;
		}
		fread(&palavra2,sizeof(palavra2),1,ponteiro);
	}
	
	if(cont!=0){
		sleep(1000);
		ajuste(5,3);	printf("A palavra digitada j%c existe!", 160);
		sleep(1000);		
	}
	
	else{
		fwrite(&palavra,sizeof(palavra),1,ponteiro);
		sleep(1000);
		ajuste(5,3);	printf("Palavra cadastrada com sucesso!!");
		sleep(1000);
	}
	
	fclose(ponteiro);
}


void ExcluirPalavra(){
	int cod;
	ListarPalavra();
	
	ajuste(5,3);	printf("Digite o c%cdigo da palavra a ser exclu%cda: ", 162, 161);
	scanf("%d", &cod);
	
	ponteiro=fopen("Palavras.dat","rb+");
	
	i=0;
	fread(&palavra,sizeof(palavra),1,ponteiro);
	while(!feof(ponteiro)){
		if(palavra.id==cod){
			i++;
			break;
		}
		fread(&palavra,sizeof(palavra),1,ponteiro);
	}
	
	ajuste(3,3);
	sleep(1000);
	if(i!=0){
		palavra.estado=0;
		fseek(ponteiro,sizeof(palavra)*(cod-1),0);
		fwrite(&palavra,sizeof(palavra),1,ponteiro);
		printf("Dados Exclu%cdos", 161);
	}
	
	else{
		printf("C%cdigo Inv%clido!!", 162, 160);
	}
	
	sleep(1000);
	
	fclose(ponteiro);
	
}

void ListarPalavra(){
	int contador=1;
	topo("Palavras");
	ajuste(2,3);	printf("Codigo");
	ajuste(0,3);	printf("Palavra");
	ajuste(0,3);	printf("Dica");
	ajuste(1,0);	risco();
	
	ponteiro=fopen("Palavras.dat","ab+");
	fread(&palavra,sizeof(palavra),1,ponteiro);
	while(!feof(ponteiro)){
		if(palavra.estado!=0){
			
			if(contador%15==0){
				ajuste(5,0);	risco();
				ajuste(5,9);	printf("Continuar a visualiza%cao", 128);
				getch();
				topo("Palavras");
				ajuste(2,3);	printf("Codigo");
				ajuste(0,3);	printf("Palavra");
				ajuste(0,3);	printf("Dica");
				ajuste(1,0);	risco();
			}
			ajuste(2,3);	printf("%d", palavra.id);
			ajuste(0,3);	printf("%s", palavra.palavra);
			ajuste(0,3);	printf("%s", palavra.dica);
			ajuste(0,3);
			
			contador++;
		}
		fread(&palavra,sizeof(palavra),1,ponteiro);
	}
	
	ajuste(5,0);	risco();
	fclose(ponteiro);
}

/*Área restrita - OPção Ranking*/
void Ranking(){
	InicioRanking:
	topo("Ranking");
	ajuste(15,6);	system("echo Escolha sua opção");
	ajuste(2,6);	printf("(A) Mostrar Ranking");
	ajuste(2,6);	printf("(B) Excluir Ranking");
	ajuste(2,6);	printf("(V) Voltar");
	
	ajuste(3,6);	printf("Op%cao: ", 135);
	op=toupper(getch());
	printf("%c", op);
	
	switch(op){
		case 'A':{
			listarRanking();
			goto InicioRanking;
			break;
		}
		
		case 'B':{
			ponteiro=fopen("Ranking.dat","wb");
			fclose(ponteiro);
			sleep(500);
			ajuste(5,6);	printf("Ranking excluido com sucesso!");
			sleep(1000);
			goto InicioRanking;
			break;
		}
		
		case 'V':{
			break;
		}
		
		default:{
			ajuste(5,6);	printf("Opcao Invalida!!");
			sleep(1000);
			goto InicioRanking;
		}
	}
	
}

/*Função para palavra aleatória*/
int palavraRandom(){
	int palavras=0, posicao;
	ponteiro=fopen("Palavras.dat","rb");
	if(!ponteiro){
		ajuste(25,6);	printf("Nenhuma palavra cadastrada!!");
		ajuste(2,7);	printf("Consulte o manual");
		return 0;
	}
	
	else{
		fread(&ler,sizeof(ler),1,ponteiro);
		while(!feof(ponteiro)){
			if(ler.estado!=0){
				palavras++;
			}
			fread(&ler,sizeof(ler),1,ponteiro);
		}
		Random:
		srand((unsigned)time(NULL));
		posicao=(int)rand()%palavras;
		rewind(ponteiro);
		fseek(ponteiro,posicao*sizeof(ler),0);
		fread(&ler,sizeof(ler),1,ponteiro);
		
		if(ler.estado==0){
			goto Random;
		}
		
		return 1;
	}
	fclose(ponteiro);
	
	
}

/*Função para mostrar o Ranking*/
void listarRanking(){
	int vetorPontuacao[MAX], vetorPosicao[MAX], tamanhoVetor, aux, contador=0, referencia=0, posicao, j;	//Variaveis utilizadas na função
	tamanhoVetor=0;
	limpa();
	
	ponteiro=fopen("Ranking.dat","ab+");
	/*Le a pontuação dos jogadores*/
	fread(&jogador,sizeof(jogador),1,ponteiro);
	while(!feof(ponteiro)){
		vetorPontuacao[tamanhoVetor]=jogador.pontuacao;
		tamanhoVetor++;
		fread(&jogador,sizeof(jogador),1,ponteiro);
	}
	
	/*Ordena o Vetor de forma decrescente*/
	for(i=0;i<tamanhoVetor;i++){
		for(j=i;j<tamanhoVetor;j++){
			if(vetorPontuacao[i]<vetorPontuacao[j]){
				aux=vetorPontuacao[i];
				vetorPontuacao[i]=vetorPontuacao[j];
				vetorPontuacao[j]=aux;
			}
		}
	}
	
	/*Pesquisa no arquivo a posição dos jogadores*/
	rewind(ponteiro);
	fread(&jogador,sizeof(jogador),1,ponteiro);
	while(contador<20 && contador<tamanhoVetor){
		if(!feof(ponteiro)){
			if(vetorPontuacao[contador]==jogador.pontuacao){
				for(i=0;i<contador;i++){	//loop para verificar se o jogador já foi lido
					if(vetorPosicao[i]==jogador.id){
						referencia++;
					}
				}
			
				if(referencia==0){	//caso o jogador não tenha sido lido, o id entra no vetor
					vetorPosicao[contador]=jogador.id;
					contador++;
				}
			}
		}
		
		else {	//quando chegar ao final do arquivo
			rewind(ponteiro);	//posiciona o ponteiro no inicio do arquivo
		}
		fread(&jogador,sizeof(jogador),1,ponteiro);
	}
	
	topo("Ranking");
	ajuste(2,5);	printf("Jogador");
	ajuste(0,4);	system("echo Pontuação");
	risco();
	
	/*Mostra o ranking*/
	posicao=0;
	rewind(ponteiro);
	fread(&jogador,sizeof(jogador),1,ponteiro);
	while(posicao<contador){
		if(!feof(ponteiro)){
			if(vetorPosicao[posicao]==jogador.id){
				ajuste(2,5);	printf("%s", jogador.nome);
				ajuste(0,4);	printf("    %d", jogador.pontuacao);
				posicao++;
			}
			fread(&jogador,sizeof(jogador),1,ponteiro);
		}
		
		else {
			rewind(ponteiro);
		}
	}
	ajuste(5,0);	risco();
	ajuste(2,10);	system("pause");
	fclose(ponteiro);
}


/*Funções para a versão em rede*/
/*----	Funções de escuta	----*/
void escuta() {
	strcpy(recebePalavra,"");
	strcpy(recebeDica,"");
	
	int recebido = 0;
	char conectado[10] = "";
	
    if (WSAStartup(MAKEWORD(2, 0), &wsa_data) != 0) {
        printf("WSAStartup() falhou!\n");
    }
    
    soq = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    
    if (soq == INVALID_SOCKET){
		closesocket(soq); // fecha conexão do socket
        printf("Perdeu PLayboy!!!!");
        exit(1);
    }
    
    /* Preenchendo a estrutura local - que vai enviar os dados ao cliente - SIM, ESSA PARTE EU COPIEI*/
    memset(&local,0,sizeof(local)); // zera a estrutura local
    local.sin_family = PF_INET;       // família AF_INET
    local.sin_port = htons(PORTAESCUTA);    // envia os dados para a porta 666
    local.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY - endereço IP da máquina em que a aplicação é executada
    
    printf("Endereco de PORTA UDP de escuta: %i\n",ntohs(local.sin_port));
    printf("Endereco de Escuta UDP: %s\n",inet_ntoa(local.sin_addr));
    
	risco();
    memset(&remota, 0, sizeof(remota)); // zera a estrutura local
    memset(&recebePalavra, 0, MAX);
	memset(&recebeDica, 0, MAX);
	
	if (bind(soq,(struct sockaddr *) &local, sizeof(local)) == SOCKET_ERROR) {
		WSACleanup();
		closesocket(soq);
		printf("Deu derrame no bind()\n\n");
		system("pause");
		exit(1);
    }
	
	printf("Aguardando conexao....\n\n");
	memset(&conectado,0,10);
	
	//while para escutar
	do {
	
		//recvfrom : recebe os dados do tipo datagram
		if (recvfrom(soq, conectado, TAMANHO_BUFFER, 0,(struct sockaddr *) &remota, &tamanhoRemota) == SOCKET_ERROR) {
			WSACleanup();
			closesocket(soq);
			printf("\nFalha na conexao....");
			system("pause");
			exit(1);
		}
		
		//Verifica se foi conectado....
		if(strcmp(conectado,"CONECTADO") == 0){
			printf("%s", conectado);
			ajuste(2,0);	printf("Recebendo dados....");
			//Recebe dados para o jogo....
			if(recvfrom(soq, recebePalavra, TAMANHO_BUFFER, 0,(struct sockaddr *) &remota, &tamanhoRemota) != SOCKET_ERROR) {	//recebe palavra
				recebido++;
				ajuste(2,0);	printf("Palavra recebida com sucesso!");
				//printf("%s", recebePalavra);
				ajuste(2,0);	printf("Aguardando Dica....");
				
				if(recvfrom(soq, recebeDica, TAMANHO_BUFFER, 0,(struct sockaddr *) &remota, &tamanhoRemota) != SOCKET_ERROR){	//recebe dica
					ajuste(2,0);	printf("Dica recebida com sucesso!");
					//printf("%s", recebeDica);
					recebido++;
				}
			}
			
			else {
				printf("\nOps....");
			}
		}
		
	}while(recebido != 2);
	
	closesocket(soq);
	printf("conexao encerrada....");
	Sleep(1000);
	
}

void criaConexao(){

	if (WSAStartup(MAKEWORD(2, 0), &wsa_data) != 0) {
        printf("WSAStartup() falhou!\n");
    }
	
	soq = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    
    if (soq == INVALID_SOCKET){
		closesocket(soq); // fecha conexão do socket
        printf("Perdeu PLayboy!!!!");
        exit(1);
    }
	
	// preparação do soquete para enviar um broadcast 
	memset(&local, 0, sizeof(local));      
	setsockopt(soq, SOL_SOCKET, SO_BROADCAST,&broadcast, sizeof(broadcast)) ;
	local.sin_family = PF_INET;
	local.sin_addr.s_addr = INADDR_BROADCAST; // endereço de broadcast
	local.sin_port = htons(PORTAESCUTA);      // Porta 666
	
	memset(&enviaPalavra, 0, MAX);
	strcpy(enviaPalavra,"CONECTADO");
 
	// Envia mensagem para porta 2008 de todas as máquinas na sub-rede 
	if (sendto(soq,enviaPalavra,(strlen(enviaPalavra)+1),0,(struct sockaddr *)&local, sizeof(local))== SOCKET_ERROR){
		WSACleanup();
		closesocket(soq);
		printf("\nDeu derrame no envio....");
		system("pause");
		exit(1);
    }
	
	else {
		ajuste(2,0);	printf("Digite uma palavra para enviar ao adversario: ");
		
		memset(&enviaPalavra, 0, MAX);
		scanf("%s", &enviaPalavra);
		
		if (sendto(soq,enviaPalavra,(strlen(enviaPalavra)+1),0,(struct sockaddr *)&local, sizeof(local)) != SOCKET_ERROR){
			ajuste(2,0);	printf("Palavra enviada com sucesso....");
			ajuste(2,0);	printf("Digite a dica para a palavra: ");
			
			memset(&enviaDica, 0, MAX);
			scanf("%s", &enviaDica);
			
			if (sendto(soq,enviaDica,(strlen(enviaDica)+1),0,(struct sockaddr *)&local, sizeof(local)) == SOCKET_ERROR){
				WSACleanup();
				closesocket(soq);
				printf("\nDeu derrame no envio....");
				system("pause");
				exit(1);
			}
			
			else {
				ajuste(2,0);	printf("Dica enviada com sucesso....");
			}
		}
		
		else {
			WSACleanup();
			closesocket(soq);
			printf("\nDeu derrame no envio....");
			system("pause");
			exit(1);
		}
	}
}


void cadastrar(char palavraRecebida[], char dicaRecebida[]) {
	int cont=0;
	palavra.id=1;
	ponteiro=fopen("Palavras.dat","ab+");
	fread(&palavra,sizeof(palavra),1,ponteiro);
	while(!feof(ponteiro)){
		palavra.id++;
		fread(&palavra,sizeof(palavra),1,ponteiro);
	}
	
	palavra.estado=1;
	strcpy(palavra.palavra,palavraRecebida);
	strcpy(palavra.dica,dicaRecebida);
	
	palavra.nivel=1;	//por padrão, todas as palavras são faceis
	maiusculo(palavra.palavra);
	palavra.dica[0]=toupper(palavra.dica[0]);
	
	struct palavras palavra2;
	
	rewind(ponteiro);
	fread(&palavra2,sizeof(palavra2),1,ponteiro);
	while(!feof(ponteiro)){
		if(strcmp(palavra.palavra,palavra2.palavra)==0){
			cont++;
		}
		fread(&palavra2,sizeof(palavra2),1,ponteiro);
	}
	
	if(cont==0){
		fwrite(&palavra,sizeof(palavra),1,ponteiro);
	}
	
	fclose(ponteiro);
}
