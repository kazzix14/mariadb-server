/* Make a db.h that will be link-time compatible with Sleepycat's Berkeley DB. */

#include <db.h>
#include <stdio.h>
#include <stdlib.h>


DB db_dummy;

#define DECL_LIMIT 100
#define FIELD_LIMIT 100
struct fieldinfo {
    char decl[DECL_LIMIT];
    unsigned int off;
    unsigned int size;
} db_fields[FIELD_LIMIT];
int db_field_counter=0;


int compare_fields (const void *av, const void *bv) {
    const struct fieldinfo *a = av;
    const struct fieldinfo *b = bv;
    if (a->off < b->off) return -1;
    if (a->off > b->off) return 1;
    return 0;
}				      

#define DB_STRUCT_SETUP(name, fstring) ({ snprintf(db_fields[db_field_counter].decl, DECL_LIMIT, fstring, #name); \
	    db_fields[db_field_counter].off = offsetof(DB, name); \
	    db_fields[db_field_counter].size = sizeof(db_dummy.name); \
	    db_field_counter++; })

void sample_offsets (void) {
    int i;
    unsigned int current_offset=0;
    int dummy_counter=0;
    int did_toku_internal=0;
    /* Do these in alphabetical order. */
    DB_STRUCT_SETUP(app_private,    "void *%s");
    DB_STRUCT_SETUP(close,          "int (*%s) (DB*, u_int32_t)");
    DB_STRUCT_SETUP(cursor,         "int (*%s) (DB *, DB_TXN *, DBC **, u_int32_t)");
    DB_STRUCT_SETUP(del,            "int (*%s) (DB *, DB_TXN *, DBT *, u_int32_t)");
    DB_STRUCT_SETUP(get,            "int (*%s) (DB *, DB_TXN *, DBT *, DBT *, u_int32_t)");
    DB_STRUCT_SETUP(key_range,      "int (*%s) (DB *, DB_TXN *, DBT *, DB_KEY_RANGE *, u_int32_t)");
    DB_STRUCT_SETUP(open,           "int (*%s) (DB *, DB_TXN *, const char *, const char *, DBTYPE, u_int32_t, int)");
    DB_STRUCT_SETUP(put,            "int (*%s) (DB *, DB_TXN *, DBT *, DBT *, u_int32_t)");
    DB_STRUCT_SETUP(remove,         "int (*%s) (DB *, const char *, const char *, u_int32_t)");
    DB_STRUCT_SETUP(rename,         "int (*%s) (DB *, const char *, const char *, const char *, u_int32_t)");
    DB_STRUCT_SETUP(set_bt_compare, "int (*%s) (DB *, int (*)(DB *, const DBT *, const DBT *))");
    DB_STRUCT_SETUP(set_flags,      "int (*%s) (DB *, u_int32_t)");
    DB_STRUCT_SETUP(stat,           "int (*%s) (DB *, void *, u_int32_t)");
    qsort(db_fields, db_field_counter, sizeof(db_fields[0]), compare_fields);
    printf("struct __toku_db {\n");
    for (i=0; i<db_field_counter; i++) {
	unsigned int this_offset = db_fields[i].off;
	if (!did_toku_internal && this_offset+sizeof(void*)>current_offset) {
	    printf("  struct __tokudb_internal *i;\n");
	    current_offset+=sizeof(void*);
	    did_toku_internal=1;
	}
	if (this_offset>current_offset) {
	    printf("  char dummy%d[%d];\n", dummy_counter++, this_offset-current_offset);
	    current_offset=this_offset;
	}
	if (this_offset<current_offset) {
	    printf("Whoops\n");
	}
	printf("  %s; /* offset=%d size=%d */\n", db_fields[i].decl, db_fields[i].off, db_fields[i].size);
	current_offset+=db_fields[i].size;
    }
    printf("}\n");
}
int main (int argc __attribute__((__unused__)), char *argv[] __attribute__((__unused__))) {
    sample_offsets();
    return 0;
}
