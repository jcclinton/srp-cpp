#ifndef _AUTHSOCKET_H
#define _AUTHSOCKET_H

#define MAX_ACCOUNT_STR 16

#include "utf8/checked.h"
#include "Common.cpp"
#include "BigNumber.cpp"
#include "Sha1.cpp"
#include "ByteBuffer.cpp"
#include "Util.cpp"
#include "BufferedSocket.cpp"

#include <iostream>


void BASIC_LOG(std::string str, std::string var)
{
	cout << str << " with var: " << var << endl;
}
void log(std::string name, BigNumber B)
{
	std::cout << name << " hex: " << B.AsHexStr() << std::endl;
	std::cout << name << " int: " << B.AsDecStr() << std::endl;
}

void slog(std::string msg)
{
	cout << msg << endl;
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

std::string CalculateShaPassHash(std::string& name, std::string& password)
{
    Sha1Hash sha;
    sha.Initialize();
    sha.UpdateData(name);
    sha.UpdateData(":");
    sha.UpdateData(password);
    sha.Finalize();

		BigNumber D;
    D.SetBinary(sha.GetDigest(), 20);
		log("raw digest", D);

    std::string encoded;
    hexEncodeByteArray(sha.GetDigest(), sha.GetLength(), encoded);
		BASIC_LOG("encoded digest: ", encoded);

    return encoded;
}


/**
 * @brief Handle login commands
 *
 */
class AuthSocket: public BufferedSocket
{
    public:
        const static int s_BYTE_SIZE = 32; /**< TODO */

        /**
         * @brief
         *
         */
        AuthSocket();
        /**
         * @brief
         *
         */
        ~AuthSocket();

        /**
         * @brief
         *
         */
        void OnAccept() override;
        /**
         * @brief
         *
         */
        void OnRead() override;
        /**
         * @brief
         *
         * @param sha
         */
        void SendProof(Sha1Hash sha);
        /**
         * @brief
         *
         * @param pkt
         * @param acctid
         */
        void LoadRealmlist(ByteBuffer& pkt);

        /**
         * @brief
         *
         * @return bool
         */
        bool _HandleLogonChallenge();
        /**
         * @brief
         *
         * @return bool
         */
        bool _HandleLogonProof();
        /**
         * @brief
         *
         * @return bool
         */
        bool _HandleReconnectChallenge();
        /**
         * @brief
         *
         * @return bool
         */
        bool _HandleReconnectProof();
        /**
         * @brief
         *
         * @return bool
         */
        bool _HandleRealmList();

        /**
         * @brief data transfer handle for patch
         *
         * @return bool
         */
        bool _HandleXferResume();
        /**
         * @brief
         *
         * @return bool
         */
        bool _HandleXferCancel();
        /**
         * @brief
         *
         * @return bool
         */
        bool _HandleXferAccept();

        /**
         * @brief
         *
         * @param rI
         */
        void _SetVSFields(const std::string& rI);

    private:

        BigNumber N, s, g, v; /**< TODO */
        BigNumber b, B; /**< TODO */
        BigNumber K; /**< TODO */
        BigNumber _reconnectProof; /**< TODO */

        bool _authed; /**< TODO */

				std::string sha_pass_hash;

        std::string _login; /**< TODO */
        std::string _safelogin; /**< TODO */

        std::string _localizationName; /**< Since GetLocaleByName() is _NOT_ bijective, we have to store the locale as a string. Otherwise we can't differ between enUS and enGB, which is important for the patch system */
        uint16 _build; /**< TODO */
        AccountTypes _accountSecurityLevel; /**< TODO */

        ACE_HANDLE patch_; /**< TODO */

        /**
         * @brief
         *
         */
        void InitPatch();
};
#endif
/// @}
