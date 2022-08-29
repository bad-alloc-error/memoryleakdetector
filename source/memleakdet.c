#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<assert.h>

#include"../include/memleakdet.h"

struct struct_db_t{
    struct db_rec_t* head;
    struct db_rec_t* tail;
    unsigned int size;
};

struct db_rec_t{
    struct db_rec_t* next;
    char struct_name[MAX_STRUCT_NAME_SIZE];
    unsigned int size;
    unsigned int num_fields;
    struct field_info_t* fields;
};

struct object_db_t{
    struct_db_t* struct_db;
    object_db_rec_t* head;
    object_db_rec_t* tail;
    unsigned int size;
};

struct object_db_rec_t{
    object_db_rec_t* next;
    void* ptr_key;
    unsigned int units;
    db_rec_t* struct_rec;
};

struct_db_t* create_struct_database(void){
    struct_db_t* db = (struct_db_t *)malloc(sizeof(struct_db_t));
    db->head = NULL;
    db->tail = NULL;
    db->size = 0;
    return db;
}

object_db_t* create_object_database(struct_db_t* struct_db){

    object_db_t* obj_db = calloc(1, sizeof(object_db_t));

    obj_db->head = NULL;
    obj_db->tail = NULL;
    obj_db->size = 0;
    obj_db->struct_db = struct_db;

    return obj_db;
}

/*Registra a estrutura que será utilizada pela aplicação cliente*/
db_rec_t* register_structure(struct_db_t* db, const char* struct_name, unsigned int sizeof_structure, field_info_t* fields, unsigned int num_fields){

   db_rec_t* struct_rec = (db_rec_t *)malloc(sizeof(db_rec_t));

    /*popula*/
    struct_rec->next = NULL;
    strncpy(struct_rec->struct_name, struct_name, MAX_STRUCT_NAME_SIZE);
    struct_rec->size = sizeof_structure;
    struct_rec->num_fields = num_fields;
    struct_rec->fields = fields;
        
    /*adiciona a estrutura ao "structure database"*/
    if(!add_struct_to_db(struct_rec, db)){
        exit(EXIT_FAILURE);
    }

    return struct_rec;
}

/*Adiciona nosso registro ao banco de registro de estruturas*/
bool add_struct_to_db(db_rec_t* structure, struct_db_t* db){

    if(db->head == NULL && db->tail == NULL){
        db->head = structure;
        db->tail = structure;
        db->size++;
        return true;
    }

    db_rec_t* node = db->tail;
    node->next = structure;
    db->tail = structure;
    db->size++;
    return true;

}

/*Procura por determinado registro(de estrutura) e retorna o endereço para esse registro*/
db_rec_t* db_peek(struct_db_t* struct_db, char* struct_name){

    db_rec_t* node = struct_db->head;

    while(node && !strncmp(node->struct_name, struct_name, MAX_STRUCT_NAME_SIZE) == 0){
        
        if(node->next == NULL && strncmp(node->struct_name, struct_name, MAX_STRUCT_NAME_SIZE) != 0){
            fprintf(stderr, "[LOG]Não existe registro da estrutura [ %s ].\nEstruturas registradas:\n", struct_name);
            dump_struct_db(struct_db);
            return NULL;
        }
        node = node->next;
    }
    return node;
}

void* fmalloc(object_db_t* obj_db, char* struct_type, unsigned int units){

    db_rec_t* struct_rec = db_peek(obj_db->struct_db, struct_type);

    if(!struct_rec){
        fprintf(stderr, "Erro\n");
        exit(EXIT_FAILURE);
    }
    void* ptr = calloc(units, struct_rec->size);
    
    /*adiciona o objeto p/ objetc_db*/
    register_object(obj_db, ptr, units, struct_rec);

    return ptr;
}   

static void register_object(object_db_t* obj_db, void* ptr, unsigned int units, db_rec_t* struct_rec){
   
   /*faz a busca para checar se não se trata do mesmo objeto*/
    object_db_rec_t* obj_rec = obj_db_peek(obj_db, ptr);

    assert(!obj_rec);

    /*popula o registro*/
    obj_rec = calloc(1, sizeof(object_db_rec_t));
    obj_rec->next = NULL;
    obj_rec->ptr_key = ptr;
    obj_rec->struct_rec = struct_rec;
    obj_rec->units = units;

    /*agora adiciona o objeto no banco de objetos*/
    if(obj_db->head == NULL && obj_db->tail == NULL){
        obj_db->head = obj_rec;
        obj_db->tail = obj_rec;
        obj_db->size++;
        return;
    }

    /*se não for o unico, faz os backups de ponteiro*/
    object_db_rec_t* node = obj_db->tail;
    node->next = obj_rec;
    obj_db->tail = obj_rec;
    obj_db->size++;
}

static object_db_rec_t* obj_db_peek(object_db_t* obj_db, void* ptr){
   
    object_db_rec_t* node = obj_db->head;

    if(node == NULL){ return NULL; }

    while(node){

        if(node->ptr_key == ptr){
            return node;
        }    

        node = node->next;
    }

    return NULL;
}

void dump_struct_db(struct_db_t* struct_db){
    
    for(db_rec_t* node = struct_db->head; node; node = node->next){
        printf("Estrutura: [ %s ] Tamanho: [ %d ] Número de Campos: [ %d ]\n",
             node->struct_name, node->size, node->num_fields);

    }

    printf("Quantidade de estruturas registradas: [ %d ]\n", struct_db->size);
}

void dump_struct_info(db_rec_t* structure){
    
    field_info_t* field = NULL;
    printf("Dump de Estrutura Registrada\n");
    printf(" + Tipo da Estrutura: [ %s ] - Tamanho: [ %d ] - Número de Campos: [ %d ]\n", 
        structure->struct_name, structure->size, structure->num_fields);
                 
        for(unsigned int i = 0; i < structure->num_fields; i++){
            field = &structure->fields[i];
            printf(" + Campo: [ %s ] - Tamanho: [ %d ] - Offset: [ %d ] - Tipo: [ %d ]\n",
                field->name, field->size, field->offset, field->data_type);   
        }
}

void dump_object_details(object_db_t* obj_db){
    object_db_rec_t* node = obj_db->head;

    printf("Número de objetos registrados: [ %d ]\n", obj_db->size);
    while(node){
        printf(" - Tipo da Estrutura: [ %s ]\n - Unidades Alocadas: [ %d ]\n - Referência do Objeto: [ %p ]\n - Próxima Referência: [ %p ]\n",
            node->struct_rec->struct_name, node->units, node, node->next);
        node = node->next;
    }
} 

void dump_object_record_details(object_db_rec_t* obj_rec){

    field_info_t* f_info = NULL;

    printf("+ Detalhes da Instância\n");
    printf(" - Tipo  [ %s ]\n", obj_rec->struct_rec->struct_name);
    
    for(unsigned int obj_i = 0; obj_i < obj_rec->units; ++obj_i){

                                  /*chunk alocado*/      
        char *curr_obj = (char *)(obj_rec->ptr_key) + (obj_i * obj_rec->struct_rec->size);
       
        for(unsigned int i = 0; i < obj_rec->struct_rec->num_fields; ++i){
            
            f_info = &obj_rec->struct_rec->fields[i];

            switch(f_info->data_type){
                
                case CHAR:

                    printf(" + [ %s ][%d] -> [ %s ] = [ %d ]\n",
                    obj_rec->struct_rec->struct_name, obj_i, f_info->name,
                    (char *)(curr_obj + f_info->offset));

                    break;

                case UINT8:
                case UINT32:
                case INT32:

                    printf(" + [ %s ][%d] -> [ %s ] = [ %d ]\n",
                    obj_rec->struct_rec->struct_name, obj_i, f_info->name,
                    *(int *)(curr_obj + f_info->offset));
                    
                    break;

                case FLOAT:

                    printf(" + [ %s ][%d] -> [ %s ] = [ %d ]\n",
                    obj_rec->struct_rec->struct_name, obj_i, f_info->name,
                    *(float *)(curr_obj + f_info->offset));

                    break;
                
                case DOUBLE:

                    printf(" + [ %s ][%d] -> [ %s ] = [ %d ]\n",
                    obj_rec->struct_rec->struct_name, obj_i, f_info->name,
                    *(double *)(curr_obj + f_info->offset));

                    break;

                case OBJ_POINTER:

                    printf(" + [ %s ][%d] -> [ %s ] = [ %d ]\n",
                    obj_rec->struct_rec->struct_name, obj_i, f_info->name,
                    (void *) *(int *)(curr_obj + f_info->offset));

            }
            
        }
    }
}
