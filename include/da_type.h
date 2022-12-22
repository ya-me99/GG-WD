#ifndef __DA_TYPE_H 
#define __DA_TYPE_H 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#define DA_CALLOC(a,b) calloc(a,b)
#define DA_PRINTF(in)  printf(in)

#define ARRAY_PRINT(arr)                                                                         \ 
   printf(" Space = %u \n Used = %u \n Load = %u  \n Unload = %u \n Size = %u \n Err = %u",      \
   arr.space,arr.used,arr.load,arr.unload,arr.size,arr.err);	                                 \

// Dynamic Array Generic Struct Dekleration

#define GEN_DA(name,unit)			\
        typedef struct				\
        {					\
         unit* array;				\
         uint64_t space;			\
         uint64_t used;				\
         uint64_t load;				\
         uint64_t unload;                       \
         uint64_t size;				\
         uint8_t err;				\
        }name;                                  \

 
// Generic Fnct Markos


#define ARRAY_BUILD_PROTO(type)  type type##_Build(uint64_t space , uint64_t load )

#define ARRAY_BUILD_DEF(type,tsize)            \
inline ARRAY_BUILD_PROTO(type)                 \
{                                              \
 type array;                                   \
                                               \
 array.array=DA_CALLOC(space,sizeof(tsize));   \
                                               \
 if(!array.array){                             \
  array.err=1;                                 \
  return array;                                \
 }                                             \
                                               \
 array.space=space;                            \
 array.used=0;                                 \
 array.size=sizeof(tsize );                    \
 array.load=load;                              \
 array.unload=0;                               \
 array.err=0;                                  \
 return array;                                 \
}                                              \


#define ARRAY_EXPAND_PROTO(type) uint8_t type##_Expand(type* arr)

#define ARRAY_EXPAND_DEF(type)                \
inline ARRAY_EXPAND_PROTO(type)               \
{									\
									\
 void* new=DA_CALLOC(arr->space+arr->load,arr->size);			\
 if(new==NULL) { arr->err=1 ; return 0; }				\
 memcpy(new,arr->array,arr->used*arr->size);				\
 free(arr->array);							\
 arr->space+=arr->load;  						\
 arr->array=new;							\
									\
 return 1;								\
}									\


#define ARRAY_RESIZE_PROTO(type) uint8_t type##_Resize(type* arr , uint64_t units) 

#define ARRAY_RESIZE_DEF(type)						\
inline ARRAY_RESIZE_PROTO(type)						\
{									\
									\
 void* new=DA_CALLOC(arr->space+units,arr->size);			\
 if(new==NULL) { arr->err=1 ; return 0; }				\
 memcpy(new,arr->array,arr->used*arr->size);				\
 free(arr->array);							\
 arr->space+=units;							\
 arr->array=new;							\
									\
 return 1;								\
}


#define ARRAY_ADD_PROTO(type,tsize) void type##_Add(tsize val, type* arr)

#define ARRAY_ADD_DEF(type,tsize)                  \
inline ARRAY_ADD_PROTO(type,tsize)                 \
{                                                  \
                                                   \
 if(arr->space==arr->used)                         \
 {                                                 \
  type##_Expand(arr);                              \
  if(arr->err==1){ return ;}                       \
 }                                                 \
                                                   \
 arr->array[arr->used]=val;                        \
 arr->used++;                                      \
}                                                  \


#define ARRAY_INSERT_PROTO(type,tsize) void type##_Insert(tsize val, uint64_t pos ,type* arr)

#define ARRAY_INSERT_DEF(type,tsize)                                  \
inline ARRAY_INSERT_PROTO(type,tsize)                                 \
{                                                                     \
 uint64_t s=arr->size;                                                \
 if(arr->space==arr->used)                                            \
 {                                                                    \
  type##_Expand(arr);                                                 \
  if(arr->err==1){ return ;}                                          \
 }                                                                    \
                                                                      \
 if(arr->used-1==pos &&  pos!=0)                                      \
 {                                                                    \
  arr->array[pos]=val;                                                \
  arr->used++;                                                        \
  return ;                                                            \
 }                                                                    \
                                                                      \
 memmove(&arr->array[pos+1],&arr->array[pos],(arr->used-pos)*s);      \
 arr->array[pos]=val;                                                 \
 arr->used++;                                                         \
}                                                                     \


#define ARRAY_APPEND_AT_PROTO(type,tsize)                               \
void type##_AppendAt(uint64_t pos, type* src, type* dest)               \

#define ARRAY_APPEND_AT_DEF(type,tsize)                                 \
inline ARRAY_APPEND_AT_PROTO(type,tsize)                                \
{								        \
 int64_t space_left=dest->space-dest->used;            	                \
                                                                        \
 if(src->used>space_left)				                \
 {								        \
  type##_Resize(dest,src->used);				        \
 }								        \
                                                                        \
 uint64_t dif=dest->used-pos;                                           \
                                                                        \
 memmove(&dest->array[pos+src->used],&dest->array[pos],dif*dest->size); \
 memmove(&dest->array[pos],src->array,src->used*src->size);             \
                                                                        \
 dest->used+=src->used;                                                 \
}                                                                       \


#define ARRAY_REMOVE_PROTO(type)  void type##_Remove(type* arr, uint64_t pos)

#define ARRAY_REMOVE_DEF(type)					\
inline ARRAY_REMOVE_PROTO(type)					\
{								\
 if(arr->used-1==pos)						\
 {								\
  arr->used--;							\
  return;							\
 }								\
								\
 int64_t dif=arr->used-pos;					\
								\
 memmove(&arr->array[pos],&arr->array[pos+1],dif*arr->size);	\
 arr->used--;							\
}                                                                  

#define ARRAY_SHRINK_PROTO(type)  void type##_Shrink(type* arr, uint64_t size)

#define ARRAY_SHRINK_DEF(type)					\
inline ARRAY_SHRINK_PROTO(type)					\
{								\
                                                                \
 void* new=DA_CALLOC(size,arr->size);                           \
                                                                \
 if(!new){                                                      \
  arr->err=1;                                                   \
  return ;                                                      \
 }                                                              \
                                                                \
 memcpy(new,arr->array,size*arr->size);                         \
 free(arr->array);                                              \
								\
 arr->space=size;                                               \
 arr->array=new;						\
 if(arr->used>size){ arr->used=size; return; }                  \
}                                                               \

#define ARRAY_FREE_PROTO(type)  void type##_Free(type arr)

#define ARRAY_FREE_DEF(type)                \
inline ARRAY_FREE_PROTO(type)               \
{                                           \
  free(arr.array);                          \
}                                           \

#define GENERIC_ARRAY_PROTO(type,tsize) \
GEN_DA(type,tsize);                     \
ARRAY_BUILD_PROTO(type);                \
ARRAY_RESIZE_PROTO(type);               \
ARRAY_EXPAND_PROTO(type);               \
ARRAY_ADD_PROTO(type,tsize);            \
ARRAY_INSERT_PROTO(type,tsize);         \
ARRAY_APPEND_AT_PROTO(type,tsize);      \
ARRAY_REMOVE_PROTO(type);               \
ARRAY_FREE_PROTO(type);                 \
ARRAY_SHRINK_PROTO(type);               \

#define GENERIC_ARRAY_DEF(type,tsize)   \
ARRAY_BUILD_DEF(type,tsize);            \
ARRAY_ADD_DEF(type,tsize);              \
ARRAY_RESIZE_DEF(type);                 \
ARRAY_EXPAND_DEF(type);                 \
ARRAY_INSERT_DEF(type,tsize);           \
ARRAY_APPEND_AT_DEF(type,tsize);        \
ARRAY_REMOVE_DEF(type);                 \
ARRAY_FREE_DEF(type);                   \
ARRAY_SHRINK_DEF(type);                 \
							       					     
// Generic Makros						

GENERIC_ARRAY_PROTO(ArrayU8,uint8_t);
GENERIC_ARRAY_PROTO(ArrayU16,uint16_t);
GENERIC_ARRAY_PROTO(ArrayU32,uint32_t);
GENERIC_ARRAY_PROTO(ArrayU64,uint64_t);
GENERIC_ARRAY_PROTO(ArrayS8,int8_t);
GENERIC_ARRAY_PROTO(ArrayS16,int16_t);
GENERIC_ARRAY_PROTO(ArrayS32,int32_t);
GENERIC_ARRAY_PROTO(ArrayS64,int64_t);
GENERIC_ARRAY_PROTO(ArrayF32,float);
GENERIC_ARRAY_PROTO(ArrayF64,double);

#endif

#ifdef DA_IMPL_BASIC
GENERIC_ARRAY_DEF(ArrayU8,uint8_t);
GENERIC_ARRAY_DEF(ArrayU16,uint16_t);
GENERIC_ARRAY_DEF(ArrayU32,uint32_t);
GENERIC_ARRAY_DEF(ArrayU64,uint64_t);
GENERIC_ARRAY_DEF(ArrayS8,int8_t);
GENERIC_ARRAY_DEF(ArrayS16,int16_t);
GENERIC_ARRAY_DEF(ArrayS32,int32_t);
GENERIC_ARRAY_DEF(ArrayS64,int64_t);
GENERIC_ARRAY_DEF(ArrayF32,float);
GENERIC_ARRAY_DEF(ArrayF64,double);
#endif

/*
// ---------------------------------------------- Pthread TS --------------------------------------------


#ifdef _DA_PTHREAD_TS

 #include <pthread.h>

 typedef enum
 {
  DA_ADD,
  DA_REMOVE,
  DA_SHRINK,
  DA_EXPAND,
  DA_RESIZE,
  DA_INSERT,
  DA_PRINT,
  DA_FREE,
  DA_APPEND_AT,
 }Array_FnType;
 
 #define GEN_DA_TS(arr_t)			\
        typedef struct				\
        {					\
         arr_t array;				\
         pthread_mutex_t mut;                   \
         Array_FnType fn_type;                  \ 
         union{                                 \
         struct { uint8_t a; }resize;           \
         struct { uint8_t a; }expand;           \
         struct { uint8_t a; }add;              \
         struct { uint8_t a; }insert;           \
         struct { uint8_t a; }append_at;        \
         struct { uint8_t a; }remove;           \
         struct { uint8_t a; }free;             \
         struct { uint8_t a; }shrink;           \
         struct { uint8_t a; }print;            \
         }args;                                 \
        }arr_t##_Ts;                            \


#define ARRAY_BUILD_PACKAGE_PROTO(arr_t)  arr_t##_Ts arr_t##_BuildPackage(uint64_t size , uint64_t load)

#define ARRAY_BUILD_PACKAGE_DEF(arr_t)				\
inline ARRAY_BUILD_PACKAGE_PROTO(arr_t)				\
{								\
 arr_t##_Ts  pack;                             \
 pthread_mutex_t mut;                          \
                                               \
 arr_t arr=arr_t##_Build(size,load);           \
                                               \
 pthread_mutex_init(&mut,NULL);                \
                                               \
 pack.array=arr;                               \
 pack.mut=mut;                                 \
 return pack;                                  \
}                                              \



#define GENERIC_ARRAY_TS_PROTO(arr_t) \
GEN_DA_TS(arr_t)                           \
ARRAY_BUILD_PACKAGE_PROTO(arr_t);          \


#define GENERIC_ARRAY_TS_DEF(arr_t)   \
ARRAY_BUILD_PACKAGE_DEF(arr_t);            \

#ifndef __DA_TS_IMPL
#define __DA_TS_IMPL
// BASE TYPES DEF
#endif


#endif 
#endif
*/
