
/**
 * @file lex.c
 * @author François Portet <francois.portet@imag.fr>
 * @brief Lexical analysis routines for MIPS assembly syntax.
 *
 * These routines perform the analysis of the lexeme of an assembly source code file.
 */

#define _POSIX_C_SOURCE 200112L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <global.h>
#include <notify.h>
#include <lex.h>

/**
 * @param line String of the line of source code to be analysed.
 * @param nline the line number in the source code.
 * @return should return the collection of lexemes that represent the input line of source code.
 * @brief This function performs lexical analysis of one standardized line.
 *
 */
Liste_lexeme_t *  lex_read_line( char *line, int nline) {

	enum Etat_t {INIT, POINT, VIRGULE, DEUX_POINTS, DECIMAL, HEXA, OCTAL, MOINS, PLUS,
		REGISTRE, RETOUR_LIGNE, SYMBOLE, DIRECTIVE, ETIQUETTE, COMMENTAIRE, DEBUT_HEXADECIMAL,
		DECIMAL_ZERO, PARANTHESE_OUVRANTE, PARANTHESE_FERMANTE, ERREUR
	};
	
	Liste_lexeme_t * debut_liste_p = NULL;
	enum Etat_t etat;
	char c;
	int i;
	
    char *seps = " \t";
    char *token = NULL;
    char save[STRLEN];

    /* copy the input line so that we can do anything with it without impacting outside world*/
    memcpy( save, line, STRLEN );

    /* get each token*/
    for( token = strtok( line, seps ); NULL != token; token = strtok( NULL, seps )) {
    	etat=INIT;
		i=0; /* On se place sur le premier caractère du token en cours */
	
		while('\0'!=(c=token[i])) { /* Tant que la ligne n'est pas terminee */
			switch(etat) {

				case INIT:
					if(isdigit(c)) {	/* Si le caractere est un chiffre */
						etat=(c=='0')? DECIMAL_ZERO : DECIMAL;
					}
					else if (c=='-') etat=MOINS;
					else if (c=='+') etat=PLUS;
					else if (c=='.') etat=POINT;
					else if (c==',') etat=VIRGULE;
					else if (c==':') etat=DEUX_POINTS;
					else if (c=='#') etat=COMMENTAIRE;
					else if (c=='$') etat=REGISTRE;
					else if (c=='(') etat=PARANTHESE_OUVRANTE;
					else if (c==')') etat=PARANTHESE_FERMANTE;
					else if (c=='\n') etat=RETOUR_LIGNE;
					else etat=SYMBOLE;
					break;
					
					
				case DEUX_POINTS:
				case VIRGULE:
				case PARANTHESE_OUVRANTE:
				case PARANTHESE_FERMANTE:
				case MOINS:
				case PLUS:
					if ( c != '\n' ) etat=ERREUR; /* Ces quatres états ont été standardisés et ne doivent pas avoir d'autre caractère */
					break;
				
				case DECIMAL_ZERO: /* On va chercher si le prochain caractere est 'x' pour savoir si la valeur sera un nombre hexadecimal ou non */
					if(c=='x' || c=='X') etat=HEXA;
					else if (isdigit(c) && c<'8') etat=OCTAL;
					else if (c=='\n') { 
						printf("le lexeme est un nombre decimal\n");}
			
					else WARNING_MSG("L'association des caracteres ne forme pas un lexeme valable");
					break;
			
				case DEBUT_HEXADECIMAL:
					if(isxdigit(c)) etat=HEXA;
					else WARNING_MSG("L'association des caracteres ne forme pas un lexeme valable");
					break;  
			
				case HEXA: 
					if(isxdigit(c)) etat=HEXA;
					else if (c=='\n'){ 
						printf("le lexeme est un nombre hexadecimal\n");}
					else WARNING_MSG("L'association des caracteres ne forme pas un lexeme valable");
					break;

				case DECIMAL: 
					if (isdigit(c)) etat=DECIMAL;
					else if (c=='\n') {
						printf("le lexeme est un nombre decimal\n");}
					else WARNING_MSG("L'association des caracteres ne forme pas un lexeme valable");
					break;

				case OCTAL: 
					if (isdigit(c) && c<'8') etat=OCTAL;
					else if (c=='\n') {
						printf("le lexeme est un nombre octal\n");}
					else WARNING_MSG("L'association des caracteres ne forme pas un lexeme valable");
					break;
			
				case SYMBOLE:
					if (c==':') etat=ETIQUETTE;
					else if(c=='\n') {
						printf("le lexeme est un symbole");}
					else if (isalpha(c)) etat=SYMBOLE;
					else WARNING_MSG("L'association des caracteres ne forme pas un lexeme valable");

				case POINT:
					if (isalpha(c)) etat=DIRECTIVE;
					else WARNING_MSG("L'association des caracteres ne forme pas un lexeme valable");
					break;
					
				case ETIQUETTE:
					if (c=='\n') {
						printf("le lexeme est une etiquette\n");}
					else WARNING_MSG("L'association des caracteres ne forme pas un lexeme valable");
					break;
			
				case DIRECTIVE:
					if (isalpha(c)) etat=DIRECTIVE;
					else if (c=='\n') {
						printf("Le lexeme est une directive");
						}
					break;

				case REGISTRE:
				    break;	
				    				
				case RETOUR_LIGNE:
				    break;					

				case COMMENTAIRE:
				
					/* Tous les caractères restants jusqu'au retour chariot font partie du commentaire */
				
					if (c=='\n') {
						printf("Le lexeme est un commentaire");}
					else etat=COMMENTAIRE;
					break;
			
				case ERREUR:
				    break;					
			}		    
		
			i++;
        }
        
        
        puts(token);
    }

    return debut_liste_p;
}

/**
 * @param file Assembly source code file name.
 * @param nlines Pointer to the number of lines in the file.
 * @return should return the collection of lexemes
 * @brief This function loads an assembly code from a file into memory.
 *
 */
void lex_load_file( char *file, unsigned int *nlines ) {

    FILE        *fp   = NULL;
    char         line[STRLEN]; /* original source line */
    char         res[2*STRLEN]; /* standardised source line, can be longeur due to some possible added spaces*/

    fp = fopen( file, "r" );
    if ( NULL == fp ) {
        /*macro ERROR_MSG : message d'erreur puis fin de programme ! */
        ERROR_MSG("Error while trying to open %s file --- Aborts",file);
    }

    *nlines = 0;

    while(!feof(fp)) {

        /*read source code line-by-line */
        if ( NULL != fgets( line, STRLEN-1, fp ) ) {
            line[strlen(line)-1] = '\0';  /* eat final '\n' */
            (*nlines)++;

            if ( 0 != strlen(line) ) {
                lex_standardise( line, res );
                lex_read_line( res, *nlines );
            }
        }
    }

    fclose(fp);
    return;
}



/**
 * @param in Input line of source code (possibly very badly written).
 * @param out Line of source code in a suitable form for further analysis.
 * @return nothing
 * @brief This function will prepare a line of source code for further analysis.
 */

/* note that MIPS assembly supports distinctions between lower and upper case*/
void lex_standardise( char* in, char* out ) {

    unsigned int i, j;

    INFO_MSG("in  = %s", in);
    
    for ( i= 0, j= 0; i < strlen(in); i++ ) {
        /*TODO : ajouter les autres transformations : fait */
        /* If comment, copy until end of line without changes */
        if ( in[i] == '#') {
        	for ( ; i < strlen(in); i++ ) {
        		out[j++]=in[i];
        	}
        }
        
        /* Make sure there is a space before ':', '$', ',', '(', ')', '-', '+' */
        if (( in[i] == ':' ) || ( in[i] == '$' ) || ( in[i] == ',' ) || ( in[i] == '(' ) || ( in[i] == ')' ) || ( in[i] == '-' ) || ( in[i] == '+' ) ) {
			if ( j > 0 ) {
				if ( out[j-1] != ' ' ) {
					out[j++]=' ';
				}
			}          
        }
        
        /* translate all spaces (i.e., tab) into simple spaces */
        if (isblank((int) in[i])) out[j++]=' ';
        /* otherwise copy as is */
        else out[j++]=in[i];

        /* Make sure there is a space after ':', ',', '(', ')', '-', '+' */
        if (( in[i] == ':' ) || ( in[i] == ',' ) || ( in[i] == '(' ) || ( in[i] == ')' ) || ( in[i] == '-' ) || ( in[i] == '+' ) ) {
			if ( i+1 < strlen(in) ) {
				if (!isblank((int) in[i+1])) {
					out[j++]=' ';
				}
			}          
        }
    }
    out[j]='\0';

    INFO_MSG("out = %s", out);
}

Liste_lexeme_t * lex_list_item_create(enum Nature_lexeme_t nature, char* data, int ligne) {
	return NULL;
}

void lex_list_visualisation(Liste_lexeme_t * liste_p) {
}


void lex_list_free(Liste_lexeme_t * liste_p) {
}





