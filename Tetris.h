#ifndef TETRIS_H
#define TETRIS_H

#include<iostream>
using namespace std;

class Tetris {
public:
	Tetris();
        Tetris(int);
        Tetris(const Tetris &);
        Tetris & operator=(const Tetris &);
        ~Tetris();
									
        char get(int,int) const;
        void removeColuna(int);
        void removeLinhasCompletas();
        int getNumColunas()const;
        int getAltura(int)const;
        int getAltura()const; 
        int getAlturaMinima()const;
        bool adicionaForma(int,int,char,int);

private:
        char **jogo;
        int *alturas;
        int largura;

        void createjogo();
        void createalturas();
        void destroy();
};

#endif