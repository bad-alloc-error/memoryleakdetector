#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stddef.h>
#include"include/memleakdet.h"


typedef struct estudante{
    char nome[30];
    char email[50];
    unsigned int idade;
}estudante_t;

int main(){

    struct_db_t* struct_db = create_struct_db();
    estudante_t estudante1;

    // strncpy(estudante1.nome, "Filipe", strlen("Filipe"));
    // strncpy(estudante1.email, "bad-alloc-error@yandex.com", strlen("bad-alloc-error@yandex.com"));
    // estudante1.idade = 28;

    static field_info_t students_info[] = {
        {"nome", sizeof(char) * 30, offsetof(estudante_t, nome), CHAR, 0},
        {"email", sizeof(char) * 50, offsetof(estudante_t, email), CHAR, 0},
        {"idade", sizeof(unsigned int), offsetof(estudante_t, idade), UINT32, 0}
    };

    db_rec_t* new_struct = register_structure(struct_db, "estudante1", sizeof(estudante_t), &students_info[0], 3);

    // print_struct_db(struct_db);
    print_struct_info(new_struct);

    /*testando o object_db_t e função fmalloc()*/

    object_db_t* obj_db = create_object_database();
    estudante_t* filipe = fmalloc(obj_db, "estudante_t", 1);
    // estudante_t* bruna = fmalloc(obj_db, "estudante_t", 1);
    // estudante_t* adelia = fmalloc(obj_db, "estudante_t", 1);

    print_object_details(obj_db);

    return 0;
}