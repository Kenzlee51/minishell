#include "minishell.h"

t_list	**ft_split2(char const *s, const char *delim)
{
	int				len;
	int delim_len;
	int i;
	char *substr;
	t_list **list;

	list = malloc(sizeof(t_list *));
	*list = NULL;
	i = 0;
	len = ft_strlen(s);
	delim_len = ft_strlen(delim);
	substr = ft_strnstr(s, delim, len);
	if (!substr)
	{
		free(list);
		return (NULL);
	}
	while (s)
	{
		 substr = ft_strnstr(s, delim, len);
		 if (substr)
		 {
			 if (substr - &s[i])
				 ft_lstadd_back(list, ft_lstnew(wdesc_new(ft_substr(s, 0, substr - &s[i]), T_NOSPC)));
			 ft_lstadd_back(list, ft_lstnew(wdesc_new(ft_substr(substr, 0, delim_len), T_NOSPC)));
			 s = substr + delim_len;
		 }
		 else
		 {
			 ft_lstadd_back(list, ft_lstnew(wdesc_new(ft_substr(s, 0, len), -1)));
			 break;
		 }
	}
	return (list);
}
