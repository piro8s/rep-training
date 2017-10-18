#include <string.h>
/**
 * Delete '\n' from string.
 * @param char *	str		The target string.
 */
void lntrim(char *);

/**
 * Count how many character in the string
 * @param char *	str		The original string.
 * @param char	c	The target characters.
 * @return int	num		The number of characters.
 */
int search(char *, const char *);

/**
 * Split string by separater string
 * @param char *	str		The original string.
 * @param const char *	delim	The separater string.
 * @param char *[]	outlist		The output array of strings.
 * @return int	cnt		The split count.
 */
int split(char *, const char *, char *[]);