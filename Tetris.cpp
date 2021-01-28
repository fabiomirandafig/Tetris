#include "Tetris.h"
//Uma determinada coluna do jogo é identificada atraves da variavel c e uma determinada linha do jogo é identificada atraves da variavel i.

Tetris::Tetris(){//Construtor padrão.
	createjogo();//Inicializa o jogo e o número de colunas com valores padrões.
	createalturas();//Inicializa as alturas do jogo com valor nulo.
}

Tetris::Tetris(int largura_){//Construtor com um argumento.
	largura = largura_;
	jogo = new char*[largura];
	for(int c = 0; c < largura; c++)//Seta as colunas para NULL,já que elas nao possuem altura.
		jogo[c] = NULL;
	createalturas();//Inicializa as alturas do jogo com valor nulo.
}

Tetris::Tetris(const Tetris &tetris){//Construtor por cópia.
	createjogo();//Inicializa o jogo com valor padrão.
	createalturas();//Inicializa as alturas do jogo com valor nulo.
	*this = tetris;//atribui jogo.
}

Tetris & Tetris::operator=(const Tetris &other){//Sobrecarga do operador de atribuição.
	if(this == &other)
		return *this;

	destroy();
	this->largura = other.getNumColunas();
	
	createalturas();
	for(int c = 0; c < getNumColunas(); c++)
		alturas[c] = other.alturas[c];

	jogo = new char*[getNumColunas()];
	for(int c = 0; c < getNumColunas(); c++)
		jogo[c] = new char[other.getAltura(c)];
	for(int c = 0; c < getNumColunas(); c++)
		for(int i = 0; i < other.getAltura(c); i++)
			jogo[c][i] = other.jogo[c][i]; 
			
	return *this;
}

Tetris::~Tetris(){//Destrutor invoca o método destroy.
	destroy();//Responsável por fazer a limpeza dos arrays dinâmicos(jogo e altura) e zerar a variável largura.
}

void Tetris::createjogo(){//Inicializa o jogo e o número de colunas com valores padrões.
	jogo = NULL;
	largura = 0;
}

void Tetris::createalturas(){//Inicializa as alturas do jogo com valor nulo.
	alturas = new int[largura];
	for(int i = 0; i < getNumColunas(); i++)
		alturas[i] = 0;
}

void Tetris::destroy(){//Método responsável por fazer a limpeza dos arrays dinâmicos(jogo e altura) e zerar a variável largura.
	for(int c = 0; c < getNumColunas(); c++)
		delete[]jogo[c];
	delete[]jogo;

	delete[]alturas;
	largura = 0;  
}

char Tetris::get(int c,int i) const{//Retorna um caractere que representa o estado de tal pixel no jogo atual.
	if(c < 0 || c > (getNumColunas()-1) || i < 0 || (alturas[c]-1) < i )//Se a coluna for invalida(negativa ou após a ultima coluna existente)
		return ' ';														//ou a linha for invalida(negativa ou após a ultima linha alocada) retorna o caractere espaço em branco padrão. 
	return jogo[c][i];													
}

void Tetris::removeColuna(int c){//Remove a coluna do jogo (diminuindo a sua largura).
	if(c < 0 || c >= getNumColunas() || getNumColunas() == 0)//Tentar remover uma coluna inexistente(negativa ou após a ultima coluna existente)
		return;												//ou se o jogo nao tiver largura(numero de colunas igual a zero), é uma operação inválida. 			
		
	//Removendo a coluna c do jogo e movendo as colunas póstumas uma posição a sua esquerda.
	for(int j = c; j < getNumColunas()-1; j++){//Altera-se a coluna que será removida e as colunas subsequentes.
		delete[]jogo[j];//Remove a coluna c passada como argumento.
		jogo[j] = new char[getAltura(j+1)];//Na localidade da coluna removida anteriormente, aloca-se uma nova coluna com a altura da coluna seguinte.
		for(int i = 0; i < getAltura(j+1); i++)//Adiciona o conteudo da coluna seguinte para a sua nova localidade no jogo(uma posição à esquerda).
			jogo[j][i] = get(j+1,i);
		alturas[j] = alturas[j+1];//A altura da nova coluna é igual à altura da coluna seguinte.
	}

	delete[]jogo[getNumColunas()-1];//Após remover a coluna c do jogo e mover as colunas subsequentes para às suas novas localidades(uma coluna à esquerda),
	largura--;						//a ultima coluna do jogo ja foi movida porem ainda há uma cópia sua na posiçao original, que deve ser desalocada.				

	int *aux;//Atualizando o array de alturas com a quantidade correta de posiçoes,isto é, como uma coluna foi removida, há uma posição a menos no array.
	aux = new int[getNumColunas()];
	for(int i = 0; i < getNumColunas(); i++)
		aux[i] = alturas[i];
	delete []alturas;
	alturas = aux;
}

void Tetris::removeLinhasCompletas(){//Remove todos os pixels do jogo que estiverem em linhas completas. 
	char **jogoaux;//Matriz auxiliar criada para armazenar o estado atual do jogo.
	jogoaux = new char*[getNumColunas()];
	for(int c = 0; c < getNumColunas(); c++)
		jogoaux[c] = new char[getAltura(c)];
	for(int c = 0; c < getNumColunas(); c++)
		for(int i = 0; i < getAltura(c); i++)
			jogoaux[c][i] = get(c,i); 

	int linhas = getAlturaMinima();	//Será percorrida até à altura minima do jogo, pois é certo que após ela não ha linhas completas.  
	bool naoremove;

//Removendo linhas completas e desalocando/alocando colunas para mantê-las "justas".
	for(int i = 0; i < linhas; i++){//Percorrendo as linhas do jogo.
		naoremove = false;//Se essa variável possui valor falso, é removido uma linha completa.
		for(int c = 0; c < getNumColunas(); c++){//Percorrendo as colunas do jogo.
			if(jogo[c][i] == ' '){//Se ha espaço nao utilizado numa linha do jogo, ela nao será removida e portanto verifica a linha seguinte.
				naoremove = true;
				break;
			}
		}
		if(naoremove)//A linha verificada possui espaco nao utilizado e portanto nao sera removida e deve verificr a linha subsequente.
			continue;

		//Se chegamos aqui, então foi encontrada uma linha completa e ela será removida.
		for(int c = 0; c < getNumColunas(); c++)//Deleta todas as colunas atuais do jogo.
			delete[]jogo[c];

		for(int c = 0; c < getNumColunas(); c++){//Realocando as novas colunas do jogo com suas respctivas alturas atualizadas.
			if(getAltura(c) == 1)//Caso especial onde a coluna c possui altura igual a 1 e como sua nova altura será zero, a coluna nao precisa ser alocada novamente.
				jogo[c] = NULL;
			jogo[c] = new char[getAltura(c)-1];//Como vai ser removida uma linha completa do jogo,cada coluna tem sua altura diminuida em uma unidade.
		}

		for(int c = 0; c < getNumColunas(); c++){//Insere caractere por caractere em cada coluna, "ignorando" somente o qual fazia parte da linha completa.
			for(int j = i; j < getAltura(c)-1; j++){
				if(j >= i)//Aqui nao insere o caractere que faz parte da linha completa que deve ser removida.
					jogo[c][j] = jogoaux[c][j+1];//Para "ignorar" um caractere de uma posição,é pego o caractere seguinte,deixando-o de fora do novo estado do jogo.
				else
					jogo[c][j] = jogoaux[c][j];
			}
		}

		for(int k = 0; k < getNumColunas(); k++){//Atualiza cada altura com uma unidade a menos, visto que uma linha foi removida.
			alturas[k]--;
		}
		i--;//Necessário verificar novamente a linha i que foi removida, ja que a linha seguinte foi deslocada para baixo e pode estar completa.
		linhas--; //Como foi removida uma linha, a altura minima do jogo diminui e portanto ocorrerá uma verificação a menos.
	}

	for(int c = 0 ; c < getNumColunas(); c++)//Desalocando o jogo auxiliar criado anteriormente.
		delete[]jogoaux[c];
	delete[]jogoaux; 
}

int Tetris::getNumColunas() const{//Retorna o número de colunas (largura) do jogo Tetris. 
	return largura;
}

int Tetris::getAltura(int c) const{//Retorna a altura da coluna c do jogo.
	if(c < 0 || c >= getNumColunas())//Se tentar acessar uma coluna inválida do jogo(negativa ou após a ultima coluna existente)
		return 0;					 //retorna o valor padrão zero como altura.
	return alturas[c];
}

int Tetris::getAltura() const{//Retorna a altura maxima do jogo atual.
	int alturamaxima = 0; 
	for(int i = 0; i < getNumColunas(); i++)
		if(alturas[i] > alturamaxima)//Se a altura comparada for maior que a maior altura atual, ela será a altura máxima do jogo.
			alturamaxima = alturas[i];//Atribui a altura máxima, que será retornada posteriormente.
	return alturamaxima;//Retorno da altura máxima do jogo.
}

int Tetris::getAlturaMinima() const{//Retorna a altura minima do jogo atual,utilizada no método removelinhascompletas.
	int alturaminima = alturas[0];
	for(int i = 1; i < getNumColunas(); i++)
		if(alturas[i] < alturaminima)//Se a altura comparada for menor que a menor altura atual, ela será a altura minima do jogo.
			alturaminima = alturas[i];//Atribui a altura minima, que será retornada posteriormente.
	return alturaminima;;//Retorno da altura minima do jogo.
}

bool Tetris::adicionaForma(int coluna, int linha, char id, int rotacao){
	char peca[4][4];//Matriz responsável por armazenar alguma das sete peças disponíveis.
					//Logo abaixo, está declarado cada peça individualmente ou em pares(exceto a peça O)de acordo com a sua respectiva rotação.
					//A matriz peça foi criada atraves do indice[i][j] sendo i representando uma linha e j representando uma coluna.
	if(id == 'I'){
		if(rotacao == 0 || rotacao == 180){
		    char peca[4][4] = {'I',' ',' ',' ',
							   'I',' ',' ',' ',
							   'I',' ',' ',' ',
							   'I',' ',' ',' ' };
		}
		else if(rotacao == 90 || rotacao == 270){
			char peca[4][4] = {'I','I','I','I',
							   ' ',' ',' ',' ',
							   ' ',' ',' ',' ',
							   ' ',' ',' ',' ' };
		}
	}
	else if(id == 'J'){
		if(rotacao == 0){
			char peca[4][4] = {'J','J','J','J',
							   ' ',' ',' ','J',
							   ' ',' ',' ',' ',
							   ' ',' ',' ',' ' };
		}
		else if(rotacao == 90){
			char peca[4][4] = {' ','J',' ',' ',
							   ' ','J',' ',' ',
					           ' ','J',' ',' ',
					           'J','J',' ',' ' };
		}
		else if(rotacao == 180){
			char peca[4][4] = {'J',' ',' ',' ',
							   'J','J','J','J',
					           ' ',' ',' ',' ',
					           ' ',' ',' ',' ' };
		}
		else if(rotacao == 270){
			char peca[4][4] = {'J','J',' ',' ',
	 		  				   'J',' ',' ',' ',
					           'J',' ',' ',' ',
					           'J',' ',' ',' ' };
		}
	}
	else if(id == 'L'){
		if(rotacao == 0){
			char peca[4][4] = {'L','L','L','L',
				               'L',' ',' ',' ',
				               ' ',' ',' ',' ',
				               ' ',' ',' ',' ' };
		}
		else if(rotacao == 90){
			char peca[4][4] = {'L','L',' ',' ',
					           ' ','L',' ',' ',
					           ' ','L',' ',' ',
					           ' ','L',' ',' ' };
		}
		else if(rotacao == 180){
			char peca[4][4] = {' ',' ',' ','L',
					           'L','L','L','L',
					           ' ',' ',' ',' ',
					           ' ',' ',' ',' ' };
		}
		else if(rotacao == 270){
			char peca[4][4] = {'L',' ',' ',' ',
					           'L',' ',' ',' ',
					           'L',' ',' ',' ',
					           'L','L',' ',' ' };
		}
	}
	else if(id == 'O'){
			char peca[4][4] = {'O','O',' ',' ',
		                	   'O','O',' ',' ',
				               ' ',' ',' ',' ',
				               ' ',' ',' ',' ' };
	}
	else if(id == 'S'){
		if(rotacao == 0 || rotacao == 180){
			char peca[4][4] = {' ','S','S',' ',
							   'S','S',' ',' ',
							   ' ',' ',' ',' ',
					           ' ',' ',' ',' ' };
		}
		else if(rotacao == 90 || rotacao == 270){
			char peca[4][4] = {'S',' ',' ',' ',
					           'S','S',' ',' ',
					           ' ','S',' ',' ',
					           ' ',' ',' ',' ' };
		}
	}
	else if(id == 'T'){
		if(rotacao == 0){
			char peca[4][4] = {'T','T','T',' ',
		       			       ' ','T',' ',' ',
					           ' ',' ',' ',' ',
					           ' ',' ',' ',' ' };
		}
		else if(rotacao == 90){
			char peca[4][4] = {' ','T',' ',' ',
					           'T','T',' ',' ',
					           ' ','T',' ',' ',
					           ' ',' ',' ',' ' };
		}
		else if(rotacao == 180){
			char peca[4][4] = {' ','T',' ',' ',
		      			       'T','T','T',' ',
					           ' ',' ',' ',' ',
					           ' ',' ',' ',' ' };
		}
		else if(rotacao == 270){
			char peca[4][4] = {'T',' ',' ',' ',
		          			   'T','T',' ',' ',
					           'T',' ',' ',' ',
					           ' ',' ',' ',' ' };
		}
	}
	else if(id == 'Z'){
		if(rotacao == 0 || rotacao == 180){
			char peca[4][4] = {'Z','Z',' ',' ',
		       			       ' ','Z','Z',' ',
					           ' ',' ',' ',' ',
					           ' ',' ',' ',' ' };
		}
		else if(rotacao == 90 || rotacao == 270){
			char peca[4][4] = {' ','Z',' ',' ',
 				               'Z','Z',' ',' ',
					           'Z',' ',' ',' ',
					           ' ',' ',' ',' ' };
		}
	}
	
	//Abaixo segue o calculo da quantidade de linhas e colunas que serão utilizadas da matriz peca,isto é,até qual linha e colunas está presente algum caractere.
	bool verifica;
	int maximocoluna = 4;//Inicialmente suponho que há pelo menos um caractere presente em cada coluna da matriz peca,logo são utilizadas todas as colunas da matriz peca.
	int maximolinha = 4;//Inicialmente suponho que há pelo menos um caractere presente em cada linha da matriz peca,logo são utilizadas todas as linhas da matriz peca.
	for(int i = 0; i < 4; i++){
		verifica = true;
		for(int j = 0; j < 4; j++){//Percorrendo as linhas da matriz peca.
			if(peca[i][j] != ' '){//Se há caractere numa determinada posição da matriz peca, logo a linha dessa posição será utilizada e
				verifica = false;    //portanto, ainda nao foi encontrada a linha da matriz que não há caractere.
				break;
			}
		}
		if(verifica){//Se percorreu uma linha completa da matriz peca e nao foi encontrado nenhum caractere, 
			maximolinha = i;//significa que será utilizada as linhas acima da linha i e portanto, i linhas.			  			
			break;
		}
	}

	for(int j = 0; j < 4; j++){
		verifica = true;
		for(int i = 0; i < 4; i++){//Percorrendo as colunas da matriz peca.
			if(peca[i][j] != ' '){//Se há caractere numa determinada posição da matriz peca, logo a coluna dessa posição será utilizada e
				verifica = false; //portanto, ainda nao foi encontrada a coluna da matriz que não há caractere.
				break;
			}
		}
		if(verifica){        //Se percorreu uma coluna completa da matriz peca e nao foi encontrado nenhum caractere, 
			maximocoluna = j;//significa que será utilizada as colunas anteriores da coluna j e portanto, j colunas.		
			break;
		}
	}

//Confere se a possível peça a ser adicionada extrapola os limites da matriz jogo, isto é, atinge posiçoes à esquerda da primeira coluna do jogo,à direita da ultima coluna do jogo ou abaixo da primeira linha.
	if((coluna < 0) || (maximocoluna > (getNumColunas()-coluna)) || ((linha+1) - maximolinha) < 0){
		return false;
	}

//Confere se uma peca não pode ser adicionada ao jogo porque um de seus pixels (não vazios) “colide” com um pixel já presente no jogo.
//Há uma relaçao entre a posição do jogo e da matriz peca, isto é, peca[linha-i][c-coluna] refere-se a posicao jogo[c][i] na matriz jogo. 
	for(int i = linha; i > linha - maximolinha; i--){//Percorrendo as linhas onde a peça eventualmente pode ser adicionada
		for(int c= coluna; c < coluna + maximocoluna; c++){//Percorrendo as colunas onde a peça eventualmente pode ser adicionada
			if(peca[linha-i][c-coluna] != ' '){//Se a posição conferida da peça há caractere...
				if(getAltura(c) < i+1)//Se estou tentanto adicionar a peça numa posição que nao foi alocada ainda na coluna, nao há conflito e portanto nao deve retornar false.
					continue;       //verifico a proxima coluna.
				else if(jogo[c][i] != ' ')//Se uma posicao da matriz peça ha caractere e a posição que esse caractere deveria ser adicionado no jogo ja há algum caractere, temos um conflito
					return false;		//e portanto,deverá retornar false.
			}
		}
	}

//Abaixo, estão as operações para adicionar a peça no jogo e manter as colunas "justas".
	int novaaltura = 0;//Nova altura se necessário redimensionar uma determinada coluna.
	char *aux;//Array auxiliar que armazena o conteudo da coluna que sera realocada. Necessária para identificar os caracteres ja presentes antes da nova peça que será adicionada.

//Verifica em qual linha do jogo o pixel superior esquerdo da peca vai ser adicionado e portanto se a nova altura dessa coluna após a inserção sera maior ou nao.
	for(int c = coluna; c < coluna + maximocoluna; c++){
		for(int i = linha; i > linha - maximolinha; i--){
			if(peca[linha-i][c-coluna] != ' '){//Se há caractere..
				novaaltura = i+1;//Se o pixel superior esquerda da peça é adicionado por exemplo na linha 1, então a altura será 2, visto que serao utilizadas as linhas 1 e 0 do jogo.
				break;
			}
		}
		if(getAltura(c) < novaaltura){//Se a nova altura da coluna c é maior que a anterior, será necesario uma novo alocação dessa coluna.
			aux = new char[novaaltura];//Array auxiliar alocado com a nova altura da coluna que esta sendo adicionada a peça.
			for(int i = 0; i < novaaltura; i++)//Armazeno em todo o array espaço em branco, para evitar lixo de memória.
				aux[i] = ' ';
			for(int i = 0; i < getAltura(c); i++)//Copio para o array auxiliar o conteudo atual(antes de inserir a peça) da coluna c.
				aux[i] = get(c,i);

			for(int i = linha; i > linha - maximolinha; i--){//Adiciona no array auxiliar os caracteres novos.
				if(peca[linha-i][c-coluna] != ' '){//Se nessa posição da matriz peça há um caractere...
					aux[i] = peca[linha-i][c-coluna];//Adiciona.
				}
			}

			delete[]jogo[c];//Desaloca a coluna c.
			jogo[c] = aux;//Aponta a coluna c para o array auxiliar, que agora está com os caracteres antigos e os que foram adicionados.
			alturas[c] = novaaltura;//Atualizado a nova altura da coluna c.
		}	
		else {//Se a nova altura da coluna c é menor ou igual que a altura atual, não é necesario uma nova alocação dessa coluna,basta somente adicionar os caracteres da peça nas posições correspondentes.
			for(int i = linha; i > linha - maximolinha; i--){//coloca a peça na coluna
				if(peca[linha-i][c-coluna] != ' ')//Se nessa posição da peça há um caractere...
					jogo[c][i] = peca[linha-i][c-coluna];//Adiciona o caractere da peca no jogo.
			}
		}
	}
	return true;
}