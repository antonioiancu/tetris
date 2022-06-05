#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#define TRUE 1
#define FALSE 0
#define LINES 10 
#define COLS 10


char Table[LINES][COLS] = {0};	                  // Tableau de jeu
int score = 0;	                                  // score du joueur
char GameOn = TRUE;	                              // GameOn = TRUE si le jeu est en cours
suseconds_t timer = 800000;                       // 8 secondes pour déplacer une pièce
int decrease = 1000; 	
FILE *f; 										  // Fichier de score


typedef struct {                                  // Structure des pièces
    char **array;                                 
    int largeur, line, col;                       
} Tetrimino;         
Tetrimino current; 



const Tetrimino TetriminoArray[7]= {
	{(char *[]){(char []){1,1},(char []){1,1}}, 2},                                                      //tetrimino O
	{(char *[]){(char []){1,1,0},(char []){0,1,1}, (char []){0,0,0}}, 3},                                //tetrimino Z
	{(char *[]){(char []){0,1,0},(char []){1,1,1}, (char []){0,0,0}}, 3},                                //tetrimino T
	{(char *[]){(char []){1,0,0},(char []){1,1,1}, (char []){0,0,0}}, 3},                                //tetrimino J
	{(char *[]){(char []){0,0,1},(char []){1,1,1}, (char []){0,0,0}}, 3},                                //tetrimino L
	{(char *[]){(char []){0,1,1},(char []){1,1,0}, (char []){0,0,0}}, 3},                                //tetrimino S
	{(char *[]){(char []){0,0,0,0}, (char []){1,1,1,1}, (char []){0,0,0,0}, (char []){0,0,0,0}}, 4}      //tetrimino I

};








Tetrimino CopyShape(Tetrimino shape){                       // Copier une pièce dans le tableau
	Tetrimino new_shape = shape;
	char **copyshape = shape.array;
	new_shape.array = (char**)malloc(new_shape.largeur*sizeof(char*));
    int i, j;
    for(i = 0; i < new_shape.largeur; i++){
		new_shape.array[i] = (char*)malloc(new_shape.largeur*sizeof(char));
		for(j=0; j < new_shape.largeur; j++) {
			new_shape.array[i][j] = copyshape[i][j];
		}
    }
    return new_shape;
}






void DeleteShape(Tetrimino shape){                  		// Effacer la pièce
    int i;
    for(i = 0; i < shape.largeur; i++){
		free(shape.array[i]);
    }
    free(shape.array);
}



int CheckPosition(Tetrimino shape){  						// Verifier si la pièce est dans le tableau
	char **array = shape.array;
	int i, j;
	for(i = 0; i < shape.largeur;i++) {
		for(j = 0; j < shape.largeur ;j++){
			if((shape.col+j < 0 || shape.col+j >= COLS || shape.line+i >= LINES)){ 
				if(array[i][j]) 
					return FALSE;
				
			}
			else if(Table[shape.line+i][shape.col+j] && array[i][j])
				return FALSE;
		}
	}
	return TRUE;
}









void ChangeRandomTetrimino(){  																	  // Choisir une pièce aléatoirement
	Tetrimino new_shape = CopyShape(TetriminoArray[rand()%7]);

    new_shape.col = rand()%(COLS-new_shape.largeur+1);
    new_shape.line = 0;
    DeleteShape(current);
	current = new_shape;
	if(!CheckPosition(current)){
		GameOn = FALSE;
	}
}





void RotateTetrimino(Tetrimino shape){           									 // Tourner la pièce
	Tetrimino temp = CopyShape(shape);
	int i, j, k, largeur;
	largeur = shape.largeur;
	for(i = 0; i < largeur ; i++){
		for(j = 0, k = largeur-1; j < largeur ; j++, k--){
				shape.array[i][j] = temp.array[k][i];
		}
	}
	DeleteShape(temp);
}





void WriteTable(){                      										  // Ecrire le tableau de jeu
	int i, j;
	for(i = 0; i < current.largeur ;i++){
		for(j = 0; j < current.largeur ; j++){
			if(current.array[i][j])
				Table[current.line+i][current.col+j] = current.array[i][j];
		}
	}
}



void WipeTable(){              												  // Effacer le tableau de jeu
	int i, j;
	for(i = 0; i < current.largeur ;i++){
		for(j = 0; j < current.largeur ; j++){
			if(current.array[i][j])
				Table[current.line+i][current.col+j] = 0;
		}
	}
}



void MakeTable(){ 														   // Créer le tableau de jeu
	char Buffer[LINES][COLS] = {0};
	int i, j;
	for(i = 0; i < current.largeur ;i++){
		for(j = 0; j < current.largeur ; j++){
			if(current.array[i][j])
				Buffer[current.line+i][current.col+j] = current.array[i][j];
		}
	}
	clear();
	for(i=0; i<COLS-9; i++)
		printw(" ");
	printw("Tetris\n");
	for(i = 0; i < LINES ;i++){
		for(j = 0; j < COLS ; j++){
			printw("%c ", (Table[i][j] + Buffer[i][j])? '@': '-');
		}
		printw("\n");
	}
	printw("\nScore: %d\n", score);
}



void UpdateScore(){            											  // Mettre à jour le score et effacer la ligne complète
	int i, j, sum, count=0;
	for(i=0;i<LINES;i++){
		sum = 0;
		for(j=0;j< COLS;j++) {
			sum+=Table[i][j];
		}
		if(sum==COLS){
			count++;
			int l, k;
			for(k = i;k > 0;k--)
				for(l=0;l<COLS;l++)
					Table[k][l]=Table[k-1][l];
			for(l=0;l<COLS;l++)
				Table[k][l]=0;
			timer-=decrease--;
		}
	}
	score += 100*count;
}




void MoveTetrimino(int action){                 		// Déplacer la pièce
	Tetrimino temp = CopyShape(current);
	switch(action){
		case 'd':
			temp.col++;  
			if(CheckPosition(temp))
				current.col++;
			break;
		case 's':
			temp.line++;  
			if(CheckPosition(temp))
				current.line++;
			else {
				WriteTable();
				UpdateScore();
                ChangeRandomTetrimino();
			}
			break;	
		case 'q':
			temp.col--;  
			if(CheckPosition(temp))
				current.col--;
			break;
		case 'z':
			RotateTetrimino(temp); 
			if(CheckPosition(temp))
				RotateTetrimino(current);
			break;
	}
	DeleteShape(temp);
	MakeTable();
}


struct timeval avant, mntn;                 	
int hasToUpdate(){ 					   		// Vérifier si la pièce doit être mise à jour
	return ((suseconds_t)(mntn.tv_sec*500000 + mntn.tv_usec) -((suseconds_t)avant.tv_sec*500000 + avant.tv_usec)) > timer;
}



int main() {                                            	// Fonction principale
	system("COLOR 1A"); 							  	// Changer la couleur du terminal
	char name[30];
	printf("Quel est ton nom?: ");
	fgets(name, 10, stdin);
	printf("Bonjour %s! \n", name);
	srand(time(0));
    score = 0;
    int x;
    initscr();
	gettimeofday(&avant, NULL);
	timeout(1);
	ChangeRandomTetrimino();
    MakeTable();
	while(GameOn){
		if ((x = getch()) != ERR) {
		  MoveTetrimino(x);
		}
		gettimeofday(&mntn, NULL);
		if (hasToUpdate()) { 
			MoveTetrimino('s');
			gettimeofday(&avant, NULL);
		}
		if ((x = getch()) != ERR) {
		MoveTetrimino(x);
		}
		gettimeofday(&mntn, NULL);
	}
	DeleteShape(current);
	endwin();
	int i, j;
	for(i = 0; i < LINES ;i++){
		for(j = 0; j < COLS ; j++){
			printf("%c ", Table[i][j] ? '@': '-');
		}
		printf("\n");
	}
	printf("\n GG\n");  
	printf("\nScore: %d\n", score);
    f=fopen("HighScores.txt","w");
    fprintf(f,"%d %s",score, name); // Ecriture du score et nom dans le fichier
    fclose(f);
	WipeTable();
	clear ();
	return 0;
}
