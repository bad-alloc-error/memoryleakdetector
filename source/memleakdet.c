#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#include"../include/memleakdet.h"

void register_structure(struct_db_t* db, const char* struct_name, field_info_t* fields){
    
    do{
        db_rec_t* struct_rec = (db_rec_t *)malloc(sizeof(db_rec_t));
        /*popula*/
        struct_rec->next = NULL;
        strncpy(struct_rec->struct_name, struct_name, MAX_STRUCT_NAME_SIZE);
        struct_rec->size = sizeof(struct_name);
        struct_rec->num_fields = sizeof(fields) / sizeof(field_info_t);
        struct_rec->fields = fields;

        /*adiciona a estrutura ao "structure database"*/
        if(add_struct_to_db(struct_rec, db)){
            exit(EXIT_FAILURE);
        }
    }while(0);
}


bool add_struct_to_db(db_rec_t* structure, struct_db_t* db){

    if(db->head == NULL && db->tail == NULL){
        db->head = structure;
        db->tail = structure;
        db->size++;
        return true;

    }else{

        db_rec_t* node = db->tail;
        db->tail = structure;
        structure->next = node;
        db->size++;
        return true;
    }

    return false;
}

void print_struct_db(struct_db_t* struct_db){
    
    for(db_rec_t* node = struct_db->head; node->next != NULL; node = node->next){
        printf("Nome da Estrutura: [ %s ] Tamanho: [ %d ] Número de Campos: [ %d ]\n",
             node->struct_name, node->size, node->num_fields);

    }

}

void print_struct_info(db_rec_t* structure){
    
    printf("Nome da Estrutura: [ %s ] Tamanho: [ %d ] Número de Campos: [ %d ]\n", 
        structure->struct_name, structure->size, structure->num_fields);

        unsigned int count = 0;
                 
        for(field_info_t* field = structure->fields; count < structure->num_fields; count++){
            printf("Nome: [ %s ] Tamanho: [ %d ] Offset: [ %d ] Tipo: [ %d ]\n",
                field->name, field->size, field->offset, field->data_type);   
        }
}
