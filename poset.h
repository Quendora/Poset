#ifndef POSET_H
    #define POSET_H
    namespace jnp1 {
        extern "C" unsigned long poset_new(void);
        extern "C" void poset_delete(unsigned long id);
        extern "C" size_t poset_size(unsigned long id);
        extern "C" bool poset_insert(unsigned long id, char const *value);
        extern "C" bool poset_remove(unsigned long id, char const *value);
        extern "C" bool poset_add(unsigned long id, char const *value1, char const *value2);
        extern "C" bool poset_del(unsigned long id, char const *value1, char const *value2);
        extern "C" bool poset_test(unsigned long id, char const *value1, char const *value2);
        extern "C" void poset_clear(unsigned long id);
    }

#endif