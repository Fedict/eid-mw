char const *_MSG_(int msgnum)
{
	char const *message = beid_messages[0][msgnum]; // default=English
	char const *lang = getenv("LANG");

	if (lang != NULL && strlen(lang) == 5 && lang[2] == '_')
	{
		int i;

		for (i = 0; i < 4; i++)
		{
			if (strstr(lang, beid_messages[i][0]) == lang)
			{
				message = beid_messages[i][msgnum];
				break;
			}
		}
	}
	return message;
}
