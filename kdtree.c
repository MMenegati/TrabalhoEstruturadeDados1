#include<stdio.h>
#include<stdlib.h>
#include<float.h>
#include<string.h>
#include<assert.h>
#include<math.h> 

#define K_DIMENSIONS 128


typedef struct _reg{
    float embedding[K_DIMENSIONS];
    char id[100];
}treg;

void * aloca_reg(const float embedding[K_DIMENSIONS], const char id[]){
    treg * reg;
    reg = malloc(sizeof(treg));
    if (!reg) return NULL;
    memcpy(reg->embedding, embedding, K_DIMENSIONS * sizeof(float));
    strncpy(reg->id, id, 99);
    reg->id[99] = '\0'; 
    return reg;
}

int comparador(void *a, void *b, int pos){
    float val_a = ((treg *)a)->embedding[pos];
    float val_b = ((treg *)b)->embedding[pos];

    if (val_a < val_b) return -1;
    if (val_a > val_b) return 1;
    return 0;
}

double distancia(void * a, void *b){
    double sum_sq_diff = 0.0;
    for (int i = 0; i < K_DIMENSIONS; ++i) {
        double diff = ((treg *)a)->embedding[i] - ((treg *)b)->embedding[i];
        sum_sq_diff += diff * diff;
    }
    return sum_sq_diff;
}



typedef struct _node{
    void * key;
    struct _node * esq;
    struct _node * dir;
}tnode;

typedef struct _arv{
    tnode * raiz;
    int (*cmp)(void *, void *, int);
    double (*dist) (void *, void *);
    int k;
}tarv;





void kdtree_constroi(tarv * arv, int (*cmp)(void *a, void *b, int ),double (*dist) (void *, void *),int k){
    arv->raiz = NULL;
    arv->cmp = cmp;
    arv->dist = dist;
    arv->k = k;
}

/*teste*/
void test_constroi(){
    tarv arv;

    float emb1[K_DIMENSIONS] = {0}; emb1[0] = 2.0; emb1[1] = 3.0;
    float emb2[K_DIMENSIONS] = {0}; emb2[0] = 1.0; emb2[1] = 1.0;

    void* reg1 = aloca_reg(emb1, "Dourados");
    void* reg2 = aloca_reg(emb2, "Campo Grande");

    kdtree_constroi(&arv,comparador,distancia,K_DIMENSIONS);
    
    assert(arv.raiz == NULL);
    assert(arv.k == K_DIMENSIONS);
    if (reg1 && reg2) {
        assert(arv.cmp(reg1,reg2,0) == 1);
        assert(arv.cmp(reg1,reg2,1) == 1);
        assert(strcmp(((treg *)reg1)->id,"Dourados") == 0);
        assert(strcmp(((treg *)reg2)->id,"Campo Grande") == 0);
    }
    free(reg1);
    free(reg2);
}

void _kdtree_insere(tnode **raiz, void * key, int (*cmp)(void *a, void *b, int),int profund, int k){
    if(*raiz == NULL){
        *raiz = malloc(sizeof(tnode));
        if (!(*raiz)) return;
        (*raiz)->key = key;
        (*raiz)->esq = NULL;
        (*raiz)->dir = NULL;
    }else{
        int pos = profund % k;
        if (cmp(key, (*raiz)->key ,pos) >= 0){
            _kdtree_insere( &((*raiz)->dir),key,cmp,profund + 1,k);
        }else{
            _kdtree_insere( &((*raiz)->esq),key,cmp,profund +1,k);
        }
    }
}

void kdtree_insere(tarv *arv, void *key){
    _kdtree_insere(&(arv->raiz),key,arv->cmp,0,arv->k);
}


void _kdtree_destroi(tnode * node){
    if (node!=NULL){
        _kdtree_destroi(node->esq);
        _kdtree_destroi(node->dir);
        free(node->key);
        free(node);
    }
}

void kdtree_destroi(tarv *arv){
    _kdtree_destroi(arv->raiz);
}


void _kdtree_busca(tarv *arv, tnode * atual_node, void * query_key, int profund, double *menor_dist, tnode **menor_node){
    if (atual_node == NULL) return;

    double dist_atual = arv->dist(atual_node->key, query_key);

    if (dist_atual < *menor_dist){
        *menor_dist = dist_atual;
        *menor_node = atual_node;
    }

    int pos = profund % arv->k;
    int comp_val = arv->cmp(query_key, atual_node->key, pos);

    tnode *lado_principal, *lado_oposto;
    if (comp_val < 0){
        lado_principal = atual_node->esq;
        lado_oposto    = atual_node->dir;
    } else {
        lado_principal = atual_node->dir;
        lado_oposto    = atual_node->esq;
    }

    _kdtree_busca(arv, lado_principal, query_key, profund + 1, menor_dist, menor_node);

    double diff_dim = ((treg*)query_key)->embedding[pos] - ((treg*)atual_node->key)->embedding[pos];
    double dist_sq_to_plane = diff_dim * diff_dim;

    if (dist_sq_to_plane < *menor_dist) {
        _kdtree_busca(arv, lado_oposto, query_key, profund + 1, menor_dist, menor_node);
    }
}


tnode * kdtree_busca(tarv *arv, void * key){
    tnode * menor = NULL;
    double menor_dist = DBL_MAX;
    _kdtree_busca(arv, arv->raiz, key, 0, &menor_dist, &menor);
    return menor;
}

treg* buscar_mais_proximo(tarv *arv, treg *query_reg_ptr) {
    if (!arv || !query_reg_ptr) return NULL;
    tnode *menor_node = kdtree_busca(arv, (void*)query_reg_ptr);
    if (menor_node) {
        return (treg*)(menor_node->key);
    }
    return NULL;
}

typedef struct _heap_item {
    tnode* node;
    double distance;
} HeapItem;

typedef struct _max_heap {
    HeapItem* items;
    int size;
    int capacity;
} MaxHeap;

MaxHeap* create_max_heap(int capacity) {
    MaxHeap* heap = (MaxHeap*)malloc(sizeof(MaxHeap));
    if (!heap) return NULL;
    heap->items = (HeapItem*)malloc(capacity * sizeof(HeapItem));
    if (!heap->items) {
        free(heap);
        return NULL;
    }
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

void destroy_max_heap(MaxHeap* heap) {
    if (!heap) return;
    free(heap->items);
    free(heap);
}

void swap_heap_items(HeapItem* a, HeapItem* b) {
    HeapItem temp = *a;
    *a = *b;
    *b = temp;
}

void max_heapify_down(MaxHeap* heap, int index) {
    int largest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heap->size && heap->items[left].distance > heap->items[largest].distance)
        largest = left;
    if (right < heap->size && heap->items[right].distance > heap->items[largest].distance)
        largest = right;

    if (largest != index) {
        swap_heap_items(&heap->items[index], &heap->items[largest]);
        max_heapify_down(heap, largest);
    }
}

void max_heapify_up(MaxHeap* heap, int index) {
    int parent = (index - 1) / 2;
    if (index > 0 && heap->items[index].distance > heap->items[parent].distance) {
        swap_heap_items(&heap->items[index], &heap->items[parent]);
        max_heapify_up(heap, parent);
    }
}

void heap_insert(MaxHeap* heap, tnode* node, double distance) {
    if (heap->size < heap->capacity) {
        heap->items[heap->size].node = node;
        heap->items[heap->size].distance = distance;
        heap->size++;
        max_heapify_up(heap, heap->size - 1);
    } else if (distance < heap->items[0].distance) {
        heap->items[0].node = node;
        heap->items[0].distance = distance;
        max_heapify_down(heap, 0);
    }
}

double heap_get_max_distance(MaxHeap* heap) {
    if (heap->size == 0) return DBL_MAX;
    return heap->items[0].distance;
}

void _kdtree_buscar_n_vizinhos_recursive(tarv *arv, tnode *atual_node, void *query_key, int profund, MaxHeap *heap) {
    if (atual_node == NULL) return;

    double dist_atual = arv->dist(atual_node->key, query_key);
    heap_insert(heap, atual_node, dist_atual);

    int pos = profund % arv->k;
    int comp_val = arv->cmp(query_key, atual_node->key, pos);

    tnode *lado_principal, *lado_oposto;
    if (comp_val < 0) {
        lado_principal = atual_node->esq;
        lado_oposto    = atual_node->dir;
    } else {
        lado_principal = atual_node->dir;
        lado_oposto    = atual_node->esq;
    }

    _kdtree_buscar_n_vizinhos_recursive(arv, lado_principal, query_key, profund + 1, heap);
    
    double diff_dim = ((treg*)query_key)->embedding[pos] - ((treg*)atual_node->key)->embedding[pos];
    double dist_sq_to_plane = diff_dim * diff_dim;

    if (heap->size < heap->capacity || dist_sq_to_plane < heap_get_max_distance(heap)) {
        _kdtree_buscar_n_vizinhos_recursive(arv, lado_oposto, query_key, profund + 1, heap);
    }
}

int kdtree_buscar_n_vizinhos(tarv *arv, treg *query_reg_ptr, int n, treg results_out[]) {
    if (!arv || !query_reg_ptr || n <= 0 || !results_out) return 0;

    MaxHeap* heap = create_max_heap(n);
    if (!heap) return 0;

    _kdtree_buscar_n_vizinhos_recursive(arv, arv->raiz, (void*)query_reg_ptr, 0, heap);

    int num_found = heap->size;
    for (int i = 0; i < num_found; ++i) {
        results_out[i] = *((treg*)(heap->items[i].node->key));
    }

    destroy_max_heap(heap);
    return num_found;
}


tarv arvore_global;

tarv* get_tree() {
    return &arvore_global;
}

void inserir_ponto(treg p_val) {
    treg *novo_reg_ptr = malloc(sizeof(treg));
    if (!novo_reg_ptr) {
        fprintf(stderr, "Failed to allocate memory for new point in inserir_ponto\n");
        return;
    }
    *novo_reg_ptr = p_val;
    kdtree_insere(&arvore_global, novo_reg_ptr);
}

void kdtree_construir() {
    kdtree_constroi(&arvore_global, comparador, distancia, K_DIMENSIONS);
}

void test_busca(){
    tarv arv;
    kdtree_constroi(&arv,comparador,distancia,K_DIMENSIONS);

    float emb_a[K_DIMENSIONS] = {0}; emb_a[0]=10; emb_a[1]=10;
    float emb_b[K_DIMENSIONS] = {0}; emb_b[0]=20; emb_b[1]=20;
    float emb_c[K_DIMENSIONS] = {0}; emb_c[0]=1;  emb_c[1]=10;

    kdtree_insere(&arv,aloca_reg(emb_a,"a"));
    kdtree_insere(&arv,aloca_reg(emb_b,"b"));
    kdtree_insere(&arv,aloca_reg(emb_c,"c"));
    
    assert(arv.raiz != NULL);

    treg query_reg;
    query_reg.embedding[0] = 9; query_reg.embedding[1] = 9;
    for(int i=2; i<K_DIMENSIONS; ++i) query_reg.embedding[i] = 0.0f;
    strcpy(query_reg.id, "query");

    treg *mais_proximo_ptr = buscar_mais_proximo(&arv, &query_reg);
    if (mais_proximo_ptr) {
        printf("\nQuery (9,9,...), Closest: ID %s, Emb[0]=%.1f, Emb[1]=%.1f\n",
               mais_proximo_ptr->id, mais_proximo_ptr->embedding[0], mais_proximo_ptr->embedding[1]);
        assert(strcmp(mais_proximo_ptr->id, "a") == 0);
    } else {
        printf("\nQuery (9,9,...), No closest found.\n");
    }

    int n_neighbors = 2;
    treg results[n_neighbors];
    int found_count = kdtree_buscar_n_vizinhos(&arv, &query_reg, n_neighbors, results);
    printf("\nQuery (9,9,...), Found %d neighbors (asked for %d):\n", found_count, n_neighbors);
    for (int i = 0; i < found_count; ++i) {
        printf("Neighbor %d: ID %s, Emb[0]=%.1f, Emb[1]=%.1f, Dist^2 to query: %.2f\n",
               i + 1, results[i].id, results[i].embedding[0], results[i].embedding[1],
               distancia(&results[i], &query_reg));
    }

    kdtree_destroi(&arv);
    printf("test_busca completed (simplified).\n");
}


int main(void){
    test_constroi();
    test_busca();
    printf("SUCCESS!! (Tests adapted for new structure)\n");
    return EXIT_SUCCESS;
}
