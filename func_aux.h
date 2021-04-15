/*
 * func_aux.h
 *
 *  Created on: 16/03/2017
 *      Author: Agustín Rosales Corona
 */


#include <termios.h>
#include <unistd.h>
#include <fcntl.h>


listPCB *creaPCB(char *dir, int PPID){

	listPCB *PCB;
	PCB = malloc(sizeof (listPCB));

	iniciarReg();

	FILE *file;
	file = fopen(dir, "r");
	if(file == NULL){
		//mvwprintw(win,9,0,"Error al procesar archivo");
		return NULL;
	}

	PCB->archivo = file;
	PCB->PID = avPID;
	avPID++;
	
	if(PPID != -1){ // Si no se mando -1 como ppid es por que SI tiene padre
		PCB->PPID = PPID;
	}
	else PCB->PPID = PCB->PID;	//sí no tiene padre, se dice que es su propio padre
	
	strcpy(PCB->nombre, dir);
	PCB->estado = LISTOS;
	iniciarRegNod(PCB);	//inicia AX, CX... EN 0
	//PCB->Hijo1 = NULL;
	//PCB->hermano = NULL;

	return PCB;
}

void mataPID(int numpid, WINDOW *win){
	int i;
	listPCB *aux2;
	listPCB *sust, *a_eliminar;

	//////////////////////////////Ejecucion
	if(Ejec->first != NULL){
		if(Ejec->first->PID == numpid){
			if(Ejec->first->PPID == Ejec->first->PID)ng--;
			Ejec->first->info = END;
			for(i=0; i<4; i++)MemDisp[i] += Ejec->first->MemAssign[i]; 
			terminaejecucion();
			visualizacion(win);
			return;
		}
	}
	
	/////////////////////////////Listos
	if(Listos->first != NULL){
		if(Listos->first->PID == numpid){
			if(Listos->first->PPID == Listos->first->PID)ng--;
			Listos->first->info = END;
			Listos->first->estado = TERMIN;
			for(i=0; i<4; i++)MemDisp[i] += Listos->first->MemAssign[i]; 
			if(Term->first == NULL){
				aux2 = Listos->first;
				Listos->first = Listos->first->sigPCB;
				aux2->sigPCB = NULL;
				Listos->size--;
				ins_nodo_en_lista_vacia(Term, aux2);
			}
			else{
				aux2 = Listos->first;
				Listos->first = Listos->first->sigPCB;
				aux2->sigPCB = NULL;
				Listos->size--;
				ins_nodo_inicio_lista(Term, aux2);
			}
			
			visualizacion(win);
			return;
		}
		
		else{
			sust = Listos->first;
			a_eliminar = sust->sigPCB;
			int i = 0;
			while(a_eliminar != NULL){
				if(a_eliminar->PID == numpid){
					if(a_eliminar->PPID == a_eliminar->PID)ng--;
					if(a_eliminar->PID == Listos->last->PID){
						Listos->last = sust;
						visualizacion(win);
					}
					sust->sigPCB = a_eliminar->sigPCB;
					for(i=0; i<4; i++)MemDisp[i] += a_eliminar->MemAssign[i];
					if(Term->first == NULL){
						ins_nodo_en_lista_vacia(Term, a_eliminar);
					}
					else{
						ins_nodo_inicio_lista(Term, a_eliminar);
					}
					a_eliminar = NULL;
					Listos->size--;
					visualizacion(win);
					return;
				}
				sust = a_eliminar;
				a_eliminar = a_eliminar->sigPCB;
				mvwprintw(win,2,80,"%d", i);
				i++;
				visualizacion(win);
			}
		}
		
	}
	
}


void MOV(int *a, int b){
	*a = b;
}

void ADD(int *a, int b){
	*a += b;
}

void SUB(int *a, int b){
	*a -= b;
}

void MUL(int *a, int b){
	*a *= b;;
}

int DIV(int *a, int b){
	if(b != 0)*a /= b;
	else return DIVZERO;
	
	return 0;
}

void INC(int *a){
	*a += 1;
}

void DEC(int *a){
	*a -= 1;
}

void iniciarReg(){
	PC = 0;
	AX = BX = CX = DX = 0;
}

void iniciarRegNod(listPCB *nodo){
	int i;
	nodo->base = 60;
	nodo->CPU = 0;
	nodo->GCPU = 0;
	nodo->PC = 0;
	nodo->Rx[0] = 0;
	nodo->Rx[1] = 0;
	nodo->Rx[2] = 0;
	nodo->Rx[3] = 0;
	nodo->Hijo1 = NULL;
	nodo->hermano = NULL;
	for(i = 0; i < 4; i++)nodo->MemAssign[i] = 0;
}

int hexADec(char *s){
	int x;
	sscanf(s, "%x", &x);
	return x;
}

void saveContext(){
	if(Ejec->first == NULL)return;
	Ejec->first->Rx[0] = AX;
	Ejec->first->Rx[1] = BX;
	Ejec->first->Rx[2] = CX;
	Ejec->first->Rx[3] = DX;
	Ejec->first->PC = PC;
	Ejec->first->archivo = globalFile;
	Ejec->first->PID = actPID;
}
void loadContext(){
	AX = Ejec->first->Rx[0];
	BX = Ejec->first->Rx[1];
	CX = Ejec->first->Rx[2];
	DX = Ejec->first->Rx[3];
	PC = Ejec->first->PC;
	globalFile = Ejec->first->archivo;
	actPID = Ejec->first->PID;
}

void terminaejecucion(){
	saveContext();
	Ejec->first->estado = TERMIN;
	if(Term->first == NULL){
		ins_nodo_en_lista_vacia(Term, Ejec->first);
		inicializacion(Ejec);
	}
	else{
		ins_nodo_inicio_lista(Term, Ejec->first);
		inicializacion(Ejec);
	}
}


void inicializacion (ListaSE *lista){
  lista->first = NULL;
  lista->last = NULL;
  lista->size = 0;
}

/* inserción en una lista vacía */


int ins_nodo_en_lista_vacia (ListaSE * lista, listPCB *nuevo_elemento){
	nuevo_elemento->sigPCB = NULL;
	lista->first = nuevo_elemento;
	lista->last = nuevo_elemento;
	lista->size++;
	return 0;
}

/* inserción al inicio de la lista */


int ins_nodo_inicio_lista (ListaSE * lista, listPCB *nuevo_elemento){
	nuevo_elemento->sigPCB = lista->first;
	lista->first = nuevo_elemento;
	lista->size++;
	return 0;
}
/*inserción al final de la lista */
int ins_fin_lista (ListaSE * lista, listPCB * actual, listPCB *nuevo_elemento){
	if (nuevo_elemento == NULL)return -1;
	actual->sigPCB = nuevo_elemento;
	nuevo_elemento->sigPCB = NULL;

	lista->last = nuevo_elemento;

	lista->size++;
	return 0;
}

/* eliminación al inicio de la lista */
int sup_inicio (ListaSE * lista){
	if (lista->size == 0)return -1;
	//listPCB *sup_elemento;
	//sup_elemento = lista->first;
	lista->first = lista->first->sigPCB;
	if (lista->size == 1)lista->last = NULL;
	lista->size--;
	return 0;
}

//sacar en cualquier parte de la lista
listPCB *sacar(ListaSE *lista, int PID){
	listPCB *sust, *a_eliminar;
	sust = lista->first;
	
	a_eliminar = sust->sigPCB;
	int i = 0;
	while(a_eliminar != NULL){
		if(a_eliminar->PID == PID){
			if(a_eliminar->PID == lista->last->PID){
				lista->last = sust;
			}
			sust->sigPCB = a_eliminar->sigPCB;
			lista->size--;
			return a_eliminar;
		}
		sust = a_eliminar;
		a_eliminar = a_eliminar->sigPCB;
		i++;
	}
	return NULL;
}

void sacareliminar(ListaSE *lista, int PID){
	listPCB *sust, *a_eliminar;
	sust = lista->first;
	
	a_eliminar = sust->sigPCB;
	
	while(a_eliminar != NULL){
		if(a_eliminar->PID == PID){
			if(a_eliminar->PID == lista->last->PID){
				lista->last = sust;
				lista->last->sigPCB = NULL;
				lista->size--;
				a_eliminar = NULL;
				return;
			}
			sust->sigPCB = a_eliminar->sigPCB;
			lista->size--;
			a_eliminar = NULL;
			return;
		}
		sust = a_eliminar;
		a_eliminar = a_eliminar->sigPCB;
	}
	return;
}




void speed(WINDOW *win){
	char str[10];
	mvwprintw(win, row-4,2,"\n");
	mvwprintw(win, row-4,2,"ingresar retardo en miliseg:");
	getstr(str);
	timeE = atoi(str);
	mvprintw(row-2, 2, "\n");
	mvprintw(row-2, 2, "establecido a: %d miliseg", timeE);
	mvprintw(9,0,"\n");
}


int kbhit(void){
	struct timeval tv;
	fd_set read_fd;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&read_fd);
	FD_SET(0, &read_fd);
	
	if(select(1,&read_fd, NULL, NULL, &tv) == -1)
		return 0;
		
	if(FD_ISSET(0, &read_fd) )
		return 1;
	
	return 0;
}



int obtOP2(FILE *file, int x, listPCB *PCB1){
	/*
	 * Obtiene el segundo operando
	 * regresa 0 si es un registro
	 * regresa 1 si es en hexadecimal
	 * regresa ERROP en caso de error
	 * los valores son guardados en la estructura
	 *  */

	//comprobar que sigue ","
	fgets(sepOP, 2, file);

	int i = 0;
	while( (OP2[i] = fgetc(file)) != '\n'){ //leer linea hasta encontrar salto de linea
		if(OP2[i] == EOF) return NOEND;	//termino el archivo
		i++;
	}
	//for(j = 0; OP2[j]; j++) OP2[j] = toupper(OP2[i]);
	caracter = OP2[i];
	if(OP2[i] == '\n')OP2[i] = '0';	//quitar salto de linea erroneo


	if(i >= 8)OP2[i] = '\0';	//el caracter 8 siempre debe ser fin de cadena
	if(debug)printf("i = %d\nOP2:%s\n",i,OP2);


	//comprobar salto de linea
	if(caracter != '\n')return ERROP;
	int id;
	if(i == 2 && esHex(OP2, i) != 0){	//es registro
		if((id = esReg(OP2)) >= 0){
			strcpy(PCB1->IR[x].OP2, OP2);
		}
		if(id == ERROP)return ERROP;
		PCB1->IR[x].regID = id;
		PCB1->IR[x].OP2[2] = '\0';
		if(debug)printf("linea.OP2 = %s\nlinea.regID = %d\n", PCB1->IR[x].OP2,PCB1->IR[x].regID);
		zero(OP2);
		return 0;
	}
	else{								//es un HEX
		if(esHex(OP2, i) == 0)obtHex(OP2, i, x, PCB1);
		else return ERROP;
		if(debug)printf("linea.Hex = %s\nlinea.valHex = %d\n",PCB1->IR[x].Hex,PCB1->IR[x].valHex);
		zero(OP2);
		return 1;
	}

}


int combinacio_operandos(listPCB *PCB1){
	/*Comprueba que registros aparecen en IR para decidir que operandos(variables)
	 * utilizar en la ejecucion
	 *
	 * regresa un valor entero indicando la combinacion de operandos:
	 *
	 * 1.- AX,AX	9.- CX,AX
	 * 2.- AX,BX	10.- CX,BX
	 * 3.- AX,CX	11.- CX,CX
	 * 4.- AX,DX	12.- CX,DX
	 * 5.- BX,AX	13.- DX,AX
	 * 6.- BX,BX	14.- DX,BX
	 * 7.- BX,CX	15.- DX,CX
	 * 8.- BX,DX	16.- DX,DX
	 * */
	if(strcasecmp(PCB1->IR[PC].OP1, "AX") == 0 && PCB1->IR[PC].typeOP2 == 0){
		if(PCB1->IR[PC].regID == 0) return 1;
		if(PCB1->IR[PC].regID == 1) return 2;
		if(PCB1->IR[PC].regID == 2) return 3;
		if(PCB1->IR[PC].regID == 3) return 4;
	}
	if(strcasecmp(PCB1->IR[PC].OP1, "BX") == 0 && PCB1->IR[PC].typeOP2 == 0){
		if(PCB1->IR[PC].regID == 0) return 5;
		if(PCB1->IR[PC].regID == 1) return 6;
		if(PCB1->IR[PC].regID == 2) return 7;
		if(PCB1->IR[PC].regID == 3) return 8;
	}
	if(strcasecmp(PCB1->IR[PC].OP1, "CX") == 0 && PCB1->IR[PC].typeOP2 == 0){
		if(PCB1->IR[PC].regID == 0) return 9;
		if(PCB1->IR[PC].regID == 1) return 10;
		if(PCB1->IR[PC].regID == 2) return 11;
		if(PCB1->IR[PC].regID == 3) return 12;
	}
	if(strcasecmp(PCB1->IR[PC].OP1, "DX") == 0 && PCB1->IR[PC].typeOP2 == 0){
		if(PCB1->IR[PC].regID == 0) return 13;
		if(PCB1->IR[PC].regID == 1) return 14;
		if(PCB1->IR[PC].regID == 2) return 15;
		if(PCB1->IR[PC].regID == 3) return 16;
	}
	return ERR;
}

int ejec_ins(listPCB *PCB1){
	/*Ejecuta las instrucciones
	 * 0.-MOV
	 * 1.-ADD
	 * 2.-SUB
	 * 3.-MUL
	 * 4.-DIV
	 * 5.-INC
	 * 6.-DEC
	 * */

	int combinacion, divstatus;
	switch(PCB1->IR[PC].typeIns){
		case 0: combinacion = combinacio_operandos(PCB1);//MOV
				switch(combinacion){
					case 1: MOV(&AX, AX); break;	case 9: MOV(&CX, AX); break;
					case 2: MOV(&AX, BX); break;	case 10: MOV(&CX, BX); break;
					case 3: MOV(&AX, CX); break;	case 11: MOV(&CX, CX); break;
					case 4: MOV(&AX, DX); break;	case 12: MOV(&CX, DX); break;
					case 5: MOV(&BX, AX); break;	case 13: MOV(&DX, AX); break;
					case 6: MOV(&BX, BX); break;	case 14: MOV(&DX, BX); break;
					case 7: MOV(&BX, CX); break;	case 15: MOV(&DX, CX); break;
					case 8: MOV(&BX, DX); break;	case 16: MOV(&DX, DX); break;
				}
				if(strcasecmp(PCB1->IR[PC].OP1, "AX") == 0 && PCB1->IR[PC].typeOP2 == 1)MOV(&AX, PCB1->IR[PC].valHex);
				if(strcasecmp(PCB1->IR[PC].OP1, "BX") == 0 && PCB1->IR[PC].typeOP2 == 1)MOV(&BX, PCB1->IR[PC].valHex);
				if(strcasecmp(PCB1->IR[PC].OP1, "CX") == 0 && PCB1->IR[PC].typeOP2 == 1)MOV(&CX, PCB1->IR[PC].valHex);
				if(strcasecmp(PCB1->IR[PC].OP1, "DX") == 0 && PCB1->IR[PC].typeOP2 == 1)MOV(&DX, PCB1->IR[PC].valHex);
				break;
		case 1:combinacion = combinacio_operandos(PCB1);//ADD
				switch(combinacion){
					case 1: ADD(&AX, AX); break;	case 9: ADD(&CX, AX); break;
					case 2: ADD(&AX, BX); break;	case 10: ADD(&CX, BX); break;
					case 3: ADD(&AX, CX); break;	case 11: ADD(&CX, CX); break;
					case 4: ADD(&AX, DX); break;	case 12: ADD(&CX, DX); break;
					case 5: ADD(&BX, AX); break;	case 13: ADD(&DX, AX); break;
					case 6: ADD(&BX, BX); break;	case 14: ADD(&DX, BX); break;
					case 7: ADD(&BX, CX); break;	case 15: ADD(&DX, CX); break;
					case 8: ADD(&BX, DX); break;	case 16: ADD(&DX, DX); break;
				}
				if(strcasecmp(PCB1->IR[PC].OP1, "AX") == 0 && PCB1->IR[PC].typeOP2 == 1)ADD(&AX, PCB1->IR[PC].valHex);
				if(strcasecmp(PCB1->IR[PC].OP1, "BX") == 0 && PCB1->IR[PC].typeOP2 == 1)ADD(&BX, PCB1->IR[PC].valHex);
				if(strcasecmp(PCB1->IR[PC].OP1, "CX") == 0 && PCB1->IR[PC].typeOP2 == 1)ADD(&CX, PCB1->IR[PC].valHex);
				if(strcasecmp(PCB1->IR[PC].OP1, "DX") == 0 && PCB1->IR[PC].typeOP2 == 1)ADD(&DX, PCB1->IR[PC].valHex);
				break;
		case 2:combinacion = combinacio_operandos(PCB1);//SUB
				switch(combinacion){
					case 1: SUB(&AX, AX); break;	case 9: SUB(&CX, AX); break;
					case 2: SUB(&AX, BX); break;	case 10: SUB(&CX, BX); break;
					case 3: SUB(&AX, CX); break;	case 11: SUB(&CX, CX); break;
					case 4: SUB(&AX, DX); break;	case 12: SUB(&CX, DX); break;
					case 5: SUB(&BX, AX); break;	case 13: SUB(&DX, AX); break;
					case 6: SUB(&BX, BX); break;	case 14: SUB(&DX, BX); break;
					case 7: SUB(&BX, CX); break;	case 15: SUB(&DX, CX); break;
					case 8: SUB(&BX, DX); break;	case 16: SUB(&DX, DX); break;
				}
				if(strcasecmp(PCB1->IR[PC].OP1, "AX") == 0 && PCB1->IR[PC].typeOP2 == 1)SUB(&AX, PCB1->IR[PC].valHex);
				if(strcasecmp(PCB1->IR[PC].OP1, "BX") == 0 && PCB1->IR[PC].typeOP2 == 1)SUB(&BX, PCB1->IR[PC].valHex);
				if(strcasecmp(PCB1->IR[PC].OP1, "CX") == 0 && PCB1->IR[PC].typeOP2 == 1)SUB(&CX, PCB1->IR[PC].valHex);
				if(strcasecmp(PCB1->IR[PC].OP1, "DX") == 0 && PCB1->IR[PC].typeOP2 == 1)SUB(&DX, PCB1->IR[PC].valHex);
				break;
		case 3:combinacion = combinacio_operandos(PCB1);//MUL
				switch(combinacion){
					case 1: MUL(&AX, AX); break;	case 9: MUL(&CX, AX); break;
					case 2: MUL(&AX, BX); break;	case 10: MUL(&CX, BX); break;
					case 3: MUL(&AX, CX); break;	case 11: MUL(&CX, CX); break;
					case 4: MUL(&AX, DX); break;	case 12: MUL(&CX, DX); break;
					case 5: MUL(&BX, AX); break;	case 13: MUL(&DX, AX); break;
					case 6: MUL(&BX, BX); break;	case 14: MUL(&DX, BX); break;
					case 7: MUL(&BX, CX); break;	case 15: MUL(&DX, CX); break;
					case 8: MUL(&BX, DX); break;	case 16: MUL(&DX, DX); break;
				}
				if(strcasecmp(PCB1->IR[PC].OP1, "AX") == 0 && PCB1->IR[PC].typeOP2 == 1)MUL(&AX, PCB1->IR[PC].valHex);
				if(strcasecmp(PCB1->IR[PC].OP1, "BX") == 0 && PCB1->IR[PC].typeOP2 == 1)MUL(&BX, PCB1->IR[PC].valHex);
				if(strcasecmp(PCB1->IR[PC].OP1, "CX") == 0 && PCB1->IR[PC].typeOP2 == 1)MUL(&CX, PCB1->IR[PC].valHex);
				if(strcasecmp(PCB1->IR[PC].OP1, "DX") == 0 && PCB1->IR[PC].typeOP2 == 1)MUL(&DX, PCB1->IR[PC].valHex);
				break;
		case 4:combinacion = combinacio_operandos(PCB1);//DIV
				switch(combinacion){
					case 1: divstatus = DIV(&AX, AX); break;	case 9: divstatus = DIV(&CX, AX); break;
					case 2: divstatus = DIV(&AX, BX); break;	case 10: divstatus = DIV(&CX, BX); break;
					case 3: divstatus = DIV(&AX, CX); break;	case 11: divstatus = DIV(&CX, CX); break;
					case 4: divstatus = DIV(&AX, DX); break;	case 12: divstatus = DIV(&CX, DX); break;
					case 5: divstatus = DIV(&BX, AX); break;	case 13: divstatus = DIV(&DX, AX); break;
					case 6: divstatus = DIV(&BX, BX); break;	case 14: divstatus = DIV(&DX, BX); break;
					case 7: divstatus = DIV(&BX, CX); break;	case 15: divstatus = DIV(&DX, CX); break;
					case 8: divstatus = DIV(&BX, DX); break;	case 16: divstatus = DIV(&DX, DX); break;
				}
				if(strcasecmp(PCB1->IR[PC].OP1, "AX") == 0 && PCB1->IR[PC].typeOP2 == 1)divstatus = DIV(&AX, PCB1->IR[PC].valHex);
				if(strcasecmp(PCB1->IR[PC].OP1, "BX") == 0 && PCB1->IR[PC].typeOP2 == 1)divstatus = DIV(&BX, PCB1->IR[PC].valHex);
				if(strcasecmp(PCB1->IR[PC].OP1, "CX") == 0 && PCB1->IR[PC].typeOP2 == 1)divstatus = DIV(&CX, PCB1->IR[PC].valHex);
				if(strcasecmp(PCB1->IR[PC].OP1, "DX") == 0 && PCB1->IR[PC].typeOP2 == 1)divstatus = DIV(&DX, PCB1->IR[PC].valHex);
				return divstatus;
				break;
		case 5:
			if(strcasecmp(PCB1->IR[PC].OP1, "AX") == 0 )INC(&AX);
			if(strcasecmp(PCB1->IR[PC].OP1, "BX") == 0 )INC(&BX);
			if(strcasecmp(PCB1->IR[PC].OP1, "CX") == 0 )INC(&CX);
			if(strcasecmp(PCB1->IR[PC].OP1, "DX") == 0 )INC(&DX);
			break;
		case 6:
			if(strcasecmp(PCB1->IR[PC].OP1, "AX") == 0 )DEC(&AX);
			if(strcasecmp(PCB1->IR[PC].OP1, "BX") == 0 )DEC(&BX);
			if(strcasecmp(PCB1->IR[PC].OP1, "CX") == 0 )DEC(&CX);
			if(strcasecmp(PCB1->IR[PC].OP1, "DX") == 0 )DEC(&DX);
			break;

		default: return ERR;
	}
	return 0;
}


void zero(char *string){
	int len, i;

	len = 8;

	for(i = 0; i < len; i++){
		string[i] = '0';
	}
	string[8] = '\0';
}

int verify(char *string, int type){

	/*compara la cadena recibida con las instrucciones definidas en el caso type = 1
	 * 1:	Instrucciones
	 * 		1.1:regresa instruccion (0 = MOV,1 = ADD,..., ERRINS si la cadena no coincide)
	 *
	 * en type = 4 verifica que las variables se mantengan dentro del rango 0-7FFFFFFFF
	 * regresa OUTRANGE si alguna sale del rango
	 *
	 * regresa ERR si no se cumple type
	 * */

	char *Instrucciones[] = {"MOV", "ADD", "SUB", "MUL", "DIV", "INC", "DEC", "END", "MAX", "GET", "USE", "FRE"};
	int i;

	if(type == 1){	//instruccion
		for(i = 0; i < 12; i++){
			if(strcasecmp(string, Instrucciones[i]) == 0 )return i;
		}
		return ERRINS;
	}

	if(type == 4){
		if(AX < 0 || BX < 0 || CX < 0 || DX < 0)return OUTRANGE;
	}
	return ERR;
}

int checkInstr(char *instr, listPCB *PCB1){
	/*
	 * verifica la instruccion y regresa cual es
	 * 0 MOV
	 * 1 ADD
	 * 2 SUB
	 * 3 MUL
	 * 4 DIV
	 * 5 INC
	 * 6 DEC
	 * 7 END
	 * 8 MAX
	 * 9 GET
	 * 10 USE
	 * 11 FRE
	 *
	 * */

	if(debug)printf("---------\nInstruccion:%s\n",instr);

	if((PCB1->IR[PC].typeIns = verify(instr, 1)) == ERRINS){
		//perror("Instruccion invalida");
		return ERRINS;
	}
	return PCB1->IR[PC].typeIns; //regresa la instruccion
}

int esReg(char *str){
	/* Recibe una cadena y la compara con los operandos definidos para comprobar si coinciden
	 * de hacerlo regresa un entero que indica cual es
	 * 0.-AX
	 * 1.-BX
	 * 2.-CX
	 * 3.-DX
	 * regresa ERROP sí no coincide con alguno
	 * */
	int i;
	for(i = 0; i < 4; i++){
		if( strncasecmp(registros[i], str, 2) == 0)return i;
	}
	return ERROP;
}

int esHex(char *str, int len){
	/* Recibe una cadena y su tamaño, y verifica que todos sus caracteres pertenescan
	 * a los de una cadena de valor hexadecimal, de hacerlo regresa 0 indicando que la cadena
	 * es un valor Hex, regresa ERROP en caso contrario
	 * */
	int i,j;
	int count = 0;
	char hex_digits[22] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','a','b','c','d','e','f'};

	for(i = 0; i < len; i++){
		for(j = 0; j < 22; j++){
			if(str[i] == hex_digits[j])count++;
		}
	}
	if(count == len)return 0;
	else return ERROP;
}

void obtHex(char *str, int len, int x, listPCB *PCB1){
	/* recibe una cadena, su tamaño y un entero el cual indica a que numero de linea pertenece
	 * del archivo leido, guarda la cadena en la estructura y su valor en decimal
	 * */
	str[len] = '\0';
	strncpy(PCB1->IR[x].Hex, str, len);
	PCB1->IR[x].Hex[len] = '\0';
	PCB1->IR[x].valHex = hexADec(str);
}

