// Librerias utilizadas 
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include "tableSymbols.c"


// Variables globales 

int consumir; 
char cadena[5*TAMANHOLEXICO];		// string utilizado para cargar mensajes de error
token t;				// token global para recibir componentes del Analizador Lexico
FILE *archivo;					// Fuente json
char buffer[2*TAMANHOBUFFER];	// Buffer para lectura de archivo fuente
char id[TAMANHOLEXICO];			// Utilizado por el analizador lexico
int delantero=-1;				// Utilizado por el analizador lexico
int fin=0;						// Utilizado por el analizador lexico
int numeroLinea=1;				// Numero de Linea



// Funciones y procedimientos

void error(const char* mensaje)
{
	printf("Lin %d: Error Lexico. %s.\n",numeroLinea,mensaje);	
}

void sigLex()
{
	int i=0;
	char c=0;
	int acepto=0;
	int estado=0;
	char msg[41];
	input e;

	while((c=fgetc(archivo))!=EOF)
	{	
		if (c==' ' || c=='\t')
			continue;	//eliminar espacios en blanco
		else if(c=='\n')
		{
			//incrementar el numero de linea
			numeroLinea++;
			continue;
		}
		else if (isalpha(c))
		{
			//es un identificador o palabra reservada
			i=0;
			do{
				id[i]=c;
				i++;
				c=fgetc(archivo);
				if (i>=TAMANHOLEXICO)
					error("Longitud de Identificador excede tamaño de buffer");
			}while(isalpha(c) || isdigit(c));
			id[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			t.pe=buscar(id);
			t.componenteLexico=t.pe->componenteLexico;
			if (t.pe->componenteLexico==-1)
			{
				strcpy(e.lexema,id);
				e.componenteLexico=ID;
				insertar(e);
				t.pe=buscar(id);
				t.componenteLexico=ID;
			}
			break;
		}
		else if (isdigit(c))
		{
				//es digito
				i=0;
				estado=0;
				acepto=0;
				id[i]=c;
				
				while(!acepto)
				{
					switch(estado){
					case 0: //una secuencia netamente de digitos, puede ocurrir . o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=0;
						}
						else if(c=='.'){
							id[++i]=c;
							estado=1;
						}
						else if(tolower(c)=='e'){
							id[++i]=c;
							estado=3;
						}
						else{
							estado=6;
						}
						break;
					
					case 1://un punto, debe seguir un digito (caso especial de array, puede venir otro punto)
						c=fgetc(archivo);						
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(c=='.')
						{
							i--;
							fseek(archivo,-1,SEEK_CUR);
							estado=6;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 2://la fraccion decimal, pueden seguir los digitos o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(tolower(c)=='e')
						{
							id[++i]=c;
							estado=3;
						}
						else
							estado=6;
						break;
					case 3://una e, puede seguir +, - o una secuencia de digitos
						c=fgetc(archivo);
						if (c=='+' || c=='-')
						{
							id[++i]=c;
							estado=4;
						}
						else if(isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 4://necesariamente debe venir por lo menos un digito
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 5://una secuencia de digitos correspondiente al exponente
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							estado=6;
						}break;
					case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (c!=EOF)
							ungetc(c,archivo);
						else
							c=0;
						id[++i]='\0';
						acepto=1;
						t.pe=buscar(id);
						if (t.pe->componenteLexico==-1)
						{
							strcpy(e.lexema,id);
							e.componenteLexico=NUM;
							insertar(e);
							t.pe=buscar(id);
						}
						t.componenteLexico=NUM;
						break;
					case -1:
						if (c==EOF)
							error("No se esperaba el fin de archivo");
						else
							error(msg);
						exit(1);
					}
				}
			break;
		}
		else if (c=='<') 
		{
			//es un operador relacional, averiguar cual
			c=fgetc(archivo);
			if (c=='>'){
				t.componenteLexico=OPREL;
				t.pe=buscar("<>");
			}
			else if (c=='='){
				t.componenteLexico=OPREL;
				t.pe=buscar("<=");
			}
			else{
				ungetc(c,archivo);
				t.componenteLexico=OPREL;
				t.pe=buscar("<");
			}
			break;
		}
		else if (c=='>')
		{
			//es un operador relacional, averiguar cual
			c=fgetc(archivo);
			if (c=='='){
				t.componenteLexico=OPREL;
				t.pe=buscar(">=");
			}
			else{
				ungetc(c,archivo);
				t.componenteLexico=OPREL;
				t.pe=buscar(">");
			}
			break;
		}
		else if (c==':')
		{
			// un operador de asignacion
			t.componenteLexico=OPASIGNACION;
			t.pe=buscar(":");
			break;
		}
		else if (c=='=')
		{
			//es un operador relacional o asignacion
			c=fgetc(archivo);
			if (c=='='){
				t.componenteLexico=OPREL;
				t.pe=buscar("==");
			}
			else{
				ungetc(c,archivo);
				t.componenteLexico=OPASIGNACION;
				t.pe=buscar("=");
			}
			break;
		}
		else if (c=='!')
		{
			t.componenteLexico=OPLOGICO;
			t.pe=buscar("!");
			break;
		}
		else if (c=='&')
		{
			c=fgetc(archivo);
			if (c=='&'){
				t.componenteLexico=OPLOGICO;
				t.pe=buscar("&&");
			}	
		}
		else if (c=='|')
		{
			c=fgetc(archivo);
			if (c=='|'){
				t.componenteLexico=OPLOGICO;
				t.pe=buscar("||");
			}	
		}
		else if (c=='+')
		{
			t.componenteLexico=OPSUMA;
			t.pe=buscar("+");
			break;
		}
		else if (c=='-')
		{
			t.componenteLexico=OPSUMA;
			t.pe=buscar("-");
			break;
		}
		else if (c=='*')
		{
			t.componenteLexico=OPMULOP;
			t.pe=buscar("*");
			break;
		}
		else if (c=='/')
		{
			t.componenteLexico=OPMULOP;
			t.pe=buscar("/");
			break;
		}
		else if (c==',')
		{
			t.componenteLexico=SYMBOLESP;
			t.pe=buscar(",");
			break;
		}
		else if (c==';')
		{
			t.componenteLexico=SYMBOLESP;
			t.pe=buscar(";");
			break;
		}
		else if (c=='.')
		{
			t.componenteLexico=SYMBOLESP;
			t.pe=buscar(".");
			break;
		}
		else if (c=='(')
		{
			t.componenteLexico=SYMBOLESP;
			t.pe=buscar("(");
			break;
		}
		else if (c==')')
		{
			t.componenteLexico=SYMBOLESP;
			t.pe=buscar(")");
			break;
		}
		else if (c=='[')
		{
			t.componenteLexico=SYMBOLESP;
			t.pe=buscar("[");
			break;
		}
		else if (c==']')
		{
			t.componenteLexico=SYMBOLESP;
			t.pe=buscar("]");
			break;
		}
		else if (c=='{')
		{
			t.componenteLexico=SYMBOLESP;
			t.pe=buscar("{");
			break;
		}
		else if (c=='}')
		{
			t.componenteLexico=SYMBOLESP;
			t.pe=buscar("}");
			break;
		}
		else if (c=='\'' || c == '\"')
		{//un caracter o una cadena de caracteres
			i=0;
			id[i]=c;
			i++;
			do{
				c=fgetc(archivo);
				if (c=='\'')
				{
					c=fgetc(archivo);
					if (c=='\'')
					{
						id[i]=c;
						i++;
						id[i]=c;
						i++;
					}
					else
					{
						id[i]='\'';
						i++;
						break;
					}
				}
				else if (c=='\"')
				{
					c=fgetc(archivo);
					if (c=='\"')
					{
						id[i]=c;
						i++;
						id[i]=c;
						i++;
					}
					else
					{
						id[i]='\"';
						i++;
						break;
					}
				}
				else if(c==EOF)
				{
					error("Se llego al fin de archivo sin finalizar un literal");
				}
				else{
					id[i]=c;
					i++;
				}
			}while(isascii(c));
			id[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			t.pe=buscar(id);
			t.componenteLexico=t.pe->componenteLexico;
			if (t.pe->componenteLexico==-1)
			{
				strcpy(e.lexema,id);
				if (strlen(id)==3 || strcmp(id,"''''")==0)
					e.componenteLexico=CAR;
				else
					e.componenteLexico=LITERAL;
				insertar(e);
				t.pe=buscar(id);
				t.componenteLexico=e.componenteLexico;
			}
			break;
		}
		else if ( c == '/')
		{
			//elimina el comentario
			while(c!=EOF)
			{
				c=fgetc(archivo);
				if (c=='/')
					break;
				else if(c=='\n')
				{
					//incrementar el numero de linea
					numeroLinea++;
				}
			}
			if (c==EOF)
				error("Se llego al fin de archivo sin finalizar un comentario");
		}
		else if (c!=EOF)
		{
			sprintf(msg,"%c no esperado",c);
			error(msg);
		}
	}
	if (c==EOF)
	{
		t.componenteLexico=EOF;
		strcpy(e.lexema,"EOF");
		sprintf(e.lexema,"EOF");
		t.pe=&e;
	}
	
}

int main(int argc,char* args[])
{
	// inicializar analizador lexico

	initTabla();
	initTablaSimbolos();
	
	if(argc > 1)
	{
		if (!(archivo=fopen(args[1],"rt")))
		{
			printf("Archivo no encontrado.\n");
			exit(1);
		}
		while (t.componenteLexico!=EOF){
			sigLex();
			printf("Numero de linea: %d\tLexema: %s\n",numeroLinea,t.pe->lexema);
		}
		fclose(archivo);
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}
	return 0;
}
