#include<string.h>
#include<stdlib.h>
#include<stdio.h>

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

struct field_info_t{
    char name[MAX_FIELD_NAME];
    unsigned int size;
    unsigned int offset;
    data_type_t data_type;
    char nested_struct_name[MAX_STRUCT_NAME_SIZE];
};

struct object_db_t{
    struct_db_t* struct_rec;
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

struct_db_t* create_struct_db(void){
    struct_db_t* db = (struct_db_t *)malloc(sizeof(struct_db_t));
    db->head = NULL;
    db->tail = NULL;
    db->size = 0;
    return db;
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
    db->tail = structure;
    structure->next = node;
    db->size++;
    return true;

}

/*Procura por determinado registro(de estrutura) e retorna o endereço para esse registro*/
db_rec_t* db_peek(struct_db_t* struct_db, char* struct_name){

    db_rec_t* node = struct_db->head;
    while(!strncmp(node->struct_name, struct_name, MAX_STRUCT_NAME_SIZE) == 0){

        /*Caso não exista o registro*/
        if(node->next == NULL){
            fprintf(stderr, "[LOG] Não existe o registro informado!\n");
            return NULL;
        }

        node = node->next;
    }

    return node;
}

void* fmalloc(object_db_t* obj_db, const char* struct_name, unsigned int units){
    db_rec_t* struct_rec = db_peek(obj_db->struct_rec, struct_name);
    void* ptr = calloc(units, struct_rec->size);
    
    /*adiciona o objeto p/ objetc_db*/
    register_object(obj_db, ptr, units, struct_rec);

    return ptr;
}   

static void register_object(object_db_t* obj_db, void* ptr, unsigned int units, db_rec_t* struct_rec){
   
   /*faz a busca para checar se não se trata do mesmo objeto*/
    object_db_rec_t* obj_rec = obj_db_peek(obj_db, ptr);

    if(obj_rec == ptr){
        fprintf(stderr, "[LOG]Não é permitido registrar duas vezes o mesmo objeto!\n");
        return;
    }

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

    /*se não for o primeiro, faz os backups de ponteiro*/
    object_db_rec_t* node = obj_db->tail;
    node->next = obj_rec;
    obj_db->tail = obj_rec;
    obj_db->size++;
}

object_db_rec_t* obj_db_peek(object_db_t* obj_db, void* ptr){
    object_db_rec_t* node = obj_db->head;

    while(!node->ptr_key == ptr){    

        if(node->next == NULL){
            fprintf(stderr, "[LOG]Objeto não localizado!\n");
            return NULL;
        }

        node = node->next;
    }

    return node;
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
                 
        for(unsigned int i = 0; i < structure->num_fields; i++){
            field = &structure->fields[i];
            printf("Nome: [ %s ] Tamanho: [ %d ] Offset: [ %d ] Tipo: [ %d ]\n",
                field->name, field->size, field->offset, field->data_type);   
        }
}
