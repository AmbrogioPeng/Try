#include<stdio.h>
#include<easyx.h>//grafica del programma 
#include<stdlib.h>
#include<time.h>
#include<Windows.h>
#pragma comment(lib,"Winmm.lib")

IMAGE bk1;
IMAGE bk2;
IMAGE endG;
IMAGE bk;//una variabile di immagine
IMAGE animale[17];//icona degli animali
IMAGE tmp;//d'appoggio che uso per disordinare l'immagine
IMAGE domanda;
int turno = 0;

enum coloreanimale
{
	rosso,
	blu
};

struct spos
{
	int row;
	int col;
}inizio = { -1,-1 },
fine = { -1,-1 };


typedef struct card//struttura della posizione delle carte
{
	//posizione x e y
	int row;
	int col;
	int n;  // tipo di animale 1=topo 2=gatto etc..
}card;

card matrice[4][4];//matrice X animali 
int stato = 0;

//funzioni X x e y della matrice
int cardx(int n)
{
	int temp = n * (100 + 10) + 57;
	return temp;
}

int cardy(int n)
{
	int temp = n * (102 + 35) + 215;
	return temp;
}

void caricamento()//caricamento 
{//assegnazione 
	loadimage(&bk, "./Grafica/back.png");//caricamento dello sfondo
	loadimage(&bk1, "./Grafica/back1.jpg");//caricamento dello sfondo
	loadimage(&bk2, "./Grafica/back2.jpg");//caricamento dello sfondo
	loadimage(&endG, "./Grafica/endgame.jpg");//caricamento dello sfondo
	//NULL
	loadimage(&animale[0], "./Grafica/icon.jpg");
	//rosso team
	loadimage(&animale[1], "./Grafica/toporosso.jpg");
	loadimage(&animale[2], "./Grafica/gattorosso.jpg");
	loadimage(&animale[3], "./Grafica/canerosso.jpg");
	loadimage(&animale[4], "./Grafica/luporosso.jpg");
	loadimage(&animale[5], "./Grafica/ghepardorosso.jpg");
	loadimage(&animale[7], "./Grafica/leonerosso.jpg");
	loadimage(&animale[6], "./Grafica/tigrerosso.jpg");
	loadimage(&animale[8], "./Grafica/elefanterosso.jpg");
	//team blu
	loadimage(&animale[9], "./Grafica/topoblu.jpg");
	loadimage(&animale[10], "./Grafica/gattoblu.jpg");
	loadimage(&animale[11], "./Grafica/caneblu.jpg");
	loadimage(&animale[12], "./Grafica/lupoblu.jpg");
	loadimage(&animale[13], "./Grafica/ghepardoblu.jpg");
	loadimage(&animale[15], "./Grafica/leoneblu.jpg");
	loadimage(&animale[14], "./Grafica/tigreblu.jpg");
	loadimage(&animale[16], "./Grafica/elefanteblu.jpg");

	loadimage(&domanda, "./Grafica/domanda.jpg");
	return;
}

IMAGE getimage(int n)
{
	if (n >= 20)
	{
		return domanda;
	}
	else
	{
		return animale[n];
	}
}

void ini(card* carta, int row, int col, int n)
{
	carta->row = row;
	carta->col = col;
	carta->n = n;
	return;
}


void iniz()
{
	srand(time(NULL));//random numero
	int conta = 1;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ini(&matrice[i][j], i, j, conta);
			conta++;
			printf("%d ", matrice[i][j].n);
		}
		printf("\n");
	}
	//disordinare 
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			int a = rand() % 4;//random numero
			int b = rand() % 4;
			card tp = matrice[i][j];
			matrice[i][j] = matrice[a][b];
			matrice[a][b] = tp;//disordino a caso con random
		}
	}

	//lo copro
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			matrice[i][j].n += 20;
		}
	}


	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			printf("%d ", matrice[i][j].n);
		}
		printf("\n");
	}

	return;
}


void mettere()
{
	int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			int x, y;
			tmp = getimage(matrice[i][j].n);
			x = cardx(j);
			y = cardy(i);
			putimage(x, y, &tmp);
		}
	}
	return;
}

bool in(int x, int y, int sx, int tp, int w, int h)
{
	if (x >= sx && x <= sx + w && y >= tp && y <= tp + h)
	{
		return true;
	}
	return false;
}

int cartecolore(card* carta)
{
	if (carta->n >= 0 && carta->n <= 8)
	{
		return rosso;
	}
	else if (carta->n >= 9 && carta->n <= 16)
	{
		return blu;
	}
}

bool cartastessocolore(card* card1, card* card2)
{
	int q, e;
	q = cartecolore(card1);
	e = cartecolore(card2);
	if (q == e)
	{
		return false;
	}
	else if (q != e)
	{
		return true;
	}

}

bool mangiacarta(card* cibo, card* cons)
{
	//trasformo da maggiore di 8 in minore di 8
	int num1 = cibo->n;
	int num2 = cons->n;
	num1 = num1 > 8 ? cibo->n - 8 : num1;//"? :" = si? altrimenti..
	num2 = num2 > 8 ? cons->n - 8 : num2;

	/*
	non fanno della stessa parte
	grande mangia piccolo
	topo mangia elefante
	*/
	if (cartastessocolore(cibo, cons) && num1 <= num2 || (num1 == 8 && num2 == 1))
	{
		return true;
	}
	else
	{
		return false;
	}

}


void azione()
{
	/*
	inizio != fine
	inizio fine stessa colonna
	inizio team != fine team
	inizio.n >= fine.n
	*/

	if ((inizio.row != -1 && fine.row != -1) && !(inizio.row == fine.row && inizio.col == fine.col))
	{
		//stessa riga o colonna valore assoluto -> |x|
		if ((abs(inizio.row - fine.row) == 1 && inizio.col == fine.col) || (abs(inizio.col - fine.col) == 1 && inizio.row == fine.row))
		{
			//mangiare
			bool ok = mangiacarta(&matrice[fine.row][fine.col], &matrice[inizio.row][inizio.col]);
			//spostamento
			if (ok || matrice[fine.row][fine.col].n == 0)
			{
				matrice[fine.row][fine.col] = matrice[inizio.row][inizio.col];
				matrice[inizio.row][inizio.col].n -= matrice[inizio.row][inizio.col].n;
				inizio = fine = { -1,-1 };

			}
		}
	}

	return;
}

//X rigirare la carta
void mousel(ExMessage* msg)
{

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			//check (i;j)
			if (in(msg->x, msg->y, cardx(j), cardy(i), 102, 100))
			{
				//check valore della carta
				if (matrice[i][j].n >= 20)
				{
					if (turno % 2 == 0 && stato != 1)
					{
						matrice[i][j].n -= 20;
						printf("inizio(%d %d) turno rosso\n", i, j);
						turno++;
					}
					else if (turno % 2 != 0 && stato != 1)
					{
						matrice[i][j].n -= 20;
						printf("inizio(%d %d) turno blu\n", i, j);
						turno++;
					}
				}
				else//altrimenti
				{
					if (stato == 0)
					{
						if (matrice[i][j].n != 0)
						{
							if (turno % 2 == 0 && matrice[i][j].n <= 8 && matrice[i][j].n > 0)
							{
								inizio = { i,j };
								stato = 1;
								printf("inizio(%d %d) turno rosso\n", i, j);
								turno++;
							}
							else if (turno % 2 != 0 && matrice[i][j].n > 8 && matrice[i][j].n <= 16)
							{
								inizio = { i,j };
								stato = 1;
								printf("inizio(%d %d) turno blu\n", i, j);
								turno++;
							}
						}
					}
					else if (stato == 1)
					{
						fine = { i,j };
						if ((abs(inizio.row - fine.row) == 1 && inizio.col == fine.col) || (abs(inizio.col - fine.col) == 1 && inizio.row == fine.row))
						{
							if (mangiacarta(&matrice[fine.row][fine.col], &matrice[inizio.row][inizio.col]) == true)
							{
								stato = 0;
								printf("fine(%d %d)\n", i, j);
							}
							else
							{
								stato = 0;
								printf("Processo annulato\n");
							}
						}
						
					}
				}

			}
		}
	}
	return;
}

void musica()
{
	mciSendString(_T("open ./musica/binks.mp3"), 0, 0, 0);
	mciSendString(_T("play ./musica/binks.mp3"), 0, 0, 0);        //loop musica
	return;
}

void musicaFine()
{
	mciSendString("close ./musica/binks.mp3", 0, 0, 0);
	mciSendString(_T("open ./musica/good.mp3"), 0, 0, 0);
	mciSendString(_T("play ./musica/good.mp3"), 0, 0, 0);        //loop musica
	putimage(0, 0, &endG);//schermata finale
	return;
}

void be()
{
	while (true)
	{
		putimage(0, 0, &bk1);//output dell'immagine
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)///tasto return
		{
			Sleep(1000);
			break;
		}
	}

	while (true)
	{

		putimage(0, 0, &bk2);//output dell'immagine
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)//tasto space
		{
			break;
		}
	}
	putimage(0, 0, &bk);//output dell'immagine
	return;
}


int main()
{

	initgraph(534, 949);//la schermata del easyx.h
	caricamento();//funzione
	iniz();//ante programma
	musica();
	be();

	while (true)//esce quando pigio exit (exit = false)
	{

		mettere();
		//controllo del messaggio
		ExMessage msg;
		//ricevere messaggio
		while (peekmessage(&msg, EX_MOUSE))
		{
			if (msg.message == WM_LBUTTONDOWN)
			{
				//stampare quello che succede
				mousel(&msg);
			}
		}
		azione();

		if (GetAsyncKeyState(VK_MENU) & 0x8000)//alt X terminare
		{
			musicaFine();
			break;
		}
			
	}

	printf("Fine Gioco");

	system("PAUSE");//controllo end
	return 0;
}