#ifndef FILE_READING
#define FILE_READING

size_t count_elements_in_file(char file_name[]);
size_t read_file(char text[], size_t amount_of_symbols, const char* file_name);
int count_strings(char text[], size_t amount_of_symbols);

#endif
