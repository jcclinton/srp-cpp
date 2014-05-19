#include <openssl/sha.h>
#include <openssl/crypto.h>

class BigNumber;

/**
 * @brief
 *
 */
class Sha1Hash
{
    public:
        /**
         * @brief
         *
         */
        Sha1Hash();
        /**
         * @brief
         *
         */
        ~Sha1Hash();

        /**
         * @brief
         *
         * @param bn0...
         */
        void UpdateBigNumbers(BigNumber* bn0, ...);

        /**
         * @brief
         *
         * @param dta
         * @param len
         */
        void UpdateData(const uint8* dta, int len);
        /**
         * @brief
         *
         * @param str
         */
        void UpdateData(const std::string& str);

        /**
         * @brief
         *
         */
        void Initialize();
        /**
         * @brief
         *
         */
        void Finalize();

        /**
         * @brief
         *
         * @return uint8
         */
        uint8* GetDigest(void) { return mDigest; }
        /**
         * @brief
         *
         * @return int
         */
        int GetLength(void) { return SHA_DIGEST_LENGTH; }

    private:
        SHA_CTX mC; /**< TODO */
        uint8 mDigest[SHA_DIGEST_LENGTH]; /**< TODO */
};
