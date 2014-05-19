#include <iostream>
#include <sstream>
#include "BigNumber.cpp"
#include "Sha1.cpp"
#include "utf8/checked.h"
#include "Util.cpp"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_ACCOUNT_STR 16

typedef struct AUTH_LOGON_CHALLENGE_C
{
    uint8   cmd;
    uint8   error;
    uint16  size;
    uint8   gamename[4];
    uint8   version1;
    uint8   version2;
    uint8   version3;
    uint16  build;
    uint8   platform[4];
    uint8   os[4];
    uint8   country[4];
    uint32  timezone_bias;
    uint32  ip;
    uint8   I_len;
    uint8   I[5];
} sAuthLogonChallenge_C;


BigNumber getDerivedKey(std::string& name, std::string& password, BigNumber salt)
{
    Sha1Hash sha;
    sha.Initialize();
    sha.UpdateData(name);
    sha.UpdateData(":");
    sha.UpdateData(password);
    sha.Finalize();

    std::string encoded;
		uint8* digest = sha.GetDigest();
    hexEncodeByteArray(digest, sha.GetLength(), encoded);

		BigNumber inner;
		inner.SetHexStr(encoded.c_str());
	//std::cout << "inner: " << inner.AsHexStr() << std::endl;

    // In case of leading zeros in the rI hash, restore them
    uint8 mDigest[SHA_DIGEST_LENGTH];
    memset(mDigest, 0, SHA_DIGEST_LENGTH);
    if (inner.GetNumBytes() <= SHA_DIGEST_LENGTH)
        { memcpy(mDigest, inner.AsByteArray(), inner.GetNumBytes()); }

    std::reverse(mDigest, mDigest + SHA_DIGEST_LENGTH);

	BigNumber x;

	sha.Initialize();
	sha.UpdateData(salt.AsByteArray(), salt.GetNumBytes());
	sha.UpdateData(mDigest, SHA_DIGEST_LENGTH);
	sha.Finalize();

	x.SetBinary(sha.GetDigest(), sha.GetLength());
	//std::cout << "derived key hex: " << x.AsHexStr() << std::endl;

	return x;
}


bool normalizeString(std::string& utf8str)
{
    wchar_t wstr_buf[MAX_ACCOUNT_STR + 1];

    size_t wstr_len = MAX_ACCOUNT_STR;
    if (!Utf8toWStr(utf8str, wstr_buf, wstr_len))
        { return false; }

    std::transform(&wstr_buf[0], wstr_buf + wstr_len, &wstr_buf[0], wcharToUpperOnlyLatin);

    return WStrToUtf8(wstr_buf, wstr_len, utf8str);
}

int main()
{
	 register int sock, c, numbytes;
    socklen_t bsize;
    struct sockaddr_in sa;
    struct tm *tm;
    FILE *client;

    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return 1;
    }

    bzero(&sa, sizeof sa);

    sa.sin_family = AF_INET;
    sa.sin_port   = htons(3724);

    if (INADDR_ANY)
        sa.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr *)&sa, sizeof sa) < 0) {
        perror("bind");
        return 2;
    }
		

    listen(sock, 4);

		 bsize = sizeof sa;

		int logon_chal_size;
		logon_chal_size = sizeof(sAuthLogonChallenge_C);
		std::cout << "chal size: " << logon_chal_size << std::endl;
		sAuthLogonChallenge_C logon_chal;

			if ((c = accept(sock, (struct sockaddr *)&sa, &bsize)) < 0) {
					perror("daytimed accept");
					return 4;
			}


    if ((numbytes = recv(c, &logon_chal, logon_chal_size, 0)) == -1){
			perror("recv");
			exit(1);
		}

			char *msg = "hello";
			int len = strlen(msg);
			send(c, msg, len, 0);


				

        //close(client);



	BigNumber N, s, g, b, v, B, K;

	N.SetHexStr("894B645E89E1535BBDAD5B8B290650530801B18EBFBF5E8FAB3C82872A3E9BB7");
	s.SetHexStr("b3d47dc40109ba25459096abdd0bfbbc3266d5b2dcf52eb586d2d2e612afdd84");
	b.SetHexStr("74FB18F873D3044C8E8131BD68BA51B932B6D1F78362A4A3F47D9EC865D62592");
	g.SetDword(7);

	BigNumber gmod = g.ModExp(b, N);

	std::string name = "ALICE";
	std::string pw = "password123";
	bool result = normalizeString(name);
	normalizeString(pw);
	std::cout << "norm name: " << name << std::endl;
	std::cout << "result: " << result << std::endl;
	// returns little endian x
	BigNumber x = getDerivedKey(name, pw, s);

	std::cout << "X hex: " << x.AsHexStr() << std::endl;
	std::cout << "X int: " << x.AsDecStr() << std::endl;


	 
	v = g.ModExp(x, N);
	std::cout << "G: " << g.AsHexStr() << std::endl;
	std::cout << "N: " << N.AsHexStr() << std::endl;
	std::cout << "V hex: " << v.AsHexStr() << std::endl;
	std::cout << "V int: " << v.AsDecStr() << std::endl;
	//std::cout << "gmod int: " << gmod.AsDecStr() << std::endl;

	B = ((v * 3) + gmod) % N;

	std::cout << "B hex: " << B.AsHexStr() << std::endl;
	std::cout << "B int: " << B.AsDecStr() << std::endl;

	BigNumber A;
	A.SetHexStr("54419A67722F9FA9BA0AAE23D0BE70A028C4213D82ACEEA742D82CF3DF1ACE07");

	std::cout << "A hex: " << A.AsHexStr() << std::endl;
	std::cout << "A int: " << A.AsDecStr() << std::endl;

	BigNumber M1;
	M1.SetHexStr("544CB6F78FC649DAC84D5D41D536F973DC7D787");



	    Sha1Hash sha;
    sha.UpdateBigNumbers(&A, &B, NULL);
    sha.Finalize();
    BigNumber u;
    u.SetBinary(sha.GetDigest(), 20);
	std::cout << "u hex: " << u.AsHexStr() << std::endl;
	std::cout << "u int: " << u.AsDecStr() << std::endl;

    BigNumber S = (A * (v.ModExp(u, N))).ModExp(b, N);
	std::cout << "skey hex: " << S.AsHexStr() << std::endl;
	std::cout << "skey int: " << S.AsDecStr() << std::endl;

    uint8 t[32];
    uint8 t1[16];
    uint8 vK[40];
    memcpy(t, S.AsByteArray(32), 32);
    for (int i = 0; i < 16; ++i)
    {
        t1[i] = t[i * 2];
    }
    sha.Initialize();
    sha.UpdateData(t1, 16);
    sha.Finalize();
    for (int i = 0; i < 20; ++i)
    {
        vK[i * 2] = sha.GetDigest()[i];
    }
    for (int i = 0; i < 16; ++i)
    {
        t1[i] = t[i * 2 + 1];
    }
    sha.Initialize();
    sha.UpdateData(t1, 16);
    sha.Finalize();
    for (int i = 0; i < 20; ++i)
    {
        vK[i * 2 + 1] = sha.GetDigest()[i];
    }
    K.SetBinary(vK, 40);

	std::cout << "key hex: " << K.AsHexStr() << std::endl;
	std::cout << "key int: " << K.AsDecStr() << std::endl;


    uint8 hash[20];
    uint8 hash2[20];

    sha.Initialize();
    sha.UpdateBigNumbers(&N, NULL);
    sha.Finalize();
    memcpy(hash, sha.GetDigest(), 20);

		BigNumber P1_t;
    P1_t.SetBinary(hash, 20);
		std::cout << "prime hash hex: " << P1_t.AsHexStr() << std::endl;
		std::cout << "prime hash int: " << P1_t.AsDecStr() << std::endl;
	
    sha.Initialize();
    sha.UpdateBigNumbers(&g, NULL);
    sha.Finalize();
    memcpy(hash2, sha.GetDigest(), 20);

		BigNumber P2_t;
    P2_t.SetBinary(hash2, 20);
		std::cout << "gen hash hex: " << P2_t.AsHexStr() << std::endl;
		std::cout << "gen hash int: " << P2_t.AsDecStr() << std::endl;

    for (int i = 0; i < 20; ++i)
    {
        hash[i] ^= hash2[i];
    }
    BigNumber t3;
    t3.SetBinary(hash, 20);
	std::cout << "p1 hex: " << t3.AsHexStr() << std::endl;
	std::cout << "p1 int: " << t3.AsDecStr() << std::endl;

    sha.Initialize();
    sha.UpdateData(name);
    sha.Finalize();
    uint8 t4[SHA_DIGEST_LENGTH];
    memcpy(t4, sha.GetDigest(), SHA_DIGEST_LENGTH);

    BigNumber P3_t;
    P3_t.SetBinary(t4, 20);
	std::cout << "name hash hex: " << P3_t.AsHexStr() << std::endl;
	std::cout << "name hash int: " << P3_t.AsDecStr() << std::endl;

    sha.Initialize();
    sha.UpdateBigNumbers(&t3, NULL);
    sha.UpdateData(t4, SHA_DIGEST_LENGTH);
    sha.UpdateBigNumbers(&s, &A, &B, &K, NULL);
    sha.Finalize();

    BigNumber M;
    M.SetBinary(sha.GetDigest(), 20);

	std::cout << "M1Server hex: " << M.AsHexStr() << std::endl;
	std::cout << "M1Server int: " << M.AsDecStr() << std::endl;

	std::cout << "M1Client hex: " << M1.AsHexStr() << std::endl;
	std::cout << "M1Client int: " << M1.AsDecStr() << std::endl;

			sha.Initialize();
			sha.UpdateBigNumbers(&A, &M, &K, NULL);
			sha.Finalize();

    BigNumber M2;
    M2.SetBinary(sha.GetDigest(), 20);

	std::cout << "M2 hex: " << M2.AsHexStr() << std::endl;
	std::cout << "M2 int: " << M2.AsDecStr() << std::endl;


}