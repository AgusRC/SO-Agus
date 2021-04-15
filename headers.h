
/*
 * headers.h
 *
 *  Created on: 16/03/2017
 *      Author: Agustín Rosales Corona
 */

void zero(char *string);

int verify(char *string, int type);

int esReg(char *str);

int esHex(char *str, int len);

void run(WINDOW *win);

void iniciarReg();

void speed(WINDOW *win);

void command(WINDOW *win);

void dispatch(WINDOW *win);

void show(WINDOW *win, int k);

void terminaejecucion();

int fork2();

int combinacio_operandos(listPCB *PCB1);

void inicializacion (ListaSE *lista);

int ejec_ins(listPCB *PCB1);

int obtOP2(FILE *file, int x, listPCB *PCB1);
int checkInstr(char *instr, listPCB *PCB1);
void obtHex(char *str, int len, int x, listPCB *PCB1);
int ins_nodo_en_lista_vacia (ListaSE * lista, listPCB *nuevo_elemento);
int ins_nodo_inicio_lista (ListaSE * lista, listPCB *nuevo_elemento);
int ins_fin_lista (ListaSE * lista, listPCB * actual, listPCB *nuevo_elemento);
int sup_inicio (ListaSE * lista);
void terminaejecucion();
void visualizacion(WINDOW *win);
listPCB *creaPCB(char *dir, int PPID);
void read_file(listPCB *PCB, int PPID, int lineas, WINDOW *win);
void iniciarRegNod(listPCB *nodo);
void fairshare(listPCB *PCB);
void mataPIDFamilia(int PID, WINDOW *win);


int banquero(int preAsign[]);
void unmark();
int todosMarcados();
void dispatchsusp(WINDOW *win);
int bloquear();
void desbloquear();
void liberarM();


