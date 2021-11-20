#include "preprocessor.h"

int main(int argc, char *argv[])
{
	FILE *f; /*original file*/
	FILE *f1; /*file without comments*/
	FILE *f2; /*file with included*/
	FILE *f3; /*included file*/
	char *fileNoComments;
	char *fileNoIncludes;
	char *included; /*the name of the 'included' file(s) will be stored here*/
	char line[501];
	char c;
	char k;
	int flag;
	int i; /*index*/
	int a; /*index*/
	int b; /*index*/
	fileNoComments = (char*)(malloc(sizeof(argv[1])+sizeof(char)));
	fileNoIncludes = (char*)(malloc(sizeof(argv[1])+sizeof(char)));
	if ((!fileNoComments)||(!fileNoIncludes))
	{
		fprintf(stderr,"Error - Problem in allocating memory\n");
		exit(1);
	}
	if (argc == 1)
	{
		fprintf(stderr,"Error - No file was given\n");
		exit(1);
	}
	if (argc > 2)
	{
		fprintf(stderr,"Error - More than one file was given\n");
		exit(1);
	}
	if ((argv[1][strlen(argv[1])-2] != '.')||(argv[1][strlen(argv[1])-1] != 'c'))
	{
		fprintf(stderr,"Error - the file's name doesn't end with \".c\"\n");
		exit(1);
	}
	strcpy(fileNoComments, argv[1]);
	fileNoComments[strlen(argv[1])]='1';
	strcpy(fileNoIncludes, argv[1]);
	fileNoIncludes[strlen(argv[1])]='2';
	f = fopen(argv[1],"r");
	flag = 0;
	if (!(f))
	{
		fprintf(stderr,"Error - No file found\n");
		exit(1);
	}
	/*creating the 1st file: removing comments*/
	f1 = fopen(fileNoComments,"w"); /*creating the 1st file (deleting comments)*/
	c = fgetc(f);
	while ((c != EOF) && (flag == 0)) /*copying the file while ignoring the comments*/
	{
		if (c == '/')
		{
			char c2 = fgetc(f);
			if (c2 == EOF)
			{
				flag = 1;
			}
			while ((c2 == '*') && (flag == 0))
			{
				char c3 = fgetc(f);
				char c4;
				while ((c3!='*') && (flag == 0))
				{
					if (c3 == EOF)
					{
						flag = 1;
						continue;
					}
					c3 = fgetc(f);
				}
				c4 = fgetc(f);
				if (c4 == '/') /*reached end of comment*/
				{
					c = fgetc(f);
					continue;
				}				
			}
			if (c2 != '*')
			{
				fputc(((int)c),f1);
				if (c2 != EOF)
				{
					fputc(((int)c2),f1);
					c = fgetc(f);
					continue;
				}
			}
		}
		else
		{
			fputc(((int)c),f1);
			c = fgetc(f);
		}
	}
	fclose(f1);
	/*creating the 2nd file: replacing includes with the included file*/
	f1 = fopen(fileNoComments,"r"); /*using fileNoComments as a source*/
	if (!(f1))
	{
		fprintf(stderr,"Error - No file found\n");
		exit(1);
	}
	f2 = fopen(fileNoIncludes,"w"); /*creating the 2nd file (deleting includes)*/
	i = 0;
	line[i] = fgetc(f1);
	while (line[i]!='\n'&&(line[i]!=EOF))
	{
		i++;
		line[i] = fgetc(f1);
	}
	while (line[0]!=EOF)
	{
		if ((strstr(line,"#include"))==NULL) /*if there is not an include*/
		{
			fseek(f1, -(i+1), SEEK_CUR);
			k = fgetc(f1);
			while ((k!=EOF)&&(k!='\n'))
			{
				fputc(((int)k),f2);
				k = fgetc(f1);
			}

		}
		else /*if there is an include*/
		{
			i=0;
			while (line[i] != '"')
			{
				if (line[i] == '\0') /*reached end of line before seeing : " */
				{
					fprintf(stderr,"\nError - There is an include command without a file name, or the file's name doesn't start with: \"\n");
					exit(1);
				}
				i++;
			}
			i--;
			if (isspace(line[i])==0)
			{
				fprintf(stderr,"\nError - There is no space after the include command\n");
				exit(1);
			}
			i+=2; 
			a = 0;
			while (line[i] != '"')
			{
				a++;
				i++;
			}
			i-=a;
			included = (char*)(malloc(a * sizeof(char)));
			if (!included)
			{
				fprintf(stderr,"\nError - Problem in allocating memory\n");
				exit(1);
			}
			b = 0;
			while (b<a)
			{
				included[b] = line[i];
				i++; b++;
			}
			f3 = fopen(included,"r");
			if (!(f3))
			{
				fprintf(stderr,"Error - The included file is not found\n");
				exit(1);
			}
			k = fgetc(f3);
			while (k != EOF)
			{
				fputc(((int)k),f2);
				k = fgetc(f3);
			}
			fclose(f3);
			free(included);
		}
		/*scanning the next line:*/
		i = 0;
		line[i] = fgetc(f1);
		while (line[i]!='\n'&&(line[i]!=EOF))
		{
			i++;
			line[i] = fgetc(f1);
		}
		fputc(((int)'\n'),f2);
	}
	fclose(f);
	fclose(f1);
	fclose(f2);
	free(fileNoComments);
	free(fileNoIncludes);
	return 0;
}
