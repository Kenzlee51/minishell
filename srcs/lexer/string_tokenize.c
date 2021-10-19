#include "minishell.h"

void	word_li_append(t_list **tokens, char *word, int flag)
{
	if ((*word) || (!(*word) && (flag & (T_NOEXP | T_DQUOTE))))
		ft_lstadd_back(tokens, ft_lstnew(wdesc_new(word, flag)));
	else
	{
		free(word);
	}
}

void	delims_free(t_list **delims)
{
	t_list *delim;
	t_list *tmp;

	delim = *delims;
	while (delim)
	{
		tmp = delim->next;
		free(delim);
		delim = tmp;
	}
}

void	word_li_free(t_list *li)
{
	if (li)
	{
		free(((t_word_desc *)li->content)->word);
		free(li->content);
		free(li);
	}
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
	if (!in_set(&str[*i + len + 1], " \t\n"))
		flags |= T_NOSPC;
	ft_lstadd_back(&tokens, ft_lstnew(wdesc_new(ft_substr(str, *i, len), T_NOEXP | T_VAR)));

}

void	handle_dquote(char *str, int *i, t_list **tokens, int flag_add)
{
	int len;

	if (str[*i] != 0x22)
		return;
	if ((*i) && str[*i - 1] == '\\')
		return;
	(*i)++;
	len = ft_strchr(&str[*i], 0x22) - &str[*i];
	if (len < 0)
	{
		//TODO handle error
		ft_putendl_fd("DQUOTE ERROR", 2);
		exit(-3);
	}
	if (!in_set(&str[*i + len + 1], " \t\n"))
		flag_add |= T_NOSPC;

	word_li_append(tokens, ft_substr(str, *i, len), T_DQUOTE | flag_add);
	*i += len + 1;
}

void	handle_quote(char *str, int *i, t_list **tokens, int flag_add)
{
	int len;

	if (str[*i] != 0x27)
		return;
	if ((*i) && str[*i - 1] == '\\')
		return;
	(*i)++;
	len = ft_strchr(&str[*i], 0x27) - &str[*i];
	if (len < 0)
	{
		//TODO handle error
		ft_putendl_fd("QUOTE ERROR", 2);
		exit(-3);
	}

	if (!in_set(&str[*i + len + 1], " \t\n"))
		flag_add |= T_NOSPC;
	word_li_append(tokens, ft_substr(str, *i, len), T_NOEXP | flag_add);
	*i += len + 1;
}

int		is_sep(char *chr)
{
	return (*chr == '|' || *chr == ' ' || *chr == '>' || *chr == '<');
}

void	scroll_ifs(char *str, int *i)
{
	while ((str[*i] == ' ') || (str[*i] == '\n') || (str[*i] == '\t'))
	{
		(*i)++;
	}
}

// splits by IFS, respecting quotes
t_list	**first_pass(char *str, int flag_add)
{
	t_list **tokens;
	int i;
	int j;
	char *ifs;

	ifs = " \t\n";


	tokens = malloc(sizeof(t_list *));
	*tokens = NULL;

	i = 0;
	while (str[i])
	{
		/* while (str[i] && str[i] != '\'' && str[i] != '\"' && in_set(&str[i], ifs)) */
		scroll_ifs(str, &i);
		while (str[i])
		{
			scroll_ifs(str, &i);
			handle_quote(str, &i, tokens, flag_add);
			handle_dquote(str, &i, tokens, flag_add);
			scroll_ifs(str, &i);
			if (!str[i])
				break;
			j = i;
			while (str[j] && !in_set(&str[j], ifs) && str[j] != '\'' && str[j] != '\"')
				j++;
			if (str[j] == '\'')
			{

				word_li_append(tokens, ft_substr(str, i, j - i), T_NOSPC | flag_add);
				handle_quote(str, &j, tokens, 0);
			}
			else if (str[j] == '\"')
			{
				word_li_append(tokens, ft_substr(str, i, j - i), T_NOSPC | flag_add);
				handle_dquote(str, &j, tokens, 0);
			}
			else
				word_li_append(tokens, ft_substr(str, i, j - i), flag_add);
			i = j;
			if (!str[j])
				break;
			scroll_ifs(str, &i);
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

void	split_on_special(t_list **tokens, t_list **delims, int spec_flg)
{
	t_list *li;
	t_list *delim;
	t_list *prev;
	t_word_desc *word_desc;
	t_list **split;

	delim = *delims;
	while (delim)
	{
		li = *tokens;
		prev = NULL;
		while (li)
		{
			word_desc = li->content;
			if (word_desc->flags & (T_DQUOTE | T_NOEXP | T_SPEC))
				split = NULL;
			else
				split = ft_split2(word_desc->word, delim->content, word_desc->flags, spec_flg);
			tokens_insert(split, &li, &prev, tokens);
			free(split);
			li = li->next;
		}
		delim = delim->next;
	}
}

t_list **string_tokenize(t_minishell *state)
{
	t_list **tokens;
	t_list *delims;

	delims = NULL;

	tokens = first_pass(state->line, 0);

	fprintf(stderr, "=== after first_pass: ===\n");
	word_list_print(tokens);

	ft_lstadd_back(&delims, ft_lstnew("|"));
	split_on_special(tokens, &delims, T_SPEC);
	delims_free(&delims);

	delims = NULL;
	ft_lstadd_back(&delims, ft_lstnew("<<"));
	ft_lstadd_back(&delims, ft_lstnew(">>"));
	ft_lstadd_back(&delims, ft_lstnew(">"));
	ft_lstadd_back(&delims, ft_lstnew("<"));
	split_on_special(tokens, &delims, T_SPEC | T_REDIR);
	delims_free(&delims);

	fprintf(stderr, "=== after split_on_special: ===\n");
	word_list_print(tokens);

	split_on_vars(tokens);

	fprintf(stderr, "=== after split_on_vars: ===\n");
	word_list_print(tokens);

	/* word_list_free(tokens); */
	/* free(tokens); */

	return (tokens);
}
