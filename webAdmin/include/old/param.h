#ifndef	PARAM_H
#define	PARAM_H
struct param {
	char *name;
	char *value;
	struct param *next;
};

int plist_init();
void plist_free();
void plist_add(struct param *item);
struct param *plist_item(char *name);
int plist_update_item(char *name, char *value);
int plist_update();
#endif
