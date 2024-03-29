#include <iostream>
#include <sstream>
#include "BigNumber.cpp"
#include "Sha1.cpp"
//#include "utf8/checked.h"
//#include "Util.cpp"
#include "AuthSocket.cpp"
#include "AuthCrypt.cpp"
#include "WorldPacket.h"
#include "ByteConverter.h"


#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <ace/Get_Opt.h>
#include <ace/Dev_Poll_Reactor.h>
#include <ace/TP_Reactor.h>
#include <ace/ACE.h>
#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>

#define MAX_ACCOUNT_STR 16

struct ServerPktHeader
{
    uint16 size;
    uint16 cmd;
};



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



int main()
{
	bool runReactor = false;

	if(runReactor){
		/**/
    ACE_Reactor::instance(new ACE_Reactor(new ACE_TP_Reactor(), true), true);
    ACE_Acceptor<AuthSocket, ACE_SOCK_Acceptor> acceptor;

    uint16 rmport = 3724;
    std::string bind_ip = "127.0.0.1";
    ACE_INET_Addr bind_addr(rmport, bind_ip.c_str());

    if (acceptor.open(bind_addr, ACE_Reactor::instance(), ACE_NONBLOCK) == -1)
    {

				slog("error: cannot bind to port");
        return 1;
    }

		slog("starting reactor loop");

		bool stopEvent = false;
    while (!stopEvent)
    {
        // dont move this outside the loop, the reactor will modify it
        ACE_Time_Value interval(0, 100000);

        if (ACE_Reactor::instance()->run_reactor_event_loop(interval) == -1)
            { break; }

    }
	}else{
		
		/**/
		
		


				




	/**/
	BigNumber N, s, g, b, v, B, K;

	N.SetHexStr("894B645E89E1535BBDAD5B8B290650530801B18EBFBF5E8FAB3C82872A3E9BB7");
	s.SetHexStr("b3d47dc40109ba25459096abdd0bfbbc3266d5b2dcf52eb586d2d2e612afdd84");
	b.SetHexStr("74FB18F873D3044C8E8131BD68BA51B932B6D1F78362A4A3F47D9EC865D62592");
	g.SetDword(7);

	BigNumber gmod = g.ModExp(b, N);

	std::string name = "ALICE";
	std::string pw = "PASSWORD123";
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
	A.SetHexStr("63547EDF0AB99A777D094B3BA93EC6739B3851D7CF956B7EF21BEE3846F6A3D5");

	std::cout << "A hex: " << A.AsHexStr() << std::endl;
	std::cout << "A int: " << A.AsDecStr() << std::endl;

	BigNumber M1;
	M1.SetHexStr("487AC99DB5E745D79F55CCCBCD44ED989FF2A65");



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


		AuthCrypt Crypt;
    Crypt.SetKey(K.AsByteArray(), 40);
    Crypt.Init();


		ServerPktHeader header;
		
		uint32 Seed;
    Seed = static_cast<uint32>(rand32());


    uint16 SMSG_AUTH_CHALLENGE = 0x1EC;
    WorldPacket pct(SMSG_AUTH_CHALLENGE, 4);
    pct << Seed;
		

    header.cmd = pct.GetOpcode();

    header.size = (uint16) pct.size() + 2;

    EndianConvertReverse(header.size);
    EndianConvert(header.cmd);

	std::cout << "header size: " << header.size << std::endl;
	std::cout << "header cmd: " << header.cmd << std::endl;

    Crypt.EncryptSend((uint8*) & header, sizeof(header));

	std::cout << "enc header size: " << header.size << std::endl;
	std::cout << "enc header cmd: " << header.cmd << std::endl;


	//253,158,37,23,60,41




		/*
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
    //sha.UpdateBigNumbers(&s, &A, &B, &K, NULL);
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
	/**/
	}


	ByteBuffer value;
	
        value << uint32(0x8d8);
        value << uint32(0x8d7);
        value << uint32(0x8d6);
        value << uint32(0x8d5);
        value << uint32(0x8d4);
        value << uint32(0x8d3);

			//std::cout << value.contents() << std::endl;
    BigNumber v;
    v.SetBinary(value.contents(), 20);
	std::cout << "v hex: " << v.AsHexStr() << std::endl;
	std::cout << "v int: " << v.AsDecStr() << std::endl;


}
