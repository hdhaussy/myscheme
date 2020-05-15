#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <error.h>
#include <errno.h>

typedef enum type type_t;
typedef struct object object_t;
typedef struct error error_t;
typedef struct boolean boolean_t;
typedef struct fixnum fixnum_t;

enum type { ERROR,BOOLEAN,FIXNUM };

struct object {
  type_t type;
};

struct boolean {
  object_t obj;
  int val;
};

struct fixnum {
  object_t obj;
  long val;
};

struct error {
  object_t obj;
  char msg[1];
};

object_t* obj_true;
object_t* obj_false;

error_t* make_error(size_t sz) {
  error_t* e = malloc(sizeof(error_t)+sz);
  e->obj.type = ERROR;
  return e;
}

boolean_t* make_boolean(int value) {
  boolean_t* b = malloc(sizeof(boolean_t));
  b->obj.type = BOOLEAN;
  b->val = value;
  return b;
}

fixnum_t* make_fixnum(long value) {
  fixnum_t* f = malloc(sizeof(fixnum_t));
  f->obj.type = FIXNUM;
  f->val = value;
  return f;
}

void init() {
  obj_true = (object_t*) make_boolean(1);
  obj_false = (object_t*) make_boolean(0);
}

void eatspace(FILE* in) {
  int ch;
  while((ch = fgetc(in)) != EOF && isspace(ch));
  ungetc(ch,in);
}

object_t* read_fixnum(FILE* in) {
  int ch = fgetc(in);
  int sign = 1;
  long num = 0;
  if(ch=='-') sign = -1;
  if(ch=='-' || ch=='+') ch = fgetc(in);
  while(isdigit(ch)) {
    ch -= '0';
    num = num * 10 + ch;
    ch = fgetc(in);
  }
  ungetc(ch,in);
  return (object_t*) make_fixnum(sign * num);
}

object_t* read(FILE* in) {
  object_t* obj;
  eatspace(in);
  int ch = fgetc(in);
  if(isdigit(ch) || ch=='-' || ch=='+') {
    ungetc(ch,in);
    obj = read_fixnum(in);
  }
  else if(ch=='#') {
    ch = fgetc(in);
    if(ch=='t') obj = obj_true; else obj = obj_false;
  }
  else {
    static const char* message = "unexpected character read: %c";
    error_t* err = make_error(strlen(message));
    sprintf(err->msg,message,(char) ch);
    obj = (object_t*) err;
  }
  return obj;
}

object_t* eval(object_t* obj) {
  return obj;
}

void print_error(FILE* out,error_t* err) {
    fprintf(out,"[ERROR %s]",err->msg);
}

void print_boolean(FILE* out,boolean_t* b) {
    if(b->val) fprintf(out,"%s","#t"); else fprintf(out,"%s","#f");
}

void print_fixnum(FILE* out,fixnum_t* f) {
    fprintf(out,"%ld",f->val);
}

void print(FILE* out,object_t* obj) {
  switch(obj->type) {
  case ERROR:
    print_error(out,(error_t*) obj);
    break;
  case BOOLEAN:
    print_boolean(out,(boolean_t*) obj);
    break;
  case FIXNUM:
    print_fixnum(out,(fixnum_t*) obj);
    break;
  }
}

int main(int argc,char** argv) {
  printf("sizeof(error_t)=%ld\n",sizeof(error_t));
  printf("sizeof(boolean_t)=%ld\n",sizeof(boolean_t));
  printf("sizeof(fixnum_t)=%ld\n",sizeof(fixnum_t));
  printf("sizeof(type_t)=%ld\n",sizeof(type_t));
  printf("sizeof(object_t)=%ld\n",sizeof(object_t));
  init();
  printf("Welcome to MyScheme\n");
  while(1) {
    printf("> ");
    print(stdout,eval(read(stdin)));
    printf("\n");
  }
  return 0;
}


