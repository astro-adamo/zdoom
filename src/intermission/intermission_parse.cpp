/*
** intermission_parser.cpp
** Parser for intermission definitions in MAPINFO 
** (both new style and old style 'ENDGAME' blocks)
**
**---------------------------------------------------------------------------
** Copyright 2010 Christoph Oelckers
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
*/


#include "intermission/intermission.h"
#include "g_level.h"

struct EndSequence
{
	SBYTE EndType;
	bool MusicLooping;
	bool PlayTheEnd;
	FString PicName;
	FString PicName2;
	FString Music;
};

enum EndTypes
{
	END_Pic,
	END_Bunny,
	END_Cast,
	END_Demon
};


FName FMapInfoParser::ParseEndGame(FScanner &sc)
{
	EndSequence newSeq;
	static int generated = 0;

	newSeq.EndType = -1;
	newSeq.PlayTheEnd = false;
	newSeq.MusicLooping = true;
	while (!sc.CheckString("}"))
	{
		sc.MustGetString();
		if (sc.Compare("pic"))
		{
			ParseAssign();
			sc.MustGetString();
			newSeq.EndType = END_Pic;
			newSeq.PicName = sc.String;
		}
		else if (sc.Compare("hscroll"))
		{
			ParseAssign();
			newSeq.EndType = END_Bunny;
			sc.MustGetString();
			newSeq.PicName = sc.String;
			ParseComma();
			sc.MustGetString();
			newSeq.PicName2 = sc.String;
			if (CheckNumber())
				newSeq.PlayTheEnd = !!sc.Number;
		}
		else if (sc.Compare("vscroll"))
		{
			ParseAssign();
			newSeq.EndType = END_Demon;
			sc.MustGetString();
			newSeq.PicName = sc.String;
			ParseComma();
			sc.MustGetString();
			newSeq.PicName2 = sc.String;
		}
		else if (sc.Compare("cast"))
		{
			newSeq.EndType = END_Cast;
		}
		else if (sc.Compare("music"))
		{
			ParseAssign();
			sc.MustGetString();
			newSeq.Music = sc.String;
			if (CheckNumber())
			{
				newSeq.MusicLooping = !!sc.Number;
			}
		}
		else
		{
			if (format_type == FMT_New)
			{
				// Unknown
				sc.ScriptMessage("Unknown property '%s' found in endgame definition\n", sc.String);
				SkipToNext();
			}
			else
			{
				sc.ScriptError("Unknown property '%s' found in endgame definition\n", sc.String);
			}

		}
	}
	switch (newSeq.EndType)
	{
	case END_Pic:
	case END_Bunny:
	case END_Cast:
	case END_Demon:
		;
	}
	FString seq;
	seq.Format("EndSequence_%d_", generated++);
	return FName(seq);
}

FName FMapInfoParser::CheckEndSequence(FScanner &sc)
{
	const char *seqname = NULL;

	if (sc.Compare("endgame"))
	{
		if (!sc.CheckString("{"))
		{
			// Make Demon Eclipse work again
			sc.UnGet();
			goto standard_endgame;
		}
		return ParseEndGame(sc);
	}
	else if (strnicmp (sc.String, "EndGame", 7) == 0)
	{
		switch (sc.String[7])
		{
		case '1':	seqname = "Inter_Pic1";		break;
		case '2':	seqname = "Inter_Pic2";		break;
		case '3':	seqname = "Inter_Bunny";	break;
		case 'C':	seqname = "Inter_Cast";		break;
		case 'W':	seqname = "Inter_Underwater";	break;
		case 'S':	seqname = "Inter_Strife";	break;
	standard_endgame:
		default:	seqname = "Inter_Pic3";		break;
		}
	}
	else if (sc.Compare("endpic"))
	{
		ParseComma();
		sc.MustGetString ();
		FString seqname;
		seqname << "EndPic_" << sc.String;
		// create sequence here
		return FName(seqname);
	}
	else if (sc.Compare("endbunny"))
	{
		seqname = "Inter_Bunny";
	}
	else if (sc.Compare("endcast"))
	{
		seqname = "Inter_Cast";
	}
	else if (sc.Compare("enddemon"))
	{
		seqname = "Inter_Demonscroll";
	}
	else if (sc.Compare("endchess"))
	{
		seqname = "Inter_Chess";
	}
	else if (sc.Compare("endunderwater"))
	{
		seqname = "Inter_Underwater";
	}
	else if (sc.Compare("endbuystrife"))
	{
		seqname = "Inter_BuyStrife";
	}
	else if (sc.Compare("endtitle"))
	{
		seqname = "Inter_Titlescreen";
	}

	if (seqname != NULL)
	{
		return FName(seqname);
	}
	return NAME_None;
}