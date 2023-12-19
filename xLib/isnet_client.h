

// クライアント側でのコマンド送信処理
int    send_command_recv_ans(int sock, char* command, int tm, Buffer* key, EVP_CIPHER* cipher);
Buffer recv_mesg_until_end(int sock, int tmm, Buffer* key, EVP_CIPHER* cipher);

Buffer send_algor_recv_spki(int sock, char* algor, int tm, Buffer* key, EVP_CIPHER* cipher);
int    send_spki_recv_ans  (int sock, Buffer mkey, int tm, Buffer* key, EVP_CIPHER* cipher);


// クライアント側での認証処理
int    check_auth(Buffer ahost, int aport, Buffer userid, Buffer passwd, int chmod, int keyex, int cryptm, char* cfn, int cont);


// クライアント側での暗号化開始・終了処理
int    start_CRYPT_transfer(int sock, int keyex, int cryptm, Buffer ipaddr, char* cfn, int tm, Buffer* shdkey, EVP_CIPHER** cipher);
int    stop_CRYPT_transfer (int sock, int tm, Buffer* shdkey, EVP_CIPHER* cipher);


