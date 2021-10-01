#include "minishell.h"

void	word_li_free(t_list *li)
{
	free(((t_word_desc *)li->content)->word);
	free(li->content);
	free(li);
}

void	word_list_free(t_list **tokens)
{
	t_list *token;
	t_list *tmp;
	t_word_desc *word;

	token = *tokens;
	while (token)
	{
		word = token->content;
		tmp = token->next;
		free(word->word);
		free(token->content);
		free(token);
		token = tmp;
	}

}

char *get_flag_name(int flag)
{
	char *result;

	result = ft_calloc(sizeof(char), 1);
	if (flag & T_NOEXP)
		result = ft_strjoin2(result, "T_NOEXP ");
	if (flag & T_DQUOTE)
		result = ft_strjoin2(result, "T_DQUOTE ");
	if (flag & T_NOSPC)
		result = ft_strjoin2(result, "T_NOSPC ");

	return (result);
}

void	word_list_print(t_list **head)
{
	t_list	*item;
	t_word_desc *word;
	char *str_flag;

	if (!head || !(*head))
	{
		fprintf(stderr, "NULL word list\n");
		return;
	}
	item = *head;
	while (item)
	{
		word = item->content;
		str_flag = get_flag_name(word->flags);
		fprintf(stderr, "[%s],\t\t[%s]\n", word->word, str_flag);
		free(str_flag);
		item = item->next;
	}
	fprintf(stderr, "<end>\n");
	/* printf("\n"); */
}

int		in_set(char *chr, const char *special)
{
	char *special_copy;

	special_copy = (char *)special;
	while (*special_copy)
	{
		if (*chr == *special_copy)
			return (1);
		else
			special_copy++;
	}
	return (0);
}

void	handle_dollar(char *str, int *i, t_list *tokens)
{
	char *delim;
	int len;
	int flags;

	delim = " \t\n$><|";
	if (str[*i] != '$')
		return;
	(*i)++;
	len = 0;
	flags = 0;
	while (str[*i + len] && in_set(&str[*i + len], delim))
	{
		len++;
	}
	if (str[*i + len + 1] != ' ')
		flags |= T_NOSPC;
	ft_lstadd_back(&tokens, ft_lstnew(wdesc_new(ft_substr(str, *i, len), T_NOEXP | T_VAR)));

}

void	handle_dquote(char *str, int *i, t_list *tokens)
{
	int len;

	if (str[*i] != 0x22)
		return;
	if ((*i) && str[*i - 1] == '\\')
		return;
	(*i)++;
	len = ft_strchr(&str[*i], 0x22) - &str[*i];
	if (len < 0)
		//TODO handle error
		exit(-3);
	ft_lstadd_back(&tokens, ft_lstnew(wdesc_new(ft_substr(str, *i, len), T_DQUOTE)));
	*i += len + 1;
}

void	handle_quote(char *str, int *i, t_list *tokens)
{
	int len;

	if (str[*i] != 0x27)
		return;
	if ((*i) && str[*i - 1] == '\\')
		return;
	(*i)++;
	len = ft_strchr(&str[*i], 0x27) - &str[*i];
	if (len < 0)
		//TODO handle error
		exit(-3);

	ft_lstadd_back(&tokens, ft_lstnew(wdesc_new(ft_substr(str, *i, len), T_NOEXP)));
	*i += len + 1;
}

int		is_sep(char *chr)
{
	return (*chr == '|' || *chr == ' ' || *chr == '>' || *chr == '<');
}



t_list	**first_pass(t_minishell *state)
{
	t_list **tokens;
	int i;
	char *str;
	int j;
	char *ifs;

	ifs = " \t\n";
	str = state->line;


	tokens = malloc(sizeof(t_list *));
	*tokens = NULL;

	i = 0;
	while (str[i])
	{
		while (str[i] && in_set(&str[i], ifs))
			i++;
		while (str[i])
		{
			handle_quote(str, &i, *tokens);
			handle_dquote(str, &i, *tokens);
			if (!str[i])
				break;
			j = i;
			while (str[j] && !in_set(&str[j], ifs))
				j++;
			ft_lstadd_back(tokens, ft_lstnew(wdesc_new(ft_substr(str, i, j - i), 0)));
			i = j;
			if (!str[j])
				break;
			i++;
		}
	}
	return (tokens);
}

void	split_print(char **split)
{
	int i;

	i = 0;
	while (split[i])
	{
		fprintf(stderr, "(%s), ", split[i++]);
	}
	fprintf(stderr, "\n");
}

void	split_on_special(t_list **tokens)
{
	t_list *li;
	t_list *prev;
	char *ifs;
	t_word_desc *word_desc;
	t_list **split;

	li = *tokens;
	ifs = "|><";
	prev = NULL;
	while (li)
	{
		word_desc = li->content;
		split = ft_split2(word_desc->word, "|", word_desc->flags);
		if (!split)
		{
			prev = li;
			fprintf(stderr, "didnt split !!\n");
		}
		else if (!prev)
		{
			fprintf(stderr, "first branch, split is: \n");
			word_list_print(split);
			*tokens = *split;
			li = ft_lstlast(*split);
			ft_lstadd_back(split, li->next);
			prev = li;
		}
		else
		{
			fprintf(stderr, "second branch, split is: \n");
			word_list_print(split);
			li = ft_lstlast(*split);
			ft_lstadd_back(split, prev->next->next);
			word_li_free(prev->next);
			prev->next = *split;
			prev = li;
		}
		free(split);

		li = li->next;
	}
}

t_list **string_tokenize(t_minishell *state)
{
	t_list **tokens;

	tokens = first_pass(state);
	split_on_special(tokens);
	fprintf(stderr, "final list:\n");
	word_list_print(tokens);

	word_list_free(tokens);

	return (tokens);
}
