#if !defined(VOCAL_BASE64_HXX)
#define VOCAL_BASE64_HXX
/*
 * $Id: Base64.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

/** Infrastructure common to VOCAL.<br><br>
 */
namespace Vocal 
{


/** Provide base 64 encoding and decoding as described in rfc822.
 */
class Base64
{
    public:

        Base64();
        

        virtual ~Base64();

        
        /** Places base 64 encoded input buffer into output buffer.
         */
        static int encode(unsigned char* outputBuf, unsigned int* outputLen,
                          unsigned char* inputBuf, unsigned int inputLen);


        /** Places base 64 decoded input buffer into output buffer.
         */
        static int decode(unsigned char* outputBuf, unsigned int* outputLen,
                          unsigned char* inputBuf, unsigned int inputLen);

        /** Base 64's alphabet.
         */
        static char basis_64[];


        /** Maps ascii to the above alphabet(basis_64).
         */
        static char index_64[128];
};


} // namespace Vocal


#endif // VOCAL_BASE64_HXX

