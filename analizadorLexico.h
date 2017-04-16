// Librerias utilizadas 
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>


// Definicion de tokens con valores numericos simbolicos
#define VAR				256
#define ARRAY			257
#define OBJECT			258
#define PR_DO			259
#define FUNCTION		300
#define PROCEDURE		301
#define PR_NUMBER		302
#define PR_BOOLEAN		303
#define PR_STRING		304
#define PR_FOR			305
#define PR_IF			306
#define PR_ELSE			307
#define PR_WHILE		308
#define PR_EVAL			309
#define PR_NULL			310
#define PR_NEW			311
#define PR_CASE			312
#define WRITE			313
#define CONST			314
#define NUM				315
#define ID				316
#define BOOL			317
#define CAR				318
#define LITERAL			319
#define OPLOGICO		320
#define OPREL			321
#define OPSUMA			322
#define OPMULOP			323
#define OPASIGNACION	324
#define SYMBOLESP		325

// constantes de tamaños
#define TAMANHOBUFFER 				5		// tamaño del buffer
#define TAMANHOLEXICO 				50		// tamaño del lexema
#define TAMANHOHASH 				101		// tamaño de la tabla hash

// estructura de entrada
typedef struct input{
	int componenteLexico;			// componente lexico
	char lexema[TAMANHOLEXICO];		// lexema
	struct input *tipoDato; 		// null puede representar variable no declarada		
} input;

// estructura de tokens
typedef struct {
	int componenteLexico;			// componente lexico
	input *pe;
} token;

// prototipos de funcion y procedimientos
void insertar(input e);
input* buscar(const char *clave);
void initTabla();
void initTablaSimbolos();
void sigLex();
