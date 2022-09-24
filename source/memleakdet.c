#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<assert.h>

#include"../include/memleakdet.h"

struct struct_db_t{
    struct struct_meta_data_t* head;
    struct struct_meta_data_t* tail;
    unsigned int size;
};

struct struct_meta_data_t{
    struct struct_meta_data_t* next;
    char struct_name[MAX_STRUCT_NAME_SIZE];
    unsigned int size;
    unsigned int num_fields;
    struct field_info_t* fields;
};

struct object_db_t{
    struct struct_db_t* struct_db;
    struct object_meta_data_t* head;
    struct object_meta_data_t* tail;
    unsigned int size;
};

struct object_meta_data_t{
    struct object_meta_data_t* next;
    void* ptr_key;
    unsigned int num_blocks;
    struct struct_meta_data_t* struct_rec;
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
struct_meta_data_t* register_structure(struct_db_t* db, const char* struct_name, unsigned int sizeof_structure, field_info_t* fields, unsigned int num_fields){

   struct_meta_data_t* struct_rec = (struct_meta_data_t *)malloc(sizeof(struct_meta_data_t));

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
static bool add_struct_to_db(struct_meta_data_t* structure, struct_db_t* db){

    if(db->head == NULL && db->tail == NULL){
        db->head = structure;
        db->tail = structure;
        db->size++;
        return true;
    }

    struct_meta_data_t* node = db->tail;
    node->next = structure;
    db->tail = structure;
    db->size++;
    return true;

}

/*Procura por determinado registro(de estrutura) e retorna o endereço para esse registro*/
static struct_meta_data_t* db_peek(struct_db_t* struct_db, char* struct_name){

    struct_meta_data_t* node = struct_db->head;

    while(node && !strncmp(node->struct_name, struct_name, MAX_STRUCT_NAME_SIZE) == 0){
        
        if(node->next == NULL && strncmp(node->struct_name, struct_name, MAX_STRUCT_NAME_SIZE) != 0){
            fprintf(stderr, "[LOG]Não existe registro da estrutura [ %s ].\n[*]Estruturas registradas:\n", struct_name);
            dump_struct_db(struct_db);
            return NULL;
        }
        node = node->next;
    }
    return node;
}

void* fmalloc(object_db_t* obj_db, char* struct_type, unsigned int num_blocks){

    struct_meta_data_t* struct_rec = db_peek(obj_db->struct_db, struct_type);

    if(!struct_rec){
        fprintf(stderr, "[LOG]Registro de Estrutura não encontrado!\n");
        exit(EXIT_FAILURE);
    }
    void* ptr = calloc(num_blocks, struct_rec->size);
    
    /*adiciona o objeto p/ objetc_db*/
    register_object(obj_db, ptr, num_blocks, struct_rec);

    return ptr;
}   

static void register_object(object_db_t* obj_db, void* ptr, unsigned int num_blocks, struct_meta_data_t* struct_rec){
   
   /*faz a busca para checar se não se trata do mesmo objeto*/
    object_meta_data_t* obj_rec = obj_db_peek(obj_db, ptr);

    assert(!obj_rec);

    /*popula o registro*/
    obj_rec = calloc(1, sizeof(object_meta_data_t));
    obj_rec->next = NULL;
    obj_rec->ptr_key = ptr;
    obj_rec->struct_rec = struct_rec;
    obj_rec->num_blocks = num_blocks;

    /*agora adiciona o objeto no banco de objetos*/
    if(obj_db->head == NULL && obj_db->tail == NULL){
        obj_db->head = obj_rec;
        obj_db->tail = obj_rec;
        obj_db->size++;
        return;
    }

    /*se não for o unico, faz os backups de ponteiro*/
    object_meta_data_t* node = obj_db->tail;
    node->next = obj_rec;
    obj_db->tail = obj_rec;
    obj_db->size++;
}

static object_meta_data_t* obj_db_peek(object_db_t* obj_db, void* ptr){
   
    object_meta_data_t* node = obj_db->head;

    if(node == NULL){ return NULL; }

    while(node){

        if(node->ptr_key == ptr){
            return node;
        }    

        node = node->next;
    }

    return NULL;
}

void dump_struct_db(const struct_db_t* struct_db){
    
    for(struct_meta_data_t* node = struct_db->head; node; node = node->next){
        printf("Estrutura: [ %s ] Tamanho: [ %d ] Número de Campos: [ %d ]\n",
             node->struct_name, node->size, node->num_fields);

    }

    printf("Quantidade de estruturas registradas: [ %d ]\n", struct_db->size);
}

void dump_struct_meta_data_info(const struct_meta_data_t* structure){
    
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

void dump_object_db(const object_db_t* obj_db){
    object_meta_data_t* node = obj_db->head;
    printf("Número de objetos registrados: [ %d ]\n", obj_db->size);
    while(node){
        printf(" - Tipo da Estrutura: [ %s ]\n - Qtde. Blocos Alocados: [ %d ]\n - Referência do Objeto: [ %p ]\n - Próxima Referência: [ %p ]\n",
            node->struct_rec->struct_name, node->num_blocks, node, node->next);
        node = node->next;
        printf("---------------------------------------------------------------\n\n");
    }

} 

/*
    Basicamente essa função primeiro vai iterar sobre os blocos que foram alocados
    pela função fmalloc() onde o ultimo parametro é a quantidade de blocos na heap
    que deve ser alocado. E dentro desse bloco ele itera sobre o array de fields do
    objeto struct_meta_data_t e pelo switch atraves do tipo do field ele mostra os valores
    de forma correta.
*/
void dump_object_record_info(const object_meta_data_t* obj_rec){

    field_info_t* field_info = NULL;

    printf("+ Detalhes da Instância\n");
    printf(" + Tipo  [ %s ]\n", obj_rec->struct_rec->struct_name);
    
    for(unsigned int block_index = 0; block_index < obj_rec->num_blocks; ++block_index){

                                  /*chunk alocado*/      
        char *curr_obj = (char *)(obj_rec->ptr_key) + (block_index * obj_rec->struct_rec->size);
       
        for(unsigned int i = 0; i < obj_rec->struct_rec->num_fields; ++i){
            
            /* "dumpando" as infos do campo do index corrente */
            field_info = &obj_rec->struct_rec->fields[i];

            switch(field_info->data_type){
                
                case CHAR:

                    printf(" + [ %s ][%d] -> [ %s ] = [ %s ]\n",
                    obj_rec->struct_rec->struct_name, block_index, field_info->name,
                    (char *)(curr_obj + field_info->offset));

                    break;

                case UINT8:
                case UINT32:
                case INT32:

                    printf(" + [ %s ][%d] -> [ %s ] = [ %d ]\n",
                    obj_rec->struct_rec->struct_name, block_index, field_info->name,
                    *(int *)(curr_obj + field_info->offset));
                    
                    break;

                case FLOAT:

                    printf(" + [ %s ][%d] -> [ %s ] = [ %f ]\n",
                    obj_rec->struct_rec->struct_name, block_index, field_info->name,
                    *(float *)(curr_obj + field_info->offset));

                    break;
                
                case DOUBLE:

                    printf(" + [ %s ][%d] -> [ %s ] = [ %f ]\n",
                    obj_rec->struct_rec->struct_name, block_index, field_info->name,
                    *(double *)(curr_obj + field_info->offset));

                    break;

                case OBJ_POINTER:

                    printf(" + [ %s ][%d] -> [ %s ] = [ %p ]\n",
                    obj_rec->struct_rec->struct_name, block_index, field_info->name,
                    (void *) *(int *)(curr_obj + field_info->offset));

                    break;

            }
            
        }
    }
}

void ffree(void* ptr, object_db_t* obj_db){

    if(!ptr){
        fprintf(stderr, "[LOG]Objeto não instanciado!\n");
        return;
    }
    
    object_meta_data_t* obj_rec = obj_db_peek(obj_db, ptr);

     if(!obj_rec){
        fprintf(stderr, "[LOG]Objeto não encontrado!\n");
        return;
    }

    free(obj_rec->ptr_key);
    obj_rec->ptr_key = NULL;
    remove_object_database(obj_rec, obj_db);
}

void remove_object_database(object_meta_data_t* obj_rec, object_db_t* obj_db){

    if(!obj_db){
        fprintf(stderr, "[LOG]Banco de objetos não existe!\n");
        exit(EXIT_FAILURE);
    }

    object_meta_data_t* node = obj_db->head;

    if(node == obj_rec){
        obj_db->head = obj_rec->next;
        free(obj_rec);
        obj_db->size--;
        return;
    }

    /*já checamos a cabeça da lista, então node passa a ser o head->next*/
    node = node->next;

    /*enquanto o próximo nó não for o nó que queremos deletar...*/
    while(node && node->next != obj_rec){
        node = node->next;
    }

    /*o nó atual que é o anterior ao nó a ser deletado, aponta para o próximo nó do nó a ser deletado*/
    node->next = obj_rec->next;

    free(obj_rec);
    obj_rec = NULL;
    obj_db->size--;
}
    
