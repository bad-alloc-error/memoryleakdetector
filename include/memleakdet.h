#ifndef _MEMLEAKDET_H_
#define _MEMLEAKDET_H_

#define MAX_STRUCT_NAME_SIZE 100
#define MAX_FIELD_NAME 100
typedef struct struct_db_t struc_db_t;
typedef struct db_rec_t db_rec_t;
typedef struct field_info_t field_info_t;


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
*   fields[2]| "idade"   | sizeof(unsigned int) |  84   | UINT32      | NULL           |
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

struct struct_db_t{
    struct struct_db_t* next;
    unsigned int size;
};

struct db_rec_t{
    struct db_rec_t* next;
    char struct_name[MAX_STRUCT_NAME_SIZE];
    unsigned int size;
    unsigned int num_fields;
    struct field_info_t* fields;
};

struct field_info_t{
    char name[MAX_FIELD_NAME];
    unsigned int size;
    unsigned int offset;
    data_type_t data_type;
    char nested_struct_name[MAX_STRUCT_NAME_SIZE];
};


#endif