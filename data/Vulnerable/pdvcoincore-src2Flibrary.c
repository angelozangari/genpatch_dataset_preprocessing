#include "library.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "des56.h"
#include "library.h"

const char *codout = "0123456789CEFHJKMNPRTXW";

char *wprint_2(chunk *v)
{
    static char s[18];
    register int i;
    register char *p;

    p = s;
    for(i = 0; i < 8; i++) {
        sprintf(p, "%02X", v->b[i] & 0xff);
        p += 2;
    }
    return(s);
}

unsigned long long switch_bytes_bigendian_littleendian(unsigned long long val)
{
    return (val << 8 & 0xFF00FF00FF00FF00ULL) | (val >> 8 & 0x00FF00FF00FF00FFULL);
}

void getv(char *s, chunk *v)
{
    register int i, t;

    if(s[0] == '0' && s[1] == 'x')
        s += 2; /* Ignore C-style 0x prefix */
    for(i = 0; i < 8; i++) {
        t = 0;
        if(*s >= '0' && *s <= '9') t = *s++ - '0';
        else if(*s >= 'a' && *s <= 'f') t = *s++ - 'a' + 10;
        else if(*s >= 'A' && *s <= 'F') t = *s++ - 'A' + 10;
        t <<= 4;
        if(*s >= '0' && *s <= '9') t |= *s++ - '0';
        else if(*s >= 'a' && *s <= 'f') t |= *s++ - 'a' + 10;
        else if(*s >= 'A' && *s <= 'F') t |= *s++ - 'A' + 10;
        v->b[i] = t;
        if(*s == '.' && i == 3) {
            s++;
        }
    }
}

char generate_digit_ver(char pincodefull[]) {
    int tot = 0;
    int peso = 2;

    for (int pos = strlen(pincodefull)-1; pos >= 0; pos--) {
        tot += peso * (pincodefull[pos]-'0');

        if (peso == 9)
            peso = 2;
        else
            peso += 1;
    }

    // FROM TOT GET MOD 11
    int val_mod = tot % 11;
    int digit_ver = 0;

    if (val_mod == 1 || val_mod == 0)
        digit_ver = 0;
    else
        digit_ver = 11 - val_mod;

    char out_v[12];
    sprintf(out_v, "%d", digit_ver);

    return out_v[0];
}

char generate_digit_ver_n1(char pincodefull[]) {
    int tot = 0;
    int peso = 2;
    int len_grupo_n1 = strlen(codout) + 1;

    for (int pos = strlen(pincodefull)-1; pos >= 0; pos--) {
        tot += peso * (pincodefull[pos]-'0');

        if (peso == 9)
            peso = 2;
        else
            peso += 1;
    }

    // FROM TOT GET MOD 11
    int val_mod = tot % len_grupo_n1;
    int digit_ver = 0;

    if (val_mod == 1 || val_mod == 0)
        digit_ver = 0;
    else
        digit_ver = len_grupo_n1 - val_mod;

    return codout[digit_ver];
}

unsigned long long chunktoull(chunk *v) {
    unsigned long long s = 0;
    register int i;

    for(i = 0; i < 8; i++) {
        s = (s << 8) | (v->b[i] & 0xff);
    }
    return(s);
}

char *encript_info(char *client_key, char *codloja, char *pdv, char *cupom_fiscal, char *tot_num_sorte,
                   char *cartaoloja) {

    char *my_full_string;
    unsigned long long cod_int =
            (((atoi(codloja)*1000LLU+atoi(pdv))*(long)1000000LLU+atoi(cupom_fiscal))*1000LLU+atoi(tot_num_sorte))*10LLU +
            atoi(cartaoloja);

    asprintf(&my_full_string, "%llu", cod_int);

    // primeiro digito verificador
    char dig_verifador = generate_digit_ver(my_full_string);
    cod_int = cod_int * 10LLU + (dig_verifador-'0');
    asprintf(&my_full_string, "%llu", cod_int);

    // segundo digito verificador com o set de letras incluso
    dig_verifador = generate_digit_ver_n1(my_full_string);

    // convertendo para hexadecimal
    char *hex_info;
    asprintf(&hex_info, "%016llX", cod_int);

    // encriptando os dados
    chunk key, olddata, newdata;
    keysched KS;

    // Setando a chave
    getv(client_key, &key);
    fsetkey(key.b, &KS);

    // encriptando
    getv(hex_info, &olddata);
    newdata = olddata;
    fencrypt(newdata.b, 0, &KS);

    unsigned long long newdataull = chunktoull(&newdata);

    char *val_ret;
    // montando a string final com o big to little endian + o segundo dígito verificador
    asprintf(&val_ret, "%016llX%c", switch_bytes_bigendian_littleendian(newdataull), dig_verifador);
    return val_ret;
}

void getInfoPdv(InfoPdv *c, char *loj, char *caix, char *val, char *num, char *cartloja)
{
    strcpy(c->loja, loj);
    strcpy(c->caixa, caix);
    strcpy(c->valCompra, val);
    strcpy(c->numCupomFiscal, num);
    strcpy(c->cartaoloja, cartloja);
}

void getInfoKey(char *keyOri, char *datBegin, char *datEnd, int *mult, char *prefixo, char *keyRet) {
    strcpy(keyRet, keyOri);
    keyRet[16] = 0;

    // prefixo
    char prefixoBase[4] = "000";
    prefixoBase[3] = 0;

    char *ptchar = &keyOri[16];
    char *ptPrefixo = &prefixoBase[0];
    for(int posC = 0; posC<3; posC++) {
        if (*ptchar != '0')
            *ptPrefixo++ = *ptchar;
        ptchar++;
    }
    *ptPrefixo = 0;

    // convert string HEX to unsigned long long
    unsigned long long ullwrkkey;
    sscanf(keyRet,"%llX", &ullwrkkey); // string to long

    // big endian little indian
    char *chaveaberta;

    ullwrkkey = switch_bytes_bigendian_littleendian(ullwrkkey);
    asprintf(&chaveaberta, "%llu", ullwrkkey);

    // multiplo
    char multstr[10];
    strcpy(&multstr[0], &chaveaberta[12]);
    multstr[3] = 0;
    *mult = atoi(multstr);

    // data de inicio
    char wrkData[20];

    strcpy(&wrkData[0], &chaveaberta[0]);
    wrkData[6]=0;
    strcpy(datBegin , &wrkData[0]);

    // Data final
    strcpy(&wrkData[0], &chaveaberta[6]);
    wrkData[6]=0;
    strcpy(datEnd , &wrkData[0]);

    // prefixo
    strcpy(prefixo, prefixoBase);
}

int EncPromocodeVar(char *loj, char *caix, char *val, char *num, char *cartloja, char *key, char *code) {
    InfoPdv info;

    getInfoPdv(&info, loj, caix, val, num, cartloja);

    return EncPromocode(info, key, code);
}

int isnumber(char *strval){
    int ct_a=0;
    for (int i=0; strval[i]!= '\0'; i++) {
        if (strval[i] < '0' || strval[i] > '9')
            ct_a ++;
    }
    if (ct_a == 0) {
        return 1;
    } else {
        return 0;
    }
}

int EncPromocode(InfoPdv info, char *key, char *code) {
    int multiplo = 0, numcupons = 0;


    // checking the parameters
    if (strlen(info.loja) > 4) {
        strcpy(code, "The length of store information is limited to 4 chars");
        return -5;
    } else if (isnumber(info.loja) == 0 ) {
        strcpy(code, "The store should contain only numbers");
        return -7;
    } else if (strlen(info.caixa) > 3) {
        strcpy(code, "The length of PDV information is limited to 3 chars");
        return -10;
    } else if (isnumber(info.caixa) == 0) {
        strcpy(code, "The PDV should contain only numbers");
        return -12;
    } else if (strlen(info.numCupomFiscal) > 6) {
        strcpy(code, "The length of COO information is limited to 6 chars");
        return -15;
    } else if (isnumber(info.numCupomFiscal) == 0) {
        strcpy(code, "The COO should contain only numbers");
        return -15;
    } else if (strcmp(info.cartaoloja, "S") != 0 && strcmp(info.cartaoloja, "N") != 0) {
        strcpy(code, "credit card status must be 'S' or 'N'");
        return -20;
    }


    // Decodificando a chave
    if (strlen(key) != 19) {
        strcpy(code, "Invalid key length");
        return -25;
    }

    char datBegin[7], datEnd[7], prefixo[3] = "000", keyRet[17] = "00000000000000000";
    getInfoKey(key, &datBegin[0], &datEnd[0], &multiplo, &prefixo[0], &keyRet[0]);

    if (multiplo <= 0){
        strcpy(code, "Key - invalid value of multiple");
        return -30;
    }

    // Status do cartao de credito
    char *scloja;
    if (strcmp(info.cartaoloja, "N") == 0) {
        scloja = "0";
    } else {
        scloja = "1";
    }

    // Valor da compra
    double valcompra = 0;

    sscanf(info.valCompra, "%lf", &valcompra);
    if (valcompra <= 0){
        strcpy(code, "The purchase value should be a valid number");
        return -35;
    }

    if (valcompra < multiplo){
        strcpy(code, "The purchase value is smaller than the configured multi value");
        return -37;
    } else if (valcompra > 1000000){
        strcpy(code, "The purchase value is greater than 1000000");
        return -38;
    }

    // Numeros dos cupons
    numcupons = (int) valcompra / multiplo;
    char strncupons[12];

    if (numcupons <= 0) {
        strcpy(code, "Invalid numbers of coupons");
        return -40;
    } else if (numcupons>999) {
        // Numero de cupons maior que o limite do campo
        numcupons=999;
    }
    sprintf(strncupons, "%d", numcupons);

    // Datas
    struct tm datnow = *localtime(&(time_t){time(NULL)});
    time_t vDatToday = mktime(&datnow);

    int year, month, day;

    // Data Inicio
    if (testaData(datBegin)) {
        strcpy(code, "Key1 - Invalid start date");
        strcpy(code, datBegin);

        return -45;
    }

    sscanf(datBegin, "%2d%2d%2d", &year, &month, &day);

    struct tm datinicio = {0};
    datinicio.tm_year = (2000 + year)-1900;
    datinicio.tm_mon = month-1;
    datinicio.tm_mday = day;
    datinicio.tm_isdst = -1;  // Horario de verão
    time_t vDatInicio = mktime(&datinicio);

    // Data final
    if (testaData(datEnd)) {
        strcpy(code, "Key - Invalid closing date");
        return -50;
    }
    sscanf(datEnd, "%2d%2d%2d", &year, &month, &day);
    struct tm datfinal = {0};
    datfinal.tm_year = (2000 + year)-1900;
    datfinal.tm_mon = month-1;
    datfinal.tm_mday = day;
    datfinal.tm_isdst = -1;  // Horario de verão
    datfinal.tm_hour = 23;
    datfinal.tm_min = 59;
    datfinal.tm_sec = 59;
    time_t vDatFinal = mktime(&datfinal);

    if (vDatInicio > vDatToday) {
        strcpy(code, "Key - Promotion start date greater than today's date");
        return -55;
    }
    if (vDatFinal < vDatToday) {
        strcpy(code, "Expired key");
        return -60;
    }

    // encrypting
    char ret_val[20];
    char *voucher = encript_info(&keyRet[0], info.loja, info.caixa, info.numCupomFiscal, strncupons, scloja);

    // Trocando as letras
    char *bvoucher = voucher;
    for (int posic = 0; posic < 19; posic++){
        if (bvoucher[0] == 0)
            break;
        if (bvoucher[0] == 'A')
            bvoucher[0] = 'X';
        else if (bvoucher[0] == 'B')
            bvoucher[0] = 'W';
        else if (bvoucher[0] == 'D')
            bvoucher[0] = 'K';
        bvoucher++;
    }

    strcpy(&ret_val[0], &prefixo[0]);   // Copiando o prefixo
    strcpy(&ret_val[0]+strlen(prefixo), voucher); // adicionando o valor do voucher

    strcpy(code, &ret_val[0]);
    return 0;
}

int testaData(char datcomp[7]) {
    int dd,mm,yy;

    sscanf(datcomp, "%2d%2d%2d", &yy, &mm, &dd);

    int wrkyy = yy;

    if (wrkyy<100) {
        wrkyy += 2000;
    }

    //check month
    if(mm>=1 && mm<=12)
    {
        //check days
        if((dd>=1 && dd<=31) && (mm==1 || mm==3 || mm==5 || mm==7 || mm==8 || mm==10 || mm==12))
            // Date is valid
            return 0;
        else if((dd>=1 && dd<=30) && (mm==4 || mm==6 || mm==9 || mm==11))
            // Date is valid
            return 0;
        else if((dd>=1 && dd<=28) && (mm==2))
            // Date is valid
            return 0;
        else if(dd==29 && mm==2 && (wrkyy%400==0 ||(wrkyy%4==0 && wrkyy%100!=0)))
            // Date is valid
            return 0;
        else
            // Day is invalid
            return -1;
    }
    else
    {
        // Month is not valid
        return -1;
    }

}


//						↓↓↓VULNERABLE LINES↓↓↓

// 22,0;22,43

// 78,0;78,36

// 164,0;164,25

// 165,0;165,27

// 166,0;166,30

// 167,0;167,35

// 168,0;168,36

// 172,0;172,27

// 200,0;200,42

// 207,0;207,41

// 209,0;209,35

// 212,0;212,41

// 214,0;214,33

// 217,0;217,33

// 247,0;247,78

// 250,0;250,62

// 253,0;253,76

// 256,0;256,60

// 259,0;259,76

// 262,0;262,60

// 265,0;265,62

// 272,0;272,43

// 280,0;280,56

// 297,0;297,68

// 302,0;302,86

// 305,0;305,67

// 314,0;314,51

// 320,0;320,41

// 330,0;330,50

// 331,0;331,31

// 347,0;347,51

// 362,0;362,77

// 366,0;366,36

// 388,0;388,61

// 389,0;389,83

// 391,0;391,30

