#include <stdio.h>
#include <stdlib.h>

int main () {
    char *locationVar;
    locationVar = getenv("location");
    printf("Location: %s\n", locationVar);
    return 0;
}
