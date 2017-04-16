// Librerias utilizadas 
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include "analizadorLexico.h"

// variables globales
input *tabla;						// tabla de simbolos
int tamanhoTabla=TAMANHOHASH;		// tamaño de la tabla
int elementos=0;					

// funciones
int h(const char* k, int m)
{
	unsigned h=0,g;
	int i;
	for (i=0;i<strlen(k);i++)
	{
		h=(h << 4) + k[i];
		if ( (g=h&0xf0000000) ){
			h=h^(g>>24);
			h=h^g;
		}
	}
	return h%m;
}

// iniciar tabla
void initTabla()
{	
	int i=0;
	tabla=(input*)malloc(tamanhoTabla*sizeof(input));
	for(i=0;i<tamanhoTabla;i++)
		tabla[i].componenteLexico =-1;
}

int esprimo(int n)
{
	int i;
	for(i=3;i*i<=n;i+=2)
		if (n%i==0)
			return 0;
	return 1;
}

int siguiente_primo(int n)
{
	if (n%2==0)
		n++;
	for (;!esprimo(n);n+=2);

	return n;
}

//en caso de que la tabla llegue al limite, duplicar el tamaño
void rehash()
{
	input *vieja;
	int i;
	vieja=tabla;
	tamanhoTabla=siguiente_primo(2*tamanhoTabla);
	initTabla();
	for (i=0;i<tamanhoTabla/2;i++)
	{
		if(vieja[i].componenteLexico!=-1)
			insertar(vieja[i]);
	}		
	free(vieja);
}

//insertar una entrada en la tabla
void insertar(input e)
{
	int pos;
	if (++elementos>=tamanhoTabla/2)
		rehash();
	pos=h(e.lexema,tamanhoTabla);
	while (tabla[pos].componenteLexico!=-1)
	{
		pos++;
		if (pos==tamanhoTabla)
			pos=0;
	}
	tabla[pos]=e;

}
//busca una clave en la tabla, si no existe devuelve NULL, posicion en caso contrario
input* buscar(const char *clave)
{
	int pos;
	pos=h(clave,tamanhoTabla);
	while(tabla[pos].componenteLexico!=-1 && strcmp(tabla[pos].lexema,clave)!=0 )
	{
		pos++;
		if (pos==tamanhoTabla)
			pos=0;
	}
	return &tabla[pos];
}

void insertTablaSimbolos(const char *s, int n)
{
	input e;
	strcpy(e.lexema,s);
	e.componenteLexico=n;
	insertar(e);
}

// iniciar tabla de simbolos
void initTablaSimbolos()
{
	const int N = 19;
	int i;
	const char *vector[]={ "var", "Array", "Object","do","function","procedure","Number",		
		"Boolean","String","for","if","else","while","eval","null","new","case","write",
		"const" };
 	for (i=0;i<N;i++)
	{
		insertTablaSimbolos(vector[i],i+256);
	}
	insertTablaSimbolos(",",SYMBOLESP);
	insertTablaSimbolos(".",SYMBOLESP);
	insertTablaSimbolos(":",SYMBOLESP);
	insertTablaSimbolos(";",SYMBOLESP);
	insertTablaSimbolos("(",SYMBOLESP);
	insertTablaSimbolos(")",SYMBOLESP);
	insertTablaSimbolos("[",SYMBOLESP);
	insertTablaSimbolos("]",SYMBOLESP);
	insertTablaSimbolos("{",SYMBOLESP);
	insertTablaSimbolos("}",SYMBOLESP);
	insertTablaSimbolos("true",BOOL);
	insertTablaSimbolos("false",BOOL);
	insertTablaSimbolos("!",OPLOGICO);
	insertTablaSimbolos("&&",OPLOGICO);
	insertTablaSimbolos("||",OPLOGICO);
	insertTablaSimbolos("<",OPREL);
	insertTablaSimbolos("<=",OPREL);
	insertTablaSimbolos("<>",OPREL);
	insertTablaSimbolos(">",OPREL);
	insertTablaSimbolos(">=",OPREL);
	insertTablaSimbolos("==",OPREL);
	insertTablaSimbolos("+",OPSUMA);
	insertTablaSimbolos("-",OPSUMA);
	insertTablaSimbolos("*",OPMULOP);
	insertTablaSimbolos("/",OPMULOP);
	insertTablaSimbolos(":",OPASIGNACION);
	insertTablaSimbolos("=",OPASIGNACION);
}
