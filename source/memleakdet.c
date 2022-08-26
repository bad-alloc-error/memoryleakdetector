#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#include"../include/memleakdet.h"


struct_db_t* create_struct_db(void){
    struct_db_t* db = (struct_db_t *)malloc(sizeof(struct_db_t));
    db->head = NULL;
    db->tail = NULL;
    db->size = 0;
    return db;
}

db_rec_t* register_structure(struct_db_t* db, const char* struct_name, unsigned int sizeof_structure, field_info_t* fields, unsigned int num_fields){

   db_rec_t* struct_rec = (db_rec_t *)malloc(sizeof(db_rec_t));

    /*popula*/
    struct_rec->next = NULL;
    strncpy(struct_rec->struct_name, struct_name, MAX_STRUCT_NAME_SIZE);
    struct_rec->size = sizeof_structure;
    struct_rec->num_fields = num_fields;
    struct_rec->fields = fields;

    printf("Nome estrutura: %s - tamanho da estrutura: %d - n. campos: %d - ponteiro para campos: %p - endereço da db_rec_t: %p\n",
        struct_name, sizeof_structure, num_fields, fields, struct_rec);
        
    /*adiciona a estrutura ao "structure database"*/
    if(!add_struct_to_db(struct_rec, db)){
        exit(EXIT_FAILURE);
    }

    return struct_rec;
}

bool add_struct_to_db(db_rec_t* structure, struct_db_t* db){

    printf("struct_db_t: head: %p tail: %p tamanho: %d\n", db->head, db->tail, db->size);
    if(db->head == NULL && db->tail == NULL){
        db->head = structure;
        db->tail = structure;
        db->size++;
        printf("db_rec_t endereço: %p - db->head endereço: %p db->tail endereço: %p\n", structure, db->head, db->tail);
        return true;
    }

    db_rec_t* node = db->tail;
    db->tail = structure;
    structure->next = node;
    db->size++;
    return true;

}

void print_struct_db(struct_db_t* struct_db){
    
    for(db_rec_t* node = struct_db->head; node; node = node->next){
        printf("Nome da Estrutura: [ %s ] Tamanho: [ %d ] Número de Campos: [ %d ]\n",
             node->struct_name, node->size, node->num_fields);

    }

    printf("Quantidade de estruturas registradas: [ %d ]\n", struct_db->size);
}

void print_struct_info(db_rec_t* structure){
    
    field_info_t* field = NULL;
    printf("Nome da Estrutura: [ %s ] Tamanho: [ %d ] Número de Campos: [ %d ]\n", 
        structure->struct_name, structure->size, structure->num_fields);
                 
        for(unsigned int count = 0; count < structure->num_fields; count++){
            field = &structure->fields[count];
            printf("Nome: [ %s ] Tamanho: [ %d ] Offset: [ %d ] Tipo: [ %d ]\n",
                field->name, field->size, field->offset, field->data_type);   
        }
}
