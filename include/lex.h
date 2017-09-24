/**
 * @file lex.h
 * @author François Portet <francois.portet@imag.fr>
 * @brief Lexem-related stuff.
 *
 * Contains lexem types definitions, some low-level syntax error codes,
 * the lexem structure definition and the associated prototypes.
 */

#ifndef _LEX_H_
#define _LEX_H_

#include <stdio.h>

/**
 * \enum Nature_lexeme_t
 * \brief Constantes de nature de lexèmes
 * 
 * La nature des lexème einsi que leur éventuelle valeur seront ensuite utilisées dans l'analyse syntaxique
 */
typedef enum Nature_lexeme_t {
 	SYMBOLE,				/**< Chaine de caractères alphanumériques ne commençant pas par un nombre.
 							     Peut désigner une étiquette ou une instruction suivant sa place dans la ligne */
 	
 	DEUX_POINTS,			/**< ':' venant juste après une étiquette. */
 	VIRGULE,				/**< ',' permettant de séparer les opérandes. */
 	PARANTHESE_OUVRANTE,	/**< '(' utilisé pour adressage indirect. */
 	PARANTHESE_FERMANTE,	/**< ')' utilisé pour finir adressage indirect. */
 	
 	REGISTRE,				/**< Nom d'un registre, commence toujours par '$'. */
 	
 	NOMBRE_DECIMAL,			/**< Nombre décimal. */
 	NOMBRE_OCTAL,			/**< Nombre octal. */
 	NOMBRE_HEXADECIMAL,		/**< Nombre hexanumérique. */
 	
 	COMMENTAIRE,			/**< Le commentaire commence par '#' et fini à la fin de la ligne. */
 	FIN_LIGNE,				/**< '\\n' indicateur de fin de ligne. */
 	DIRECTIVE,				/**< commence par un '.'. */
 	ERREUR					/**< Mauvaise configuration de caractères. */
} Nature_lexeme_t;

/**
 * \struct Lexeme_t
 * \brief Type de l'objet lexème
 * 
 * La nature des lexème einsi que leur éventuelle valeur seront ensuite utilisées dans l'analyse syntaxique
 */
typedef struct Lexeme_t {
	enum Nature_lexeme_t nature;	/**< Identifie la nature du lexème. */
	char * data;					/**< Donnée éventuelle stockée sous forme de chaine de caractères.
										 Pour les lexèmes mono-caractères identifiables par leur nature, on laisse NULL. */
	int ligne;						/**< A voir si c'est nécessaire. */
	struct Lexeme_t * suiv;			/**< Pointeur sur le prochain lexeme. A voir si nécessaire ainsi que précédent */
} Lexeme_t;

typedef struct Liste_lexeme_t {
	Lexeme_t lexeme;
	struct Liste_lexeme_t * suiv;
} Liste_lexeme_t;


Liste_lexeme_t * lex_read_line( char *, int);
void	lex_load_file( char *, unsigned int * );
void	lex_standardise( char*, char*  );

Liste_lexeme_t * lex_list_item_create(enum Nature_lexeme_t nature, char* data, int ligne);
void lex_list_visualisation(Liste_lexeme_t * liste_p);
void lex_list_free(Liste_lexeme_t * liste_p);

#endif /* _LEX_H_ */

