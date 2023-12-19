
#include "common.h"
#include "ldap_tool.h"


int main()
{
    JBXL_LDAP_Host* ldap_host = new_LDAP_Host();
    JBXL_LDAP_Dn*   ldap_bind = new_LDAP_Dn();

    read_ldap_config(NULL, ldap_host, ldap_bind);

//////////////////////////////////////////////////////
    copy_s2Buffer("202.26.150.51", &ldap_host->hostname);
    ldap_host->useSSL = FALSE;
    ldap_host->port   = 9000;
    ldap_bind->dnbind = make_Buffer_bystr("cn=Manager");
    ldap_bind->passwd = make_Buffer_bystr(".......");
    ldap_bind->base   = make_Buffer_bystr("ou=user,dc=nsl,dc=tuis,dc=ac,dc=jp");
////////////////////////////////////////////////////////

    printf("CERT = %d\n", ldap_host->reqCert);
    printf("PORT = %d\n", ldap_host->port);
    if (ldap_host->useSSL==TRUE) printf("use SSL\n");

    LDAP* ld = open_ldap_connection(ldap_host, ldap_bind);
    if (ld==NULL) {
        printf("LDAP Open FAIL\n");
        exit(1);
    }

    int ret;
    ret = simple_check_ldap_passwd(ld, "iseki", NULL, ldap_bind);
    jbxl_fprint_state_jp(stderr, ret);

    close_ldap_connection(ld, &ldap_host, &ldap_bind);

    return 0;
}

