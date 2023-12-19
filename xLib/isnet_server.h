
extern Buffer* Base64_DHspki;
extern Buffer* Base64_RSAspki;

extern EVP_CIPHER* CRYPT_Type;
extern Buffer* CRYPT_SharedKey;
extern DH*     DHkey;

extern Buffer* User_ID;
extern Buffer* User_Passwd;
extern Buffer* User_Salt;

extern int     No_isNet_Chlng;
extern int     Use_isNet_Ldap;
extern int     Use_isNet_Crypt;


// サーバ側でのコマンド受信処理
int    command_HELLO  (Buffer operand, Buffer comment, int sock);    // 100
int    command_BYE    (Buffer operand, Buffer comment, int sock);    // 110
int    command_USERID (Buffer operand, Buffer comment, int sock);    // 120
int    command_PASSWD (Buffer operand, Buffer comment, int sock);    // 130
int    command_NOCHLNG(Buffer operand, Buffer comment, int sock);    // 140
int    command_KEYEX  (Buffer operand, Buffer comment, int sock);    // 160
int    command_CRYPT  (Buffer operand, Buffer comment, int sock);    // 170

