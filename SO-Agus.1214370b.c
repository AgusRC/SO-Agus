/*
 * SO-Agus.1214370b.c
 *
 *  Created on: 16/03/2017
 *      Author: Agustín Rosales Corona
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include "globalvar.h"
#include "headers.h"
#include "func_aux.h"
#include "graphs.h"


char str[80];
listPCB *aux1;

int main(){

	//inicializacion(PCB);
	iniciarReg();

	
	Listos =  malloc (sizeof (ListaSE));
	Term = malloc (sizeof (ListaSE));
	Ejec = malloc (sizeof (ListaSE));
	susp = malloc(sizeof (ListaSE));
	block = malloc(sizeof (ListaSE));
	inicializacion(Listos);
	inicializacion(Term);
	inicializacion(Ejec);
	inicializacion(susp);
	inicializacion(block);

	WINDOW *win = initscr();
	initscr();
	if(has_colors() == FALSE)
	{	endwin();
		mvwprintw(win,0,0,"Your terminal does not support color\n");
		return -1;
	}
	start_color();			/* Start color 			*/
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	wbkgd(win, COLOR_PAIR(1));

	win = stdscr;
	getyx(win, y, x);


	getmaxyx(stdscr,row,col);	/* get the number of rows and columns */

	mvwprintw(win, 0, 0, "Sistema a1214370b");

	wrefresh(win);
	fflush(stdout);

	do{
		desbloquear();
		command(win);
	}while( strcmp(str, "salir") != 0 );

	wrefresh(win);		/* Print it on to the real screen */
	//wgetch(win);		/* Wait for user input */
	endwin();
	return 0;
}


void command(WINDOW *win){
	int numpid, PID_P = 0, lineas = 0, i, j;

		desbloquear();
		mvwprintw(win, 1, 2, "%s", "\n");
		mvwprintw(win, 1, 2, "%s", ">");
		getstr(str);
		

		if(strncasecmp(str, "ejecutar",8) == 0){
			mvwprintw(win, 2, 2, "\n");
			run(win);
		}
		else if(strcasecmp(str, "velocidad") == 0 || strcasecmp(str, "vel") == 0){
			mvwprintw(win, 2, 2, "\n");
			speed(win);
		}
		else if(strcasecmp(str, "salir") == 0){
			endwin();
			exit(0);
		}
		else if(strncasecmp(str, "mata", 4) == 0){
			mvwprintw(win, 2, 2, "\n");
			if(strlen(str) <= 4)return;
			numpid = atoi(&str[5]);
			mataPIDFamilia(numpid, win);
		}
		else if(strncasecmp(str, "fork", 4) == 0){
			for(i = 4; i<strlen(str); i++){
				if(isdigit(str[i])){
					PID_P = atoi(&str[i]);
					break;
				}
			}
			for(j=i+1; j<strlen(str); j++){
				if(isdigit(str[i])){
					lineas = atoi(&str[j]);
					break;
				}
			}
			fork2(PID_P, lineas, win);
		}
		else mvwprintw(win,2,2,"Comando no valido");
		desbloquear();
}



void run(WINDOW *win){
	int i;

	char str2[80];

	for(i=0; i<80; i++){			//romper ciclo cuando se encuentre "txt" (no necesario)
		str2[i] = str[9+i];
		if(str2[i] == 't' && i > 3){
			if(str2[i-1] == 'x'){
				if(str2[i-2] == 't'){
					break;
				}
			}
		}
	}
	str2[i+1]= '\0';

	mvwprintw(win, row-4,2,"\n");
	mvprintw(3, 0, "\n");
	mvprintw(3, 0, "Archivo: -%s-", str2);
	mvprintw(9,0,"\n");

	wrefresh(win);
	
	listPCB *PCB = (creaPCB(str2, -1));
	//Leer, revisar y ejecutar archivo
	if(PCB != NULL){
		ng++;
		wrefresh(win);
		read_file(PCB, -1, 0, win);
		}

}




void read_file(listPCB *PCB, int PPID, int lineas, WINDOW *win){
	int reg,i, i2, vr, inRange, ejec, k, ban;
	char instr[4];
	char OP1[3];
	char sepINOP[2];
	
	char aux;
	
	
	instr[4] =	'\0';
	OP1[3] =	'\0';
	sepINOP[2] =	'\0';
	sepOP[2] = 	'\0';
	

	//////////////////////////////////////////////////////////////////7
	if(Listos->first == NULL){
		ins_nodo_en_lista_vacia(Listos, PCB);
	}
	else ins_fin_lista(Listos, Listos->last, PCB);
	visualizacion(win);


	/*Procesamiento del programa*/
	while(Listos->first != NULL || Ejec->first != NULL || block->first != NULL || susp->first != NULL){
	
		desbloquear();
		
		//if(Listos->first == NULL)continue;
		
		dispatch(win);
		if(Ejec->first == NULL)continue;
		loadContext();
		
		visualizacion(win);
		wrefresh(win);
		
		for(k = 0; k < timer; k++){
			
			if(globalFile == NULL){
				mvwprintw(win, 0,0, "globalFile NULL");
				wrefresh(win);
				return;
			}
			
			//leer instruccion
			fgets(instr, 4, globalFile);

			if((vr = checkInstr(instr, Ejec->first)) == ERRINS){
				Ejec->first->info = ERRINS;
				terminaejecucion();
				visualizacion(win);
				return;
			}

			

			///////// ENcontrado END como intruccion
			if(Ejec->first->IR[PC].typeIns == END){
				mataPID(Ejec->first->PID, win);
				visualizacion(win);
				wrefresh(win);
				return;
			}

			strcpy(Ejec->first->IR[PC].Ins, instr);	//guarda en struct

			//verificar fin de instruccion------------------
			fgets(sepINOP, 2, globalFile);
			if(strcmp(sepINOP, " ") != 0 && Ejec->first->IR[PC].typeIns < 10){
				Ejec->first->info = ERRINS;//informa de falta de "_"
				terminaejecucion();
				visualizacion(win);
				return;
			}

			//Si instr es MAX
			if(Ejec->first->IR[PC].typeIns == 8 && PC == 0){
				Ejec->first->info = 8;
				for(i2 = 0; i2 < 4; i2++){
					fgets(&aux, 2, globalFile);
					Ejec->first->MemMaxAssign[i2] = atoi(&aux);
					
				}
				if( fgetc(globalFile) != '\n' )Ejec->first->info = ERROP;	
			}
			
			//Si es GET
			if(Ejec->first->IR[PC].typeIns == 9){
				int preAsign[4];
				Ejec->first->info = 9;
				for(i = 0; i < 4; i++){
					aux = fgetc(globalFile);
					preAsign[i] = atoi(&aux);
				}
				if( fgetc(globalFile) != '\n' )Ejec->first->info = ERROP;
				
				ban = banquero(preAsign);
				if(Ejec->first != NULL)mvwprintw(win, 0, 115, "[%02d]ban():%d", Ejec->first->PID, ban);
				else mvwprintw(win, 0, 115, "[%02d]ban():%d", susp->last->PID, ban);
				visualizacion(win);
				wrefresh(win);
				usleep(900000);
				if(ban)break;
			}
			
			//Si es USE
			if(Ejec->first->IR[PC].typeIns == 10){
				Ejec->first->tStart = time(NULL);
				Ejec->first->timeBlock = 5;
				if( bloquear() ) break;
			}
			
			//Si es FRE
			if(Ejec->first->IR[PC].typeIns == 11){
				liberarM();
				visualizacion(win);
				wrefresh(win);
			}

			if(Ejec->first->info != END && Ejec->first->IR[PC].typeIns < 8){
				//leer operando 1
				fgets(OP1, 3, globalFile);
				reg = esReg(OP1);
				if(reg >= 0)strcpy(Ejec->first->IR[PC].OP1, OP1); // si es registro valido
				else if(Ejec->first->info != END){		//si hay un error en el operando y la instr no es END
					Ejec->first->info = ERROP;
					terminaejecucion();
					visualizacion(win);
					return;
				}


				//leer operando 2
				if(Ejec->first->IR[PC].typeIns < 5){
					if( (Ejec->first->IR[PC].typeOP2 = obtOP2(Ejec->first->archivo, PC, Ejec->first)) == ERROP){
						Ejec->first->info = ERROP;
						terminaejecucion();
						visualizacion(win);
						return;
					}
					if(Ejec->first->IR[PC].typeOP2 == NOEND){
						Ejec->first->info = NOEND;
						terminaejecucion();
						visualizacion(win);
						return;
					}
				}

				else{	//Si solo es instruccion de un parametro verificar que termine en salto de linea
					OP1[3] = fgetc(globalFile);
					for(i = 0; OP1[i]; i++) OP1[i] = toupper(OP1[i]);
					caracter = OP1[3];
					OP1[3] = '\0';	//el caracter siempre debe ser fin de cadena
					if(caracter != '\n' || Ejec->first->info != END){
						Ejec->first->info = ERROP;
						//terminaejecucion();
						visualizacion(win);
						//return;
					}
				}
			}

			/**********Ejecucion de instrucciones***********/
			if(PC >= lineas && Ejec->first->IR[PC].typeIns < 8)ejec = ejec_ins(Ejec->first);
			/**********************************************/

			if(ejec == DIVZERO){
				Ejec->first->info = DIVZERO;
				terminaejecucion();
				visualizacion(win);
				return;
			}

			inRange = verify(NULL,4);	//verifica que no se salga de rango
			if(inRange == OUTRANGE){
				mvwprintw(win,9,0,"Fuera de Rango");
				Ejec->first->info = OUTRANGE;
				terminaejecucion();
				visualizacion(win);
				return;
			}

			show(win, k);
			usleep(timeE*1000);
			PC++;
			lines = PC;
			
			visualizacion(win);
			wrefresh(win);
		}
		
		saveContext();
		iniciarReg();
		fairshare(Ejec->first);
		
		if(Ejec->first != NULL && Ejec->first->estado != TERMIN && Ejec->first != NULL && Ejec->first->estado != SUSPEN){
			Ejec->first->estado = LISTOS;

			if(Listos->first == NULL){
				ins_nodo_en_lista_vacia(Listos, Ejec->first);
				inicializacion(Ejec);
			}
			else{
				ins_fin_lista(Listos, Listos->last, Ejec->first);
				inicializacion(Ejec);
			}
		}
		visualizacion(win);
		wrefresh(win);
		
		dispatchsusp(win);
		desbloquear();

		if(kbhit())command(win);
		wrefresh(win);
	}
	
	//fclose(globalFile);
	visualizacion(win);
	wrefresh(win);
	return;
}


int banquero(int preAsign[]){
	int MemDispSim[4];
	int MemAsignRes[4];
	int i, algunoCumple = 1, marcados = 0, marcados2;
	
	listPCB *auxList = Listos->first;
	listPCB *auxSusp = susp->first;
	listPCB *auxBlock = block->first;
	
	for(i = 0; i < 4; i++){
		MemDispSim[i] = MemDisp[i];
		MemAsignRes[i] = Ejec->first->MemAssign[i];
	}
	
	for(i = 0; i < 4; i++){
		if(preAsign[i] > MemDisp[i]){		//susp si no hay recursos disp para otorgar
			//saveContext();
			Ejec->first->tipSusp = NODISP;
			Ejec->first->estado = SUSPEN;
			if(susp->first == NULL){
				ins_nodo_en_lista_vacia(susp, Ejec->first);
			}
			else{
				ins_fin_lista(susp, susp->last, Ejec->first);
			}
			inicializacion(Ejec);
			//terminaejecucion();
			return 1;
		}
	}
	
	///////////////pretender Asignacion//////////////////////////////////////
	
	for(i = 0; i < 4; i++){
		Ejec->first->MemAssign[i] += preAsign[i];
		MemDispSim[i] -= preAsign[i];
	}
	
	unmark();		//desmarcar todo los procesos
	
	while(algunoCumple){
		marcados2 = marcados;
		while( auxList != NULL ){
			for(i = 0; i<4; i++){
				if( auxList->Marca == 0 && (auxList->MemMaxAssign[i]-auxList->MemAssign[i]) <= MemDispSim[i] ){
					auxList->Marca = 1;		//marcar
					marcados++;
					MemDispSim[i] = MemDispSim[i] + auxList->MemAssign[i];
				}
			}
			auxList = auxList->sigPCB;
		}
		while( auxSusp != NULL ){
			for(i = 0; i<4; i++){
				if( auxSusp->Marca == 0 && (auxSusp->MemMaxAssign[i]-auxList->MemAssign[i]) <= MemDispSim[i] ){
					auxSusp->Marca = 1;		//marcar
					marcados++;
					MemDispSim[i] = MemDispSim[i] + auxSusp->MemAssign[i];
				}
			}
			auxSusp = auxSusp->sigPCB;
		}
		
		while( auxBlock != NULL ){
			for(i = 0; i<4; i++){
				if( auxBlock->Marca == 0 && (auxBlock->MemMaxAssign[i]-auxBlock->MemAssign[i]) <= MemDispSim[i] ){
					auxBlock->Marca = 1;		//marcar
					marcados++;
					MemDispSim[i] = MemDispSim[i] + auxBlock->MemAssign[i];
				}
			}
			auxBlock = auxBlock->sigPCB;
		}
		
		if(marcados > marcados2)algunoCumple = 1;
		else algunoCumple = 0;
	}

	if( todosMarcados() == 1 ){
		for(i=0; i<4; i++)MemDisp[i] = MemDisp[i] - preAsign[i];
		return 0;
	}
	else{
		for(i = 0; i<4; i++)Ejec->first->MemAssign[i] = MemAsignRes[i];
		//saveContext();
		Ejec->first->tipSusp = RIESGO;
		Ejec->first->estado = SUSPEN;
		if(susp->first == NULL){
			ins_nodo_en_lista_vacia(susp, Ejec->first);
		}
		else{
			ins_fin_lista(susp, susp->last, Ejec->first);
		}
		inicializacion(Ejec);
		//terminaejecucion();
		return 1;
	}

}


int todosMarcados(){

	listPCB *auxList = Listos->first;
	listPCB *auxSusp = susp->first;
	listPCB *auxBlock = block->first;
	
	while(auxList != NULL){
		if(auxList->Marca == 0) return 0;
		auxList = auxList->sigPCB;
	}
	while(auxSusp != NULL){
		if(auxSusp->Marca == 0) return 0;
		auxSusp = auxSusp->sigPCB;
	}
	while(auxBlock != NULL){
		if(auxBlock->Marca == 0)return 0;
		auxBlock = auxBlock->sigPCB; 
	}


	return 1;
}

void unmark(){
	listPCB *auxList = Listos->first;
	listPCB *auxSusp = susp->first;
	listPCB *auxBlock = block->first;
	
	while(auxList != NULL){
		auxList->Marca = 0;
		auxList = auxList->sigPCB;
	}
	while(auxSusp != NULL){
		auxSusp->Marca = 0;
		auxSusp = auxSusp->sigPCB;
	}
	while(auxBlock != NULL){
		auxBlock->Marca = 0;
		auxBlock = auxBlock->sigPCB;
	}
	

}

int bloquear(){
	saveContext();
	Ejec->first->estado = BLOCK;
	if( block->first == NULL){
		ins_nodo_en_lista_vacia(block, Ejec->first);
	}
	else{
		ins_fin_lista(block, block->last, Ejec->first);
	}
	sup_inicio(Ejec);

	return 1;
}

void desbloquear(){
	if(block->first == NULL)return;
	
	time_t tActual = time(NULL);
	listPCB *actual = block->first;
	
	while(actual != NULL){
		if(tActual >= actual->tStart + actual->timeBlock){
			if( Listos->first == NULL){
				
				if(actual == block->first)sup_inicio(block);
				//else sacareliminar(block, actual->PID);
				ins_nodo_en_lista_vacia(Listos, actual);
			}
			else{
				if(actual->PID == block->first->PID)sup_inicio(block);
				//else sacareliminar(block, actual->PID);
				ins_fin_lista(Listos, Listos->last, actual);
			}
			
			//if(actual->PID == block->first->PID)sup_inicio(block);
			//else sacareliminar(block, actual->PID);
			
			return;
		}
		actual = actual->sigPCB;
	}
}


void liberarM(){
	int i;
	
	for(i = 0; i < 4; i++){
		MemDisp[i] += Ejec->first->MemAssign[i];
	}
	
	for(i = 0; i < 4; i++){
		Ejec->first->MemAssign[i] = 0;
	}
}

void dispatch(WINDOW *win){
	int menorP, PID;
	listPCB *PCB;

	if(Listos->first != NULL){
			PCB = Listos->first;
			PID = PCB->PID;
			menorP = PCB->prioridad;
			while(PCB != NULL){
				if(PCB->prioridad < menorP && PCB->prioridad != 0){
					menorP = PCB->prioridad;
					PID = PCB->PID;
				}
				PCB = PCB->sigPCB;
			}
		
			if(PID == Listos->first->PID){
				Listos->first->estado = EJECUC;
				Ejec->first = Listos->first;
				Listos->first = Listos->first->sigPCB;
				Listos->size--;
				Ejec->size++;
				Ejec->first->sigPCB = NULL;
				return;
			}
			else{
				Ejec->first = (sacar(Listos, PID));
				Ejec->size++;
				Ejec->first->estado = EJECUC;
				Ejec->first->sigPCB = NULL;
				return;
			}
		}

}

void dispatchsusp(WINDOW *win){
	int i;
	if(susp->first == NULL)return;
	
	susp->first->PC = 0;
	for(i = 0; i<4; i++)susp->first->MemMaxAssign[i] = 0;
	rewind( susp->first->archivo);	//regresar el archivo a la peticion de GET
	susp->first->estado = LISTOS;
	if(Listos->first == NULL){
		ins_nodo_en_lista_vacia(Listos, susp->first);
	}
	else ins_fin_lista(Listos, Listos->last, susp->first);

	
	sup_inicio(susp);
	visualizacion(win);
	wrefresh(win);
	usleep(100000);
}


int fork2(int PID_PADRE, int lineas, WINDOW *win){

	listPCB *aux, *hijo;

	if(Listos->first != NULL){
		if(Listos->first->PID == PID_PADRE){
			listPCB *PCB = (creaPCB(Listos->first->nombre, PID_PADRE));
			PCB->GCPU = Listos->first->GCPU;
			if(Listos->first->Hijo1 == NULL){
				Listos->first->Hijo1 = PCB;
				
			}
			else{
				hijo = Listos->first->Hijo1;
				while(hijo->hermano != NULL){
					hijo = hijo->hermano;
				}
				hijo->hermano = PCB;
			}
			read_file(PCB, PID_PADRE, lineas, win);
			return 0;
		}
		else{
			aux = Listos->first->sigPCB;
			while(aux != NULL){
				if(aux->PID == PID_PADRE){
					listPCB *PCB = (creaPCB(aux->nombre, PID_PADRE));
					PCB->GCPU = aux->GCPU;
					if(aux->Hijo1 == NULL){
						aux->Hijo1 = PCB;
						
					}
					else{
						hijo = aux->Hijo1;
						while(hijo->hermano != NULL){
							hijo = hijo->hermano;
						}
						hijo->hermano = PCB;
						
					}
					read_file(PCB, PID_PADRE, lineas, win);
					return 0;
				}
				aux = aux->sigPCB;
			}
		}
	}
	return 1;
}


void fairshare(listPCB *PCB){

	if(PCB == NULL)return;	
	PCB->CPU += 20*5;
	PCB->GCPU += 20*5;
	Wk = 1.00/ng;
	
	//actualizar el GCPU en todos los procesos con el mismo PPID
	listPCB *aux = Listos->first;
	while(aux != NULL){
		if(PCB->PPID == aux->PPID)aux->GCPU = PCB->GCPU;
		aux = aux->sigPCB;
	}
	
	//Recalcular prioridad
	PCB->prioridad = PCB->base + ((PCB->CPU)/2) + ((PCB->GCPU)/(4*Wk));

}


void mataPIDFamilia(int PID, WINDOW *win){
	listPCB *sentenciado;
	if(PID == Listos->first->PID){
		sentenciado = Listos->first;
	}
	else{
		sentenciado = (sacar(Listos, PID));
	}
	
	
	if(sentenciado == NULL)return;
	
	else{
		if(sentenciado->Hijo1 == NULL){
			mataPID(sentenciado->PID, win);
			ng--;
			return;
		}
		listPCB *p = sentenciado->Hijo1;
		listPCB *aux;
		while(p != NULL){
			aux = p->hermano;
			
			if(p->Hijo1 != NULL){
				mataPIDFamilia(p->PID, win);
			}
			else{
				p->hermano = NULL;
				mataPID(p->PID, win);
			}
			p = aux;
		}
		
		if(sentenciado->PID != sentenciado->PPID){
			listPCB *padre = NULL;
			listPCB *aux2 = Listos->first;
			
			while(aux2 != NULL){
				if(aux2->PID == sentenciado->PPID)break;
				aux2 = aux2->sigPCB;
			}
			if(sentenciado->PPID == aux2->PID)padre = aux2;
			
			if(padre != NULL){
				if(padre->Hijo1->PID == sentenciado->PID){
					padre->Hijo1 = sentenciado->hermano;
					sentenciado->hermano = NULL;
				}
				else{
					listPCB *h1 = padre->Hijo1;
					listPCB *h2 = h1->hermano;
					while(h2 != NULL){
						if(h2->PID == sentenciado->PID){
							h1->hermano = h2->hermano;
							h2->hermano = NULL;
						}
						h1 = h1->hermano;
						h2 = h2->hermano;
					}
				}
			}
		}
		sentenciado->Hijo1 = NULL;
		sentenciado->info = END;
		sentenciado->estado = TERMIN;
		mataPID(sentenciado->PID, win);
		ng--;
	}
	
}




