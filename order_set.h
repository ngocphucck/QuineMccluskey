#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
	char** set;
	int length;
	int maxlength;
} order_set;

int ischild(char* str1, char* str2)
{
	for (int i = 0; i < strlen(str1); ++i)
	{
		if (str1[i] != str2[i] && str2[i] != '_')
			return 0;
	}	
	return 1;
}

void Add(order_set* container, char* _value)
{
	char* value = (char*) malloc(strlen(_value) * sizeof(char));
	strcpy(value, _value);
	if (container->set == NULL)
	{
		
		container->set = (char**) malloc(1 * sizeof(char*));
		container->set[0] = NULL;
		container->length = 0;
		container->maxlength = 1;
	}
	
	if (container->maxlength == container->length)
	{
		container->maxlength *= 2;
		container->set = (char**) realloc(container->set, (container->maxlength) * sizeof(char*));
		for (int i = container->length; i < container->maxlength; ++i)
			container->set[i] = NULL;
	}
	
	int position = 0;
	for (; position < container->length; ++position)
	{
		if (container->set[position] != NULL && ischild(value, container->set[position])) 
			return;
		if (container->set[position] != NULL && strcmp(value, container->set[position]) > 0)
			break;
	}
	
	int j = container->length - 1;
	for (;j >= position; --j)
	{
		container->set[j + 1] = container->set[j];
	}
	container->set[position] = value;	
	++(container->length);
}

void Delete(order_set* container, char* value)
{
	for (int i = 0; i < container->length; ++i)
	{
		if (strcmp(value, container->set[i]) == 0)
		{
			for (; i < container->length; ++i)
			{
				if (i == container->length - 1)
				{
					--(container->length);
					container->set[i] = NULL;
					return;
				}
				container->set[i] = container->set[i + 1];
			}
			--(container->length);
			return;		
		}
	}	
}

void Display(order_set* container)
{
	for (int i = 0; i < container->length; ++i)
	{
		if (container->set[i] != NULL)
			printf("%s\n", container->set[i]);
	}
}

void Reallocate(order_set* container)
{
	for (int i = 0; i < container->length; ++i)
		free(container->set[i]);
	free(container->set);
}

void Copy(order_set* new_set, order_set* old_set)
{
	new_set->length = old_set->length;
	new_set->maxlength = old_set->maxlength;
	new_set->set = (char**) malloc(new_set->maxlength * sizeof(char*));
	for (int i = 0; i < new_set->length; ++i)
	{
		new_set->set[i] = (char*) malloc(strlen(old_set->set[i]));
		strcpy(new_set->set[i], old_set->set[i]);
	}
}

order_set* Union(order_set* s1, order_set* s2)
{
	order_set* new_set = (order_set*) malloc(sizeof(order_set));
	new_set->set = NULL;
	if (s1->set != NULL)
		Copy(new_set, s1);
	for (int i = 0; i < s2->length; ++i)
		Add(new_set, s2->set[i]);
	return new_set;
}

order_set* Subset(order_set* s1, order_set* s2)
{
	order_set* new_set = (order_set*) malloc (sizeof(order_set));
	new_set->set = NULL;
	for (int i = 0; i < s1->length; ++i)
		for (int j = 0; j < s2->length; ++j)
		{
			if (ischild(s2->set[j], s1->set[i]))
				Add(new_set, s2->set[j]);
		}
	return new_set;
}


order_set* Subtraction(order_set* s1, order_set* s2)
{
	order_set* new_set = (order_set*) malloc(sizeof(order_set));
	new_set->set = NULL;
	if (s1->set != NULL)
		Copy(new_set, s1);
	for (int i = 0; i < s1->length; ++i)
		for (int j = 0; j < s2->length; ++j)
		{
			if (strcmp(s2->set[j], s1->set[i]) == 0)
			{
				Delete(new_set, s1->set[i]);
			}
		}
	return new_set;
}
