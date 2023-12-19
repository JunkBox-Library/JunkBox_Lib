/**
@brief   SMTPツールプログラム
@file    smtp_tool.c
@author  Fumi.Iseki (C)
@date    2005 12/25
*/

#ifdef CPLUSPLUS
    #undef CPLUSPLUS
#endif


#include "smtp_tool.h"
#include "mime_tool.h"


/**
char*  get_smtp_rcpt(char* mesg)

mesg から RCPT TO: コマンドを検索し，配送先のメールアドレスを返す．
*/
char*  get_smtp_rcpt(char* mesg)
{
    char* pp;
    char* pt;
    char* pc;
    char* pa = NULL;

    pp = awk(mesg, ':', 1);
    pc = pack_char(pp, ' ');
    freeNull(pp);
    if (pc==NULL) return NULL;

    if (!strcasecmp("RCPT TO", pc)) {
        pp = (char*)malloc(strlen((const char*)mesg)+1);
        pt = strstr(mesg, ":");
        if (pp==NULL || pt==NULL) {
            freeNull(pc);
            freeNull(pp);
            return NULL;
        }
        memset(pp, 0, strlen((const char*)mesg)+1);
        memcpy(pp, pt, strlen((const char*)pt));

        pa = get_smtp_mailbox(pp);
        freeNull(pp);
    }

    freeNull(pc);
    return pa;
}
    


/**
char*   get_smtp_mailbox(char* mesg)

@<forward-path> から @<mailbox> を返す．see RFC821 @n
RFC821の @<mailbox> とは，通常のメールアドレスのこと．

後ろの文字から探索し，(':' || '<' || ' ') 〜 !(' ' && '>' && CR && LF) を取り出す
*/
char*   get_smtp_mailbox(char* mesg)
{
    int i, j, k, l;
    char* mb;

    i = strlen((const char*)mesg);
    mb = (char*)malloc(i+1);
    if (mb==NULL) return NULL;
    memset(mb, 0, i+1);

    i--;
    while(i>=0) {
        if (mesg[i]!=' ' && mesg[i]!='>' && mesg[i]!=CHAR_CR && mesg[i]!=CHAR_LF) break;    
        i--;
    }
    l = i;

    while(i>=0) {
        if (mesg[i]==':' || mesg[i]=='<' || mesg[i]==' ') break;    
        i--;
    }
    k = i + 1;

    for (i=k, j=0; i<=l; i++) mb[j++] = mesg[i];

    return mb;
}



/**
int   is_smtp_onecommand(char* mesg, char* com)

DATA, RSETなどの空白無し，引数なしのコマンドを識別する．
*/
int   is_smtp_onecommand(char* mesg, char* com)
{
    int   ret = FALSE;
    char* pp;
    char* pc;

    pp = pack_char(mesg, ' ');
    pc = awk(pp, ' ', 1);
    freeNull(pp);
    if (pc==NULL) return FALSE;

    if (!strncasecmp(com, pc, strlen((const char*)com))) ret = TRUE;

    freeNull(pc);
    return ret;
}
    


/**
int  smtp_check_dot(char* mesg) 

メールの終わり "@\r@\n.@\r@\n" を探す．
    
@return @\r@\n.@\r@\n を見つけたら TRUE, 無かったら FALSE
*/
int  smtp_check_dot(char* mesg) 
{
    static int preDot = OFF;
    char* p = mesg;

    if (preDot==ON) {                               // 文頭で ".\r\n" を探す．
        if (!strncmp(".\r\n", mesg, 3)) {
            preDot  = OFF;
            return TRUE;
        }
        preDot = OFF;
    }

    while(*p!='\0') {
        if (!strncmp("\r\n", p, 2)) {               // 文中で "\r\n" を探す．   
            if (p[2]=='\0') {                       // 文末で "\r\n" を探す．
                preDot = ON;
                return FALSE;
            }
            else if (!strncmp("\r\n.\r\n", p, 5)) { // 文中で "\r\n.\r\n" を探す．
                preDot = OFF;
                return TRUE;
            }
        }
        p++;
    }

    return FALSE;
}


