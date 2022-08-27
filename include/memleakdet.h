#ifndef _MEMLEAKDET_H_
#define _MEMLEAKDET_H_

#include<stdbool.h>

#define MAX_STRUCT_NAME_SIZE 100
#define MAX_FIELD_NAME 100
typedef struct struct_db_t struct_db_t;
typedef struct db_rec_t db_rec_t;
typedef struct field_info_t field_info_t;
typedef struct object_db_rec_t object_db_rec_t;
typedef struct object_db_t object_db_t;


/*
*
*   Suponhamos uma struct chamada estudante com 3 campos.
*
*    struct estudante{
*        char nome[30];
*        char email[50];
*        int idade;
*    };
*    
*   representação db_rec_t
*                   
*   *next    struct_name   ds_size               num_fields  *fields
*   ------------------------------------------------------------------
*   |NULL    | "estudante" | sizeof(estudante_t) |    3   |   fields*| 
*   ------------------------------------------------------------------
*                                                               |
*                                                               |
*        representação do field_info_t                          |
*        a estrutura db_rec_t tem um ponteiro para um array     |
*        do tipo field_info_t e essa é a representação das      |
*        duas estruturas instanciadas na memória                |
*                                                               v
*
*            name      size                offset  data_type  nested_struct_name
*            ---------------------------------------------------------------------------                                                  
*   fields[0]| "nome" | sizeof(char) * 30 |  0   |  CHAR    |  NULL                    |
*            ---------------------------------------------------------------------------
*
*            name        size               offset  data_type  nested_struct_name
*            ---------------------------------------------------------------------------                                                   
*   fields[1]| "email" | sizeof(char) * 50 |  30   | CHAR      | NULL                  |
*            ---------------------------------------------------------------------------
* 
*            name        size                    offset  data_type     nested_struct_name
*            ---------------------------------------------------------------------------                                                   
*   fields[2]| "idade"   | sizeof(unsigned int) |  80   | UINT32      | NULL           |
*            ---------------------------------------------------------------------------
*
*   A estrutura field_info_t armazenada informações de cada campo da estrutura registrada
*   em db_rec_t.
*/


typedef enum{
    UINT8,
    UINT32,
    INT32,
    CHAR,
    FLOAT,
    DOUBLE,
    OBJ_STRUCTURE,
    OBJ_POINTER
} data_type_t;


void print_struct_db(struct_db_t* struct_db);
void print_struct_info(db_rec_t* structure);
void* fmalloc(object_db_t* obj_db, const char* struct_name, unsigned int units);
static void register_object(object_db_t* obj_db, void* ptr, unsigned int units, db_rec_t* struct_rec);
object_db_rec_t* obj_db_peek(object_db_t* obj_db, void* ptr);
struct_db_t* create_struct_db(void);
db_rec_t* db_peek(struct_db_t* struct_db, char* struct_name);
db_rec_t* register_structure(struct_db_t* db, const char* struct_name, unsigned int sizeof_structure, field_info_t* fields, unsigned int num_fields);
bool add_struct_to_db(db_rec_t* structure, struct_db_t* db);


#endif