#ifndef FUNCS_H
#define FUNCS_H

extern char** getArray(char* txt, char* delimiter, int count);
extern char* replaceWord(const char* s, const char* oldW, const char* newW);
extern char* convertTRChar(char* str);
extern BOOL fileExist(STRPTR filePath);

#endif