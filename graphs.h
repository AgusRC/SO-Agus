/*
 * graphs.h
 *
 *  Created on: 16/03/2017
 *      Author: Agustín Rosales Corona
 */

/* visualización de la lista */

WINDOW *create_newwin(int height, int width, int starty, int startx);


void visualizacion(WINDOW *win){

	init_pair(1, COLOR_YELLOW, COLOR_BLACK);

	WINDOW *ejecBox;
	ejecBox = newwin(4, 50, 18, 1);
	wbkgd(ejecBox, COLOR_PAIR(1));
	wborder(ejecBox, '|', '|', '-','_',' ',' ',' ',' ');

	WINDOW *listBox;
	listBox = newwin(20, 60, 18, 60);
	wbkgd(listBox, COLOR_PAIR(1));
	wborder(listBox, '|', '|', '-','_',' ',' ',' ',' ');
	
	WINDOW *termBox;
	termBox = newwin(20, 60, 18, 125);
	wbkgd(termBox, COLOR_PAIR(1));
	wborder(termBox, '|', '|', '-','_',' ',' ',' ',' ');
	
	WINDOW *suspBox;
	suspBox = newwin(6, 50, 24, 1);
	wbkgd(suspBox, COLOR_PAIR(1));
	wborder(suspBox, '|', '|', '-','_',' ',' ',' ',' ');
	
	WINDOW *blockBox;
	blockBox = newwin(10, 60, 4, 125);
	if(blockBox == NULL)exit(1);
	wbkgd(blockBox, COLOR_PAIR(1));
	wborder(blockBox, '|', '|', '-','_',' ',' ',' ',' ');

	listPCB *actual;
	int i;
	
	//getmaxyx(stdscr,row,col);
	/*for(i=0; i<row; i++){
		mvwprintw(win, 20+i, 0, "\n");
	}*/
	
	mvwprintw(win, 0, 85, "M.Disp:");
	for(i = 0; i<4; i++)mvwprintw(win, 0, 95+(4*i), "[%02d]", MemDisp[i]);
	wrefresh(win);
	
	mvwprintw(ejecBox, 0, 0, "Ejecucion [%04d]",Ejec->size);
	if(Ejec->first != NULL){
		i=0;
		actual = Ejec->first;
		while (actual != NULL){
			mvwprintw(ejecBox, 2+i, 1, "[PID:%02d][PC:%04d]",actual->PID,actual->PC);
			mvwprintw(ejecBox, 2+i, 18, "[Ejecutando]");
			mvwprintw(ejecBox, 2+i, 30, "[file:%s]", actual->nombre);
			mvwprintw(ejecBox, 2+i, 39,"[PPID:%02d]", actual->PPID);
			actual = actual->sigPCB;
			i++;
		}
	}else mvwprintw(ejecBox,2,1,"\t[vacia]\t\t\t\t\t");
	wrefresh(ejecBox);

	mvwprintw(listBox, 0, 0, "Listos [%04d]", Listos->size);
	if(Listos->first != NULL){
		i=0;
		actual = Listos->first;
		while (actual != NULL){
			mvwprintw(listBox, 2+i*3, 1, "[PID:%02d][PPID:%02d][PC:%04d]",actual->PID, actual->PPID, actual->PC);
			mvwprintw(listBox, 2+i*3, 27, "[  Listos ]");
			mvwprintw(listBox, 2+i*3, 37, "[file:%s]", actual->nombre);
			mvwprintw(listBox, 2+i*3, 46, "[p:%f][CPU:%f][GCPU:%f]",actual->prioridad, actual->CPU, actual->GCPU);
			actual = actual->sigPCB;
			i++;
		}
	}else mvwprintw(listBox,2,1,"\t[vacia]\t\t\t\t\t");
	wrefresh(listBox);

	mvwprintw(termBox, 0, 0, "Terminados [%04d]", Term->size);
	if(Term->first != NULL){
		i=0;
		actual = Term->first;
		while (actual != NULL){
			mvwprintw(termBox, 2+i, 1, "[PID:%02d][PC:%04d]",actual->PID,actual->PC);
			mvwprintw(termBox, 2+i, 18, "[terminado]");
			mvwprintw(termBox, 2+i, 29, "[info:%04d]",actual->info);
			mvwprintw(termBox, 2+i, 40, "[file:%s]", actual->nombre);
			mvwprintw(termBox, 2+i, 49,"[PPID:%02d]", actual->PPID);
			mvwprintw(termBox, 3+i, 1, "AX[%08d] BX[%08d] CX[%08d] DX[%08d]", actual->Rx[0],actual->Rx[1],actual->Rx[2],actual->Rx[3]);
			actual = actual->sigPCB;
			i = i+3;
		}
	}else mvwprintw(termBox,2,1,"\t[vacia]\t\t\t\t\t");
	wrefresh(termBox);
	
	mvwprintw(suspBox, 0, 1, "Suspendidos [%04d]", susp->size);
	if(susp->first != NULL){
		i=0;
		actual = susp->first;
		while(actual != NULL){
			mvwprintw(suspBox, 2+i, 1, "[PID:%02d][PC:%04d][Edo:%03d]", actual->PID,actual->PC, actual->estado);
			if(actual->tipSusp == NODISP)mvwprintw(suspBox, 2+i, 27, "[MemInsuficiente]");
			if(actual->tipSusp == RIESGO)mvwprintw(suspBox, 2+i, 27, "[AsignacRiesgosa]");
			actual = actual->sigPCB;
			i++;
		}
	}else mvwprintw(suspBox,2,1,"\t[vacia]\t\t\t\t\t");
	wrefresh(suspBox);
	
	mvwprintw(blockBox, 0, 1, "Bloqueados[%04d]", block->size);
	if(block->first != NULL){
		i=0;
		actual= block->first;
		while(actual != NULL){
			mvwprintw(blockBox, 2+i, 1, "[PID:%02d][PC:%04d]", actual->PID,actual->PC);
			mvwprintw(blockBox, 2+i, 18, "[TB:%04d][Ti:%04d][Tf:%04d]", actual->timeBlock, actual->tStart-1500900000, actual->tStart+10-1500900000);
			actual = actual->sigPCB;
			i++;
		}
	}else mvwprintw(blockBox, 2, 1, "\t[vacia]\t\t\t\t");
	wrefresh(blockBox);
	
	
}



void show(WINDOW *win, int k){
	

	mvwprintw(win, 0, 55, "ng:%d, Wk:%f", ng, Wk);
	
	//Memoria
	mvwprintw(win, 0, 35, "MAX[%02d]:%d %d %d %d",Ejec->first->PID,Ejec->first->MemMaxAssign[0], Ejec->first->MemMaxAssign[1], Ejec->first->MemMaxAssign[2], Ejec->first->MemMaxAssign[3]);
	mvwprintw(win, 1, 35, "Asi:%d %d %d %d",Ejec->first->MemAssign[0], Ejec->first->MemAssign[1], Ejec->first->MemAssign[2], Ejec->first->MemAssign[3]);
	
	
	
	//PID
	mvwprintw(win,4,0,"PID: %04d",Ejec->first->PID);
	//IR: INS OP1 OP2
	if(Ejec->first->IR[PC].typeOP2 == 0)mvwprintw(win,5,0,"IR: %s %s,%s\t",Ejec->first->IR[PC].Ins,Ejec->first->IR[PC].OP1,Ejec->first->IR[PC].OP2);
	//IR: INS OP1 HEX
	else mvwprintw(win,5,0,"IR: %s %s,%s\t\t",Ejec->first->IR[PC].Ins,Ejec->first->IR[PC].OP1,Ejec->first->IR[PC].Hex);
	//			PC:	AX:	BX:	CX:	DX:
	mvwprintw(win,7,0,"[PC:%04d]\t[AX:%08d]\t[BX:%08d]\t[CX:%08d]\t[DX:%08d]",PC,AX,BX,CX,DX);
	//mvwprintw(win,9,0,"\n\n");
	mvwprintw(win,8,0, "timer = %d",k);
	wrefresh(win);
}


WINDOW *create_newwin(int height, int width, int starty, int startx)
{
	WINDOW *local_win;
	local_win = newwin(height, width, starty, startx);
	//box(local_win, 0 , 0);
	/* 0, 0 gives default characters
	* for the vertical and horizontal
	* lines
	*/
	
	wrefresh(local_win);
	/* Show that box
	*/
	return local_win;
}

