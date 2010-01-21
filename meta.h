#ifndef _AOL_META_H_
#define  _AOL_META_H_


struct Meta {
    char *key
    char *values;
};

struct MetaSet {
    struct *Meta;
    int count;
};

void addMeta(MetaSet &, Meta);


#endif
