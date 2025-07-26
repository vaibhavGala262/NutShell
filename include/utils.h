#ifndef UTILS_H
#define UTILS_H

// Prompt generation functions
char* get_simple_prompt(void);
char* get_fancy_prompt(void);       // shorten dir struct
char* get_full_prompt(void);      // with username
char* get_colored_prompt(void); // for coloured shell 

#endif
