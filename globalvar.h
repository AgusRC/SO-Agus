
/*
 * globalvar.h
 *
 *  Created on: 16/03/2017
 *      Author: Agustín Rosales Corona
 */

//#define ERR -1
#define END 	 7
#define ERRINS 	-2
#define ERROP 	-3
#define OUTRANGE -4
#define DIVZERO -5
#define NOEND 	-7
#define NOAUT 	-8

//Estados
#define EJECUC 100
#define LISTOS 101
#define TERMIN 102
#define SUSPEN 103
#define BLOCK  104

//tipSusp
#define NODISP -201
#define RIESGO -202

//Memoria
int M[4]= {10, 10, 10, 10};

int row, col, x, y;

int timeE = 100;			//tiempo entre ejecucion de cada instruccion

char OP2[9] = {'0','0','0','0','0','0','0','0','\0'};
char *registros[] = {"AX","BX","CX","DX"};
char caracter;
char sepOP[2];

FILE *globalFile;
int AX, BX, CX, DX;		//Registros
int PC = 0;			//Program Count
int lines;
int debug = 0;
int timer = 5;			//timer de cada proceso
int avPID = 0;			//contador de PID
int actPID;			//PID del proceso en ejecucion
float Wk;
int ng = 0;			//numero de grupos

int MemDisp[4] = 	{10, 10, 10, 10};


typedef struct _nPCB {
	FILE *archivo;		// Apuntador al archivo de instrucciones
	int PID;		// Identificador del Proceso
	int PPID;
	int base;		//Prioridad base
	float prioridad;	//prioridad calculada
	float CPU;		//tiempo de CPU consumido por el proceso
	float GCPU;		//tiempo de GCPU consumido por el grupo
	int Rx[4];		// Registros propios del proceso Ax, Bx, Cx, Dx.
	int PC;			//Contador del programa
	int estado;		//Estado actual del proceso
	int tipSusp;		//Razon de estar suspendido -201: recursos no disp | -202: asignacion riesg.
	char nombre[80];	//Nombre del archivo de instrucciones
	int seek;		//posicion en el fichero
	int info;		//Informacion adicional (errores)
	int MemMaxAssign[4];	//Maximo
	int MemAssign[4];	//Asignado
	int Marca;		//0: desmarcado | 1:marcado
	time_t timeBlock;
	time_t tStart;
	struct IR{
		char Ins[4];
		int typeIns;
		char OP1[3];
		int typeOP2; 	//1:Hex|0:registro
		char OP2[3];
		int regID;	//identifica que registro tiene el OP2 AX:0, BX:1, CX:2, DX:3
		char Hex[9];
		int valHex;
	} IR[1000];
	struct _nPCB *sigPCB;	//Apuntador al Siguiente proceso.
	struct _nPCB *Hijo1;
	struct _nPCB *hermano;
	struct _nPCB *sigGrupo;
}listPCB;



typedef struct ListaIdentificar {
  listPCB *first;
  listPCB *last;
  int size;
}ListaSE;




//Iniciar la lista
ListaSE *Listos; // Inicializa lista de listos
ListaSE *Ejec;	//
ListaSE *Term;	//

ListaSE *susp;
ListaSE *block;


