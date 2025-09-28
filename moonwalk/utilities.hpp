#ifndef UTILITIES_H
#define UTILITIES_H
#define log(format, ...) printf("[DEBUG] " format "\n", ##__VA_ARGS__)
#define wait() { BOOLEAN _t = getchar(); }
#endif
