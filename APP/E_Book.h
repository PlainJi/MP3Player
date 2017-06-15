#ifndef __E_BOOK_H
#define __E_BOOK_H

#include "sys.h"
#define TP_EBOOK_PREPAGE	(ScreenX>0   && ScreenX<176 && ScreenY>18  && ScreenY<101)
#define TP_EBOOK_NEXTPAGE	(ScreenX>0   && ScreenX<176 && ScreenY>101 && ScreenY<220)
#define TP_EBOOK_PREONE		(ScreenX>0   && ScreenX<36  && ScreenY>0   && ScreenY<18 )
#define TP_EBOOK_NEXTONE	(ScreenX>140 && ScreenX<176 && ScreenY>0   && ScreenY<18 )

u8 Ebook(u8 state);

#endif