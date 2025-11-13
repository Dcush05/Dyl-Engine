//UTILITY LIBRARY	
//
//
//
//IMPLEMENTATION:
// ---DYNAMIC ARRAY--
// 	Usage: Allocating a large dataset on the heap, has dynamic sizing.
// 	Example:
// 		storing an array of vec2 on the heap with the size of 5. The vec2's are our elements
//
// 		vec2 entity_pos = {0,0};
// 		dyl_array vec2_array = {0};
// 		dyl_array_init(&vec2_array, sizeof(vec2), 5); //initializes the array with capacity being 5
// 		dyl_array_ push(&vec2_array, &entity_pos); //pushes the element
// 		vec2* array_to_print = (vec2*)vec2_array.data;
// 		printf("Array at 0 index is %d" array_to_print(0)->x);
// 		dyl_array_free(&vec2_array); //frees the element
//
//
//
//
//
//
//
//
#ifndef DYL_LIB_H
#define DYL_LIB_H
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <immintrin.h>

#define DEFAULT_ARRAY_SIZE 64


#define XOR_SWAP(a,b) \
do{ \
 	(a) = b^a; \
	(b) = b^a; \
	(a) = b^a; \
	}while(0) \

#define ASSERT(_e, ...) if (!(_e)) { \
    fprintf(stderr, "Assertion failed: (%s), file %s, line %d: ", #_e, __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__); \
    exit(1); \
}
#define ASSERT_STR_COMP(str1, str2) ASSERT(strcmp(str1, str2) == 0)
#define ASSERT_INT_COMP(x,y) ASSERT(x == y)
#define PRINT_DEBUG(...) fprintf(stderr, __VA_ARGS__, "file: %s, line: %d\n", __FILE__, __LINE__);


#define getrandom( min, max ) (SHORT)((rand() % (int)(((max) + 1) - \
                               (min))) + (min))

//MY INT TYPES

typedef uint8_t u8_d;
typedef uint16_t u16_d;
typedef uint32_t u32_d;
typedef uint64_t u64_d;
#define COLOR_BLUE 0,0,255,255
#define COLOR_RED 255,0,0,255
#define COLOR_GREEN 0,255,0,255
#define COLOR_WHITE 255,255,255,255
//COLOR
typedef struct {int r,g,b,a; }Color;
//vectors
typedef struct {int x, y;} vec2i;
typedef struct {float x, y;} vec2f;
typedef struct {int x,y,z;} vec3i;
typedef struct {float x, y, z;} vec3f;
typedef union {
	
	struct
	{

		float m00, m01, m02, m03;
		float m10, m11, m12, m13;
		float m20, m21, m22, m23;
		float m30, m31, m32, m33;
	};
	float flat_array[16];
	float m[4][4];
	#ifdef __SSE__
		__m128 rows[4];
	#endif
}d_mat4f;




static inline vec2i vec2i_add(vec2i vec1, vec2i vec2) {return (vec2i){vec1.x + vec2.x, vec1.y + vec2.y};}
static inline vec2i vec2i_subtract(vec2i vec1, vec2i vec2) {return (vec2i){vec1.x - vec2.x, vec1.y - vec2.y};}
static inline vec2i vec2i_division(vec2i vec1, vec2i vec2){return (vec2i){vec1.x / vec1.x, vec1.y/vec2.y};}
static inline vec2f vec2f_add(vec2f vec1, vec2f vec2) {return (vec2f){vec1.x + vec1.x, vec1.y + vec2.y};}	
static inline vec2f vec2f_subtract(vec2f vec1, vec2f vec2) {return (vec2f){vec1.x - vec2.x, vec1.y - vec2.y};}


#define vec2_add(a,b) _Generic((a), vec2i: vec2i_add, vec2f: vec2f_add)((a),(b))
#define vec2_subtract(a,b) _Generic((a), vec2i: vec2i_subtract, vec2f: vec2f_subtract)((a),(b))
#define vec2_multiplication(a,b) _Generic((a), vec2i: vec2i_multiplication, vec2f: vec2f_multiplication)((a),(b))
#define vec2_division(a,b) _Generic((a), vec2i: vec2i_division, vec2f: vec2f_division)((a),(b))







//DYNAMIC ARRAYS
typedef struct
{
	void* data;
	size_t capacity;
	size_t count;
	size_t element_size;
}dyl_array;




static inline void dyl_array_init(dyl_array* array, size_t element_size, size_t capacity)
{
	if(!array || element_size == 0 || capacity == 0)
	{
		fprintf(stderr, "Unable to allocate array\n");
		return;
	}
	array->element_size = element_size;
	array->capacity = capacity;
	array->count = 0;
	array->data = malloc(array->element_size * array->capacity);
	if(!array->data)
	{

		fprintf(stderr, "Failed to allocate array\n");
		memset(array, 0, sizeof(dyl_array));
		return;
	}


}
static inline void dyl_array_push(dyl_array* array, void* data)
{

	if(!array || !data)
	{
		fprintf(stderr, "Unable to push element to array");
		return;
	}

	if(array->count == array->capacity)
	{
		size_t new_capacity = array->capacity ? array->capacity * 2 : 1;
		void* new_data = realloc(array->data, new_capacity * array->element_size);
        	if (!new_data) {
            		fprintf(stderr, "Failed to allocate memory when growing array\n");
            		return;
        	}
        	array->data = new_data;
       		array->capacity = new_capacity;

	}

	void* dest = (char*)array->data + array->count * array->element_size;
	memcpy(dest, data, array->element_size);
	array->count++;
	
}
static inline dyl_array dyl_array_slice( dyl_array* array,unsigned int start, unsigned int end)
{
	
	if(!array || start > end)
	{
		return (dyl_array){0};
	}
	dyl_array new_array;
	size_t capacity = end - start + 1;
	dyl_array_init(&new_array, array->element_size,capacity);
	if(new_array.data == NULL)
	{
		return (dyl_array){0};
	}
	void* src = (char*)array->data + start * array->element_size;
	memcpy(new_array.data, src, capacity * array->element_size);
	return new_array;
}



//ALGORITHMS



//SEARCHING


//TODO: CREATE COMPAROTORS FOR YOUR SEARCH FUNCTION POINTER ARG
static inline int dyl_array_search(dyl_array* array, void* target, int(*cmp)(void*, void*) )
{
	assert(array || target);


	for(size_t i = 0; i < array->count; ++i)
	{
		
		void* element = (char*)array->data + i * array->element_size;
		
		if(cmp(target, element) == 0)
		{
			return i;
		}
	}

	return -1;	 //unable to find what we are searching for
}



static inline void dyl_array_free(dyl_array* array)
{
	if(!array)
		return;

	free(array->data);
	array->data = NULL;
	array->count = 0;
	array->element_size = 0;
	array->capacity = 0;
}

//SORTING



//STRINGS
typedef struct
{
	char* string;
	uint64_t length;
}dyl_string;


//write_string("hi");
static inline dyl_string* write_string(const char* string)
{
	if(string == NULL)
		return NULL;

	uint64_t len = strlen(string);

	dyl_string* new_string = (dyl_string*)malloc(sizeof(dyl_string));
	if(new_string == NULL)
		return NULL;

	new_string->string = (char*)malloc(len+1); //+1 for null terminator thingy
	if(new_string->string == NULL)
	{
		free(new_string);
		return NULL;
	}




	strcpy(new_string->string, string);
	new_string->length = len;
	return new_string;	
}

static inline uint64_t get_length(dyl_string* string)
{
	return string->length;
}


static inline void string_free(dyl_string* string)
{
	if(string == NULL)
		return;

	free(string->string);
	string->string = NULL;
	string->length = 0;
}



//LINKED LISTS


typedef struct node
{
	void* data;
	struct node* next;
}node;


typedef struct
{
	size_t capacity;
	size_t count;
	size_t element_size;
	node* head;

}dyl_linked_list;


static inline void dyl_linked_list_init(dyl_linked_list* list)
{
	if(!list)
		return; 

//	list->element_size = element_size;
	list->head = NULL;

}

static inline void list_append(dyl_linked_list* list, void* data)
{
	if(!list || !data)
		return;
	
	

	if(list->head == NULL)
	{
		node* new_node = (node*)malloc(sizeof(node));
		if(!new_node)
		{
			fprintf(stderr, "Unable to allocate node\n");
			return;
		}
		new_node->data = data;
		new_node->next = NULL;
		list->head = new_node;
		return;
	}
	// Traverse to the end of the list
	node* current = list->head;
	while(current->next != NULL)  // Changed from == to !=
	{
		current = current->next;  // Move to next node
	}
	
	// Create and attach new node at the end
	node* new_node = (node*)malloc(sizeof(node));
	if(!new_node)
	{
		fprintf(stderr, "Unable to allocate node\n");
		return;
	}
	new_node->data = data;
	new_node->next = NULL;
	current->next = new_node;
}

static inline void list_print(dyl_linked_list* list)
{
}

//HASH_MAPS


typedef struct
{
	char* value;
	char* key;
}dyl_item;


typedef struct
{
	size_t count;
	size_t size;
	dyl_item** items;
}dyl_hash_table;



static inline dyl_item* new_item(const char* key, const char* value)
{
	dyl_item* item = (dyl_item*)malloc(sizeof(dyl_item));
	item->key = strdup(key);
	item->value = strdup(value);
	return item;
}


static inline dyl_hash_table* dyl_table_init()
{
	dyl_hash_table* table = (dyl_hash_table*)malloc(sizeof(dyl_hash_table));

	table->size = 53;
	table->count = 0;
	table->items = (dyl_item**)calloc(table->size, sizeof(dyl_item*));
	return table;

}


static inline void dyl_del_item(dyl_item* item)
{
	free(item->key);
	free(item->value);
	free(item);
}

static inline void dyl_del_ht(dyl_hash_table* ht)
{
	for(size_t i = 0; i < ht->size; ++i)
	{
		if(ht->items[i] != NULL)
		{
			dyl_del_item(ht->items[i]);
		}
	}
	free(ht->items);
	free(ht);
}

static inline int hash(const char* string, int a, int mod)
{
	long hash = 0;
	size_t length = strlen(string);
	for(int i = 0; i < length; ++i)
	{
		hash += (long)pow(a, length - (i+1)) * string[i];
		hash %= mod;
	}
	return (int)hash;

}
#define HT_PRIME_1 151
#define HT_PRIME_2 163
static inline int dyl_ht_get_hash(const char* s, const int num_buckets, const int attempt)
{
	const int hash_a = hash(s, HT_PRIME_1, num_buckets);
	const int hash_b = hash(s, HT_PRIME_2, num_buckets);
	return(hash_a + (attempt * (hash_b + 1))) % num_buckets;
}

static inline void dyl_ht_insert(dyl_hash_table* ht, const char* key, const char* value)
{
	dyl_item* item = new_item(key, value);
	int index = dyl_ht_get_hash(item->key, ht->size, 0);
	dyl_item* cur_item = ht->items[index];
	int i  = 1;
	while(cur_item != NULL)
	{
		index = dyl_ht_get_hash(item->key, ht->size, i);
		cur_item = ht->items[index];
		i++;

	}
	ht->items[index] = item;
	ht->count++;
}

static inline char* dyl_ht_get_value(dyl_hash_table* ht, const char* key)
{
	int index = dyl_ht_get_hash(key, ht->size, 0);
	dyl_item* item = ht->items[index];

	int i = 1;
	while(item != NULL)
	{
		if(strcmp(item->key, key) == 0)
		{
			return item->value;
		}
		index = dyl_ht_get_hash(key, ht->size, i);
		item = ht->items[index];
		++i;
	}

	return NULL;

}
static inline void dyl_ht_print(dyl_hash_table* ht, const char* key)
{
	int index = dyl_ht_get_hash(key, ht->size, 0);
	dyl_item* item = ht->items[index];
	int i = 1;
	while(item != NULL)
	{
		if(strcmp(item->key, key) == 0)
		{
			printf("Key - %s, Value - %s\n", item->key, item->value);
		}
		index = dyl_ht_get_hash(key, ht->size, i);
		item = ht->items[index];
		++i;
	}
}





//ARENA
typedef struct
{
	uint8_t *data;
	size_t length;
	size_t offset;

}Arena;
static inline Arena arena_alloc(size_t size)
{

	Arena arena = {0};
	//uint8_t *memory = (uint8_t* )malloc(size);
	
	arena.data = (uint8_t* )malloc(size);
	if(!arena.data)
	{
		
		fprintf(stderr, "Arena Memory allocation failed\n");
		return arena;
	}
	arena.length = size;
	arena.offset = 0;
	return arena;
}
static inline void *arena_push(Arena* arena, size_t size)
{
	
	if(!arena || !arena->data)
		return NULL;
	if(arena->offset+size <= arena->length)
	{
		void *p = &arena->data[arena->offset];
		arena->offset += size;
		memset(p, 0, size);
		return p;

	}
	fprintf(stderr,"Ran out of memory\n");
	return NULL;
}
static inline void arena_reset(Arena* arena)
{
	if(!arena)
		return;
	arena->offset = 0;
}
static inline void arena_track(Arena* arena)
{
	if(arena && arena->data)
	{
		printf("Size of arena allocation: %zu\n", arena->length);
	}
}
static inline void arena_track_leaks(Arena* arena)
{
	if(arena && arena->data)
	{
		printf("Leaked %zu bytes at %p\n", arena->length, arena->data);
	}
	printf("No leaks!\n");
}
static inline void arena_free(Arena* arena)
{
	if(arena && arena->data)
	{
		free(arena->data);
		arena->data = NULL;
		arena->length = 0;
		arena->offset= 0;
		
	}
}






#endif
