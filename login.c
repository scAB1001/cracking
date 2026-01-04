/* REFERENCE
 * https://www.youtube.com/watch?v=FkEh4B5CKfI
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main()
{
    char *password = "andreas123";
    char *input = calloc(256, sizeof(char));

    printf("  > Login: ");
    scanf("%s", input);

    strcmp(input, password) ? printf("ACCESS DENIED\n") : printf("ACCESS GRANTED\n");

    return 0;
}