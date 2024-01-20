#include "common.h"

LONG showMessage(struct Window *w, STRPTR title, STRPTR text, STRPTR buttonText)
{
	struct EasyStruct easy = {
	    sizeof (struct EasyStruct),
		0,
	    "",
        "",
        ""
	};

	easy.es_TextFormat = text;
	easy.es_Title = title;
    easy.es_GadgetFormat = buttonText;
	return	EasyRequest(w, &easy, NULL, NULL);
}


