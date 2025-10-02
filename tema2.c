#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct satellite{
    int freq;
    char *name;
}Satellite;

typedef struct node{
    struct node *st, *dr;
    struct satellite s;
}*Node;

typedef struct cell{
    struct cell *next;
    Node data;
}Queuecell;

typedef struct Queue{
    Queuecell *first, *last;
}Queue;

typedef int (*CompF)(Node, Node);

typedef struct heap{
    int memory, nrElem;
    Node *v;
    CompF comp;
}Heap;

Heap *init_heap(int mem, CompF comp){
    Heap *heap = malloc(sizeof(struct heap));
    heap->comp = comp;
    heap->nrElem = 0;
    heap->memory = mem;
    heap->v = malloc(mem * sizeof(struct node));
    return heap;
}

int minHeap(Node n1, Node n2){
    if(n1->s.freq != n2->s.freq)
        return n1->s.freq < n2->s.freq;
    if(strcmp(n1->s.name, n2->s.name) < 0)
        return 1;
    return 0;
}

void swap_nodes(Node* n1, Node* n2){
    Node tmp_node = *n1;
    *n1 = *n2;
    *n2 = tmp_node;
}

void insert_heap(Heap *heap, Node new_node){
    int nr = heap->nrElem;
    if(heap->nrElem >= heap->memory){
        heap->memory++;
        heap->v = realloc(heap->v, heap->memory * sizeof(Node));
    }
    heap->v[heap->nrElem] = new_node;
    // Pastrez proprietatea de minHeap cand adaug un element nou
    while(nr > 0 && heap->comp(heap->v[nr], heap->v[(nr - 1) / 2]) == 1){
        swap_nodes(&heap->v[nr], &heap->v[(nr - 1) / 2]);
        nr--;
        nr /= 2;
    }
    heap->nrElem++;
}

Node extract_and_sort_heap(Heap *heap){
    int st, dr, i, okay;
    // Cu i iterez prin heap si cu okay verific daca am pus ultimul element
    // pe pozitia sa corespunzatoare minHeap-ului
    i = okay = 0;
    if(heap->nrElem == 0){
        return NULL;
    }
    // Retin primul element (cel pe care vreau sa il scot)
    Node tmp = heap->v[0];
    heap->nrElem--;
    // Refac heap-ul ca sa isi pastreze proprietatea de minHeap
    heap->v[0] = heap->v[heap->nrElem];
    while(okay == 0){
        okay = 1;
        st = i * 2 + 1;
        dr = i * 2 + 2;
        if(st < heap->nrElem && heap->comp(heap->v[st], heap->v[i])){
            okay = 0;
            if(dr < heap->nrElem && heap->comp(heap->v[dr], heap->v[st])){
                swap_nodes(&heap->v[i], &heap->v[dr]);
                i = dr;
            }
            else{
                swap_nodes(&heap->v[i], &heap->v[st]);
                i = st;
            }
        }
        else if(dr < heap->nrElem && heap->comp(heap->v[dr], heap->v[i])){
            okay = 0;
            swap_nodes(&heap->v[i], &heap->v[dr]);
            i = dr;
        }
    }
    return tmp;
}

Node build_tree(FILE *file_in){
    int nr_sat;
    fscanf(file_in, "%d", &nr_sat);
    Heap *heap = init_heap(10, minHeap);
    // Pun toti satelitii in heap
    for(int i = 0; i < nr_sat; i++){
        Node new_node = malloc(sizeof(struct node));
        new_node->st = new_node->dr = NULL;
        new_node->s.name = malloc(16 * sizeof(char));
        fscanf(file_in, "%d%s", &new_node->s.freq, new_node->s.name);
        insert_heap(heap, new_node);
    }
    while(heap->nrElem >= 2){
        // Construiesc legaturile arborelui binar
        Node new_node = malloc(sizeof(struct node)), p, q;
        p = extract_and_sort_heap(heap);
        q = extract_and_sort_heap(heap);
        // Adun cele 2 frecvente si concatenez numele lor
        new_node->s.freq = p->s.freq + q->s.freq;
        int mem = strlen(p->s.name) + strlen(q->s.name);
        mem++;
        new_node->s.name = malloc(mem * sizeof(char));
        strcpy(new_node->s.name, p->s.name);
        strcat(new_node->s.name, q->s.name);
        // Stabilesc nodul stang si nodul drept din arbore
        if(minHeap(p, q) == 1){
            new_node->st = p;
            new_node->dr = q;
        }
        else{
            new_node->st = q;
            new_node->dr = p;
        }
        insert_heap(heap, new_node);
    }
    Node root = extract_and_sort_heap(heap);
    free(heap->v);
    free(heap);
    return root;
}

void enqueue(Queue *q, Node new_node){
    Queuecell *tmp = malloc(sizeof(struct cell));
    tmp->next = NULL;
    tmp->data = new_node;
    if(q->first == NULL){
        q->first = q->last = tmp;
    }
    else{
        q->last->next = tmp;
        q->last = tmp;
    }
}

Node dequeue(Queue *q){
    Queuecell *tmp;
    Node new_node = q->first->data;
    if(q->first == q->last){
        free(q->first);
        q->first = q->last = NULL;

    }
    else{
        tmp = q->first;
        q->first = q->first->next;
        free(tmp);
    }
    return new_node;
}

void BFS(Node root, FILE *file_out){
    // Initializez coada si adaug radacina in coada
    Queue *q = malloc(sizeof(struct Queue));
    q->first = NULL;
    enqueue(q, root);
    Node new_node;
    // nr reprezinta numarul de noduri de pe nivelul curent
    int nr = 1;
    while(q->first != NULL){
        // tmp calculeaza numarul de noduri de pe nivelul urmator
        int tmp = nr;
        for(int i = 0; i < nr && q->first != NULL; i++){
            // Elimin nodul curent din coada, il afisez, si adaug nodurile din
            // stanga si dreapta nodului curent in coada
            new_node = dequeue(q);
            tmp--;
            fprintf(file_out, "%d-%s ", new_node->s.freq, new_node->s.name);
            if(new_node->st != NULL){
                tmp++;
                enqueue(q, new_node->st);
            }
            if(new_node->dr != NULL){
                tmp++;
                enqueue(q, new_node->dr);
            }
            free(new_node->s.name);
            free(new_node);
        }
        // Actualizez numarul de noduri de pe nivelul urmator
        nr = tmp;
        fprintf(file_out, "\n");
    }
    free(q);
}

void task2(Node root, FILE *file_in, FILE *file_out){
    char codif_bin[1001];
    fgets(codif_bin, 1001, file_in);
    int length = strlen(codif_bin);
    Node tmp = root;
    for(int i = 0; i < length; i++){
        if(codif_bin[i] == '0' && tmp->st != NULL){
            tmp = tmp->st;
        }
        else if(codif_bin[i] == '1' && tmp->dr != NULL){
            tmp = tmp->dr;
        }
        // Am ajuns la o frunza
        if(tmp->st == NULL && tmp->dr == NULL){
            fprintf(file_out, "%s ", tmp->s.name);
            tmp = root;
        }
    }
}

void free_tree(Node root){
    if(root == NULL){
        return;
    }
    free_tree(root->st);
    free_tree(root->dr);
    free(root->s.name);
    free(root);
}

//
int get_bin_codif(Node root, char sat_name[16], char **codif, int mem, int niv){
    if(root == NULL){
        return 0;
    }
    if(niv >= mem - 1){
        mem++;
        *codif = realloc(*codif, mem * sizeof(char));
    }
    if(strcmp(root->s.name, sat_name) == 0){
        (*codif)[niv] = '\0';
        return 1;
    }
    // Aplic DFS
    (*codif)[niv] = '0';
    if(get_bin_codif(root->st, sat_name, codif, mem, niv + 1) == 1){
        return 1;
    }
    (*codif)[niv] = '1';
    if(get_bin_codif(root->dr, sat_name, codif, mem, niv + 1) == 1){
        return 1;
    }
    return 0;
}

void task3(Node root, FILE *file_in, FILE *file_out){
    char sat_name[16];
    fscanf(file_in, "%s", sat_name);
    char *codif = malloc(7 * sizeof(char));
    get_bin_codif(root, sat_name, &codif, 7, 0);
    fprintf(file_out, "%s", codif);
    free(codif);
}

Node lowest_common_parent(Node root, char first_sat[], char second_sat[]){
    if(root == NULL){
        return NULL;
    }
    // Aplic DFS
    Node st = lowest_common_parent(root->st, first_sat, second_sat);
    Node dr = lowest_common_parent(root->dr, first_sat, second_sat);
    if(strcmp(root->s.name, first_sat) == 0 || strcmp(root->s.name, second_sat) == 0 || (st != NULL && dr != NULL)){
        return root;
    }
    else if(st != NULL){
        return st;
    }
    else{
        return dr;
    }
}

void task4(Node root, FILE* file_in, FILE* file_out){
    int nr_sat;
    fscanf(file_in, "%d", &nr_sat);
    fgetc(file_in);
    // Aloc dinamic toti satelitii
    char **sat = malloc(nr_sat * sizeof(char *));
    for(int i = 0; i < nr_sat; i++){
        sat[i] = malloc(16 * sizeof(char));
        fscanf(file_in, "%s", sat[i]);
    }
    Node node1 = lowest_common_parent(root, sat[0], sat[1]);
    for(int i = 2; i < nr_sat; i++){
        node1 = lowest_common_parent(root, node1->s.name, sat[i]);
    }
    fprintf(file_out, "%s", node1->s.name);
    for(int i = 0; i < nr_sat; i++){
        free(sat[i]);
    }
    free(sat);
}

typedef struct multinode{
    struct multinode **child;
    struct satellite s;
}*MultiNode;

typedef int (*CompF_task5)(MultiNode, MultiNode);

typedef struct heap_task5{
    int memory, nrElem;
    MultiNode *v;
    CompF_task5 comp;
}Heap_task5;

Heap_task5 *init_heap_task5(int mem, CompF_task5 comp){
    Heap_task5 *heap = malloc(sizeof(struct heap_task5));
    heap->comp = comp;
    heap->nrElem = 0;
    heap->memory = mem;
    heap->v = malloc(mem * sizeof(struct multinode));
    return heap;
}

int minHeap_task5(MultiNode n1, MultiNode n2){
    if(n1->s.freq != n2->s.freq)
        return n1->s.freq < n2->s.freq;
    if(strcmp(n1->s.name, n2->s.name) < 0)
        return 1;
    return 0;
}

void swap_nodes_task5(MultiNode* n1, MultiNode* n2){
    MultiNode tmp = *n1;
    *n1 = *n2;
    *n2 = tmp;
}

void insert_heap_task5(Heap_task5 *heap, MultiNode new_node){
    int nr = heap->nrElem;
    if(heap->nrElem >= heap->memory){
        heap->memory++;
        heap->v = realloc(heap->v, heap->memory * sizeof(Node));
    }
    heap->v[heap->nrElem] = new_node;
    // Pastrez proprietatea de minHeap cand adaug un element nou
    while(nr > 0 && heap->comp(heap->v[nr], heap->v[(nr - 1) / 2])){
        swap_nodes_task5(&heap->v[nr], &heap->v[(nr - 1) / 2]);
        nr--;
        nr /= 2;
    }
    heap->nrElem++;
}

MultiNode extract_and_sort_heap_task5(Heap_task5 *heap){
    int st, dr, i, okay;
    // Cu i iterez prin heap si cu okay verific daca am pus ultimul element
    // pe pozitia sa corespunzatoare minHeap-ului
    i = okay = 0;
    if(heap->nrElem == 0){
        return NULL;
    }
    // Retin primul element (cel pe care vreau sa il scot)
    MultiNode tmp = heap->v[0];
    heap->nrElem--;
    // Refac heap-ul ca sa isi pastreze proprietatea de minHeap
    heap->v[0] = heap->v[heap->nrElem];
    while(okay == 0){
        okay = 1;
        st = i * 2 + 1;
        dr = i * 2 + 2;
        if(st < heap->nrElem && heap->comp(heap->v[st], heap->v[i])){
            okay = 0;
            if(dr < heap->nrElem && heap->comp(heap->v[dr], heap->v[st])){
                swap_nodes_task5(&heap->v[i], &heap->v[dr]);
                i = dr;
            }
            else{
                swap_nodes_task5(&heap->v[i], &heap->v[st]);
                i = st;
            }
        }
        else if(dr < heap->nrElem && heap->comp(heap->v[dr], heap->v[i])){
            okay = 0;
            swap_nodes_task5(&heap->v[i], &heap->v[dr]);
            i = dr;
        }
    }
    return tmp;
}

MultiNode build_tree_task5(FILE *file_in){
    int nr_sat;
    fscanf(file_in, "%d", &nr_sat);
    Heap_task5 *heap = init_heap_task5(10, minHeap_task5);
    // Pun toti satelitii in heap
    for(int i = 0; i < nr_sat; i++){
        MultiNode new_node = malloc(sizeof(struct multinode));
        new_node->s.name = malloc(16 * sizeof(char));
        fscanf(file_in, "%d%s", &new_node->s.freq, new_node->s.name);
        new_node->child = malloc(3 * sizeof(MultiNode));
        new_node->child[0] = new_node->child[1] = new_node->child[2] = NULL;
        insert_heap_task5(heap, new_node);
    }
    while(heap->nrElem >= 2){
        // Construiesc legaturile arborelui binar (doar primii 2 copii)
        MultiNode new_node = malloc(sizeof(struct multinode)), p, q;
        new_node->child = malloc(3 * sizeof(MultiNode));
        p = extract_and_sort_heap_task5(heap);
        q = extract_and_sort_heap_task5(heap);
        // Adun cele 2 frecvente si concatenez numele lor
        new_node->s.freq = p->s.freq + q->s.freq;
        int mem = strlen(p->s.name) + strlen(q->s.name);
        mem++;
        new_node->s.name = malloc(mem * sizeof(char));
        strcpy(new_node->s.name, p->s.name);
        strcat(new_node->s.name, q->s.name);
        // Stabilesc nodul stang si nodul drept din arborele binar si
        // initializez urmatorul copil cu NULL pentru ca nu stiu de care nod se
        // poate lega arborele multicai
        if(minHeap_task5(p, q) == 1){
            new_node->child[0] = p;
            new_node->child[1] = q;
            new_node->child[2] = NULL;
        }
        else{
            new_node->child[0] = q;
            new_node->child[1] = p;
            new_node->child[2] = NULL;
        }
        insert_heap_task5(heap, new_node);
    }
    MultiNode root = extract_and_sort_heap_task5(heap);
    free(heap->v);
    free(heap);
    return root;
}

MultiNode search_multitree(MultiNode multiroot, char sat[]){
    if(multiroot == NULL){
        return NULL;
    }
    else if(strcmp(multiroot->s.name, sat) == 0){
        return multiroot;
    }
    for(int i = 0; multiroot->child[i] != NULL; i++){
        MultiNode new_node = search_multitree(multiroot->child[i], sat);
        if(new_node != NULL){
            return new_node;
        }
    }
    return NULL;
}

void build_multitree(MultiNode multiroot, FILE *file_in){
    int nr_arbori, i, mem, nr_noduri_parinte, j, nr_noduri_copil;
    fscanf(file_in, "%d", &nr_arbori);
    fgetc(file_in);
    for(int m = 0; m < nr_arbori; m++){
        i = 0;
        mem = 16;
        // Aloc dinamic numele satelitului de care se leaga arborele multicai
        char *name = malloc(mem * sizeof(char));
        MultiNode bin_tree_node, multitree_root_node, parent_node;
        fscanf(file_in, "%c", &name[i]);
        while(name[i] != '\n'){
            i++;
            fscanf(file_in, "%c", &name[i]);
            // Realoc numele
            if(i % 15 == 0){
                mem *= 2;
                name = realloc(name, mem * sizeof(char));
            }
        }
        name[i] = '\0';
        // Caut numele prin tot arborele
        bin_tree_node = search_multitree(multiroot, name);
        free(name);

        // Aloc dinamic radacina arborelui multicai si ii asociez datele din
        // fisier
        multitree_root_node = malloc(sizeof(struct multinode));
        fscanf(file_in, "%d", &multitree_root_node->s.freq);
        fgetc(file_in);
        char *name_multiroot = malloc(16 * sizeof(char));
        fgets(name_multiroot, 16, file_in);
        name_multiroot[strlen(name_multiroot) - 1] = '\0';
        multitree_root_node->s.name = malloc((strlen(name_multiroot) + 1) * sizeof(char));
        strcpy(multitree_root_node->s.name, name_multiroot);
        free(name_multiroot);


        // Leg nodul din arborele binar gasit anterior de radacina arborelui
        // multicai pentru fiecare arbore
        j = 0;
        while(bin_tree_node->child[j] != NULL){
            j++;
        }
        // Aloc memorie pentru copilul satelitului din arborele binar si
        // urmatorul copil il las NULL
        // Aloc memorie pentru eventualul copil al sau si pun urmat
        bin_tree_node->child[j] = multitree_root_node;
        bin_tree_node->child[j]->child = malloc(sizeof(MultiNode));
        bin_tree_node->child[j]->child[0] = NULL;
        bin_tree_node->child[j + 1] = NULL;


        fscanf(file_in, "%d", &nr_noduri_parinte);
        fgetc(file_in);
        for(int k = 0; k < nr_noduri_parinte; k++){
            // Aloc dinamic numele parintelui
            char *parent_name = malloc(16 * sizeof(char));
            fgets(parent_name, 16, file_in);
            parent_name[strlen(parent_name) - 1] = '\0';
            // Caut numele parintelui in tot arborele
            parent_node = search_multitree(multiroot, parent_name);
            free(parent_name);
            fscanf(file_in, "%d", &nr_noduri_copil);
            fgetc(file_in);
            int l = 0;
            // Realoc memorie suficienta cat pentru toti copiii parintelui
            parent_node->child = realloc(parent_node->child, (nr_noduri_copil + 1) * sizeof(MultiNode));
            for(l = 0; l < nr_noduri_copil; l++){
                // Aloc memorie pentru fiecare copil si asociez datele
                parent_node->child[l] = malloc(sizeof(struct multinode));
                fscanf(file_in, "%d", &parent_node->child[l]->s.freq);
                fgetc(file_in);
                char *child_name = malloc(16 * sizeof(char));
                fgets(child_name, 16, file_in);
                child_name[strlen(child_name) - 1] = '\0';
                parent_node->child[l]->s.name = malloc(strlen(child_name) + 1);
                strcpy(parent_node->child[l]->s.name, child_name);
                free(child_name);
                // Aloc memorie pentru fiecare copil al copilului parintelui
                // curent si il initializez pe primul ca fiind NULL
                parent_node->child[l]->child = malloc(sizeof(MultiNode));
                parent_node->child[l]->child[0] = NULL;
            }
            parent_node->child[l] = NULL;
        }
    }
}

MultiNode lowest_common_parent_multitree(MultiNode multiroot, char sat1[], char sat2[]){
    if(multiroot == NULL){
        return NULL;
    }
    else if(strcmp(multiroot->s.name, sat1) == 0 || strcmp(multiroot->s.name, sat2) == 0){
        return multiroot;
    }
    int found_both = 0;
    MultiNode tmp = NULL;
    // Aplic DFS
    for(int i = 0; multiroot->child[i] != NULL; i++){
        MultiNode search = lowest_common_parent_multitree(multiroot->child[i], sat1, sat2);
        if(search != NULL){
            found_both++;
            tmp = search;
        }
    }
    if(found_both == 2){
        return multiroot;
    }
    return tmp;
}

int find_dist(MultiNode multiroot, char sat[], int search, int *dist){
    if(multiroot == NULL){
        return 0;
    }
    else if(strcmp(multiroot->s.name, sat) == 0){
        // Actualizez distanta
        *dist = search + 1;
        return 1;
    }
    for(int i = 0; multiroot->child[i] != NULL; i++){
        if(find_dist(multiroot->child[i], sat, search + 1, dist) == 1){
            return 1;
        }
    }
    return 0;
}

void task5(MultiNode multiroot, FILE *file_in, FILE *file_out){
    int nr1 = 0, nr2 = 0;
    // Citesc cei doi sateliti pentru care trebuie sa determin distanta
    char *sat1, *sat2;
    sat1 = malloc(16);
    sat2 = malloc(16);
    int i = 0;
    fscanf(file_in, "%c", &sat1[i]);
    while(sat1[i] != ' '){
        i++;
        fscanf(file_in, "%c", &sat1[i]);
    }
    sat1[i] = '\0';
    fgets(sat2, 16, file_in);
    sat2[strlen(sat2) - 1] = '\0';
    // Caut satelitii corespunzatori celor 2 nume dar si cel mai mic parinte al lor
    MultiNode n1, n2, n3;
    n1 = search_multitree(multiroot, sat1);
    n2 = search_multitree(multiroot, sat2);
    n3 = lowest_common_parent_multitree(multiroot, n1->s.name, n2->s.name);
    if(find_dist(n3, n1->s.name, 0, &nr1) == 1 && find_dist(n3, n2->s.name, 0, &nr2) == 1){
        fprintf(file_out, "%d", nr1 + nr2 - 2);
    }
    free(sat1);
    free(sat2);
}

void free_multitree(MultiNode multiroot){
    if(multiroot == NULL){
        return;
    }
    for(int i = 0; multiroot->child[i] != NULL; i++){
        free_multitree(multiroot->child[i]);
    }
    free(multiroot->s.name);
    free(multiroot->child);
    free(multiroot);
}

int main(int argc, char *argv[]){
    FILE *file_in = fopen(argv[2], "r"), *file_out = fopen(argv[3], "w");
    char task[4];
    strcpy(task, argv[1]);
    if(strcmp(task, "-c1") == 0){
        Node root = build_tree(file_in);
        BFS(root, file_out);
    }
    else if(strcmp(task, "-c2") == 0){
        Node root = build_tree(file_in);
        int nr_codif;
        fscanf(file_in, "%d", &nr_codif);
        fgetc(file_in);
        for(int i = 1; i <= nr_codif; i++){
            task2(root, file_in, file_out);
            fprintf(file_out, "\n");
        }
        free_tree(root);
    }
    else if(strcmp(task, "-c3") == 0){
        Node root = build_tree(file_in);
        int nr_sat;
        fscanf(file_in, "%d", &nr_sat);
        fgetc(file_in);
        for(int i = 1; i <= nr_sat; i++){
            task3(root, file_in, file_out);
        }
        free_tree(root);
    }
    else if(strcmp(task, "-c4") == 0){
        Node root = build_tree(file_in);
        task4(root, file_in, file_out);
        free_tree(root);
    }
    else if(strcmp(task, "-c5") == 0){
        MultiNode multiroot = build_tree_task5(file_in);
        build_multitree(multiroot, file_in);
        task5(multiroot, file_in, file_out);
        free_multitree(multiroot);
    }
    fclose(file_in);
    fclose(file_out);
}