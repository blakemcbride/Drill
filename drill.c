

/*  Typing drill by Blake McBride (blake@edge.net)
    Placed in the public domain in 1998
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#ifdef _MSC_VER
#include <conio.h>
#else
extern char _getch(void);
#endif


static	char	*Levels[256];

static	int	NLevels;

static	char	Letters[1024];   /*  must be big!!  */

static	int	CurrentLevel;
static	int	NLetters;
static	int	MinWordLen = 1;
static	int	MaxWordLen = 7;
static	int	MinWords = 10;
static	int	MaxWords = 15;
static	long	NumErrors;
static	long	NumChars;
static	int	UseWeights;
static	time_t	BeginningTime;

static	void	fill_letters(void);
static	void	drill(void);
static	char	*make_word(char *word);
static	char	*make_line(char *line);
static	int	get_line(char *line);
static	void	read_levels(char *file);
static	void	usage(char *pgm);


/*
  Ideas of things to add:
  --------------------------
  line length and word length range
  selecting weighting on command line
*/

int main(int argc, char *argv[])
{
	int	n, i;

	if (argc > 1  &&  !strcmp(argv[1], "-w")) {
		UseWeights = 1;
		argc--;
		argv++;
	}
	if (argc != 3) {
		usage(argv[0]);
		exit(1);
	}
	read_levels(argv[1]);
	CurrentLevel = atoi(argv[2]);
	if (CurrentLevel < 1  ||  CurrentLevel > NLevels) {
		printf("Invalid level; must be between 1 and %d\n", NLevels);
		exit(1);
	}
	BeginningTime = time(NULL);
	srand((unsigned int)BeginningTime);
	fill_letters();
	drill();
	return 0;
}

static	void	fill_letters(void)
{
	int	i, weight;
	int	unique = 1;  /*  unique letters used  */
	char	*p;
	
	for (NLetters=i=0 ; i < CurrentLevel ; i++) {
		if (UseWeights) {
			switch (CurrentLevel - i) {
			case 1:
				weight = (unique * 35) / 100;
				break;
			case 2:
				weight = (unique * 10) / 100;
				break;
			case 3:
				weight = (unique * 10) / 100;
				break;
			default:
				weight = 1;
				break;
			}
			if (!weight)
				weight = 1;
		} else
			weight = 1;
		while (weight--)
			for (p = Levels[i] ; *p ; ) {
				if (!weight)
					unique++;
				Letters[NLetters++] = *p++;
			}
	}
	Letters[NLetters] = '\0';
	printf("\n\nLetters at level %d = %s\n\n\n", CurrentLevel, Letters);
}						

static	void	drill(void)
{
	char	line[256];
	int	r = 1;
	time_t	end;

	while (r) {
		make_line(line);
		printf("%s\n", line);
		r = get_line(line);
	}
	if (NumChars) {
		time_t	tot = time(NULL) - BeginningTime;
		printf("\n\nNumber of characters typed:  %ld\n", NumChars);
		printf("Number of errors:  %ld\n", NumErrors);
		printf("Error rate:  %ld%%\n", (NumErrors * 100L) / NumChars);
		printf("Accuracy:  %ld%%\n\n", 100L - (NumErrors * 100L) / NumChars);
		printf("Total time: %d:%02d\n", (int)(tot / 60L), (int)(tot % 60L));
		printf("Speed: %d characters per second\n", (int) (NumChars / (tot ? tot : 1L)));
	}
}


static	char	*make_word(char *word)
{
	int	i, len = rand() % (MaxWordLen - MinWordLen) + MinWordLen;

	for (i=0 ; i < len ; )
		word[i++] = Letters[rand() % NLetters];
	word[i] = '\0';
	return word;
}

static	char	*make_line(char *line)
{
	int	i, words = rand() % (MaxWords - MinWords) + MinWords;
	char	*p = line;

	for (i=0 ; i < words ; i++) {
		if (i)
			*p++ = ' ';
		make_word(p);
		for ( ; *p ; p++);
	}
	*p = '\0';
	return line;
}

static	int	get_line(char *line)
{
	char	c, expect = *line;

	while (expect) {
		c = _getch();
		if (c < ' '  &&  c != '\r'  &&  c != '\n') {
			if (c == 1)		/*  ^A - give a new line  */
				return 1;
			if (c == 21) {		/*  ^U - next level  */
				CurrentLevel++;
				if (CurrentLevel > NLevels)
					CurrentLevel = NLevels;
				fill_letters();
				return 1;
			}
			if (c == 4) {		/*  ^D - previous level  */
				CurrentLevel--;
				if (!CurrentLevel)
					CurrentLevel = 1;
				fill_letters();
				return 1;
			}
			if (c == 17)		/*  ^Q - Quit  */
				return 0;
		}
		if (c == '\r')
			c = '\n';
		if (toupper(c) != expect) {
			putchar('\a');
			NumErrors++;
		} else {
			putchar(c);
			if (c == '\n')
				puts("\n\n");
			else
				line++;
			if (expect == '\n')
				expect = '\0';
			else if (*line)
				expect = *line;
			else
				expect = '\n';
		}
		if (c != '\n'  &&  c >= ' ')
			NumChars++;
	}
	return 1;
}

static	void	read_levels(char *file)
{
	FILE	*fp = fopen(file, "r");
	char	buf[256], buf2[256], *p;
	int	i, i2;
	char	c;

	if (!fp) {
		printf("Can't open %s\n", file);
		exit(1);
	}
	NLevels = 0;
	while (fgets(buf, sizeof buf, fp)) {
		for (i=i2=0 ; (c=buf[i])  &&  c != ' '  &&  c != '\t'  &&  c != '\r'  &&  c != '\n' ; i++)
			if (isprint(c))
				buf2[i2++] = c;
		if (i2) {
			buf2[i2] = '\0';
			p = (char *) malloc(strlen(buf2)+1);
			strcpy(p, buf2);
			Levels[NLevels++] = p;
		}
	}
	fclose(fp);
}

static	void	usage(char *pgm)
{
	fprintf(stderr, "\nUsage:  %s  [-w]  <char-table>  <level>", pgm);
	fprintf(stderr, "%s\n", "");
	fprintf(stderr, "%s\n", "<char-table> is one of the character tables such as qwerty or dvorak");
	fprintf(stderr, "%s\n", "<level> is the starting level (line in the character table, starts at 1)");
	fprintf(stderr, "%s\n", "-w means to put more weight on the more recent levels");
	fprintf(stderr, "%s\n", "");
	fprintf(stderr, "%s\n", "Once running the following control keys are in effect:");
	fprintf(stderr, "%s\n", "\n\t^A\tgive a new line to type");
	fprintf(stderr, "%s\n", "\t^U\tgoto the next level");
	fprintf(stderr, "%s\n", "\t^D\tgoto the previous level");
	fprintf(stderr, "%s\n", "\t^Q\tquit\n");
}

