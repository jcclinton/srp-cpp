struct bignum_st;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef signed long int32;
typedef unsigned long long uint64;

/**
 * @brief
 *
 */
class BigNumber
{
    public:
        /**
         * @brief
         *
         */
        BigNumber();
        /**
         * @brief
         *
         * @param bn
         */
        BigNumber(const BigNumber& bn);
        /**
         * @brief
         *
         * @param uint32
         */
        BigNumber(uint32);
        /**
         * @brief
         *
         */
        ~BigNumber();

        /**
         * @brief
         *
         * @param uint32
         */
        void SetDword(uint32);
        /**
         * @brief
         *
         * @param uint64
         */
        void SetQword(uint64);
        /**
         * @brief
         *
         * @param bytes
         * @param len
         */
        void SetBinary(const uint8* bytes, int len);
        /**
         * @brief
         *
         * @param str
         */
        void SetHexStr(const char* str);

        /**
         * @brief
         *
         * @param numbits
         */
        void SetRand(int numbits);

        /**
         * @brief
         *
         * @param bn
         * @return BigNumber operator
         */
        BigNumber operator=(const BigNumber& bn);

        /**
         * @brief
         *
         * @param bn
         * @return BigNumber operator
         */
        BigNumber operator+=(const BigNumber& bn);
        /**
         * @brief
         *
         * @param bn
         * @return BigNumber operator
         */
        BigNumber operator+(const BigNumber& bn)
        {
            BigNumber t(*this);
            return t += bn;
        }
        /**
         * @brief
         *
         * @param bn
         * @return BigNumber operator
         */
        BigNumber operator-=(const BigNumber& bn);
        /**
         * @brief
         *
         * @param bn
         * @return BigNumber operator
         */
        BigNumber operator-(const BigNumber& bn)
        {
            BigNumber t(*this);
            return t -= bn;
        }
        /**
         * @brief
         *
         * @param bn
         * @return BigNumber operator
         */
        BigNumber operator*=(const BigNumber& bn);
        /**
         * @brief
         *
         * @param bn
         * @return BigNumber operator
         */
        BigNumber operator*(const BigNumber& bn)
        {
            BigNumber t(*this);
            return t *= bn;
        }
        /**
         * @brief
         *
         * @param bn
         * @return BigNumber operator
         */
        BigNumber operator/=(const BigNumber& bn);
        /**
         * @brief
         *
         * @param bn
         * @return BigNumber operator
         */
        BigNumber operator/(const BigNumber& bn)
        {
            BigNumber t(*this);
            return t /= bn;
        }
        /**
         * @brief
         *
         * @param bn
         * @return BigNumber operator
         */
        BigNumber operator%=(const BigNumber& bn);
        /**
         * @brief
         *
         * @param bn
         * @return BigNumber operator
         */
        BigNumber operator%(const BigNumber& bn)
        {
            BigNumber t(*this);
            return t %= bn;
        }

        /**
         * @brief
         *
         * @return bool
         */
        bool isZero() const;

        /**
         * @brief
         *
         * @param bn1
         * @param bn2
         * @return BigNumber
         */
        BigNumber ModExp(const BigNumber& bn1, const BigNumber& bn2);
        /**
         * @brief
         *
         * @param
         * @return BigNumber
         */
        BigNumber Exp(const BigNumber&);

        /**
         * @brief
         *
         * @return int
         */
        int GetNumBytes(void);

        /**
         * @brief
         *
         * @return bignum_st
         */
        struct bignum_st* BN() { return _bn; }

        /**
         * @brief
         *
         * @return uint32
         */
        uint32 AsDword();
        /**
         * @brief
         *
         * @param minSize
         * @return uint8
         */
        uint8* AsByteArray(int minSize = 0);

        /**
         * @brief
         *
         * @return const char
         */
        const char* AsHexStr();
        /**
         * @brief
         *
         * @return const char
         */
        const char* AsDecStr();

    private:
        struct bignum_st* _bn; /**< TODO */
        uint8* _array; /**< TODO */
};
