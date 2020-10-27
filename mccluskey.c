#include "order_set.h"

void elimination();
char* adj(char*, char*);
char* convert_to_binary(int, int);
int findmax(int*, int);
void loop(order_set*, order_set*, int, int, char**, int*, char***, order_set*, int*);
char* bintostr(char*);
char* formular(order_set*);

int main()
{	
	elimination();
	return 0;
}

char* adj(char* str1, char* str2)
{
	int flag = 0;
	int pos;
	char* str = NULL;
	for (int i = 0; i < strlen(str1); ++i)
	{
		if (str1[i] != str2[i])
		{
			++flag;
			pos = i;
		}
	}
	if (flag == 1)
	{
		str = (char*) malloc(strlen(str1));
		strcpy(str, str1);
		str[pos] = '_';
	}
	return str;
}

char* convert_to_binary(int number_of_var, int num)
{
	char* str = (char *) malloc(number_of_var * sizeof(char) + 1);
	for (int i = 0; i < number_of_var; ++i)
		str[i] = '0';
	str[number_of_var] = '\0';
	int iter = num;
	int count = 0;
	while (iter != 0)
	{
		char bit = iter % 2;
		if (bit == 0)
			str[number_of_var - 1 - count] = '0';
		else 
			str[number_of_var - 1 - count] = '1';
		++count;
		iter /= 2;
	}
	return str;
}

void elimination()
{
	/* Input the logic function*/
	int* minterm; 
	int* dontcare;
	int number_of_var;
	int number_of_minterm;
	int number_of_dontcare;
	printf("Enter number of variables: ");
	scanf("%d", &number_of_var);
	printf("Enter number of minterms: ");
	scanf("%d", &number_of_minterm);
	minterm = (int*) malloc(number_of_minterm * sizeof(int));
	if (number_of_minterm != 0)
		printf("Input the minterm: ");
	for (int i = 0; i < number_of_minterm; ++i)
		scanf("%d", &minterm[i]);
	printf("Enter number of dont cares: ");
	scanf("%d", &number_of_dontcare);
	dontcare = (int*) malloc(number_of_dontcare * sizeof(int));
	if (number_of_dontcare != 0)
		printf("Input the dontcare: ");
	for (int i = 0; i < number_of_dontcare; ++i)
		scanf("%d", &dontcare[i]);
	
	/*Dont care term*/
	order_set* DC = (order_set*) malloc(sizeof(order_set));
	DC->set = NULL;
	for (int i = 0; i < number_of_dontcare; ++i)
		Add(DC, convert_to_binary(number_of_var, dontcare[i]));

	/*Minterm */
	order_set* MT = (order_set*) malloc(sizeof(order_set));
	MT->set = NULL;
	for (int i = 0; i < number_of_minterm; ++i)
		Add(MT, convert_to_binary(number_of_var, minterm[i]));

	/*Build the table include the group in each layer*/
	int column = 1;
	int* size;
	order_set* Table = (order_set*) malloc(sizeof(order_set));
	size = (int*) malloc(sizeof(int) * column);
	Table[0].set = NULL;
	for (int i = 0; i < number_of_minterm; ++i)
		Add(&Table[0], convert_to_binary(number_of_var, minterm[i]));
	for (int i = 0; i < number_of_dontcare; ++i)
		Add(&Table[0], convert_to_binary(number_of_var, dontcare[i]));
	size[0] = number_of_minterm + number_of_dontcare;
	while (1)
	{
		order_set Group;
		Group.set = NULL;	
		for (int i = 0; i < size[column - 1]; ++i)
			for (int j = i + 1; j < size[column - 1]; ++j)
			{
				char* str = adj(Table[column - 1].set[i], Table[column - 1].set[j]);
				if (str != NULL)
					Add(&Group, str);
			}
		if (Group.set == NULL)
			break;
		column++;
		size = (int*) realloc(size, sizeof(int) * column);
		size[column - 1] = Group.length;
		Table = (order_set*) realloc(Table, sizeof(order_set) * column);
		Copy(&Table[column - 1], &Group);
		Reallocate(&Group);	
	}
	
	/*Build prime implicants */
	order_set* PI = (order_set*) malloc (sizeof(order_set));
	PI->set = NULL;
	for (int i = column - 1; i >= 0; --i)
		PI = Union(PI, &Table[i]);

	/*Count number of minterm*/
	char** minterm_element = Union(MT, DC)->set;
	int* minterm_count = (int*) malloc (sizeof(int) * (number_of_dontcare + number_of_minterm));
	char*** PI_element = (char***) malloc(sizeof(char**) * (number_of_dontcare + number_of_minterm));
	for (int i = 0; i < number_of_dontcare + number_of_minterm; ++i)
	{
		minterm_count[i] = 0;
		for (int j = 0; j < PI->length; ++j)
			if (ischild(minterm_element[i], PI->set[j]))
			{
				minterm_count[i]++;
				if (minterm_count[i] == 1)
				{
					PI_element[i] = (char**) malloc(sizeof(char*) * minterm_count[i]);
					PI_element[i][0] = PI->set[j];
				}
				else
				{
					PI_element[i] = (char**) realloc(PI_element[i], sizeof(char*) * minterm_count[i]);
					PI_element[i][minterm_count[i] - 1] = PI->set[j];
				}
			}
	}
	
	/* Dismiss the doncare element*/
	for (int i = 0; i < number_of_dontcare + number_of_minterm; ++i)
		for (int j = 0; j < number_of_dontcare; ++j)
			if (strcmp(minterm_element[i], DC->set[j]) == 0)
				minterm_count[i] = 0;

	/*Build the essential prime implicants*/
	order_set* EPI = (order_set*) malloc(sizeof(order_set));
	EPI->set = NULL;
	for (int i = 0; i < (number_of_dontcare + number_of_minterm); ++i)
	{
		if (minterm_count[i] == 1)
		{
			Add(EPI, PI_element[i][0]);
			minterm_count[i] = 0;
		}
	}
	printf("Essense Prime implicants: \n");
	Display(EPI);
	printf("\n");

	/*Non essential prime implicants*/
	for (int i = 0; i < (number_of_dontcare + number_of_minterm); ++i)
	{
		for (int j = 0; j < EPI->length; ++j)
		{
			if (ischild(minterm_element[i], EPI->set[j]))
			{
				minterm_count[i] = 0;
				break;
			}
		}
	}

	order_set* NEPI = Subtraction(PI, EPI);
	printf("Nonessense Prime implicants: \n");
	Display(NEPI);


	int* nonessence_count = (int*) malloc(sizeof(int) * NEPI->length);
	for (int i = 0; i < NEPI->length; ++i)
		nonessence_count[i] = 0;
	for (int i = 0; i < number_of_dontcare + number_of_minterm; ++i)
		if (minterm_count[i] != 0)
			for (int j = 0; j < minterm_count[i]; ++j)
			{
				for (int k = 0; k < NEPI->length; ++k)
				{
					if (strcmp(PI_element[i][j], NEPI->set[k]) == 0)
						nonessence_count[k]++;
				}
			}

	order_set* F = (order_set*) malloc (sizeof(order_set));
	F->set = NULL;
	if (EPI->set != NULL)
		Copy(F, EPI);

	order_set* nonessence =(order_set*) malloc (sizeof(order_set));
	nonessence->set = NULL;
	order_set* traversal = (order_set*) malloc(sizeof(order_set));
	traversal->set = NULL;
	int* count = (int* ) malloc (sizeof(int) * NEPI->length);
	for (int i = 0; i < NEPI->length; ++i)
		count[i] = 0;
	loop(nonessence, traversal, number_of_dontcare + number_of_minterm, number_of_dontcare + number_of_minterm, minterm_element, minterm_count, PI_element, NEPI, count);

	F = Union(F, nonessence);
	char* str = formular(F);
	printf("*********************************\n");
	if (number_of_minterm == 0)
		printf("F = 0\n");
	else
		printf("F = %s\n", formular(F));
	printf("*********************************\n");
}

int findmax(int * array, int length)
{
	int position = 0;
	int max = 0;
	for (int i = 0; i < length; ++i)
		if (max < array[i])
		{
			max = array[i];
			position = i;
		}	
	if (max > 0)
		return position;
	else 
		return -1;
}

/*Patric algorithm*/
void loop(order_set* update, order_set* traversal, int remain, int length, char** minterm_element, int* minterm_count, char*** PI_element, order_set* NEPI, int* count)
{
	while (minterm_count[length - remain] == 0 &&  remain > 0)
	{
		remain--;
	}

	if (remain == 0) 
	{
		if (traversal->length < update->length || update->set == NULL)
		{
			if (update->set != NULL)
				Reallocate(update);
			Copy(update, traversal);
		}
		return;
	}

	for (int i = 0; i < minterm_count[length - remain]; ++i)
	{
		Add(traversal, PI_element[length - remain][i]);
		for (int j = 0; j < NEPI->length; ++j)
			if (strcmp(NEPI->set[j], PI_element[length - remain][i]) == 0)
			{
				count[j]++;
				break;
			}
		loop(update, traversal, remain - 1, length, minterm_element, minterm_count, PI_element, NEPI, count);
		for (int j = 0; j < NEPI->length; ++j)
			if (strcmp(NEPI->set[j], PI_element[length - remain][i]) == 0)
			{
				if (count[j] == 1)	
				{
					Delete(traversal, PI_element[length - remain][i]);
				}
				count[j]--;
			}
	}

}

char* bintostr(char* bin)
{
	char a = 97;
	char* str = (char*) malloc (sizeof(char));
	for (int i = 0; i < strlen(bin); ++i)
	{
		if (bin[i] == '0') 
		{
			strcat(str, &a);
			strcat(str, "'");
		}
		else if (bin[i] == '1')
			strcat(str, &a);
		else;
		a++;
	}
	return str;
}

char* formular(order_set* result)
{
	char* str = (char*) malloc (sizeof(char));
	for (int i = 0; i < result->length; ++i)
	{
		strcat(str, bintostr(result->set[i]));
		if (i != result->length - 1)
			strcat(str, " + ");
	}
	if (strlen(str) == 0)
		return "1";
	return str;
}
