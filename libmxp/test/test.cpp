/***************************************************************************
 *   Copyright (C) 2004 by Tomas Mecir                                     *
 *   kmuddy@kmuddy.org                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 ***************************************************************************/

#include <libmxp/libmxp.h>

#include <stdio.h>
#include <string.h>

void displayResult (mxpResult *res)
{
  switch (res->type)
  {
    case 0: {
      printf ("Nothing.\n");
    }
    break;
    case 1: {
      char *s = (char *) res->data;
      if (s && (strcmp (s, "\r\n") == 0))  //newlines are always sent separately
        printf ("Newline.\n");
      else
        printf ("Text: %s\n", s?s:"");
    }
    break;
    case 2: {
      int *t = (int *) res->data;
      printf ("Line tag: %d\n", *t);
    }
    break;
    case 3: {
      flagStruct *fs = (flagStruct *) res->data;
      char *s = fs->name;
      if (fs->begin)
        printf ("Start of flag: %s\n", s?s:"");
      else
        printf ("End of flag: %s\n", s?s:"");
    }
    break;
    case 4: {
      varStruct *vs = (varStruct *) res->data;
      char *n = vs->name;
      char *v = vs->value;
      if (vs->erase)
        printf ("Erase variable: %s\n", n?n:"");
      else
        printf ("Variable: %s, value: %s\n", n?n:"", v?v:"");
    }
    break;
    case 5: {
      formatStruct *fs = (formatStruct *) res->data;
      unsigned char mask = fs->usemask;
      if (mask & USE_BOLD)
        printf ("Formatting: %s\n",
            (fs->attributes & Bold) ? "bold" : "not bold");
      if (mask & USE_ITALICS)
        printf ("Formatting: %s\n",
            (fs->attributes & Italic) ? "italics" : "not italics");
      if (mask & USE_UNDERLINE)
        printf ("Formatting: %s\n",
            (fs->attributes & Underline) ? "underline" : "not underline");
      if (mask & USE_STRIKEOUT)
        printf ("Formatting: %s\n",
            (fs->attributes & Strikeout) ? "strikeout" : "not strikeout");
      if (mask & USE_FG)
        printf ("Formatting: foreground color (%d, %d, %d)\n", fs->fg.r, fs->fg.g, fs->fg.b);
      if (mask & USE_BG)
        printf ("Formatting: background color (%d, %d, %d)\n", fs->bg.r, fs->bg.g, fs->bg.b);
      if (mask & USE_FONT)
        printf ("Formatting: font %s\n", fs->font?fs->font:"");
      if (mask & USE_SIZE)
        printf ("Formatting: size %d\n", fs->size);
    }
    break;
    case 6: {
      linkStruct *ls = (linkStruct *) res->data;
      printf ("URL link: name %s, URL %s, text %s, hint %s\n",
          ls->name?ls->name:"", ls->url?ls->url:"", ls->text?ls->text:"", ls->hint?ls->hint:"");
    }
    break;
    case 7: {
      sendStruct *ss = (sendStruct *) res->data;
      printf ("Send link: name %s, command %s, text %s, hint %s, to prompt: %s, menu: %s\n",
          ss->name?ss->name:"", ss->command?ss->command:"", ss->text?ss->text:"",
          ss->hint?ss->hint:"", ss->toprompt?"yes":"no", ss->ismenu?"yes":"no");
    }
    break;
    case 8: {
      char *s = (char *) res->data;
      printf ("Expire: %s\n", s?s:"");
    }
    break;
    case 9: {
      char *s = (char *) res->data;
      printf ("Send this: %s\n", s?s:"");
    }
    break;
    case 10: {
      printf ("Horizontal line\n");
    }
    break;
    case 11: {
      soundStruct *ss = (soundStruct *) res->data;
      if (ss->isSOUND)
        printf ("Sound: file %s, URL %s, volume %d, %d repeats, priority %d, type %s\n",
            ss->fname?ss->fname:"", ss->url?ss->url:"", ss->vol, ss->repeats, ss->priority,
            ss->type?ss->type:"");
      else
        printf ("Music: file %s, URL %s, volume %d, %d repeats, continue %s, type %s\n",
            ss->fname?ss->fname:"", ss->url?ss->url:"", ss->vol, ss->repeats,
            ss->continuemusic?"yes":"no", ss->type?ss->type:"");
    }
    break;
    case 12: {
      windowStruct *ws = (windowStruct *) res->data;
      printf ("Create window: name %s, title %s, left %d, top %d, width %d, height %d, scrolling %s, floating %s\n",
          ws->name?ws->name:"", ws->title?ws->title:"", ws->left, ws->top, ws->width, ws->height,
          ws->scrolling?"yes":"no", ws->floating?"yes":"no");
    }
    break;
    case 13: {
      internalWindowStruct *ws = (internalWindowStruct *) res->data;
      char *s = 0;
      switch (ws->align) {
        case Left: s = "left"; break;
        case Right: s = "right"; break;
        case Bottom: s = "bottom"; break;
        case Top: s = "top"; break;
        case Middle: s = "middle (invalid!)"; break;
      };
      printf ("Create internal window: name %s, title %s, align %s, scrolling %s\n",
          ws->name?ws->name:"", ws->title?ws->title:"", s, ws->scrolling?"yes":"no");
    }
    break;
    case 14: {
      char *s = (char *) res->data;
      printf ("Close window: %s\n", s?s:"");
    }
    break;
    case 15: {
      char *s = (char *) res->data;
      printf ("Set active window: %s\n", s?s:"");
    }
    break;
    case 16: {
      moveStruct *ms = (moveStruct *) res->data;
      printf ("Move cursor: X=%d, Y=%d\n", ms->x, ms->y);
    }
    break;
    case 17: {
      printf ("Erase text: %s\n", res->data ? "rest of frame" : "rest of line");
    }
    break;
    case 18: {
      relocateStruct *rs = (relocateStruct *) res->data;
      printf ("Relocate: server %s, port %d\n", rs->server, rs->port);
    }
    break;
    case 19: {
      printf (res->data ? "Send username\n" : "Send password\n");
    }
    break;
    case 20: {
      imageStruct *is = (imageStruct *) res->data;
      char *s = 0;
      switch (is->align) {
        case Left: s = "left"; break;
        case Right: s = "right"; break;
        case Bottom: s = "bottom"; break;
        case Top: s = "top"; break;
        case Middle: s = "middle"; break;
      };
      printf ("Image: name %s, URL %s, type %s, height %d, width %d, hspace %d, vspace %d, align %s\n",
          is->fname?is->fname:"", is->url?is->url:"", is->type?is->type:"", is->height,
          is->width, is->hspace, is->vspace, s);
    }
    break;
    case 21: {
      char *s = (char *) res->data;
      printf ("Image map: %s\n", s?s:"");
    }
    break;
    case 22: {
      gaugeStruct *gs = (gaugeStruct *) res->data;
      printf ("Gauge: variable %s, max.variable %s, caption %s, color (%d, %d, %d)\n",
          gs->variable?gs->variable:"", gs->maxvariable?gs->maxvariable:"",
          gs->caption?gs->caption:"", gs->color.r, gs->color.g, gs->color.b);
    }
    break;
    case 23: {
      statStruct *ss = (statStruct *) res->data;
      printf ("Status bar: variable %s, max.variable %s, caption %s\n",
          ss->variable?ss->variable:"", ss->maxvariable?ss->maxvariable:"",
          ss->caption?ss->caption:"");
    }
    break;
    case -1: {
      char *s = (char *) res->data;
      printf ("Error: %s\n", s?s:"");
    }
    break;
    case -2: {
      char *s = (char *) res->data;
      printf ("Warning: %s\n", s?s:"");
    }
    break;
  }
}

int main ()
{
  RGB white = {255, 255, 255};
  RGB black = {0, 0, 0};
  RGB yellow = {255, 255, 0};

  //create MXP handler
  MXPHANDLER h = mxpCreateHandler ();

  //initialize the MXP handler
  mxpSetDefaultText (h, "fixed", 12, false, false, false, false, white, black);
  mxpSetHeaderParams (h, 1, "fixed", 28, true, true, true, false, yellow, black);
  mxpSetHeaderParams (h, 2, "fixed", 24, true, true, true, false, yellow, black);
  mxpSetHeaderParams (h, 3, "fixed", 20, true, true, true, false, yellow, black);
  mxpSetHeaderParams (h, 4, "fixed", 16, true, true, false, false, yellow, black);
  mxpSetHeaderParams (h, 5, "fixed", 14, true, true, false, false, yellow, black);
  mxpSetHeaderParams (h, 6, "fixed", 12, true, true, false, false, yellow, black);
  mxpSetDefaultGaugeColor (h, white);
  mxpSetNonProportFont (h, "fixed");
  mxpSetClient (h, "KMuddy", "0.7");

  //feed the library with text
  mxpProcessText (h, "\e[1z");
  mxpProcessText (h, "&unknownentity;&lt;&gt;<c yellow back=blue>starting</c> text");
  mxpProcessText (h, " and<font size=11 blue blackx face='\"\"><<>Times New Roman' 13>more");
  mxpProcessText (h, "<font face=\"baaah\"></font></font> text\r\n\e[1zline 2");
  mxpProcessText (h, " <S><b><i>with bold text, &Aacute;, &lt;, &gt;, unfinished &");
  mxpProcessText (h, "\e[3z</BoLd>:)<a href=\r\nline 3");
  mxpProcessText (h, "\e[6z");
  mxpProcessText (h, "<hr><p>line1\r\nline2\r\nline3\r\nline4\r\n all on the same line!</p>");
  mxpProcessText (h, "break line here<br><nobr>this won't break \r\nstill");
  mxpProcessText (h, "the same line\r\nanother line");
  mxpProcessText (h, "<c darkgreen> dark green <h>High-color</h> Normal color</c>\r\n");
  mxpProcessText (h, "<a \"http://www.kmuddy.org\" KMuddy kmuddy>Click here for KMuddy</a>");
  mxpProcessText (h, "<send href=\"buy bread|buy milk\" hINt=\"this is a hint|bread hint|");
  mxpProcessText (h, "milk hint\" expire=link>yes, bread</send>");
  mxpProcessText (h, "<expire link><expire>\n");
  mxpProcessText (h, "<VeRsIoN>\n");
  mxpProcessText (h, "<sound bah.wav v=10000 p=30 c=1><music chrm.mid u=none>");
  mxpProcessText (h, "<gauge hp max=maxhp caption='Your HP' color=green><stat sp max=maxsp");
  mxpProcessText (h, "caption='Spell points' invalid>");
  mxpProcessText (h, "<relocate port=8080 name=bah server=test><user><password>\n");
  mxpProcessText (h, "<image bah.jpg URL=www.url.org t=type1 h=100 w=50 align=middle");
  mxpProcessText (h, "vspace=10 hspace=10><image bah.jpg ISMaP><send showmap>");
  mxpProcessText (h, "<image bah.jpg ismap></send>\n");
  mxpProcessText (h, "<!en test mytest DESC=desc PUBLISH>&test;<!en test DELETE>");
  mxpProcessText (h, "<v bah>some<b></b>thing</v>\n");
  mxpProcessText (h, "<h2>Heading 2</h2><h5>Heading 5</h5>\r\n<small>small</small>");
  mxpProcessText (h, "<tt>non-proportional</tt>\r\n");

  mxpProcessText (h, "<frame name=Status redirect Height=30>");
  mxpProcessText (h, "text for status");
  mxpProcessText (h, "<frame _previous redirect>");
  mxpProcessText (h, "<dest status>100</dest>");
  mxpProcessText (h, "<dest status eof>100</dest>");
  mxpProcessText (h, "<frame Status close>\r\n");
  mxpProcessText (h, "<!-- this is a comment, <b> stuff not parsed -->\r\n");
  mxpProcessText (h, "This will break the line: \rnew line\r\n");
  
  //OKay, here goes the !element testing...
  //EMPTY and OPEN flags have been successfully tested :)
  mxpProcessText (h, "<!element boldred '<color red><b>'>");
  mxpProcessText (h, "normal, <boldred>bold red</boldred>, normal again\n");
  mxpProcessText (h, "<!element ex '<send>'><ex>n</ex><ex>s</ex>\r\n");
  mxpProcessText (h, "<!entity col 'This should not be displayed' private>");
  mxpProcessText (h, "<!element bt '<b><c &col;>' att='col=red' flag=hrm>");
  mxpProcessText (h, "<bt>bold red <bt blue>bold blue</bt></bt>\r\n");
  mxpProcessText (h, "<!attlist bt 'col=green'>");
  mxpProcessText (h, "<bt>bold green <bt blue>bold blue</bt></bt>\r\n");
  mxpProcessText (h, "<!el bt delete><bt>in invalid tag</bt>\r\n");
  mxpProcessText (h, "<!el tagtest '<b><u><i>' tag=28>\r\n");
  mxpProcessText (h, "\e[28zthis should be b,u,i\r\nnormal\r\n");
  
  //display results
  while (mxpHasResults (h))
  {
    mxpResult *res = mxpNextResult (h);
    displayResult (res);
  }

  //free memory
  mxpDestroyHandler (h);
}
