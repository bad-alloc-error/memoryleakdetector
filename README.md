# memoryleakdetector
Garbage Collector / Memory Leak Detector

O que é o memory leak detector?

Essa ferramenta será uma lib e sua aplicação C/C++ usará esta lib para gerenciamento de memória.

A lib acompanhará todos os objetos na heap que a aplicação criou e como esses objetos na heap mantêm referências uns aos outros.

O detector ira mostrar os objetos "leakados", se tiver algum.

Algumas estruturas:

 - struct_db_t e object_db_t: é o "banco de dados" que armazena/gerencia as estruturas object_meta_data_t e              struct_meta_data_t;

 - object_meta_data_t mantém as informações relacionadas aos objetos instânciados pela aplicação cliente. Cada instância tem sua estrutura object_meta_data_t.

 - struct_meta_data_t manter as informações relacionadas as estruturas que serão instânciadas pela aplicação cliente. É dever da aplicação repassar essas informações, mais detalhes abaixo.



<b>Primeira Fase: Structure Database</b>

Implementação do registro de estruturas. A lib precisa saber as informações sobre todas as estruturas que estão sendo usadas pela aplicação cliente. É responsabilidade da aplicação cliente dizer a lib durante sua iniciaçização sobre todas as estruturas que serão usadas, nesse projeto esse processo é chamado de registro de estruturas ou "structure database".

A lib irá manter uma "banco de dados" de estruturas que irá armazenar as informações dessas estruturas. Entenda que aqui banco de dados não é um banco relacional ou SGDBs etc e sim apenas no sentido estrito da palavra: vai manter uma coleção de informações sobre algo de forma organizada.

A chave usada para encontrar esse registros será o nome da propria estrutura.

<b>Segunda Fase: Object Database</b>

Toda vez que a aplicação cliente alocar novos chunks de memória, pode ser via malloc, calloc, usando outro gerenciador de memoria ou uma função wrapper que é o caso aqui, a lib tem que ser informada sobre essa alocação com os metadados sobre o objeto.

A lib então vai armazenar informações relevantes sobre o objeto, como os detalhes da estrutura do objeto, endereço...

Objeto entende-se como as estruturas que são usadas pela aplicação e foram instanciadas.

Feito isso sera populado a estrutura com essas informações do novo objeto.

